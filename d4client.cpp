
// All sources
// https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int



#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>



int main(int argc, char const *argv[])
{

    // Handle incorrect number of arguments
    if (argc < 3) {
        std::cout << "Missing arguments, correct usage is: ./client <ip address> <port>" << std::endl; 
        return -1;
    }
    else if (argc > 3){
        std::cout << "Too many arguments, correct usage is: client <ip address> <port>" << std::endl; 
    }

    
    // Here we take the command line arguments and put into variables
    const char* target_ip = argv[1];

    // Adapted from https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int
    std::stringstream ss(argv[2]);
    int target_port;

    if (!(ss >> target_port)) {
        std::cerr << "Invalid number: " << argv[2] << " correct usage is: client <ip address> <port>" << '\n';
        return -1;
    }
    else if (!ss.eof()) {
        std::cerr << "Trailing characters after number: " << argv[2] << " correct usage is: client <ip address> <port>" << '\n';
        return -1;
    }


    // the socket function requires a domain, a type and a protocol
    // here we use domain AF_INET, which corresponds to ipv4 protocols,
    // type is SOCK_STREAM since that appears to be the default for two way connection based byte 
    // streams and the other two options added things I don't think we need
    // lastly the protocol is set to 0 because there typically is only one protocol that supports a
    // given domain/protocol family and type combination
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mySocket < 0){
        perror("Failed to create socket");

        return -1;
    }


    // Here we're making the target address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(target_port);


    // This takes the char* input and makes a 32bti unsigned integer version of it to set into the address struct we're making
    if (inet_pton(AF_INET, target_ip, &server_address.sin_addr) != 1){
        std::cout << "Unable to make ip address" << std::endl;

        return -1;
    }


    // Now we connect using the socket we made to the address we made
    // This connect() function needs a socket, const struct sockaddr *address, and socklen_t address_len
    if (connect(mySocket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        perror("Failed to connect");
        return -1;
    }


    // Here we start message handling to send to the server
    std::cout << "Enter command to send to the server: " << std::endl;
    std::string message;
    std::cin >> message;


    // I decided to make adding SYS optional on the client side for convenience, this just adds SYS to the input string if 
    // the user did not do so already. This should probably be removed later if the client and server will support doing something
    // other than just send SYS commands and receive results
    if ( message.substr(0,3) != "SYS"){
        message = "SYS " + message;
    }


    // Here we try sending the message and handle things that could have gone wrong
    int numberOfBytesSent = send(mySocket, message.c_str(), message.length(), 0); // last input are flags, which I don't think are necessary here

    if (numberOfBytesSent == -1){
        perror("Failed to send message");
    }
    else if (numberOfBytesSent < message.length()){
        std::cout << "Did not send the entire message for some reason" << std::endl;
    }


//  Receive somthing back
/*

    char buffer[1024];

    int numberOfBytesReceived = recv(mySocket, buffer, sizeof(buffer), 0);


    if (numberOfBytesReceived < 0){
        std::cout << "Failed to receive message" << std::endl;
        return -1;
    }
    else if (numberOfBytesReceived == 0){
        std::cout << "Failed to receive message because the server closed the connection" << std::endl;
        return -1;
    }

    std::cout << "Received from server: " << '\n' << buffer << std::endl;;

 */

    if (close(mySocket) <0){
        perror("Failed to close socket");
        return -1;
    }

    // message.c_str() returns a pointer to the array of characters that compose the message string

    return 0;
}

// close socket



// Use Pull and not select to make a set of sockets

// select blocks on all of the sockets at once instead of just one


// We could use read() and write() but we should use send() and receive() instead







//  I need to add receiving something back from the server and printing it out, clean up so that nothing is hardcoded and the arguments are properly checked 