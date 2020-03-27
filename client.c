/**
 * @file client.c
 * @brief Based on https://www.abc.se/~m6695/udp.html
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 11.12.2019
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "conf.h"

int main(int argc, char **argv) {
	struct sockaddr_in si_other;
	int s, slen=sizeof(si_other);
	char buf[BUFLEN];
	char buf2[BUFLEN];
	int overtime = 0;
	int port = PORT;

	double time_total = 0, time_max = 0, time_min = INFINITY;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == s) {
		printf("Failed to create socket!\n");
		return -1;
	}

	if (argc > 1) {
		port = atoi(argv[1]);
	}

	memset(&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);

	if (inet_aton(SRV_IP, &si_other.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	for (int i = 0; i < NPACK; i++) {
		struct timespec spec_pre, spec_post;
		double time;

		//printf("Sending packet %d: ", i);

		sprintf(buf, "This is packet %d\n", i);

		clock_gettime(CLOCK_REALTIME, &spec_pre);
		if (sendto(s, buf, BUFLEN, 0, &si_other, slen)==-1) {
			printf("sendto() failed!\n");
			return -1;
		}

		if (recvfrom(s, buf2, BUFLEN, 0, (void *)&si_other, &slen)==-1) {
			printf("recvfrom() failed!\n");
			return -1;
		}

		clock_gettime(CLOCK_REALTIME, &spec_post);

		if (spec_post.tv_nsec < spec_pre.tv_nsec) {
			spec_post.tv_nsec += 1000000000;
		}
		time =  ((double) (spec_post.tv_nsec - spec_pre.tv_nsec)) / 1000000.;
		//printf("%10.2lf ms\n", time);

		time_total += time;
		if (time < time_min) {
			time_min = time;
		}

		//printf("%0.2lf\n", time);

		if (time > 0.9) {
			//printf("%d: big value = %0.2lf\n", i, time);
			overtime++;
		}
		if (time > time_max) {
			//printf("%d: New max = %0.2lf\n", i, time);
			time_max = time;
		}

		if (memcmp(buf, buf2, sizeof(buf))) {
			printf("%d: Buffer mismatch\n", i);
		}
	}

	printf("=====================\n");
	printf("Avg: %0.2lfms\n", time_total / NPACK);
	printf("Min: %0.2lfms\n", time_min);
	printf("Max: %0.2lfms\n", time_max);
	printf("Overtime: %d\n", overtime);

	close(s);
	return 0;
}
