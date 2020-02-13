/**
 * @file server.c
 * @brief Based on https://www.abc.se/~m6695/udp.html
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 11.12.2019
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conf.h"

int main() {
	struct sockaddr_in si_me, si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == s) {
		printf("Failed to create socket!\n");
		return -1;
	}

	memset(&si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (-1 == bind(s, (void *) &si_me, sizeof(si_me))) {
		printf("bind() failed!\n");
		return -1;
	}

	for (int i = 0; i < NPACK; i++) {
		if (-1 == recvfrom(s, buf, BUFLEN, 0, (void *)&si_other, &slen)) {
			printf("recvfrom() failed!\n");
			return -1;
		}

		if (sendto(s, buf, BUFLEN, 0, (void *) &si_other, slen)==-1) {
			printf("sendto() failed!\n");
			return -1;
		}
		printf("Received packet from %s:%d\n"
			"Data: %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	}

	close(s);
	return 0;
}
