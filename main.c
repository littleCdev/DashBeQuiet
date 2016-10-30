#include<stdio.h>
#include<stdlib.h> 
#include<string.h>
#include<netinet/udp.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include <time.h>


#ifdef DEBUG
	#define debug(args...)		fprintf(stdout,"%s:%i ",__FILE__,__LINE__);fprintf(stdout,args);fprintf(stdout,"\r\n")
#else
	#define debug(args...)
#endif

/// 44:65:0D:XX:XX:XX 

 
struct DHCPOptions{
	char MagicDHCPCookie[4]; // "\x63\x82\x53\x63"
	
	// DHCP message type 
	int8_t MsgOp ;//= 53;
	int8_t MsgLen;// = 1;
	int8_t Msg;// = 5;
	
	// server identifier
	int8_t SIOp;// = 54;
	int8_t SILen;// = 4;
	char   SI[4];// = {10,0,0,20};
	
	// leasetime
	int8_t LTOp;// = 51;
	int8_t LTLen;// = 4;
	int32_t LT;// = 600;
		
	// subnet mask
	int8_t SNOp;//  = 1;
	int8_t SNLen;// = 4;
	char   SN[4];// = {255,255,255,0};

	// Routers (only using one)
	int8_t ROOp;//  = 3;
	int8_t ROLen;// = 4; // can be 4*n
	char   RO[4]; // can be 4*n
	
	// DNS
	int8_t DNSOp;// = 6;
	int8_t DNSLen;// = 4;
	char   DNS[4];// = {10,0,1,133};
	
	char OptionEnd; // =0xFF
};
struct DHCPOptions DhcpOps = {
	.MagicDHCPCookie = {0x63,0x82,0x53,0x63},
	.MsgOp = 53,
	.MsgLen = 1,
	.Msg = 5,
	
	.SIOp = 54,
	.SILen = 4,
	.SI    = {10,0,0,20},
	
	.LTOp = 51,
	.LTLen=4,
	.LT   = 600,
	
	.SNOp = 1,
	.SNLen= 4,
	.SN   = {255,0,0,0},
	
	.ROOp = 3,
	.ROLen= 4,
	.RO   = {10,0,1,153},
	
	.DNSOp= 6,
	.DNSLen=4,
	.DNS  = {10,0,1,153},
	.OptionEnd = 0xFF
};

// http://www.tcpipguide.com/free/t_BOOTPMessageFormat.htm
struct BOOTP{
	int8_t Op;
	int8_t HType;
	int8_t HLen;
	int8_t Hops;
	int32_t XID;
	int16_t Secs;
	int16_t Flags;
	char CIAddr[4];
	char YIAddr[4];
	char SIAddr[4];
	char GIAddr[4];
	char CHAddr[16];
	char SName[64];
	char File[128];
	char Vendor[64];
};

 
void dumpBootpMsg(struct BOOTP *msg){
	int i=0;
	printf("BOOTP Packet\n");
	printf("   |-Operation Code:\t%i\n",msg->Op);
	printf("   |-Hardware Type:\t%i\n",msg->HType);
	printf("   |-Hardware Addr Length:\t%i\n",msg->HLen);
	printf("   |-Hops\t\t%i\n",msg->Hops);
	printf("   |-Transaction ID:\t%04x\n",msg->XID);
	printf("   |-Seconds:\t\t%is\n",msg->Secs);
	printf("   |-Flags:\t\t%i\n",msg->Flags);
	
	printf("   |-Client IP Address:\t");
	for(i=0;i<4;i++)
		printf("%i ",msg->CIAddr[i]);
	printf("\n");	
	
	printf("   |-Server IP Address:\t");
	for(i=0;i<4;i++)
		printf("%i ",msg->CIAddr[i]);
	printf("\n");
	
	printf("   |-Your IP Address:\t");
	for(i=0;i<4;i++)
		printf("%i ",msg->YIAddr[i]);
	printf("\n");
	
	printf("   |-Gateway IP Address:\t");
	for(i=0;i<4;i++)
		printf("%i ",msg->GIAddr[i]);
	printf("\n");

	printf("   |-Client Hardware Address:\n\t\t   ");
	for(i=0;i<16;i++)
		printf("%02X ",(msg->CHAddr[i]& 0xFF));
	printf("\n");
	
	printf("   |-Vendor:\n");
	int b=0;
	for(i=0;i<64;i=i+8){
		printf("\t\t   ");
		for(b=0;b<8;b++)	printf( "%02X ", (msg->Vendor[b+i] & 0xFF));
		printf("\t");
		for(b=0;b<8;b++)	printf( "%03i ", (uint8_t)msg->Vendor[b+i] );
		printf("\t");
		for(b=0;b<8;b++){	
			if(msg->Vendor[b+i] > 32 && msg->Vendor[b+i] < 128)
				printf( "%C", msg->Vendor[b+i]);
			else
				printf( ".");
		}
		
		printf("\n");
	}
	printf("\n");
}


///
/// checks if the mac starts with 44:65:0D wich seems to be for Dashbuttons
/// 1 = dashbutton
/// 
int checkForDashMac(char *sMac){
	if(memcmp("\x44\x65\x0D",sMac,3) == 0)
		return 1;		
	return 0;
}

