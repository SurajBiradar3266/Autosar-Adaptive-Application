#ifndef VEHICLEDATA_H
#define VEHICLEDATA_H

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace VehicleData {

    class VehicleData {
    public:
        explicit VehicleData(int port);
        ~VehicleData();
        
        double receiveData();

    private:
        int sockfd;
        int server_port;  // Renamed to avoid shadowing
        struct sockaddr_in server_addr;

        void initSocket();
    };

}

#endif

