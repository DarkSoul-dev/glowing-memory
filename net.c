#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define PACKET_SIZE 64
#define MAX_HOPS 30
#define TIMEOUT 1

// Calculate the checksum for the ICMP packet
unsigned short calculate_checksum(unsigned short *addr, int len) {
    unsigned int sum = 0;
    unsigned short checksum;

    while (len > 1) {
        sum += *addr++;
        len -= 2;
    }

    if (len == 1) {
        *(unsigned char *)&checksum = *(unsigned char *)addr;
        sum += checksum;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    checksum = ~sum;

    return checksum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <IP address or hostname>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in target_addr;
    struct hostent *target_host;
    int icmp_socket;
    int ttl = 1;
    int packet_count = 0;

    target_host = gethostbyname(argv[1]);
    if (target_host == NULL) {
        perror("gethostbyname");
        return 1;
    }

    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_addr.s_addr = *((unsigned long *)target_host->h_addr);

    icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (icmp_socket < 0) {
        perror("socket");
        return 1;
    }

    // Set the TTL (Time-To-Live) for the outgoing packets
    if (setsockopt(icmp_socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
        perror("setsockopt");
        return 1;
    }

    printf("Tracing route to %s [%s]\n", argv[1], inet_ntoa(target_addr.sin_addr));
    while (ttl <= MAX_HOPS) {
        struct timeval timeout = {TIMEOUT, 0};
        setsockopt(icmp_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(struct timeval));

        char packet_data[PACKET_SIZE];
        struct icmp *icmp_packet = (struct icmp *)packet_data;
        memset(packet_data, 0, PACKET_SIZE);

        icmp_packet->icmp_type = ICMP_ECHO;
        icmp_packet->icmp_code = 0;
        icmp_packet->icmp_seq = packet_count;
        icmp_packet->icmp_id = getpid();
        for (int i = 0; i < PACKET_SIZE; ++i)
            packet_data[i] = i;

        icmp_packet->icmp_cksum = calculate_checksum((unsigned short *)icmp_packet, sizeof(packet_data));

        // Send the ICMP echo request packet
        if (sendto(icmp_socket, packet_data, PACKET_SIZE, 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) <= 0) {
            perror("sendto");
            return 1;
        }

        // Receive the ICMP reply packet
        char recv_buffer[PACKET_SIZE];
        struct sockaddr_in response_addr;
        socklen_t addr_len = sizeof(response_addr);

        if (recvfrom(icmp_socket, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&response_addr, &addr_len) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("%2d  *\n", ttl);
            } else {
                perror("recvfrom");
            }
        } else {
            // Extract the TTL from the received packet
            int *received_ttl = (int *)(recv_buffer + 8);
            printf("%2d  %s [%s]\n", ttl, target_host->h_name, inet_ntoa(response_addr.sin_addr));
        }

        // If we've reached the destination, exit
        if (response_addr.sin_addr.s_addr == target_addr.sin_addr.s_addr) {
            break;
        }

        ++ttl;
        ++packet_count;
    }

    close(icmp_socket);
    return 0;
}

