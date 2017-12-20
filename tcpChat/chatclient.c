/*******************************************************************************************
 * Author: 	Jacob Karcz
 * Date: 	5.05.2017
 * Course:	CS372-400: Intro to Computer Networks
 * Program 1: 	chatclient.c
 * Description: A simple command line TCP chat application.
 *              This is the client program, it connects to a server program at an
 *              an address specified by the first (command line) argument and a port number
 *              specified by the second (command line) argument. Then the program connects 
 *              to the server and the two hosts can chat. The client sends the first message.
********************************************************************************************/



/* headers
 ------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>

/* constants
 ------------*/
#define BUFFER_SIZE    500 //socket buffer
#define DEBUG          0

/* booleans!
 ------------*/
enum bool { true, false };
typedef enum bool bool;

 /* Function Declarations
  ----------------------------*/
void error(const char *msg); //fatal error, exit(1)
struct sockaddr_in serverSock_init(char* hostName, char* portNum);
int connectSocket (struct sockaddr_in serverAddress);
int nameXchange(int connectionFD, char* userName, char* serverName);
int chat(int connectionFD, char* clientName, char* serverName);


/*******************************************************************************************
 * Function: 		int main(int argc, char *argv[])
 * Description:		main function of the client, it gets the user's handle, connects to the
 *                  server and chats, then disconnects before exiting.
 * Parameters:		argv[0] is the program's name
 *                  argv[1] is the server's hostName
 *                  argv[2] is the server's port number
 * Pre-Conditions: 	The server must be running at the specifieed socket tupple
 * Post-Conditions: The two hosts have a chat, after which this program terminates.
 ********************************************************************************************/
int main(int argc, char *argv[]) {

	//variabales
    int socketFD;
    
    char userName[10];
    char serverName[10];

    struct sockaddr_in serverAddress;
    
    //check usage
    if (argc < 3) { fprintf(stderr,"USAGE: %s <hostName> <port number>\n", argv[0]); exit(0); }
    if(DEBUG) {printf("chatclient: %d\n connecting...\n", getpid());}
	
    // Set up the server address struct
    serverAddress = serverSock_init(argv[1], argv[2]);

    // Set up the socket & connect tot he server
    socketFD = connectSocket(serverAddress);
    
    //get client handle
    memset(userName, 0, sizeof(userName));
    printf("Please enter a user name (handle) up to 10 characters long: ");
    scanf("%s", userName);

    //exchange handles
    memset(serverName, 0, sizeof(serverName));
    nameXchange(socketFD, userName, serverName);
    
    //chat
    chat(socketFD, userName, serverName);

	//close out
	close(socketFD); // Close the socket
    printf("Connection closed... exiting now.\n");
    
	return 0;
}

/*******************************************************************************************
 * Function: 		struct sockaddr_in serverSock_init(char* hostName, char* portNum)
 * Description:		Function to set up the server socket sockaddr_in struct to enable the 
 *                  connection
 * Parameters:		The same 2 arguments provided to main on the command line,
 *                  hostName is argv[1], the server's host name
 *                  portNum is argv[2], the server's port number
 * Pre-Conditions: 	The server must be running at the specifieed socket tupple
 * Post-Conditions: returns a sockaddr_in struct that is initialized and ready to connect
 ********************************************************************************************/
struct sockaddr_in serverSock_init(char* hostName, char* portNum) {
    int portNumber;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(portNum); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a TCP socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname(hostName); // Convert the machine name into hostent struct
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address to the sockaddr struct
    
    return serverAddress;
}


/*******************************************************************************************
 * Function: 		int connectSocket (struct sockaddr_in serverAddress)
 * Description:		Function to set up the server socket file descriptor & 
 *                  connect the client to the server
 * Parameters:		a sockaddr_in struct initialized with the server's data
 * Pre-Conditions: 	The server must be running at the specifieed socket tupple, 
 *                  the  struct must be properli initialized
 * Post-Conditions: The client and server are connected, or it throws an error and exists
 ********************************************************************************************/
