#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define PORT 30000

//Primitives
#define PACKET_ID 0XFFFF
#define CLIENT_ID 0XFF
#define END_OF_PACKETID 0XFFFF

//server responses
#define PAID 0XFFFB
#define NOT_PAID 0XFFF9
#define DOES_NOT_EXIST 0XFFFA

//Request packet format
struct requestPacket {
	
	uint16_t packetID;
	uint8_t clientID;
	uint16_t Acc_Per;
	uint8_t segment_No;
	uint8_t length;
	uint8_t technology;
	unsigned int SourceSubscriberNo;
	uint16_t endpacketID;
};

//Response packet format
struct responsePacket {
	
	uint16_t packetID;
	uint8_t clientID;
	uint16_t type;
	uint8_t segment_No;
	uint8_t length;
	uint8_t technology;
	unsigned int SourceSubscriberNo;
	uint16_t endpacketID;
};

//print all the packet details
void printPacketDetails(struct requestPacket request) {
	printf("\n ------ Packet details ------ \n");
	printf("	Subscriber no: %u \n",request.SourceSubscriberNo);
	printf("	Segment no : %d \n",request.segment_No);
	printf("	Length : %d\n",request.length);
	printf("	Technology : %d \n", request.technology);
}

//create requestPacket with data
struct requestPacket createRequestPacket() {
	struct requestPacket request;
	request.packetID = PACKET_ID;
	request.clientID = CLIENT_ID;
	request.Acc_Per = 0XFFF8;
	request.endpacketID = END_OF_PACKETID;
	return request;
}


int main(int argc, char**argv){
	struct requestPacket request;
	struct responsePacket response;
	char line[30];
	FILE *fp;
	int sockfd,n = 0;
	struct sockaddr_in cliaddr;
	socklen_t addr_size;
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	int i = 1;
	struct timeval timer;
	
	timer.tv_sec = 3; 					//socket timeout
	timer.tv_usec = 0;	
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timer,sizeof(struct timeval));
	int retryCounter = 0;
	if(sockfd < 0) {
		printf("\n ERROR: Socket Failure \n");
	}
	bzero(&cliaddr,sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cliaddr.sin_port=htons(PORT);
	addr_size = sizeof cliaddr;
	request = createRequestPacket();
	fp = fopen("input.txt", "rt");		//get input data from txt file
	if(fp == NULL){
		printf("\n ERROR: File not found \n");
	}
	
	while(fgets(line, sizeof(line), fp) != NULL) {
		n = 0;
		retryCounter = 0;
		printf("\n");
		char * words;
		words = strtok(line," ");
		request.length = strlen(words);
		request.SourceSubscriberNo = (unsigned) atoi(words);
		words = strtok(NULL," ");
		request.length += strlen(words);
		request.technology = atoi(words);
		words = strtok(NULL," ");
		request.segment_No = i;
        printPacketDetails(request);
		while(n <= 0 && retryCounter < 3) {
			sendto(sockfd,&request,sizeof(struct requestPacket),0,(struct sockaddr *)&cliaddr,addr_size);
			n = recvfrom(sockfd,&response,sizeof(struct responsePacket),0,NULL,NULL);
			if(n <= 0 ) {
				printf("\n ERROR: No response from server\n");
				printf("Sending packet again \n");
				retryCounter++;
			}
			else if(n > 0) {
				printf("\n Response from Server: ");
				if(response.type == NOT_PAID) {
					printf("Subscriber has not paid message \n");
				}
				else if(response.type == DOES_NOT_EXIST ) {
					printf("Subscriber does not exist on database message \n");
				}
				else if(response.type == PAID) {
					printf("Subscriber permitted to access the network message \n");

				}
			}
		}
		//no ACK recieved after sending packet 3 times
		if(retryCounter>= 3 ) {
			printf("\n ERROR: Server does not respond \n");
			exit(0);
		}
		printf("\n ---------------------------------------------------------------------- \n");
		i++;
	}
	fclose(fp);
}

