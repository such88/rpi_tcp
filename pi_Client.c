#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include"pi_header.h"

int str_to_Upper_case(char * str){
	int index =0;
	if(str[index] == '\0')
		return -1;
	else{
		while(str[index++]){
			if(str[index]>=65 && str[index]<=90);
			else{
				if(str[index]>=97 && str[index]<=122){
					str[index] = str[index] - 32;
				}
				else
					return 0;
			}
		
		}
	}
	return 1;
}

ushort cal_Checksum(char msg[]){
	int sum =0, index;
	ushort chksum;
	
	for(index=0; index<1022;index++){
		sum += msg[index];
	}
	chksum = ~sum;
	printf("sender chksum is %d\n",chksum);
	
	return chksum;	
}

int main(void){
	
	int sock;
	FILE *file_fp;
	static int pkt_no = 0;
	static int file_fd;
	int size;
	unsigned short pkt_size = 0;
	struct sockaddr_in server;
	uchar file_name[256], send_buf[1024] ={'\0'} ;
	uchar server_ack[10];
	char *file_type;
	short index = 0,type_index = 0;	
	ushort chksum;
	int i;

	puts("Enter File name");
	scanf("%[^\n]s",file_name);

/*	if((file_fd = open(file_name,O_RDONLY)) < 0){
		perror("open");
		return -1;
	}*/

/**-------------------------------------------------
*	get the file type	
*---------------------------------------------------*/
	if((file_fp = fopen(file_name,"rb")) == '\0'){
		perror("fopen");
		return -1;
	}
	file_type = file_name;

	do{
		(char *)file_type++;
	}while(file_name[type_index++] != '.');

	file_type = (char *)calloc(1,10);

	while((file_type[index++] = file_name[type_index++]) != '\0');

	printf("file type is: %s\n",file_type);

/**---------------------------------------------------
*	create socket
*----------------------------------------------------*/
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		return -1;
	}

	puts("socket created");	

	//server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_addr.s_addr = inet_addr("192.168.1.3");
	server.sin_family = AF_INET;
	server.sin_port = htons(8090);
	
	if(connect(sock,(struct sockaddr *)&server, sizeof(server))<0){
		perror("connect");
		return -1;
	}

	puts("Connected to server");

	if(send(sock,"READY",6,0)<0){
		perror("send");
		return -1;
	}
	sleep(1);
	if((size = recv(sock, server_ack, sizeof(server_ack), 0)) < 1){
		perror("recv");
		return -1;
	}
	if(strcmp(server_ack,"OK")==0){
		send(sock, file_name, strlen(file_name)+1, 0);
	}
	
	recv(sock, server_ack, sizeof(server_ack), 0);
	puts("recv done");

    fseek(file_fp, 0, SEEK_SET);

	while((size = (fread(send_buf, 1, sizeof(send_buf), file_fp))) > 0){
		
		printf("\nsize is = %d\n",size);
		
		/*chksum = cal_Checksum(send_buf);
		for(i=0; i<2; i++){
			send_buf[1022+i] = (chksum>>(8 * i)) & 0xFF;
			
		}*/
		
			send(sock, send_buf,size,0);
			printf("sending %d packet to server\n",pkt_no);
			
			if(recv(sock, server_ack, sizeof(server_ack), 0) < 1){
				perror("recv");
				return -1;
			}
/*			if(strcmp(server_ack,"GOT IT")){
				printf("server didn't receive pkt %d\n", pkt_no);
				puts("sending again");
				send(sock, send_buf,sizeof(send_buf),0);
				
			}*/
		
		pkt_no++;
		size = 0;
	
	}
		
	fclose(file_fp);

	close(sock);

	return 0;
}
