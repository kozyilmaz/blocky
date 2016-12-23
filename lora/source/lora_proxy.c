#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "cJSON.h"

#define LORA_HDR_SIZE    12
#define LORA_ACK_SIZE    12
#define LORA_BUFFER_SIZE 4096
#define LORA_PROXY_PORT  "1680"

#define PROTOCOL_VERSION 2

#define PKT_PUSH_DATA    0
#define PKT_PUSH_ACK     1
#define PKT_PULL_DATA    2
#define PKT_PULL_RESP    3
#define PKT_PULL_ACK     4

int is_port_valid (const char *port)
{
	char *end;
	unsigned long v;

	if (!port) {
		return -1;
	}
	errno = 0;
	v = strtoul(port, &end, 10);
	if ((errno == ERANGE && v == ULONG_MAX) || (errno != 0 && v == 0)) {
		/* conversion failed */
		return -1;
	}
	if (end == port) {
		/* no digits found */
		return -1;
	}
	if (v <= 0 || v > UINT16_MAX) {
		/* invalid port number */
		return -1;
	}
	return v;
}

int main(int argc, char *argv[])
{
	int i;
	int s;
	int rc;
	int sock;
	struct addrinfo hints;
	struct addrinfo *rp;
	struct addrinfo *result;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;

	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	char *port;
	char *buffer;
	char *command;
	ssize_t nread;

	uint32_t raw_mac_h; /* most significant nibble, network order */
	uint32_t raw_mac_l; /* least significant nibble, network order */
	uint64_t gw_mac;    /* mac address of the client (gateway) */
	uint8_t ack_command;

	if (argc != 2) {
		port = strdup(LORA_PROXY_PORT);
	} else {
		port = strdup(argv[1]);
	}
	if (is_port_valid(port) < 0) {
		fprintf(stderr, "bad port: %s\n", port);
		free(port);
		exit(EXIT_FAILURE);
	}

	/* socket to bind() */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* for wildcard IP address */

	s = getaddrinfo(NULL, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo() %s\n", gai_strerror(s));
		free(port);
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1) {
			/* socket() failed! */
			continue;
		}
		if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
			/* bind() success! */
			break;
		}
		close(sock);
	}
	if (rp == NULL) {
		/* no address succeeded */
		fprintf(stderr, "bind() failed\n");
		freeaddrinfo(result);
		free(port);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);

	buffer = (char *) malloc(sizeof(char) * LORA_BUFFER_SIZE);
	if (!buffer) {
		fprintf(stderr, "malloc() failed\n");
		close(sock);
		free(port);
		exit(EXIT_FAILURE);
	}

	for (;;) {
		peer_addr_len = sizeof(struct sockaddr_storage);
		nread = recvfrom(sock, buffer, LORA_BUFFER_SIZE, 0, (struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1) {
			continue;
		}
		s = getnameinfo((struct sockaddr *) &peer_addr, peer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
		if (s != 0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
			continue;
		}

		/* check LoRa gateway packet */
		if (nread < LORA_HDR_SIZE) {
			fprintf(stderr, "invalid header\n");
			continue;
		}
		if (buffer[0] != PROTOCOL_VERSION) {
			fprintf(stderr, "invalid version %u\n", buffer[0]);
			continue;
		}
		raw_mac_h = *((uint32_t *)(buffer + 4));
		raw_mac_l = *((uint32_t *)(buffer + 8));
		gw_mac = ((uint64_t)ntohl(raw_mac_h) << 32) + (uint64_t)ntohl(raw_mac_l);

		/* prepare gateway ACK */
		switch (buffer[3]) {
			case PKT_PUSH_DATA:
				fprintf(stderr, "PUSH_DATA from gateway 0x%08X%08X @%s:%s (%zd bytes)\n", (uint32_t)(gw_mac >> 32), (uint32_t)(gw_mac & 0xFFFFFFFF), host, service, nread);
				ack_command = PKT_PUSH_ACK;
				break;
			case PKT_PULL_DATA:
				fprintf(stderr, "PULL_DATA from gateway 0x%08X%08X @%s:%s\n", (uint32_t)(gw_mac >> 32), (uint32_t)(gw_mac & 0xFFFFFFFF), host, service);
				ack_command = PKT_PULL_ACK;
				break;
			default:
				fprintf(stderr, "unexpected command %u\n", buffer[3]);
				continue;
		}
		/* send ACK */
		buffer[3] = ack_command;
		if (sendto(sock, (void *)buffer, LORA_ACK_SIZE, 0, (struct sockaddr *) &peer_addr, peer_addr_len) != LORA_ACK_SIZE) {
			fprintf(stderr, "ack send failed!\n");
		}

		/* parse JSON payload */
		if (nread > LORA_HDR_SIZE && nread < LORA_BUFFER_SIZE) {
			buffer[nread] = '\0';
			cJSON * root = cJSON_Parse(buffer + LORA_HDR_SIZE);
			if (!root) {
				continue;
			}
			cJSON *rxpk = cJSON_GetObjectItem(root, "rxpk");
			if (rxpk) {
				for (i = 0 ; i < cJSON_GetArraySize(rxpk) ; i++) {
					cJSON *item = cJSON_GetArrayItem(rxpk, i);
					cJSON *data = cJSON_GetObjectItem(item, "data");
					fprintf(stderr, "LoRa node data: %s\n", (data != NULL) ? data->valuestring : "NULL");
#if 0
#define MQTT_ENDPOINT    "_your_endpoint_"
#define MQTT_USERNAME    "_your_username_"
#define MQTT_PASSWORD    "_your_password_"
#define MQTT_DEVICEID    "_your_clientid_"
#define MQTT_TOPIC       "_your_topic__"
					/* send data to MQTT endpoint */
					rc = asprintf(&command, "mosquitto_pub -h %s -i %s -t %s -u %s -P %s -m %s", MQTT_ENDPOINT, MQTT_DEVICEID, MQTT_TOPIC, MQTT_USERNAME, MQTT_PASSWORD, data->valuestring);
					if (rc != -1) {
						rc = system(command);
						(void) rc;
						free(command);
					}
#else
#define ETH_CONTRACT_ABI "[{ constant: true, inputs: [], name: \"creator\", outputs: [{ name: \"\", type: \"address\" }], payable: false, type: \"function\" }, { constant: false, inputs: [{ name: \"data\", type: \"string\" }], name: \"request\", outputs: [], payable: false, type: \"function\" }, { constant: false, inputs: [], name: \"kill\", outputs: [], payable: false, type: \"function\" }, { constant: false, inputs: [{ name: \"gateway\", type: \"address\" }, { name: \"data\", type: \"string\" }], name: \"activate\", outputs: [], payable: false, type: \"function\" }, { inputs: [], payable: false, type: \"constructor\" }, { anonymous: false, inputs: [{ indexed: true, name: \"from\", type: \"address\" }, { indexed: false, name: \"message\", type: \"string\" }], name: \"Notify\", type: \"event\" }, { anonymous: false, inputs: [{ indexed: true, name: \"from\", type: \"address\" }, { indexed: true, name: \"to\", type: \"address\" }, { indexed: false, name: \"message\", type: \"string\" }], name: \"Process\", type: \"event\" }]"
#define ETH_CONTRACT_ADR "0x6ac7c9caeef8b05decb99e8ca6d74ca8863d5a52"
#define ETH_PASS         ""
#define ETH_IPC          "ipc:////home/pi/private-iot-chain/geth.ipc"
#define ETH_RESP_LEN     256
					/* send data to Ethereum */
					rc = asprintf(&command, "geth --exec 'var bridge = eth.contract(%s).at(\"%s\"); personal.unlockAccount(eth.coinbase, \"%s\"); bridge.request(\"%s\",{from: eth.coinbase})' attach %s", ETH_CONTRACT_ABI, ETH_CONTRACT_ADR, ETH_PASS,  data->valuestring, ETH_IPC);
					if (rc != -1) {
						FILE *p = popen(command, "r");
						if (p != NULL) {
							char *line, buf[ETH_RESP_LEN];
							line = fgets(buf, ETH_RESP_LEN, p);
							if (line != NULL) printf("(geth): %s\n", line);
							pclose(p);
						}
						(void) rc;
						free(command);
					}
#endif
				}
			}
			cJSON *stat = cJSON_GetObjectItem(root, "stat");
			if (stat) {
				cJSON *time = cJSON_GetObjectItem(stat, "time");;
				cJSON *rxnb = cJSON_GetObjectItem(stat, "rxnb");;
				cJSON *rxok = cJSON_GetObjectItem(stat, "rxok");;
				cJSON *rxfw = cJSON_GetObjectItem(stat, "rxfw");;
				fprintf(stderr, "LoRa gateway stats:\n"
						"   gateway system time         : %s\n"
						"   number of packets received  : %d\n"
						"   number of valid packets     : %d\n"
						"   number of packets forwarded : %d\n", (time != NULL) ? time->valuestring : "NULL", (rxnb != NULL) ? rxnb->valueint : 0, (rxok != NULL) ? rxok->valueint : 0, (rxfw != NULL) ? rxfw->valueint : 0);
			}
			cJSON_Delete(root);
		}
	}
	free(buffer);
	close(sock);
	free(port);
	return -1;
}
