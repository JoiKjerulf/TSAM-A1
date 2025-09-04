
// All sources
// https://stackoverflow.com/questions/2797813/how-to-convert-a-command-line-argument-to-int
// Live coding practice lecture from Stephan Schiffel



// Server:
// https://stackoverflow.com/questions/8538324/what-is-the-difference-between-popen-and-system-in-c
// https://stackoverflow.com/questions/44610978/popen-writes-output-of-command-executed-to-cout



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
        return -1;
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


    // Here we start message handling to send to the server. I use std::getLine to get the entire line entered by 
    // the user since later we will add support for options and flags. This also allows the user to enter
    // either SYS <command> or just <command>



    std::cout << "Enter command to send to the server: " << std::endl;
    std::string message;
    std::getline(std::cin, message);

    while(message != "quit"){


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
            return -1;
        }
        else if (numberOfBytesSent < message.length()){
            std::cout << "Did not send the entire message for some reason" << std::endl;
        }


        //  Receive something back
        // The server is configured to send outputs from command with a preceeding header line in the format "SIZE <n>\n"

        // Read the header line character by character until we hit a newline
        std::string header;
        char single_char;
        while (true) {
            if (recv(mySocket, &single_char, 1, 0) <= 0) {
                std::cerr << "Failed to read header - connection closed\n";
                return -1;
            }
            
            if (single_char == '\n')
                break;
                
            header += single_char;
        }

        // Parse the SIZE value from the header. This sscanf pattern matching if statement was generated with an AI tool and then checked for correctness
        // According to man sscanf, %zu means there's a variable to extract, it's of type size_t, and it's an optionally signed decimal integer
        // which is what we want as we need to extract the number of bytes intended to be sent.
        size_t bytes_to_read = 0;
        if (sscanf(header.c_str(), "SIZE %zu", &bytes_to_read) != 1) {
            std::cerr << "Invalid header format: " << header << "\n";
            return -1;
        }

        if(bytes_to_read >= 1){
            std::cout << "------- Received from server: --------" << std::endl;
        }

        // now read exactly bytes_to_read and stream to stdout
        char buffer[1024]; 
        size_t totalReceivedBytes = 0;
        while (totalReceivedBytes < bytes_to_read) {

            int numberOfBytesReceived = recv(mySocket, buffer, sizeof(buffer), 0);
            if (numberOfBytesReceived < 0){
                std::cout << "Failed to receive message" << std::endl;
                return -1;
            }
            else if (numberOfBytesReceived == 0){
                std::cout << "Failed to receive message because the server closed the connection" << std::endl;
                return -1;
            }
            else{
                totalReceivedBytes += numberOfBytesReceived;
                std::cout.write(buffer, numberOfBytesReceived); // Doing this with .write() prevents duplicate buffer printing
            }
        }

        std::cout << '\n' << "--------------------------------------" << std::endl;

        // Start new loop of receiving command, sending it and receiving something back
        std::cout << "Enter command to send to the server: " << std::endl;
        std::getline(std::cin, message);

    }
 
    // Last thing to do is to explicitly close the connection after we're done
    if (close(mySocket) <0){
        perror("Failed to close socket");
        return -1;
    }


    return 0;
}

