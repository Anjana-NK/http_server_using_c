# Simple HTTP Server in C (Windows - Winsock)

## Overview

This project is a **basic multithreaded HTTP server written in C** using the **Winsock API** on Windows.

It can:

* Accept multiple client connections
* Handle HTTP **GET requests**
* Serve static files (HTML, text, images)
* Return proper HTTP responses (200 OK / 404 Not Found)

---

## Features

* Multithreaded (handles multiple clients using `CreateThread`)
* Serves static files from current directory
* MIME type detection (HTML, TXT, JPG, PNG)
* URL decoding (`%20` → space)
* Default file support (`index.html`)
* Basic error handling (404)

---

## Technologies Used

* C Programming
* Winsock2 (Windows Sockets API)
* Windows Threads (`CreateThread`)
* File I/O (`fopen`, `fread`)

---

## How It Works

### 1. Server Setup

* Initializes Winsock using `WSAStartup`
* Creates a socket using `socket()`
* Binds to port **8080**
* Starts listening using `listen()`  

### 2. Client Handling

* Accepts connections using `accept()`
* Creates a new thread for each client  

### 3. Request Processing

* Receives HTTP request using `recv()`
* Parses:
  GET /file HTTP/1.1  
* Extracts requested file path  

### 4. File Handling

* Decodes URL (e.g., `%20` → space)
* Defaults to `index.html` if `/` is requested
* Opens file using `fopen()`  

### 5. Response Building

* If file exists:

  * Sends `HTTP/1.1 200 OK`
  * Sends file content
* If file not found:

  * Sends `HTTP/1.1 404 Not Found`
--- 
## Compilation & Running

### Compile  
gcc server.c -o server -lws2_32  

### Run (PowerShell)

.\server  
###  Output  
Server running on http://localhost:8080  

--- 
## Screenshot 
<img width="1919" height="905" alt="Screenshot 2026-04-08 213231" src="https://github.com/user-attachments/assets/bcb00204-f961-4929-b30f-0adcdc98a2a4" />
<img width="1369" height="657" alt="Screenshot 2026-04-08 213241" src="https://github.com/user-attachments/assets/6e5997d4-882d-4d05-a111-4eb6db4fdee3" />



## Key Concepts Learned

* Socket Programming
* HTTP Protocol Basics
* Multithreading in C
* File Handling
* Memory Management (`malloc`, `free`)
* String Parsing (`sscanf`, `strcmp`)

---

## Learning Outcome

By building this project, you understand how:

* Web browsers communicate with servers
* HTTP requests/responses work
* Low-level networking is implemented in C

---

## Acknowledgment

This project is a **learning implementation** to understand how real web servers work internally.  
