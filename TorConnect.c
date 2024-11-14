#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#define SOCKS5_VERSION 0x05
#define SOCKS5_CONNECT 0x01
#define SOCKS5_IPV4 0x01
#define TOR_PROXY_PORT 9050  // Default port for Tor's SOCKS proxy

int connect_to_socks_proxy(const char *proxy_host, int proxy_port);
int socks5_connect(int sock, const char *hostname, int port);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        return 1;
    }

    const char *hostname = argv[1];
    int port = atoi(argv[2]);

    // Connect to the Tor SOCKS proxy
    int sock = connect_to_socks_proxy("127.0.0.1", TOR_PROXY_PORT);
    if (sock == -1) {
        perror("Failed to connect to Tor SOCKS proxy");
        return 1;
    }

    // Request connection to the target hostname through SOCKS5
    if (socks5_connect(sock, hostname, port) == -1) {
        perror("SOCKS5 connection failed");
        close(sock);
        return 1;
    }

    // Send an HTTP GET request as an example
    char request[1024];
    snprintf(request, sizeof(request), "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", hostname);
    send(sock, request, strlen(request), 0);

    // Receive and print the response
    char response[4096];
    ssize_t bytes_received;
    while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        printf("%s", response);
    }

    // Close the socket
    close(sock);
    return 0;
}

// Connect to the SOCKS proxy
int connect_to_socks_proxy(const char *proxy_host, int proxy_port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in proxy_addr;
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
    if (inet_pton(AF_INET, proxy_host, &proxy_addr.sin_addr) <= 0) {
        perror("Invalid proxy address");
        close(sock);
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) < 0) {
        perror("Connection to proxy failed");
        close(sock);
        return -1;
    }

    // SOCKS5 authentication (no authentication)
    uint8_t auth_request[] = {SOCKS5_VERSION, 1, 0};
    send(sock, auth_request, sizeof(auth_request), 0);

    uint8_t auth_response[2];
    if (recv(sock, auth_response, sizeof(auth_response), 0) != 2 || auth_response[1] != 0) {
        perror("SOCKS5 authentication failed");
        close(sock);
        return -1;
    }

    return sock;
}

// Request SOCKS5 connection to hostname:port
int socks5_connect(int sock, const char *hostname, int port) {
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL) {
        fprintf(stderr, "Hostname resolution failed\n");
        return -1;
    }

    uint8_t connect_request[10];
    connect_request[0] = SOCKS5_VERSION;      // Version 5
    connect_request[1] = SOCKS5_CONNECT;      // Connect command
    connect_request[2] = 0;                   // Reserved
    connect_request[3] = SOCKS5_IPV4;         // IPv4 address type

    // IP address (4 bytes)
    memcpy(&connect_request[4], host->h_addr, 4);

    // Port number (2 bytes, network byte order)
    connect_request[8] = (port >> 8) & 0xFF;
    connect_request[9] = port & 0xFF;

    // Send the connection request
    send(sock, connect_request, sizeof(connect_request), 0);

    // Receive the connection response
    uint8_t response[10];
    if (recv(sock, response, sizeof(response), 0) != 10 || response[1] != 0) {
        fprintf(stderr, "SOCKS5 connection request failed\n");
        return -1;
    }

    return 0;
}
