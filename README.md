
# 🎵 Concurrent Music Streaming Over TCP - MusicStreamTCP

**MusicStreamTCP** is a project that provides both a server and client application for streaming music over TCP. It ensures efficient and reliable transmission of audio data from a server to multiple clients simultaneously, allowing users to enjoy uninterrupted music streaming.

## 📁 **Project Overview**
This project implements a concurrent, multi-threaded music streaming service using TCP. The **server** streams MP3 files to multiple **clients** concurrently, ensuring that each client receives the correct audio data stream in real time.

### ⚙️ **Features:**
- **Concurrent streaming**: Supports multiple clients connected simultaneously using multithreading.
- **TCP-based communication**: Ensures reliable, ordered delivery of audio data.
- **Efficient song transmission**: Streams MP3 files with minimal latency.

---

## 🚀 **How to Run the Project**

### **Client Application**
The **client** application doesn't require any command-line arguments. Multiple clients can be launched from the same machine, and each client will request a song from the server.

To compile and run the **client**:
```bash
gcc -o client music_tcp_client.c
./client
```

### **Server Application**
The **server** application is multi-threaded and takes three command-line arguments:
1. **port number (P)**: The port on which the server listens.
2. **root directory (DIR)**: The folder containing the MP3 songs.
3. **maximum streams (N)**: The maximum number of clients that can connect concurrently.

To compile and run the **server**:
```bash
gcc -o server music_tcp_server.c
./server <port> <DIR> <N>
```

### **Installing mpg123 for Client**
The client application uses `mpg123` to play songs. You can install the `mpg123` dependency on Linux using one of the following commands based on your Linux distribution:

For Ubuntu/Debian-based systems:
```bash
sudo apt install mpg123
```

For Fedora:
```bash
sudo dnf install mpg123
```

For Arch Linux:
```bash
sudo pacman -S mpg123
```

For Flatpak:
```bash
flatpak install mpg123
```

---

## 🧑‍💻 **How It Works**
- The **server** listens for incoming connections on a specified port and streams requested songs to each client.
- The **client** connects to the server and plays the streamed audio file in real time.
- Multiple clients can request different or the same songs simultaneously, and the server efficiently handles all the connections concurrently.

---

## 🛠️ **Tech Stack**
- **C**: Programming language used for server-client implementation.
- **Multithreading**: To handle multiple client requests concurrently.
- **TCP/IP**: Reliable transport protocol used for streaming.
- **mpg123**: A command-line audio player used by the client to play MP3 files.

---

## 📝 **Usage Example**
Here's an example to demonstrate the project in action:

1. **Running the Server**:
   ```bash
   gcc -o server music_tcp_server.c
   ./server 8080 /path/to/mp3/folder 5
   ```

2. **Running Multiple Clients**:
   On different terminals (or machines), run the following:
   ```bash
   gcc -o client music_tcp_client.c
   ./client
   ```

   Each client will connect to the server and stream music concurrently.

