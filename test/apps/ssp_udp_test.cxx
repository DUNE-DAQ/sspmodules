#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <Winsock2.h>
    #include <direct.h>
    #define close closesocket
    #pragma comment(lib, "Ws2_32.lib")
    typedef int socklen_t;  // Define socklen_t for Windows
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

#define PORT 55001
#define BUFFER_SIZE 1024
#define TIMEOUT_SEC 5
#define RETRY_COUNT 5

typedef struct {
    unsigned int total_pkt_len;
    unsigned int address;
    unsigned int command;
    unsigned int size;
    unsigned int status;
} Ctrl_Header;

typedef struct {
    Ctrl_Header header;
    unsigned int data[BUFFER_SIZE];
} Ctrl_Packet;

int connectToDevice(const char *ip, struct sockaddr_in *server_addr) {
    int sockfd;

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return -1;
    }
#endif

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &server_addr->sin_addr);

    return sockfd;
}

void disconnectDevice(int sockfd) {
    close(sockfd);
#ifdef _WIN32
    WSACleanup();
#endif
}

int sendReceive(int sockfd, struct sockaddr_in *server_addr, Ctrl_Packet *tx, Ctrl_Packet *rx) {
    socklen_t addr_len = sizeof(*server_addr);
    for (int attempt = 0; attempt < RETRY_COUNT; ++attempt) {
        if (sendto(sockfd, (const char*)tx, tx->header.total_pkt_len, 0, (struct sockaddr*)server_addr, addr_len) < 0) {
            perror("Send failed");
            continue;
        }

        if (recvfrom(sockfd, (char*)rx, sizeof(Ctrl_Packet), 0, (struct sockaddr*)server_addr, &addr_len) < 0) {
            perror("Receive failed");
            continue;
        }

        return 0;
    }
    return -1;
}

int DeviceRead(int sockfd, struct sockaddr_in *server_addr, unsigned int address, unsigned int *value) {
    Ctrl_Packet tx, rx;
    tx.header.total_pkt_len = sizeof(Ctrl_Header);
    tx.header.address = address;
    tx.header.command = 0x01; // Assuming cmdRead is 0x01
    tx.header.size = 1;
    tx.header.status = 0;

    if (sendReceive(sockfd, server_addr, &tx, &rx) != 0) {
        return -1;
    }

    if (tx.header.address != rx.header.address) {
        return -1;
    }

    *value = rx.data[0];
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device_ip> <address>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *device_ip = argv[1];
    unsigned int address = strtoul(argv[2], NULL, 16); // Convert address from hexadecimal

    unsigned int value;
    struct sockaddr_in server_addr;

    int sockfd = connectToDevice(device_ip, &server_addr);
    if (sockfd < 0) {
        return EXIT_FAILURE;
    }

    if (DeviceRead(sockfd, &server_addr, address, &value) == 0) {
        printf("Read value: 0x%08X\n", value);
    } else {
        fprintf(stderr, "Failed to read from device\n");
    }

    disconnectDevice(sockfd);
    return EXIT_SUCCESS;
}
