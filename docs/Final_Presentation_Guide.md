# Final Project Presentation Guide

## Slide 1 — Title

Linux-Based Multi-Client TCP Chat Application Using C++

Name: Naresh Kumar Sahoo
Branch: B.Tech CSE
University: SOA University

---

## Slide 2 — Problem Statement

Traditional basic socket programs generally demonstrate communication
between only one client and one server.

The objective of this project is to develop a Linux-based multi-client
chat application that supports simultaneous users, communication,
authentication, rooms, private messaging, and persistent message
storage.

---

## Slide 3 — Project Objectives

- Build a TCP-based chat application
- Support multiple simultaneous clients
- Use C++ and Linux system programming
- Implement multithreading
- Use mutex synchronization
- Implement authentication
- Implement chat rooms
- Implement private messaging
- Store data using SQLite
- Maintain the project using Git/GitHub

---

## Slide 4 — Technologies Used

- C++
- Linux / Ubuntu
- POSIX TCP sockets
- std::thread
- std::mutex
- SQLite
- GCC/G++
- Make
- Git
- GitHub
- VS Code

---

## Slide 5 — System Architecture

```text
Client 1 ──┐
Client 2 ──┼── TCP ──> C++ Chat Server ──> SQLite
Client 3 ──┘