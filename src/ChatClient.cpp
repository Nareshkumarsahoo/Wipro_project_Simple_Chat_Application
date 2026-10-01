#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

std::mutex consoleMutex;


// ======================================================
// Receive messages from server during normal chat
// ======================================================

void receiveMessages(int clientSocket) {

    while (true) {

        char buffer[1024] = {0};

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0
        );

        if (bytesReceived <= 0) {

            std::lock_guard<std::mutex> lock(
                consoleMutex
            );

            std::cout
                << "\nDisconnected from server."
                << std::endl;

            break;
        }

        std::string message(
            buffer,
            bytesReceived
        );

        {
            std::lock_guard<std::mutex> lock(
                consoleMutex
            );

            std::cout << "\r";

            std::cout << "\033[K";

            std::cout
                << message
                << std::endl;

            std::cout
                << "Enter message: ";

            std::cout.flush();
        }
    }
}


// ======================================================
// Receive authentication message
// ======================================================

bool receiveAuthMessage(
    int clientSocket,
    std::string& message
) {

    char buffer[2048] = {0};

    int bytesReceived = recv(
        clientSocket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytesReceived <= 0) {

        return false;
    }

    message = std::string(
        buffer,
        bytesReceived
    );

    return true;
}


// ======================================================
// Main
// ======================================================

int main() {

    // ==================================================
    // 1. Create client socket
    // ==================================================

    int clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (clientSocket == -1) {

        std::cerr
            << "Failed to create client socket"
            << std::endl;

        return 1;
    }

    std::cout
        << "Client socket created successfully!"
        << std::endl;


    // ==================================================
    // 2. Configure server address
    // ==================================================

    sockaddr_in serverAddress{};

    serverAddress.sin_family =
        AF_INET;

    serverAddress.sin_port =
        htons(5000);


    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &serverAddress.sin_addr
        ) <= 0) {

        std::cerr
            << "Invalid server address"
            << std::endl;

        close(clientSocket);

        return 1;
    }


    // ==================================================
    // 3. Connect to server
    // ==================================================

    if (connect(
            clientSocket,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)
        ) == -1) {

        std::cerr
            << "Failed to connect to server"
            << std::endl;

        close(clientSocket);

        return 1;
    }

    std::cout
        << "Connected to server successfully!"
        << std::endl;


    // ==================================================
    // 4. AUTHENTICATION
    // ==================================================

    bool authenticated = false;

    while (!authenticated) {

        // ----------------------------------------------
        // Receive authentication message
        // ----------------------------------------------

        std::string serverMessage;

        if (!receiveAuthMessage(
                clientSocket,
                serverMessage
            )) {

            std::cerr
                << "Server disconnected."
                << std::endl;

            close(clientSocket);

            return 1;
        }


        // ----------------------------------------------
        // AUTH MENU
        // ----------------------------------------------

        if (
            serverMessage.find(
                "AUTH_MENU"
            ) != std::string::npos
        ) {

            std::cout
                << std::endl;

            std::cout
                << "=== Chat Authentication ==="
                << std::endl;

            std::cout
                << "1. Register"
                << std::endl;

            std::cout
                << "2. Login"
                << std::endl;

            std::cout
                << "Enter choice: ";

            std::cout.flush();


            std::string choice;

            std::getline(
                std::cin,
                choice
            );


            // Send choice
            send(
                clientSocket,
                choice.c_str(),
                choice.length(),
                0
            );

            continue;
        }


        // ----------------------------------------------
        // USERNAME
        // ----------------------------------------------

        if (
            serverMessage.find(
                "USERNAME"
            ) != std::string::npos
        ) {

            std::cout
                << "Username: ";

            std::cout.flush();


            std::string username;

            std::getline(
                std::cin,
                username
            );


            send(
                clientSocket,
                username.c_str(),
                username.length(),
                0
            );

            continue;
        }


        // ----------------------------------------------
        // PASSWORD
        // ----------------------------------------------

        if (
            serverMessage.find(
                "PASSWORD"
            ) != std::string::npos
        ) {

            std::cout
                << "Password: ";

            std::cout.flush();


            std::string password;

            std::getline(
                std::cin,
                password
            );


            send(
                clientSocket,
                password.c_str(),
                password.length(),
                0
            );

            continue;
        }


        // ----------------------------------------------
        // AUTH SUCCESS
        // ----------------------------------------------

        if (
            serverMessage.find(
                "AUTH_SUCCESS"
            ) != std::string::npos
        ) {

            std::cout
                << "\n"
                << serverMessage
                << std::endl;

            authenticated = true;

            continue;
        }


        // ----------------------------------------------
        // AUTH FAILED
        // ----------------------------------------------

        if (
            serverMessage.find(
                "AUTH_FAILED"
            ) != std::string::npos
        ) {

            std::cout
                << "\n"
                << serverMessage
                << std::endl;

            std::cout
                << "\nAuthentication failed."
                << std::endl;

            std::cout
                << "Please try again."
                << std::endl;

            continue;
        }


        // ----------------------------------------------
        // AUTH ERROR
        // ----------------------------------------------

        if (
            serverMessage.find(
                "AUTH_ERROR"
            ) != std::string::npos
        ) {

            std::cout
                << "\n"
                << serverMessage
                << std::endl;

            continue;
        }


        // ----------------------------------------------
        // Unknown authentication message
        // ----------------------------------------------

        std::cout
            << serverMessage
            << std::endl;
    }


    // ==================================================
    // Authentication completed
    // ==================================================

    std::cout
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;

    std::cout
        << "        Authentication Successful"
        << std::endl;

    std::cout
        << "========================================"
        << std::endl;


    // ==================================================
    // 5. Start receiving thread
    // ==================================================

    std::thread receiver(
        receiveMessages,
        clientSocket
    );

    receiver.detach();


    // ==================================================
    // 6. Send chat messages
    // ==================================================

    while (true) {

        std::string message;

        {
            std::lock_guard<std::mutex> lock(
                consoleMutex
            );

            std::cout
                << "Enter message: ";

            std::cout.flush();
        }


        std::getline(
            std::cin,
            message
        );


        // ----------------------------------------------
        // Exit condition
        // ----------------------------------------------

        if (message == "exit") {

            {
                std::lock_guard<std::mutex> lock(
                    consoleMutex
                );

                std::cout
                    << "Closing chat..."
                    << std::endl;
            }

            break;
        }


        // ----------------------------------------------
        // Send message
        // ----------------------------------------------

        send(
            clientSocket,
            message.c_str(),
            message.length(),
            0
        );
    }


    // ==================================================
    // 7. Close socket
    // ==================================================

    close(clientSocket);

    return 0;
}