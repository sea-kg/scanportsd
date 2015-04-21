#include "ping.h"

// http://www.ping127001.com/pingpage/ping.text
// http://stackoverflow.com/questions/13620607/creating-ip-network-packets
// http://www.tenouk.com/Module43a.html
// http://stackoverflow.com/questions/8290046/icmp-sockets-linux
// http://stackoverflow.com/questions/14084416/reading-icmp-reply-with-select-in-linux-using-c

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#include <arpa/inet.h>
#include <netdb.h>

/*
#include <sys/time.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
*/

unsigned short in_cksum(unsigned short *addr, int len);

bool ping(QString sIp) {
	bool bResult = false;
	// struct iphdr *ip;
    struct icmphdr* icmp;
    struct sockaddr_in connection;
    // char *src_addr="192.168.1.34";
    char *packet, *buffer;
    int sockfd;
    // int optval;

	// todo check memory leaks
    // packet = new char[sizeof(struct iphdr) + sizeof(struct icmphdr)];
    int packetlen = sizeof(struct icmphdr);
    packet = new char[packetlen];
    memset(packet, 0, packetlen);
    
    int bufferlen = sizeof(struct iphdr) + sizeof(struct icmphdr);
    buffer = new char[bufferlen];
    memset(buffer, 0, bufferlen);
    // ip = (struct iphdr*) packet;
    // icmp = (struct icmphdr*) (packet + sizeof(struct iphdr));
    icmp = (struct icmphdr*) (packet);

    /*ip->ihl         = 5;
    ip->version     = 4;
    ip->tot_len     = sizeof(struct iphdr) + sizeof(struct icmphdr);
    ip->protocol    = IPPROTO_ICMP;
    // ip->saddr       = inet_addr(src_addr);
    ip->daddr       = inet_addr(sIp.toStdString().c_str());*/

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.sequence = 1;
//    icmp->.un.echo.id = 1234;//arbitrary id
    icmp->checksum = in_cksum((unsigned short *)icmp, packetlen);
    // ip->check = in_cksum((unsigned short *)ip, sizeof(struct iphdr)); 

    /* open ICMP socket */
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("socket");
        return false;
    }
     /* IP_HDRINCL must be set on the socket so that the kernel does not attempt 
     *  to automatically add a default ip header to the packet*/
    // setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(int));

    connection.sin_family       = AF_INET;
    connection.sin_addr.s_addr  = inet_addr(sIp.toStdString().c_str());;
    sendto(sockfd, packet, packetlen, 0, (struct sockaddr *)&connection, sizeof(struct sockaddr));
    // printf("Sent %d byte packet to %s\n", packetlen, sIp.toStdString().c_str());

	struct timeval timeout = {10, 0}; //wait max 3 seconds for a reply
	fd_set read_set;
	memset(&read_set, 0, sizeof read_set);
    FD_SET(sockfd, &read_set);
    //wait for a reply with a timeout
    int rc = select(sockfd + 1, &read_set, NULL, NULL, &timeout);
	if (rc == 0) {
		free(packet);
		free(buffer);
		return false;
	} else if (rc < 0) {
		perror("Select");
		return false;
	}

    // addrlen = sizeof(connection);
    socklen_t slen = sizeof(connection);
    if (recvfrom(sockfd, buffer, bufferlen, 0, (struct sockaddr *)&connection, &slen) == -1) {
		free(packet);
        perror("recv");
        bResult = false;
	} else {
		free(packet);
		struct iphdr *ip_reply = (struct iphdr*) buffer;
        struct icmphdr * icmp_reply = (struct icmphdr*) (buffer + sizeof(struct iphdr));
		
		// char *cp = (char *)&ip_reply->saddr;
		// ip_reply = (struct iphdr*) buffer;

		// TODO check ip replay address
		// TODO check id
		// printf("Received %d byte reply from %u.%u.%u.%u:\n", ntohs(ip_reply->tot_len), cp[0]&0xff,cp[1]&0xff,cp[2]&0xff,cp[3]&0xff);
		// http://lxr.free-electrons.com/source/include/uapi/linux/icmp.h#L26
		if (icmp_reply->type == ICMP_ECHOREPLY && icmp_reply->code == 0) // good reply
		{
			// printf("icmp_reply->type: %d\n", icmp_reply->type);
			bResult = true;
		} else {
			
			// printf("icmp_reply->type: %d\n", icmp_reply->type);
		}
		free(buffer);
	    // printf("Received %d byte reply from %u.%u.%u.%u:\n", ntohs(ip_reply->tot_len), cp[0]&0xff,cp[1]&0xff,cp[2]&0xff,cp[3]&0xff);
        // printf("ID: %d\n", ntohs(ip_reply->id));
        // printf("TTL: %d\n", ip_reply->ttl);
	}
	close(sockfd);
	return bResult;
}

unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    // mop up an odd byte, if necessary
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    // add back carry outs from top 16 bits to low 16 bits
    sum = (sum >> 16) + (sum & 0xffff);     // add hi 16 to low 16
    sum += (sum >> 16);             // add carry
    answer = ~sum;              // truncate to 16 bits
    return (answer);
}
