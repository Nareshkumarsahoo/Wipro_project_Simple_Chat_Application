# System Architecture

## Linux-Based Multi-Client Chat Application Using C++

## 1. Introduction

The Linux-Based Multi-Client Chat Application follows a client-server architecture.

Multiple clients connect to a central C++ server using TCP/IP. The server manages client connections, authentication, chat rooms, messaging, private messaging, and database operations.

SQLite is used for persistent storage of user accounts and chat messages.

---

## 2. High-Level Architecture

```text
                    +----------------+
                    |    Client 1    |
                    +-------+--------+
                            |
                            |
                    +-------v--------+
                    |    Client 2    |
                    +-------+--------+
                            |
                            |
                    +-------v--------+
                    |    Client 3    |
                    +-------+--------+
                            |
                            |
                       TCP / IP
                            |
                            v
              +--------------------------+
              |       C++ Chat Server    |
              +--------------------------+
                    |        |        |
                    |        |        |
                    v        v        v
                Thread 1  Thread 2  Thread 3
                    |        |        |
                    +--------+--------+
                             |
                             v
                    +----------------+
                    | SQLite Database|
                    |    chat.db     |
                    +----------------+
                       |          |
                       v          v
                     users     messages
