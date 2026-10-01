#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <mutex>
#include <sqlite3.h>
#include <functional>

sqlite3 *db;

// ============================================
// Password hashing
// ============================================
std::string hashPassword(const std::string &password)
{

    std::hash<std::string> hasher;

    return std::to_string(
        hasher(password));
}

// ============================================
// Register new user
// ============================================
bool registerUser(
    const std::string &username,
    const std::string &password)
{
    std::string sql =
        "INSERT INTO users (username, password) "
        "VALUES (?, ?);";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {

        std::cerr
            << "Failed to prepare registration query"
            << std::endl;

        return false;
    }

    std::string hashedPassword =
        hashPassword(password);

    sqlite3_bind_text(
        stmt,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_text(
        stmt,
        2,
        hashedPassword.c_str(),
        -1,
        SQLITE_TRANSIENT);

    bool success =
        sqlite3_step(stmt) == SQLITE_DONE;

    if (!success)
    {

        std::cerr
            << "Registration failed: "
            << sqlite3_errmsg(db)
            << std::endl;
    }

    sqlite3_finalize(stmt);

    return success;
}

// ============================================
// Login existing user
// ============================================
bool loginUser(
    const std::string &username,
    const std::string &password)
{
    std::string sql =
        "SELECT password FROM users "
        "WHERE username = ?;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {

        std::cerr
            << "Failed to prepare login query"
            << std::endl;

        return false;
    }

    sqlite3_bind_text(
        stmt,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT);

    bool success = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {

        const char *storedPassword =
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, 0));

        std::string hashedPassword =
            hashPassword(password);

        if (storedPassword != nullptr &&
            hashedPassword == storedPassword)
        {

            success = true;
        }
    }

    sqlite3_finalize(stmt);

    return success;
}

void saveMessage(
    const std::string &username,
    const std::string &room,
    const std::string &message)
{
    std::string sql =
        "INSERT INTO messages (username, room, message) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {

        std::cerr << "Failed to prepare SQL statement"
                  << std::endl;
        return;
    }

    sqlite3_bind_text(
        stmt,
        1,
        username.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_text(
        stmt,
        2,
        room.c_str(),
        -1,
        SQLITE_TRANSIENT);

    sqlite3_bind_text(
        stmt,
        3,
        message.c_str(),
        -1,
        SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "Failed to save message"
                  << std::endl;
    }

    sqlite3_finalize(stmt);
}

// ============================================
// Send chat history for current room
// ============================================
void sendChatHistory(
    int clientSocket,
    const std::string &room)
{
    std::string sql =
        "SELECT username, message, timestamp "
        "FROM messages "
        "WHERE room = ? "
        "ORDER BY id ASC "
        "LIMIT 20;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(
            db,
            sql.c_str(),
            -1,
            &stmt,
            nullptr) != SQLITE_OK)
    {

        std::cerr
            << "Failed to prepare history query"
            << std::endl;

        return;
    }

    sqlite3_bind_text(
        stmt,
        1,
        room.c_str(),
        -1,
        SQLITE_TRANSIENT);

    std::string history =
        "--- Chat History ---\n";

    bool hasMessages = false;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {

        hasMessages = true;

        const char *username =
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, 0));

        const char *message =
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, 1));

        const char *timestamp =
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, 2));

        history +=
            "[" + std::string(timestamp ? timestamp : "") + "] " +
            std::string(username ? username : "") + ": " +
            std::string(message ? message : "") +
            "\n";
    }

    if (!hasMessages)
    {
        history += "No previous messages in this room.\n";
    }

    history += "--------------------\n";

    sqlite3_finalize(stmt);

    send(
        clientSocket,
        history.c_str(),
        history.length(),
        0);
}

struct ClientInfo
{
    int socket;
    std::string username;
    std::string room;
};

std::vector<ClientInfo> clients;
std::mutex clientsMutex;

// ============================================
// Broadcast message to all clients in same room
// except sender
// ============================================
void broadcastMessage(
    const std::string &message,
    int senderSocket)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string senderRoom;

    // Find sender's room
    for (const auto &client : clients)
    {

        if (client.socket == senderSocket)
        {
            senderRoom = client.room;
            break;
        }
    }

    // Send only to clients in same room
    for (const auto &client : clients)
    {

        if (client.socket != senderSocket &&
            client.room == senderRoom)
        {

            send(
                client.socket,
                message.c_str(),
                message.length(),
                0);
        }
    }
}

// ============================================
// Broadcast system message to all clients
// ============================================
void broadcastSystemMessage(
    const std::string &message)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto &client : clients)
    {

        send(
            client.socket,
            message.c_str(),
            message.length(),
            0);
    }
}

