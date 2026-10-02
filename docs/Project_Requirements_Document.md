# Project Requirements Document (PRD)

## Linux-Based Multi-Client Chat Application Using C++

---

## 1. Document Information

| Item | Details |
|---|---|
| Project Title | Linux-Based Multi-Client Chat Application Using C++ |
| Programming Language | C++ |
| Operating System | Linux / Ubuntu |
| Development Environment | Ubuntu on WSL 2 |
| Database | SQLite |
| Network Protocol | TCP/IP |
| Build System | Makefile |
| Version Control | Git and GitHub |
| Project Type | Individual Capstone Project |
| Author | Naresh Kumar Sahoo |
| Branch | Computer Science and Engineering |

---

# 2. Project Introduction

The Linux-Based Multi-Client Chat Application is a TCP client-server application developed in C++ and designed to run in a Linux environment.

The application allows multiple clients to connect to a central server and communicate with each other.

The server uses multithreading so that multiple clients can be handled concurrently. Mutex-based synchronization is used to protect shared client information.

SQLite is used for persistent storage of registered users and chat messages.

The project combines Linux system programming, C++ programming, TCP/IP networking, socket programming, multithreading, synchronization, database management, and software architecture.

---

# 3. Problem Statement

Traditional simple chat programs often support only a single client or provide limited communication functionality.

There is a need for a practical Linux-based application that demonstrates how multiple users can communicate through a central server while also demonstrating important system programming concepts.

The project addresses this problem by developing a multi-client TCP chat application in C++.

The server manages multiple client connections, user authentication, chat rooms, private messaging, online users, and persistent chat history.

---

# 4. Project Objectives

The main objectives of the project are:

1. To develop a TCP-based client-server chat application in C++.
2. To understand Linux socket programming.
3. To support multiple simultaneous clients.
4. To use multithreading for concurrent client handling.
5. To use mutexes for synchronization of shared resources.
6. To implement user registration and login.
7. To store user information using SQLite.
8. To store chat messages persistently.
9. To provide room-based communication.
10. To provide private messaging between users.
11. To provide chat history retrieval.
12. To provide online-user information.
13. To understand Linux system programming concepts.
14. To use Git and GitHub for version control.
15. To use a Makefile for project compilation.

---

# 5. Project Scope

## 5.1 In Scope

The project includes:

- TCP client-server communication
- Multiple simultaneous clients
- Client authentication
- User registration
- User login
- Password hashing
- Chat messaging
- Broadcast messaging
- Chat rooms
- Room joining and leaving
- Online user listing
- Private messaging
- Chat history
- SQLite database integration
- Multithreading
- Mutex synchronization
- Linux-based development
- Makefile-based compilation
- Git/GitHub version control

## 5.2 Out of Scope

The current version does not include:

- Graphical user interface
- File sharing
- Image sharing
- Video calling
- Voice calling
- Internet-scale deployment
- Advanced administrator functionality
- Production-grade authentication
- Production-grade encryption

These may be considered for future enhancement.

---

# 6. Target Environment

The application is designed for a Linux environment.

The development environment used for the project is:

- Ubuntu
- WSL 2
- GNU C++ compiler
- Visual Studio Code
- SQLite

The application uses Linux/POSIX networking APIs for TCP communication.

---

# 7. Functional Requirements

Functional requirements describe what the system should do.

## FR-01: Server Startup

The system shall allow the chat server to start and listen for incoming TCP connections.

The server shall use port `5000`.

---

## FR-02: Client Connection

The system shall allow a client to connect to the chat server using TCP.

---

## FR-03: User Registration

The system shall allow a new user to create an account using a username and password.

The registered user information shall be stored in the SQLite database.

---

## FR-04: User Login

The system shall allow an existing user to log in using valid credentials.

The server shall verify the supplied credentials against the stored information.

---

## FR-05: Multiple Clients

The system shall support multiple clients connected to the server simultaneously.

---

## FR-06: Concurrent Client Handling

The server shall create a separate thread for handling each connected client.

---

## FR-07: Normal Messaging

Authenticated users shall be able to send normal text messages.

---

## FR-08: Room-Based Messaging

The system shall support multiple chat rooms.

The current rooms are:

- `lobby`
- `java`
- `python`

Messages shall be associated with the user's current room.

---

## FR-09: Join Room

Users shall be able to join a valid room using:

```text
/join <room>
