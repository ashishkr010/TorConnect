# TorConnect
This C program, "TorConnect," establishes a secure connection to a target website via the Tor network using a SOCKS5 proxy. By connecting to Tor's local proxy at 127.0.0.1:9050, it routes traffic from any Linux command directly through Tor, providing privacy and anonymity for command-line operations.

# Toralizer

Toralizer is a C-based command-line tool that routes any Linux command's network traffic through the Tor network using a SOCKS5 proxy. This tool allows you to connect securely and anonymously to specified IP addresses or domain names by leveraging Tor's privacy features. It’s especially useful for applications like `curl`, `ssh`, and `npm`, which can now run through Tor with minimal setup.

## Features

- **Privacy-Enhanced Command-Line Access**: Redirects any command’s network requests through the Tor network, adding an extra layer of privacy.
- **SOCKS5 Proxy Integration**: Automatically connects to Tor's SOCKS5 proxy at `127.0.0.1:9050`.
- **Flexible and Secure**: Works seamlessly with various network-based commands in Linux.
- **Network Programming in C**: Demonstrates networking fundamentals, including sockets, host resolution, and SOCKS5 protocol handling.

## Requirements

- **Tor**: Ensure Tor is installed and running on your system.
    ```bash
    sudo apt install tor
    sudo service tor start
    ```
- **C Compiler**: A C compiler like `gcc` to compile the source code.

## Installation

1. **Clone the Repository**:
    ```bash
    git clone https://github.com/yourusername/toralizer.git
    cd toralizer
    ```

2. **Compile the Code**:
    ```bash
    gcc -o toralizer toralizer.c
    ```

## Usage

Run the program by specifying a hostname and port. For example, to connect to `example.com` over HTTP (port 80):

```bash
./toralizer example.com 80

