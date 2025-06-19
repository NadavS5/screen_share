#include "Communication.hpp"

#include <iostream>
#include <ws2tcpip.h>
#include <bit>
#include <cstdint>
Stream::Stream(std::string address, UINT16 port)
    : mAddress(std::move(address)), mPort(port), mSocket(INVALID_SOCKET) // initialize mSocket here
{


    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cerr << "ERROR: error initializing WSA, " << WSAGetLastError() << std::endl;
        return;
    }

    this->mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET) {
        std::cerr << "ERROR: eror initializing socket, " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in sockData;
    sockData.sin_family = AF_INET;
    inet_pton(AF_INET, mAddress.c_str(), &sockData.sin_addr);
    sockData.sin_port = htons(mPort);

    if (connect(mSocket, (SOCKADDR*)&sockData, sizeof(sockData)) == SOCKET_ERROR) {
        std::cerr << "socket connect failed:" << WSAGetLastError() << std::endl;
        return;
    }
    
    std::cout << "connection established\n";
}
Stream::~Stream() {
    WSACleanup();
}

std::vector<char> Stream::RecvBySize() {
    

    char sizeHeader[RECV_SIZE_FIELD];
    recv(mSocket, sizeHeader, RECV_SIZE_FIELD, 0);

    uint32_t rawSize = std::bit_cast<uint32_t>(sizeHeader);  // Safe reinterpretation
    int size = ntohl(rawSize);
    
    std::vector<char> data(size);

    while (size != 0) {
        size -= recv(mSocket, data.data(), size, 0);
    }
    return data;
}
void Stream::SendBySize(char* data, int size) {

    char sizeHeader[RECV_SIZE_FIELD];
    uint32_t netValue = htonl(size);
    std::memcpy(sizeHeader, &netValue, RECV_SIZE_FIELD);

    int sent = send(mSocket, sizeHeader, RECV_SIZE_FIELD, 0);
    if (sent == SOCKET_ERROR) {
        std::cerr << "packet send failed: " << WSAGetLastError() << std::endl;
        return;
    }

    int total_sent = 0;
    while (total_sent < size) {
        int sent = send(mSocket, data + total_sent, size - total_sent, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "packet send failed: " << WSAGetLastError() << std::endl;
            return;
        }
        total_sent += sent;
    }

}