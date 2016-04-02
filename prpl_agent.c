
#include <fcntl.h>
#include <net/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TUN_MTU 1496

struct prpl_hdr {
	uint32_t rule_token;
};

int open_raw_socket()
{
	int fd = -1, yes = 1;

	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1) {
		perror("open_raw_socket(): can't open socket");
		return -1;
	}

	if(setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &yes, sizeof(int)) == -1) {
		perror("open_raw_socket(): setsockopt()");
		return -1;
	}

	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("open_raw_socket(): setsockopt()");
		return -1;
	}

	return fd;
}

int attach_virtual_interface(const char *name, int flags)
{
	struct ifreq ifr;
	int fd = -1;
	char tun_name[IFNAMSIZ];

	strcpy(tun_name, name);
	
	// open tunnel device
	if((fd = open("/dev/net/tun", O_RDWR)) < 0) {
		perror("Cannot open /dev/net/tun");
		return -1;
	}

	// set up interface request
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, tun_name, IFNAMSIZ);
	ifr.ifr_flags = flags;

	// request interface
	if((ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0) {
		perror("cannot initialize tunnel");
		close(fd);
		return -1;
	}

	return fd;
}

double time_diff(struct timeval* a, struct timeval* b)
{
	long diff_sec  = b->tv_sec  - a->tv_sec;
	long diff_usec = b->tv_usec - a->tv_usec;
	return (diff_sec * 1000000 + diff_usec)/1000000.0;
}

int main(int argc, char const *argv[])
{
	if(argc < 5) {
		printf("usage: %s tun token prefix log-file\n", argv[0]);
		exit(1);
	}

	const char* tun_name = argv[1];
	uint32_t token = atoi(argv[2]);
	const char* prefix = argv[3];
	const char* log_file = argv[4];

	struct timeval a, b;
	double diff = 0;
	int fd_max, test_fd, select_res;
	ssize_t rx_bytes;

	FILE* fp = fopen(log_file, "a");

	if(!fp) {
		perror("cannot open log file");
	}

	fd_set fds, read_fds;
	pid_t pid = getpid();

	int tun_fd = attach_virtual_interface(tun_name, IFF_TUN | IFF_NO_PI);

	if(tun_fd < 0)
		perror("can't open tun device"), exit(1);

	int raw_fd = open_raw_socket();

	if(raw_fd < 0)
		perror("can't open raw socket");

	FD_SET(tun_fd, &fds);
	fd_max = tun_fd;

	printf("%s *** %i %s:%i raw:%i\n", prefix, pid, tun_name, tun_fd, raw_fd);

	unsigned char rx_buf[TUN_MTU] = {0};
	unsigned char tx_buf[TUN_MTU + sizeof(struct prpl_hdr)] = {0};

	while(1) {
		read_fds = fds;

		select_res = select(fd_max+1, &read_fds, NULL, NULL, NULL);

		if(select_res == -1)
			perror("Select failed"), exit(1);

		for(test_fd = 3; test_fd <= fd_max; test_fd++) {
			if(FD_ISSET(test_fd, &read_fds)) {

				gettimeofday(&a, 0);

				rx_bytes = read(test_fd, rx_buf, sizeof(rx_buf));
				
				ssize_t tx_bytes = -1;
				// fit to IP header struct
				struct ip* ip_hdr = (struct ip*) rx_buf;

				int ip_len = (int) ntohs(ip_hdr->ip_len);
				int ip_proto = (int) ip_hdr->ip_p;
				char* ip_src = inet_ntoa(ip_hdr->ip_src);
				char* ip_dst = inet_ntoa(ip_hdr->ip_dst);

				struct sockaddr_in dst;
				memset(&dst, 0, sizeof(dst));
				dst.sin_family = AF_INET, dst.sin_addr.s_addr = inet_addr(ip_dst);

				struct prpl_hdr* prpl_hdr = (struct prpl_hdr*) rx_buf;
				prpl_hdr->rule_token = token;
				
				memcpy(tx_buf + sizeof(struct prpl_hdr), rx_buf, rx_bytes);

				// send modified buffer
				// tx_bytes = sendto(raw_fd, tx_buf, rx_bytes + sizeof(struct prpl_hdr), 0, (struct sockaddr*)&dst, sizeof(dst));

				// send unmodified buffer
				tx_bytes = sendto(raw_fd, rx_buf, rx_bytes, 0, (struct sockaddr*)&dst, sizeof(dst));

				gettimeofday(&b, 0);

				printf("%s 0x%x %i %i %i %s %s %i %f\n", prefix, token, (int) rx_bytes, (int) tx_bytes, ip_len, ip_src, ip_dst, ip_proto, time_diff(&a, &b));
				fprintf(fp, "%s 0x%x %i %i %i %s %s %i %f\n", prefix, token, (int) rx_bytes, (int) tx_bytes, ip_len, ip_src, ip_dst, ip_proto, time_diff(&a, &b));
			}
		}
	}
	return 0;
}

