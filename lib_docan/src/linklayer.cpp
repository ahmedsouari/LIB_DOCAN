#include "../include/linklayer.h"
#include "../include/canframe.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

using namespace std;

LinkLayer::LinkLayer(const std::string &interface)
{
    _socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket_fd < 0)
    {
        perror("Erreur lors de la création du socket CAN");
        throw std::runtime_error("Failed to create CAN socket");
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, interface.c_str());
    if (ioctl(_socket_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("Erreur lors de la configuration de l'interface CAN");
        close(_socket_fd);
        throw std::runtime_error("Failed to configure CAN interface");
    }

    struct sockaddr_can addr = {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Erreur lors de la liaison du socket CAN");
        close(_socket_fd);
        throw std::runtime_error("Failed to bind CAN socket");
    }
}

LinkLayer::~LinkLayer()
{
    if (_socket_fd >= 0)
    {
        close(_socket_fd);
    }
}

bool LinkLayer::init(string _interface, uint8_t bitrate)
{
    /* Initialize the LinkLayer */
    LinkLayer linkLayer(_interface);
    if (!linkLayer.init(_interface, bitrate))
    {
        cerr << "Failed to initialize LinkLayer." << endl;
        return false;
    }

    string command = "sudo ip link set " + _interface + " type can bitrate " + to_string(bitrate);
    string command2 = "sudo ip link set " + _interface + " up";
    system(command.c_str());
    system(command2.c_str());
    return true;
}

uint8_t LinkLayer::readFrame(struct CanFrame &frame)
{

    int socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd < 0)
    {
        perror("Erreur lors de la création du socket CAN");
        return 1; /*Error*/
    }

    struct sockaddr_can addr = {};
    struct ifreq ifr = {};
    strcpy(ifr.ifr_name, "can0");
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("Erreur lors de la configuration de l'interface CAN");
        close(socket_fd);
        return 1;
    }

    /* Bind the socket to the CAN interface */
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Erreur lors de la liaison du socket CAN");
        close(socket_fd);
        return 1; /*Error*/
    }

    /* CANFD SUPPORT ENABLE */
    int enable_canfd = 1;
    if (setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd)) < 0)
    {
        perror("Erreur lors de l'activation de CAN FD");
        close(socket_fd);
        return 1; /*Error*/
    }

    ssize_t num_bytes = ::read(socket_fd, &frame, sizeof(frame));
    if (num_bytes < 0)
    {
        perror("Erreur lors de la lecture de la trame CAN FD");
        close(socket_fd);
        return 1; /*Error*/
    }
    else if (num_bytes == sizeof(frame))
    {
        cout << "Debug: Received frame with ID: 0x" << hex << frame.can_id << dec << endl;
        cout << "Debug: DLC: " << static_cast<int>(frame.dlc) << endl;
        cout << "Debug: Data: [";
        for (int i = 0; i < frame.dlc; ++i)
        {
            cout << "0x" << hex << static_cast<int>(frame.data[i]);
            if (i < frame.dlc - 1)
                cout << ", ";
        }
        cout << "]" << endl;
    }
    else
    {
        cerr << "Erreur: Taille inattendue de la trame CAN FD reçue." << endl;
        close(socket_fd);
        return 1; /*Error*/
    }

    close(socket_fd);
    return 0; /* Success */
}

uint8_t LinkLayer::sendFrame(const CanFrame &frame)
{
    struct can_frame rawFrame = {};
    rawFrame.can_id = frame.can_id;
    rawFrame.can_dlc = frame.dlc;
    memcpy(rawFrame.data, frame.data, frame.dlc);

    ssize_t num_bytes = ::write(_socket_fd, &rawFrame, sizeof(rawFrame));
    if (num_bytes != sizeof(rawFrame))
    {
        perror("Erreur lors de l'envoi de la trame CAN");
        return 1; /* Error */
    }

    return 0; /* Success */
}
