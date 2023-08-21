#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <string>
#include <iomanip>

std::mutex mutex;

std::vector<uint16_t> receive1(std::vector<uint16_t> &currentTimestamp, int sock)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::lock_guard<std::mutex> lock(mutex);
    size_t i = 0;

    while (i < 1000)
    {

        char buffer_1[1248];
        sockaddr_in clientAddr_1;
        socklen_t clientLen_1 = sizeof(clientAddr_1);
        ssize_t bytesReceived_1 = recvfrom(sock, buffer_1, sizeof(buffer_1), 0, (struct sockaddr *)&clientAddr_1, &clientLen_1);

        if (bytesReceived_1 == -1)
        {
            perror("Receive failed");
            close(sock);
        }
        // std::cout << "\nTime data LIDAR 1: ";

        // std::cout << "\nTime data: ";
        // Get milisecond value from the buffer
        uint8_t ms_1 = buffer_1[26];
        uint8_t ms_2 = buffer_1[27];
        uint16_t milisecond = (ms_1 << 8) | ms_2;
        // std::cout << std::dec << milisecond << "ms  ";

        // Get mikrosecond value from the buffer
        uint8_t us_1 = buffer_1[28];
        uint8_t us_2 = buffer_1[29];
        uint16_t mikrosecond = (us_1 << 8) | us_2;
        // std::cout << std::dec << mikrosecond << "us" << std::endl;

        // int headerOffset = 20;
        // int headerLength = 10;
        // for (int i = headerOffset; i < headerOffset + headerLength && i < bytesReceived_1; ++i)
        // {
        //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<float>(buffer_1[i]) << " ";
        // }

        currentTimestamp.push_back(mikrosecond);
        // // Get current timestamp from system
        // currentTimestamp.push_back(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        i++;
    }

    return currentTimestamp;
}

std::vector<uint16_t> receive2(std::vector<uint16_t> &currentTimestamp, int sock)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::lock_guard<std::mutex> lock(mutex);
    size_t i = 0;

    while (i < 1000)
    {

        char buffer_2[1248];
        sockaddr_in clientAddr_2;
        socklen_t clientLen_2 = sizeof(clientAddr_2);
        ssize_t bytesReceived_2 = recvfrom(sock, buffer_2, sizeof(buffer_2), 0, (struct sockaddr *)&clientAddr_2, &clientLen_2);

        if (bytesReceived_2 == -1)
        {
            perror("Receive failed");
            close(sock);
        }

        // std::cout << "\nTime data LIDAR 2: ";
        // int headerOffset = 20;
        // int headerLength = 10;
        // for (int i = headerOffset; i < headerOffset + headerLength && i < bytesReceived_2; ++i)
        // {
        //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<float>(buffer_2[i]) << " ";
        // }
        uint8_t ms_1 = buffer_2[26];
        uint8_t ms_2 = buffer_2[27];
        uint16_t milisecond = (ms_1 << 8) | ms_2;
        // std::cout << std::dec << milisecond << "ms  ";

        uint8_t us_1 = buffer_2[28];
        uint8_t us_2 = buffer_2[29];
        uint16_t mikrosecond = (us_1 << 8) | us_2;
        // std::cout << std::dec << mikrosecond << "us" << std::endl;

        currentTimestamp.push_back(mikrosecond);
        // currentTimestamp.push_back(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        i++;
    }

    return currentTimestamp;
}

int main()
{
    // Settings for LIDAR 1
    const char *udp_ip1 = "192.168.1.102";
    int msop_port1 = 2010;

    // Settings for LIDAR 2
    const char *udp_ip2 = "192.168.1.102";
    int msop_port2 = 2012;

    // Create UDP sockets for LIDARs
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddress1, serverAddress2;

    // Bind the sockets to the specified IPs and ports LIDAR 1
    serverAddress1.sin_family = AF_INET;
    serverAddress1.sin_port = htons(msop_port1);
    serverAddress1.sin_addr.s_addr = inet_addr(udp_ip1);
    if (bind(sock1, (struct sockaddr *)&serverAddress1, sizeof(serverAddress1)) == -1)
    {
        perror("Bind for LIDAR 1 failed");
        close(sock1);
        return 1;
    }

    // Bind the sockets to the specified IPs and ports LIDAR 2
    serverAddress2.sin_family = AF_INET;
    serverAddress2.sin_port = htons(msop_port2);
    serverAddress2.sin_addr.s_addr = inet_addr(udp_ip2);
    if (bind(sock2, (struct sockaddr *)&serverAddress2, sizeof(serverAddress2)) == -1)
    {
        perror("Bind for LIDAR 2 failed");
        close(sock1);
        return 1;
    }
    std::vector<uint16_t> currentTimestamp_1;
    std::vector<uint16_t> currentTimestamp_2;

    std::thread threadX(receive1, std::ref(currentTimestamp_1), sock1);
    std::thread threadY(receive2, std::ref(currentTimestamp_2), sock2);

    threadX.join();
    threadY.join();

    // Calculate time differences
    for (size_t i = 0; i < 100; i++)
    {
        // std::cout << "DEBUG1" << std::endl;

        auto timeDifference_ms = std::abs(currentTimestamp_1[i] - currentTimestamp_2[i]);
        std::cout << "Time difference microsecond: " << timeDifference_ms << std::endl;
    }

    return 0;
}
