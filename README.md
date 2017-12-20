# tcp_chat

## To run chat server:
- from the directory containing the source code enter:
   -  ```chatserve.py <portNumber>``` for the regular assignment
   - ```threadChatserve.py <portNumber>``` for the threaded version assignment

## To run chat client:
- from the directory containing the source code:
   - ### to compile:
       - enter ```make```
   - ### to run:
       - enter ```chatclient <server> <port number>```
   - ### to remove executable:
        - enter ```make clean```

## Additional info:
The user name (handle) must be one word (no spaces) up to 10 characters long.
chatClient must send the first message, after this the two will take turns
sending messages to each other. To close the connection from either end,
enter “\quit”. To exit the server application, press ctrl-c (send SIGINT).
Happy chatting! :)

## Sources:
### client:
- Beej’s Guide
- OSU CS344 “Intro to Operating Systems” Lectures by Prof. Brewster
- OSU CS372 “Intro to Networking” Lectures by Prof. Redfield
### server:
- Official Python 3 Documentation
- OSU CS344 “Intro to Operating Systems” Lectures by Prof. Brewster
- OSU CS372 “Intro to Networking” Lectures by Prof. Redfield
- textbook: Computer Networking - A Top-Down Approach 6th Edition