// just print the time to stdout
void printTime(){
	time_t timer;
    char buffer[26];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S\n", tm_info);
	printf(buffer);
}

int main()
{

	int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	
    unsigned char *buffer = (unsigned char *)malloc(512);
    
	if(sockfd < 0)
    {
        printf("Socket Error\n");
        return 1;
    }
    
	struct sockaddr_in server_sock;
    server_sock.sin_family = AF_INET;
    server_sock.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sock.sin_port = htons(67);
     
    if (bind(sockfd, (struct sockaddr *) &server_sock, sizeof(server_sock)) == -1) {
        close(sockfd);
		perror("server: bind()");
        exit(1);
    }
    
	struct sockaddr_in client_sock;
	socklen_t client_sock_len = sizeof(client_sock);
    while(1)
    {
		// clear buffer
		memset(buffer,0,512);
				
		int recvLen = recvfrom(sockfd, buffer, 512, 0,(struct sockaddr *)&client_sock, &client_sock_len);

		if(recvLen > 0){
			debug("got: \t%i bytes\n",recvLen);				
		}	
		
		// skip smaller packages
		if(recvLen < sizeof(struct BOOTP) -64){ // vendor is optional
			continue;
		}
		
		struct BOOTP bootpMsg;
		memcpy(&bootpMsg,buffer,recvLen);
		
		if(checkForDashMac(bootpMsg.CHAddr)!=1){
			printf("no Dashbutton\n");
			continue;
		}
		printTime();
		printf("yay this is a dash\n");
		
		
		// find the requested IP from dashbutton
		int i=0;
		uint8_t requestedIp[] = {0,0,0,0};
		// 4byte offset Magic keks
		// 64byte Vendor/Options
		// Option: 50, Length: 4
		for(i=4;i<63;i++){
			if(bootpMsg.Vendor[i] == 50 &&
			   bootpMsg.Vendor[i+1] == 4){
//				printf("startbyte for requested IP in vendor: %i\n",i);	   
				requestedIp[0] = bootpMsg.Vendor[i+2];
				requestedIp[1] = bootpMsg.Vendor[i+3];
				requestedIp[2] = bootpMsg.Vendor[i+4];
				requestedIp[3] = bootpMsg.Vendor[i+5];
				break;
			}
		}
		
		debug("Requested IP: %i.%i.%i.%i\n",requestedIp[0],requestedIp[1],requestedIp[2],requestedIp[3]);


		struct BOOTP bootpAnswer;
		bootpAnswer.Op = 2;		// reply
		bootpAnswer.HType = 1;	// ethernet
		bootpAnswer.HLen = 6;	// 6 byte for hardwareaddress
		bootpAnswer.Hops = 0;	// o hopes
		bootpAnswer.XID = bootpMsg.XID; // transaction id from request
		bootpAnswer.Secs = 0;	// 0 seconds
		bootpAnswer.Flags = 0;	// no flags
		
		
		// set from the request
		bootpAnswer.YIAddr[0]=requestedIp[0];
		bootpAnswer.YIAddr[1]=requestedIp[1];
		bootpAnswer.YIAddr[2]=requestedIp[2];
		bootpAnswer.YIAddr[3]=requestedIp[3];
				
		bootpAnswer.CIAddr[0]=0;
		bootpAnswer.CIAddr[1]=0;
		bootpAnswer.CIAddr[2]=0;
		bootpAnswer.CIAddr[3]=0;
		
		// my IP/server = 10.0.0.20
		bootpAnswer.SIAddr[0]=10;
		bootpAnswer.SIAddr[1]=0;
		bootpAnswer.SIAddr[2]=0;
		bootpAnswer.SIAddr[3]=20;
		
		bootpAnswer.GIAddr[0]=0;
		bootpAnswer.GIAddr[1]=0;
		bootpAnswer.GIAddr[2]=0;
		bootpAnswer.GIAddr[3]=0;

		// clear HW.address and copy from request
		memset(&bootpAnswer.CHAddr,0,16);
		for(i=0;i<bootpMsg.HLen;i++){
			bootpAnswer.CHAddr[i] = bootpMsg.CHAddr[i];
		}

		// copy DHCP Options into vendor field
		memset(&bootpAnswer.Vendor,0,64);
		memcpy(&bootpAnswer.Vendor,&DhcpOps,sizeof(DhcpOps));

#ifdef DEBUG
		dumpBootpMsg(&bootpMsg);
		dumpBootpMsg(&bootpAnswer);
#endif		
		
		// send packet to client address
		char ipbuffer[16] = {0};
		snprintf(ipbuffer,16,"%i.%i.%i.%i",requestedIp[0],requestedIp[1],requestedIp[2],requestedIp[3]);
		client_sock.sin_addr.s_addr = inet_addr(ipbuffer);

		
		int n = sendto(sockfd, &bootpAnswer, sizeof(struct BOOTP), 0, 
	       (struct sockaddr *) &client_sock, client_sock_len);
		
		if(n<0){
			printf("error sending answer\n");
		}
	}
    
	close(sockfd);
}
 
