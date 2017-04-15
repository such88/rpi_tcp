#include"pi_header.h"

/*int cal_Checksum(char msg[]){
	int chksum, sum =0, index;
	
	for(index=0; msg[index];index++){
		sum += msg[index];
		printf("sum is %d\n",sum);
	}
	chksum = ~sum;
	printf("sender chksum is %d\n",chksum);
	
	return chksum;	
}*/

ushort chk_msg_validity(char msg[]){
	int sum =0, index;
	char result_msg[1024];
	ushort result;
	
	for(index =0; msg[index]!='\0';index++){
		result_msg[index] = msg[index];
	}
	
	ushort chksum = (result_msg[index]&0xFF)|(result_msg[index+1]&0x00FF);
	
	printf("\nmsg at server end = %s\n",result_msg);
	printf("\nchksum at server end is = %d\n",chksum);
	
	for(index=0; index<1022;index++){
		sum += result_msg[index];
	}
	sum += chksum;
	chksum= ~sum;
	printf("receiver chksum is %d\n",chksum);
	
	if(chksum ==0){
		result = 0;
	}
	else{
		result =1;
	}
	
	return result;
}

int main(void){
	
	ushort fd;
	FILE *fp;
	ushort socket_desc, cli_socket, read_size, sockaddr_len;
	struct sockaddr_in server, client;
	uchar client_start[10], client_msg[1024] = {'\0'};
	uchar file_name[256];
	ushort pkt_size = 0;
	ssize_t msg_size = 0;

	if((socket_desc = socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		return -1;
	}
	puts("socket created");
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8090);

	if(bind(socket_desc,(struct sockaddr*) &server, sizeof(server)) < 0){
		perror("bind");
		return -1;
	}
	puts("bind with port");


	listen(socket_desc, 2);
	puts("server listening");	

	sockaddr_len = sizeof(struct sockaddr_in);
	
	if((cli_socket = accept(socket_desc,(struct sockaddr *)&client, (socklen_t *)&sockaddr_len)) < 0){
		perror("accept");
		return -1;
	}
	puts("accept succeed");

	if(recv(cli_socket,client_start,10,0) <0){
		perror("recv");
		return -1;
	}
	if(strcmp(client_start,"READY") != 0){
		puts("Incorrect client start message\n");
		return -1;
	}
	else{
		if(send(cli_socket,"OK",3,0) < 0){
			perror("send");
			return -1;
		}
		recv(cli_socket,file_name,sizeof(file_name), 0);
	}

	
	if((fd = open(file_name, O_CREAT, S_IRWXU)) <0){
		perror("open");
		return -1;
	}
	
	if((fp = fopen(file_name,"ab+")) == 0){
		perror("fopen");
		return -1;
	} 
	
	/*if((fd = open("tst.jpg", O_CREAT, S_IRWXU)) <0){
		perror("open");
		return -1;
	}
	
	if((fp = fopen("tst.jpg","ab+")) == 0){
		perror("fopen");
		return -1;
	}*/

	send(cli_socket,"OK",3,0);


	while(1){
		puts("In while\n");
		if((msg_size = recv(cli_socket, client_msg, sizeof(client_msg), 0)) >0){
			
			printf("Server recv packet %d\n", pkt_size++);
			printf("recv pkt size is %d\n",msg_size);
			if(send(cli_socket,"GOT IT",7,0) < 0){
					perror("send");
					return -1;
			}			
			//if(chk_msg_validity(client_msg) == (ushort)0){
			if(fwrite(client_msg, 1, msg_size, fp)<0){
					perror("fwrite");
					return -1;
				}
			}
		else{
			perror("recv");
			return -1;
		}

	}
	if(!fclose(fp));
	else{
		perror("fclose");
	}


	write(fd, EOF, 1);
		
	close(fd);


	return 0;
}