// ============================================
// Broadcast message to users in a room
// ============================================
void broadcastRoomMessage(
    const std::string &message,
    const std::string &room,
    int excludeSocket = -1)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto &client : clients)
    {

        if (client.room == room &&
            client.socket != excludeSocket)
        {

            send(
                client.socket,
                message.c_str(),
                message.length(),
                0);
        }
    }
}

// ============================================
// Send private message
// Returns true if user was found
// ============================================
bool sendPrivateMessage(
    const std::string &sender,
    const std::string &receiver,
    const std::string &message)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (const auto &client : clients)
    {

        if (client.username == receiver)
        {

            std::string privateMessage =
                "[Private] " +
                sender +
                ": " +
                message;

            send(
                client.socket,
                privateMessage.c_str(),
                privateMessage.length(),
                0);

            return true;
        }
    }

    return false;
}

// ============================================
// Change client's room
// ============================================
bool changeRoom(
    int clientSocket,
    const std::string &newRoom)
{
    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto &client : clients)
    {

        if (client.socket == clientSocket)
        {

            client.room = newRoom;

            return true;
        }
    }

    return false;
}

// ============================================
// Leave current room and return to lobby
// ============================================
bool leaveRoom(int clientSocket)
{

    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto &client : clients)
    {

        if (client.socket == clientSocket)
        {

            if (client.room == "lobby")
            {
                return false;
            }

            client.room = "lobby";

            return true;
        }
    }

    return false;
}

// ============================================
// Available rooms
// ============================================
std::vector<std::string> rooms = {
    "lobby",
    "java",
    "python"};

// ============================================
// Send room list
// ============================================
void sendRoomList(int clientSocket)
{

    std::lock_guard<std::mutex> lock(clientsMutex);

    std::string roomList =
        "Available Rooms:\n";

    int count = 1;

    for (const auto &room : rooms)
    {

        roomList +=
            std::to_string(count) +
            ". " +
            room +
            "\n";

        count++;
    }

    send(
        clientSocket,
        roomList.c_str(),
        roomList.length(),
        0);
}

bool roomExists(const std::string &roomName)
{

    for (const auto &room : rooms)
    {

        if (room == roomName)
        {
            return true;
        }
    }

    return false;
}

bool sendAuthMessage(
    int clientSocket,
    const std::string &message)
{
    std::string data = message + "\n";

    return send(
               clientSocket,
               data.c_str(),
               data.length(),
               0) > 0;
}

