# Linux-Based Multi-Client Chat Application Using C++

## 1. Project Overview

The **Linux-Based Multi-Client Chat Application** is a TCP client-server chat system developed in C++ and designed to run in a Linux environment.

The application allows multiple clients to connect to a central server and communicate with each other. The server uses multithreading to handle multiple clients simultaneously and SQLite for persistent storage of user accounts and chat messages.

The project demonstrates concepts related to:

- Linux
- C++ programming
- TCP/IP networking
- Socket programming
- Multithreading
- Mutex-based synchronization
- Database management
- Authentication
- Git and GitHub
- Makefile-based compilation


## 2. Project Objectives

The main objectives of this project are:

1. To understand TCP client-server communication.
2. To implement socket programming in C++.
3. To support multiple clients simultaneously.
4. To use threads for concurrent client handling.
5. To implement synchronization using mutexes.
6. To implement user registration and login.
7. To store user information using SQLite.
8. To store and retrieve chat history.
9. To implement room-based communication.
10. To implement private messaging.
11. To understand Linux-based application development.


## 3. Features

### Authentication

- User registration
- User login
- Password hashing
- Invalid login handling
- SQLite-based user storage

### Chat

- Normal text messaging
- Multiple simultaneous clients
- Server-side client management
- Client disconnect handling

### Rooms

- Lobby
- Java room
- Python room
- Join room
- Leave room
- List available rooms
- Room-specific messaging

### Private Messaging

Users can send private messages using:

```text
/msg <username> <message>
