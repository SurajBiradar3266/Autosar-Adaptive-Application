#include "VehicleData.h"
#include <cstring>

namespace VehicleData {

    VehicleData::VehicleData(int port) : server_port(port) {
        initSocket();
    }

    VehicleData::~VehicleData() {
        close(sockfd);
    }

    void VehicleData::initSocket() {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            std::cerr << "Error opening socket" << std::endl;
            exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(server_port);

        if (bind(sockfd, reinterpret_cast<const struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
            std::cerr << "Error binding socket" << std::endl;
            exit(1);
        }
    }

    double VehicleData::receiveData() {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);

        int n = recvfrom(sockfd, reinterpret_cast<char*>(buffer), sizeof(buffer), MSG_WAITALL, reinterpret_cast<struct sockaddr*>(&client_addr), &len);
        if (n < 0) {
            std::cerr << "Error receiving data" << std::endl;
            return 0.0; // return 0.0 to indicate an error
        }

        buffer[n] = '\0';
        try {
            return std::stod(buffer); // Convert the received string to double
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid data received: " << buffer << std::endl;
            return 0.0; // return 0.0 to indicate an invalid conversion
        } catch (const std::out_of_range& e) {
            std::cerr << "Data out of range: " << buffer << std::endl;
            return 0.0; // return 0.0 to indicate data out of range
        }
    }

}