// ============================================
// Handle each connected client
// ============================================
void handleClient(int clientSocket)
{

    // ========================================
    // AUTHENTICATION
    // ========================================

    std::string username;
    bool authenticated = false;

    while (!authenticated)
    {

        // ----------------------------------------
        // Send authentication menu
        // ----------------------------------------

        sendAuthMessage(
            clientSocket,
            "AUTH_MENU\n"
            "=== Chat Authentication ===\n"
            "1. Register\n"
            "2. Login\n"
            "Enter choice:");

        // ----------------------------------------
        // Receive choice
        // ----------------------------------------

        char choiceBuffer[32] = {0};

        int choiceBytes = recv(
            clientSocket,
            choiceBuffer,
            sizeof(choiceBuffer) - 1,
            0);

        if (choiceBytes <= 0)
        {

            close(clientSocket);
            return;
        }

        std::string choice(
            choiceBuffer,
            choiceBytes);

        // ----------------------------------------
        // Validate choice
        // ----------------------------------------

        if (choice != "1" && choice != "2")
        {

            sendAuthMessage(
                clientSocket,
                "AUTH_ERROR\n"
                "[Auth] Invalid choice. Please enter 1 or 2.");

            continue;
        }

        // ----------------------------------------
        // Username
        // ----------------------------------------

        sendAuthMessage(
            clientSocket,
            "USERNAME\n"
            "Username:");

        char usernameBuffer[1024] = {0};

        int usernameBytes = recv(
            clientSocket,
            usernameBuffer,
            sizeof(usernameBuffer) - 1,
            0);

        if (usernameBytes <= 0)
        {

            close(clientSocket);
            return;
        }

        username = std::string(
            usernameBuffer,
            usernameBytes);

        // ----------------------------------------
        // Empty username
        // ----------------------------------------

        if (username.empty())
        {

            sendAuthMessage(
                clientSocket,
                "AUTH_ERROR\n"
                "[Auth] Username cannot be empty.");

            continue;
        }

        // ----------------------------------------
        // Password
        // ----------------------------------------

        sendAuthMessage(
            clientSocket,
            "PASSWORD\n"
            "Password:");

        char passwordBuffer[1024] = {0};

        int passwordBytes = recv(
            clientSocket,
            passwordBuffer,
            sizeof(passwordBuffer) - 1,
            0);

        if (passwordBytes <= 0)
        {

            close(clientSocket);
            return;
        }

        std::string password(
            passwordBuffer,
            passwordBytes);

        // ----------------------------------------
        // Empty password
        // ----------------------------------------

        if (password.empty())
        {

            sendAuthMessage(
                clientSocket,
                "AUTH_ERROR\n"
                "[Auth] Password cannot be empty.");

            continue;
        }

        // ========================================
        // REGISTER
        // ========================================

        if (choice == "1")
        {

            if (registerUser(
                    username,
                    password))
            {

                sendAuthMessage(
                    clientSocket,
                    "AUTH_SUCCESS\n"
                    "[Auth] Registration successful!\n"
                    "[Auth] You are now logged in.");

                authenticated = true;
            }
            else
            {

                sendAuthMessage(
                    clientSocket,
                    "AUTH_FAILED\n"
                    "[Auth] Registration failed.\n"
                    "[Auth] Username may already exist.");
            }
        }

        // ========================================
        // LOGIN
        // ========================================

        else if (choice == "2")
        {

            if (loginUser(
                    username,
                    password))
            {

                sendAuthMessage(
                    clientSocket,
                    "AUTH_SUCCESS\n"
                    "[Auth] Login successful!");

                authenticated = true;
            }
            else
            {

                sendAuthMessage(
                    clientSocket,
                    "AUTH_FAILED\n"
                    "[Auth] Invalid username or password.");
            }
        }
    }
    // ========================================
    // STORE AUTHENTICATED USER
    // ========================================

    {
        std::lock_guard<std::mutex> lock(
            clientsMutex);

        for (auto &client : clients)
        {

            if (client.socket == clientSocket)
            {

                client.username = username;
                client.room = "lobby";

                break;
            }
        }
    }

    // ========================================
    // Authentication successful
    // ========================================

    std::cout
        << username
        << " authenticated successfully."
        << std::endl;

    // ========================================
    // Global join notification
    // ========================================

    std::string joinMessage =
        "[System] " +
        username +
        " joined the chat";

    broadcastSystemMessage(
        joinMessage);

    // ========================================
    // CHAT LOOP
    // ========================================

    while (true)
    {

        char buffer[1024] = {0};

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0);

        // ------------------------------------
        // Client disconnected
        // ------------------------------------

        if (bytesReceived <= 0)
        {

            std::cout
                << username
                << " disconnected."
                << std::endl;

            break;
        }

        // ------------------------------------
        // Create message
        // ------------------------------------

        std::string message(
            buffer,
            bytesReceived);

        // ====================================
        // AVAILABLE ROOMS
        // ====================================

        if (message == "/rooms")
        {

            sendRoomList(
                clientSocket);

            continue;
        }

        // ====================================
        // JOIN ROOM
        //
        // Format:
        // /join <room>
        // ====================================

        if (message.rfind("/join ", 0) == 0)
        {

            std::string newRoom =
                message.substr(6);

            // --------------------------------
            // Validate room
            // --------------------------------

            if (!roomExists(newRoom))
            {

                std::string error =
                    "[Error] Room '" +
                    newRoom +
                    "' does not exist.";

                send(
                    clientSocket,
                    error.c_str(),
                    error.length(),
                    0);

                continue;
            }

            // --------------------------------
            // Check empty room name
            // --------------------------------

            if (newRoom.empty())
            {

                std::string error =
                    "Usage: /join <room>";

                send(
                    clientSocket,
                    error.c_str(),
                    error.length(),
                    0);

                continue;
            }

            // --------------------------------
            // Get current room
            // --------------------------------

            std::string oldRoom;

            {
                std::lock_guard<std::mutex> lock(
                    clientsMutex);

                for (const auto &client : clients)
                {

                    if (client.socket == clientSocket)
                    {

                        oldRoom = client.room;

                        break;
                    }
                }
            }

            // --------------------------------
            // Change room
            // --------------------------------

            changeRoom(
                clientSocket,
                newRoom);

            // --------------------------------
            // Notify users in new room
            // --------------------------------

            std::string joinRoomMessage =
                "[System] " +
                username +
                " joined room: " +
                newRoom;

            broadcastRoomMessage(
                joinRoomMessage,
                newRoom,
                clientSocket);

            // --------------------------------
            // Confirm to user
            // --------------------------------

            std::string confirmation =
                "[System] You joined room: " +
                newRoom;

            send(
                clientSocket,
                confirmation.c_str(),
                confirmation.length(),
                0);

            continue;
        }

        // ====================================
        // LEAVE ROOM
        // ====================================

        if (message == "/leave")
        {

            std::string oldRoom;

            // --------------------------------
            // Get current room
            // --------------------------------

            {
                std::lock_guard<std::mutex> lock(
                    clientsMutex);

                for (const auto &client : clients)
                {

                    if (client.socket == clientSocket)
                    {

                        oldRoom = client.room;

                        break;
                    }
                }
            }

            // --------------------------------
            // Already in lobby
            // --------------------------------

            if (oldRoom == "lobby")
            {

                std::string error =
                    "[System] You are already in the lobby";

                send(
                    clientSocket,
                    error.c_str(),
                    error.length(),
                    0);

                continue;
            }

            // --------------------------------
            // Change room to lobby
            // --------------------------------

            leaveRoom(
                clientSocket);

            // --------------------------------
            // Notify users in old room
            // --------------------------------

            std::string leaveRoomMessage =
                "[System] " +
                username +
                " left room: " +
                oldRoom;

            broadcastRoomMessage(
                leaveRoomMessage,
                oldRoom,
                clientSocket);

            // --------------------------------
            // Confirm to user
            // --------------------------------

            std::string roomMessage =
                "[System] You left room: " +
                oldRoom +
                "\n[System] You are now in lobby";

            send(
                clientSocket,
                roomMessage.c_str(),
                roomMessage.length(),
                0);

            continue;
        }

        // ====================================
        // PRIVATE MESSAGE
        //
        // Format:
        // /msg username message
        // ====================================

        if (message.rfind("/msg ", 0) == 0)
        {

            std::string command =
                message.substr(5);

            // --------------------------------
            // Find space
            // --------------------------------

            size_t spacePosition =
                command.find(' ');

            // --------------------------------
            // Invalid format
            // --------------------------------

            if (spacePosition == std::string::npos)
            {

                std::string error =
                    "Usage: /msg <username> <message>";

                send(
                    clientSocket,
                    error.c_str(),
                    error.length(),
                    0);

                continue;
            }

            // --------------------------------
            // Receiver username
            // --------------------------------

            std::string receiver =
                command.substr(
                    0,
                    spacePosition);

            // --------------------------------
            // Private message
            // --------------------------------

            std::string privateMessage =
                command.substr(
                    spacePosition + 1);

            // --------------------------------
            // Send private message
            // --------------------------------

            bool sent =
                sendPrivateMessage(
                    username,
                    receiver,
                    privateMessage);

            // --------------------------------
            // Receiver not found
            // --------------------------------

            if (!sent)
            {

                std::string error =
                    "[Error] User '" +
                    receiver +
                    "' is not online.";

                send(
                    clientSocket,
                    error.c_str(),
                    error.length(),
                    0);
            }

            continue;
        }

        // ====================================
        // CHAT HISTORY
        // ====================================

        if (message == "/history")
        {

            std::string currentRoom;

            // --------------------------------
            // Find current user's room
            // --------------------------------

            {
                std::lock_guard<std::mutex> lock(
                    clientsMutex);

                for (const auto &client : clients)
                {

                    if (client.socket == clientSocket)
                    {

                        currentRoom = client.room;

                        break;
                    }
                }
            }

            // --------------------------------
            // Send history
            // --------------------------------

            sendChatHistory(
                clientSocket,
                currentRoom);

            continue;
        }

        // ====================================
        // ONLINE USERS
        // ====================================

        if (message == "/online")
        {

            std::string currentRoom;

            // --------------------------------
            // Find current user's room
            // --------------------------------

            {
                std::lock_guard<std::mutex> lock(
                    clientsMutex);

                for (const auto &client : clients)
                {

                    if (client.socket == clientSocket)
                    {

                        currentRoom = client.room;

                        break;
                    }
                }
            }

            // --------------------------------
            // Create online users list
            // --------------------------------

            std::string onlineUsers =
                "Users in room '" +
                currentRoom +
                "':\n";

            std::lock_guard<std::mutex> lock(
                clientsMutex);

            int count = 1;

            for (const auto &client : clients)
            {

                // Ignore unauthenticated clients
                if (client.username.empty())
                {
                    continue;
                }

                // Show only users in same room
                if (client.room == currentRoom)
                {

                    onlineUsers +=
                        std::to_string(count) +
                        ". " +
                        client.username +
                        "\n";

                    count++;
                }
            }

            // --------------------------------
            // Send online users
            // --------------------------------

            send(
                clientSocket,
                onlineUsers.c_str(),
                onlineUsers.length(),
                0);

            continue;
        }

        // ====================================
        // DISPLAY NORMAL MESSAGE
        // ====================================

        std::cout
            << "Message from "
            << username
            << ": "
            << message
            << std::endl;

        // ====================================
        // EXIT COMMAND
        // ====================================

        if (message == "exit")
        {

            std::cout
                << "Chat ended by "
                << username
                << "."
                << std::endl;

            break;
        }

        // ====================================
        // NORMAL ROOM BROADCAST
        // ====================================

        // Get current room of sender
        std::string currentRoom;

        {
            std::lock_guard<std::mutex> lock(
                clientsMutex);

            for (const auto &client : clients)
            {

                if (client.socket == clientSocket)
                {

                    currentRoom = client.room;

                    break;
                }
            }
        }

        // ------------------------------------
        // Save message to database
        // ------------------------------------

        saveMessage(
            username,
            currentRoom,
            message);

        // ------------------------------------
        // Broadcast message
        // ------------------------------------

        std::string broadcast =
            username +
            ": " +
            message;

        broadcastMessage(
            broadcast,
            clientSocket);
    }

    // ========================================
    // REMOVE CLIENT
    // ========================================

    {
        std::lock_guard<std::mutex> lock(
            clientsMutex);

        for (
            auto it = clients.begin();
            it != clients.end();
            ++it)
        {

            if (it->socket == clientSocket)
            {

                clients.erase(it);

                break;
            }
        }
    }

    // ========================================
    // NOTIFY REMAINING USERS
    // ========================================

    std::string leaveMessage =
        "[System] " +
        username +
        " left the chat";

    broadcastSystemMessage(
        leaveMessage);

    // ========================================
    // CLOSE SOCKET
    // ========================================

    close(clientSocket);
}

