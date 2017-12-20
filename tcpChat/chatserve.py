#/usr/local/bin/python3

# Author: Jacob Karcz
# Date: 05.05.2017
# Course: CS372-400: Intro to Computer Networks
# File Name: chatserve.py
# Description: A simple command line TCP chat application.
#              This is the server program, it connects to a client application at a
#              port number specified by the command line argument.
#              The server application begins running first and gets the user's handle,
#              then sets up and binds the socket and begins listening for clients.
#              It then accepts the client's connection, the 2 exchange handles, and begin
#              to chat until one of them closes the connection by entering "\quit".
#              The server runs until it receives the SIGINT signal (ctrl-c)

# get the necessary libraries
from socket import *
import sys #module w argv
import signal
import fcntl
import string

#register SIGINT signal handler
def signal_handler(signal, frame):
    print(" SiGINT received, exiting chat server application")
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
#signal.pause()

# chat function
# receives: client's socket, client's handle, and the user's handle
# ouputs: chats with the client until the connection is severed on either end
def chat(clientSocket, clientHandle, serverHandle):
    #clientMsg = clientSocket.recv(500) #flush it
    while 1:
        clientMsg = clientSocket.recv(501)
        clientMsg = str(clientMsg).rstrip('\n')
        #print "\{{}\} {}".format(clientHandle, clientMsg)
        if clientMsg == "":
            print("{} has terminated the chat connection. Disconnecting now.".format(clientHandle))
            break;
        print("{0}>> {1}".format(clientHandle, clientMsg))
        serverMsg = raw_input("{0}>> ".format(serverHandle))
        if serverMsg == "\quit":
            print ("You've terminated the chat with {}".format(clientHandle))
            #serverMsg = "Chat closed by {}. Goodbye!".format(serverHandle)
            #clientSocket.send(bytes(serverMsg, 'UTF-8'))
            break
        while serverMsg == "":
            serverMsg = raw_input("{0}>> ".format(serverHandle))
        clientSocket.send(bytes(serverMsg))

# handle (user name) exchange "handshake" function
# input: connection socket, server's handle
# output: cleint's handle
def handleXchange(clientSocket, serverHandle):
    clientHandle = clientSocket.recv(10)
    clientSocket.send(bytes(serverHandle))
    #clientHandle.decode('UTF-8'))
    return str(clientHandle).rstrip('\n')


#check user input and such
numArgs = len(sys.argv)
if len(sys.argv) != 2:
    print ("Usage {} <server port number>\n".format(argv[0]))
    exit(1)

#get server's handle
user = raw_input("please enter a handle (user name): ")
while len(user) == 0 or len(user) > 10:
    user = raw_input("please enter a handle up to 10 characters long: ")

#set up the server socket at specified port
servingPort = sys.argv[1]
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', int(servingPort)))
#serverSocket.bind((socket.gethostname(), serverPort))
serverSocket.listen(1) #listening for 1 connection Max

#print confirmation message, starting server on port servingPort
print("Now serving on port {}".format(sys.argv[1]))
#print("Server at {}".format(serverSocket.getfqdn(IP_ADDRESS)))
#print([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] if not ip.startswith("127.")][:1])
#print("Server at {}".format(serverSocket.getsockname())) #prints addr:0.0.0.0


#wait for clients forever
while 1:
    #connect to clients
    connectionSocket, addr = serverSocket.accept()
    print("Connection received from {}".format(addr))
    #get client's handle
    clientHandle = handleXchange(connectionSocket, user)
    #chat
    chat(connectionSocket, clientHandle, user)
    #close out the client
    connectionSocket.close()
    print("Connection closed with client at socket {}".format(addr))
    print("awaiting new connections...")
