#include "../include/session.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

using namespace std;
// ================== methode write =========================
void Session::write(const CanFrame& frame) {
    int socket_fd;
    struct sockaddr_can addr;
    struct can_frame can_frame;
    struct ifreq ifr;

    socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd < 0) {
        perror("Erreur lors de la création du socket CAN");
        return;
    }

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Erreur lors de la configuration de l'interface CAN");
        close(socket_fd);
        return;
    }

    system("sudo ip link set can0 down");

    system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ip link set can0 up");

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Erreur lors de la liaison du socket CAN");
        close(socket_fd);
        return;
    }

    can_frame.can_id = frame.id; 
    can_frame.can_dlc = frame.dlc; 
    memcpy(can_frame.data, frame.data, frame.dlc); 

    if (::write(socket_fd, &can_frame, sizeof(can_frame)) != sizeof(can_frame)) {
        perror("Erreur lors de l'envoi de la trame CAN");
    } else {
        cout << "Trame CAN envoyée avec succès : ID=0x" << hex << can_frame.can_id << endl;
    }

    close(socket_fd);
}

// ================== methode read =========================
void Session::read() {
    int socket_fd;
    struct sockaddr_can addr;
    struct canfd_frame canfd_frame; 
    struct ifreq ifr;

    socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_fd < 0) {
        perror("Erreur lors de la création du socket CAN");
        return;
    }

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Erreur lors de la configuration de l'interface CAN");
        close(socket_fd);
        return;
    }

    system("sudo ip link set can0 down");

    system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ip link set can0 up");

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Erreur lors de la liaison du socket CAN");
        close(socket_fd);
        return;
    }

    int enable_canfd = 1;
    if (setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd)) < 0) {
        perror("Erreur lors de l'activation de CAN FD");
        close(socket_fd);
        return;
    }

    ssize_t num_bytes = ::read(socket_fd, &canfd_frame, sizeof(canfd_frame));
    if (num_bytes < 0) {
        perror("Erreur lors de la lecture de la trame CAN FD");
    } else if (num_bytes == sizeof(canfd_frame)) {
        cout << "=== Trame CAN Reçue ===" << endl;
        cout << "ID: 0x" << hex << canfd_frame.can_id << dec << endl;
        cout << "DLC: " << static_cast<int>(canfd_frame.len) << endl;
        cout << "Data: [";
        for (int i = 0; i < canfd_frame.len; ++i) {
            cout << "0x" << hex << static_cast<int>(canfd_frame.data[i]);
            if (i < canfd_frame.len - 1) cout << ", ";
        }
        cout << "]" << endl;
        cout << "=========================" << endl;
    }

    close(socket_fd);
}

// ================== methode shutdown =========================
void Session::shutdown() {
    cout << "Arrêt de l'interface CAN..." << endl;
    system("sudo ip link set can0 down");
    cout << "Interface CAN arrêtée avec succès." << endl;
}