int connectSocket (struct sockaddr_in serverAddress) {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");
    if(DEBUG){ printf("chat_client socket created\n"); }

    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
    error("CLIENT: ERROR connecting");
    if(DEBUG){ printf("chatclient connected\n"); }

    return socketFD;
}


/*******************************************************************************************
 * Function: 		int nameXchange(int connectionFD, char* userName, char* serverHandle)
 * Description:		a function for the server and client applications to exchange handle 
 *                  names for identification throughout the chat. Similar to a TCP handshake.
 * Parameters:		The connection file descriptor, the client's handle, and 
 *                  an empty array to hold the server's handle
 * Pre-Conditions: 	The server must be running at the specifieed socket tupple,
 *                  client's handle must be properly initialized
 * Post-Conditions: The server receives the client's name, and the server's handle is initialized
 ********************************************************************************************/
int nameXchange(int connectionFD, char* userName, char* serverHandle) { 
    int sendC = send(connectionFD, userName, strlen(userName), 0);
    while (sendC < 0) {sendC = send(connectionFD, userName, strlen(userName), 0);}
    int recvS = recv(connectionFD, serverHandle, 10, 0);
    while (recvS < 0) { recvS = recv(connectionFD, serverHandle, 10, 0); } 
    return 0;
}

/*******************************************************************************************
 * Function: 		int chat(int connectionFD, char* clientName, char* serverName)
 * Description:		The chat function, the heart of the application.
 *                  The client and server exchange messages until either the client closes
 *                  the connection by entering "\quit" or the server closes the connection.
 * Parameters:		The connection file descriptor, the client's handle, the server's handle
 * Pre-Conditions: 	The server and client must be connected.
 * Post-Conditions: The hosts chat until one of them closes the connection.
 ********************************************************************************************/
int chat(int connectionFD, char* clientName, char* serverName) {
    int B_sent = -18,
        connectStatus = -18;
    
    char clientMsg[BUFFER_SIZE +2];
    char serverMsg[BUFFER_SIZE +2];
    memset(clientMsg, 0, sizeof(clientMsg));
    memset(serverMsg, 0, sizeof(serverMsg));
    
    fgets(clientMsg, BUFFER_SIZE, stdin);
    memset(clientMsg, 0, sizeof(clientMsg));    
    fflush(stdin);
    while(1) {
        //send cleint's message
        printf("%s>> ", clientName);
        fgets(clientMsg, BUFFER_SIZE + 1, stdin);
        if (strcmp(clientMsg, "\\quit\n") == 0) {
            printf("You've ended the chat connection with %s. Disconecting\n", serverName);
            //send(connectionFD, "This connection has been severed. Goodbye.", 42, 0);
            return 0;
        }
        else if (strlen(clientMsg) < 1 || strcmp(clientMsg, "\n") == 0) {
            memset(clientMsg, 0, sizeof(clientMsg));
            continue; //ignore bad input
        }
        B_sent = send(connectionFD, clientMsg, strlen(clientMsg), 0);
        while (B_sent < 0) {send(connectionFD, clientMsg, strlen(clientMsg), 0);}
            
        //get server's message
        connectStatus = recv(connectionFD, serverMsg, BUFFER_SIZE, 0);
        while (connectStatus < 0) {recv(connectionFD, serverMsg, BUFFER_SIZE, 0);}
        if (connectStatus == 0) {
            printf("Chat connection closed by %s. Goodbye!\n", serverName);
            return 1;
        }
        printf("%s>> %s\n", serverName, serverMsg);
        
        //clear buffers
        memset(clientMsg, 0, sizeof(clientMsg));
        memset(serverMsg, 0, sizeof(serverMsg));
        }
    return -1; //something terrible happened
}



 /* Error Reporting Function
 ************************************************************/
void error(const char *errorMessage) { perror(errorMessage); exit(1); }


