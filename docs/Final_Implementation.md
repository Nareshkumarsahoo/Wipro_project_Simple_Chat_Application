# Final Implementation

## Linux-Based Multi-Client TCP Chat Application Using C++

## 1. Project Overview

The Linux-Based Multi-Client TCP Chat Application is a client-server chat system developed using C++ and Linux system programming concepts.

The application allows multiple users to connect to a central server, authenticate themselves, communicate through chat rooms, send private messages, and retrieve previous chat history.

SQLite is used for persistent storage of users and chat messages.

---

## 2. Technologies Used

| Technology | Purpose |
|---|---|
| C++17 | Application development |
| Linux / Ubuntu | Operating environment |
| TCP/IP | Network communication |
| POSIX Sockets | Client-server communication |
| std::thread | Multi-client handling |
| std::mutex | Synchronization |
| SQLite3 | Persistent database |
| Makefile | Build automation |
| Git | Version control |
| GitHub | Source-code repository |

---

## 3. Implemented Features

The following features have been implemented:

- TCP server
- TCP client
- Multiple simultaneous clients
- Thread-per-client architecture
- Mutex-based synchronization
- User registration
- User login
- Password verification
- Chat rooms
- Room switching
- Room-wise online users
- Public messaging
- Private messaging
- Chat history
- SQLite database
- Client disconnection handling
- Server restart support
- Makefile-based compilation
- Git/GitHub version control

---

## 4. Server Implementation

The server performs the following operations:

1. Creates a TCP socket.
2. Binds the socket to the configured port.
3. Starts listening for connections.
4. Accepts client connections.
5. Creates a separate thread for each client.
6. Authenticates users.
7. Processes chat commands.
8. Broadcasts messages.
9. Handles private messages.
10. Manages chat rooms.
11. Stores messages in SQLite.
12. Removes disconnected clients.

---

## 5. Client Implementation

The client:

1. Creates a TCP socket.
2. Connects to the server.
3. Performs authentication.
4. Starts a receiving thread.
5. Allows the user to send commands and messages.
6. Displays messages received from the server.
7. Supports room operations and private messaging.
8. Allows the user to exit safely.

---

## 6. Database Implementation

SQLite is used to store persistent application data.

### Users Table

```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL
);
