# UML Diagrams

## Linux-Based Multi-Client TCP Chat Application in C++

**Project:** Linux-Based Multi-Client TCP Chat Application  
**Language:** C++  
**Operating System:** Linux / Ubuntu  
**Architecture:** Client-Server  
**Protocol:** TCP  
**Database:** SQLite

---

# 1. Class Diagram

The application uses a client-server architecture. The server manages multiple connected clients using threads and synchronization mechanisms.

```mermaid
classDiagram

    class ChatServer {
        +startServer()
        +acceptClients()
        +handleClient()
        +broadcastMessage()
        +sendPrivateMessage()
        +joinRoom()
        +leaveRoom()
        +getOnlineUsers()
        +saveMessage()
        +loadHistory()
    }

    class ClientInfo {
        +int socket
        +string username
        +string room
    }

    class ChatClient {
        +connectToServer()
        +sendMessage()
        +receiveMessages()
        +authenticate()
        +joinRoom()
        +displayMessages()
    }

    class SQLiteDatabase {
        +connect()
        +registerUser()
        +authenticateUser()
        +saveMessage()
        +loadHistory()
    }

    class Authentication {
        +register()
        +login()
        +hashPassword()
        +verifyPassword()
    }

    ChatServer "1" --> "*" ClientInfo : manages
    ChatServer --> SQLiteDatabase : stores/retrieves data
    ChatServer --> Authentication : authenticates users
    ChatClient --> ChatServer : TCP connection
    Authentication --> SQLiteDatabase : user data
