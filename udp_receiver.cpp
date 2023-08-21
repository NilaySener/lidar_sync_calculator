#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // Setting the destination IP and the DIFOP port
    const char *udp_ip = "192.168.1.102";
    int udp_port = 2010;

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Bind the socket to the specified IP and port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(udp_port);
    serverAddr.sin_addr.s_addr = inet_addr(udp_ip);
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Bind failed");
        close(sock);
        return 1;
    }

    while (true)
    {
        try
        {
            // Receive UDP packet
            char buffer[1248];
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            ssize_t bytesReceived = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientLen);

            if (bytesReceived == -1)
            {
                perror("Receive failed");
                close(sock);
                return 1;
            }

            // Capture the timestamp at the receiving moment (ns level)
            auto currentTimestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

            // Timestamp transformation
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(currentTimestamp)).count();
            auto nanoseconds = currentTimestamp % 1000000000;
            std::time_t timestamp = seconds;
            std::tm *timeInfo = std::localtime(&timestamp);
            char formattedTimestamp[20];
            std::strftime(formattedTimestamp, sizeof(formattedTimestamp), "%Y-%m-%d %H:%M:%S", timeInfo);

            // std::cout << "\nTime data: ";
            // // Get milisecond value from the buffer
            // uint8_t ms_1 = buffer[26];
            // uint8_t ms_2 = buffer[27];
            // uint16_t milisecond = (ms_1 << 8) | ms_2;
            // std::cout << std::dec << milisecond << "ms  ";

            // // Get mikrosecond value from the buffer
            // uint8_t us_1 = buffer[28];
            // uint8_t us_2 = buffer[29];
            // uint16_t mikrosecond = (us_1 << 8) | us_2;
            // std::cout << std::dec << mikrosecond << "us" << std::endl;

            // Print the Time Data (Y/M/D/S/Ms)
            std::cout << "\nTime data: ";
            int headerOffset = 20;
            int headerLength = 10;
            for (int i = headerOffset; i < headerOffset + headerLength && i < bytesReceived; ++i)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<float>(buffer[i]) << " ";
            }
            //     std::cout << "Received packet at timestamp: " << formattedTimestamp << '.' << std::setfill('0') << std::setw(9) << nanoseconds << std::endl;
        }
        catch (...)
        {
            break;
        }
    }

    close(sock);

    return 0;
}
