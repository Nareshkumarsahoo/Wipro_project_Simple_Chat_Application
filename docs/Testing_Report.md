# Testing and Integration Report

## Linux-Based Multi-Client TCP Chat Application in C++

**Project:** Linux-Based Multi-Client TCP Chat Application  
**Language:** C++  
**Operating System:** Linux / Ubuntu  
**Protocol:** TCP  
**Database:** SQLite  

---

# 1. Introduction

Testing was performed throughout the development of the Linux-based multi-client chat application.

The purpose of testing was to verify:

- TCP client-server communication
- Multiple client connections
- Concurrent message handling
- User authentication
- Chat rooms
- Private messaging
- Online-user functionality
- SQLite database connectivity
- Message persistence
- Chat history
- Client disconnection handling
- Build and compilation

Testing was performed in the Linux/Ubuntu environment using WSL2.

---

# 2. Testing Environment

| Component | Details |
|---|---|
| Operating System | Ubuntu Linux on WSL2 |
| Compiler | g++ |
| Language | C++17 |
| Network Protocol | TCP |
| Database | SQLite3 |
| Build Tool | Make |
| Editor | Visual Studio Code |
| Version Control | Git |
| Repository | GitHub |

---

# 3. Build and Compilation Testing

## Test Case TC-01: Server Compilation

### Objective

Verify that the chat server compiles successfully.

### Command

```bash
make server
