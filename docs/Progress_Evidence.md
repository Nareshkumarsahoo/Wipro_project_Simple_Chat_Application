# Progress Evidence

## Linux-Based Multi-Client Chat Application Using C++

**Author:** Naresh Kumar Sahoo  
**Branch:** B.Tech Computer Science and Engineering  
**University:** SOA University  
**Project Type:** Individual Capstone Project  

---

# 1. Purpose

This document records the development progress, implementation milestones,
testing activities, Git commits, and demonstration evidence of the
Linux-Based Multi-Client Chat Application.

The project was developed incrementally through six stages:

1. Project Introduction
2. Requirements & Development Plan
3. System Design & Architecture
4. Initial Implementation & Prototype
5. Testing, Integration & Improvement
6. Final Implementation & Presentation

---

# 2. Stage 1 — Project Introduction

## Objective

The project was selected to demonstrate practical knowledge of:

- Linux
- C++
- Linux system programming
- TCP/IP networking
- Multithreading
- Synchronization
- SQLite database
- Software architecture
- Git and GitHub

## Initial Project Concept

The initial concept was to build a Linux-based multi-client chat
application using C++ TCP sockets.

The server would accept multiple clients and allow them to communicate
through a central server.

## Initial Architecture

```text
Client 1 ──┐
Client 2 ──┼── TCP ──> C++ Chat Server
Client 3 ──┘