// ============================================
// MAIN
// ============================================
int main()
{

    int dbResult = sqlite3_open("database/chat.db", &db);

    if (dbResult != SQLITE_OK)
    {
        std::cerr << "Failed to open database: "
                  << sqlite3_errmsg(db)
                  << std::endl;
        return 1;
    }

    std::cout << "Database connected successfully!" << std::endl;

    // ========================================
    // 1. Create server socket
    // ========================================
    int serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0);

    if (serverSocket == -1)
    {

        std::cerr
            << "Failed to create socket"
            << std::endl;

        return 1;
    }

    std::cout
        << "Server socket created successfully!"
        << std::endl;

    // ========================================
    // 2. Allow port reuse
    // ========================================
    int opt = 1;

    if (setsockopt(
            serverSocket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt)) == -1)
    {

        std::cerr
            << "Failed to set socket options"
            << std::endl;

        close(serverSocket);
        sqlite3_close(db);

        return 1;
    }

    // ========================================
    // 3. Configure server address
    // ========================================
    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_addr.s_addr =
        INADDR_ANY;

    serverAddress.sin_port =
        htons(5000);

    // ========================================
    // 4. Bind
    // ========================================
    if (bind(
            serverSocket,
            (struct sockaddr *)&serverAddress,
            sizeof(serverAddress)) == -1)
    {

        std::cerr
            << "Bind failed"
            << std::endl;

        close(serverSocket);

        return 1;
    }

    std::cout
        << "Server bound to port 5000"
        << std::endl;

    // ========================================
    // 5. Listen
    // ========================================
    if (listen(
            serverSocket,
            5) == -1)
    {

        std::cerr
            << "Listen failed"
            << std::endl;

        close(serverSocket);

        return 1;
    }

    std::cout
        << "Server is listening on port 5000..."
        << std::endl;

    // ========================================
    // 6. Accept multiple clients
    // ========================================
    while (true)
    {

        sockaddr_in clientAddress{};

        socklen_t clientAddressLength =
            sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (struct sockaddr *)&clientAddress,
            &clientAddressLength);

        if (clientSocket == -1)
        {

            std::cerr
                << "Failed to accept client"
                << std::endl;

            continue;
        }

        std::cout
            << "Client connected!"
            << std::endl;

        // ------------------------------------
        // Add client to shared list
        // ------------------------------------
        {
            std::lock_guard<std::mutex> lock(
                clientsMutex);

            ClientInfo clientInfo;

            clientInfo.socket =
                clientSocket;

            clientInfo.username = "";

            clientInfo.room =
                "lobby";

            clients.push_back(
                clientInfo);
        }

        // ------------------------------------
        // Create client thread
        // ------------------------------------
        std::thread clientThread(
            handleClient,
            clientSocket);

        // Allow thread to run independently
        clientThread.detach();
    }

    // Close server socket and database
    close(serverSocket);
    sqlite3_close(db);

    return 0;
}