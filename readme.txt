
Introduction

    This was developed on MacOS, and should also work on linux, but not on windows systems as
    the source code would likely have to be adapted somewhat considerably to run on windows.

    To compile, simply run:
        make all

    This should compile 4 different files: server, d3client, d4client, d4server

    Note that 'server' is compiled from the original source provided for the assignment.

    'd3client' is made to connect to 'server', and 'd4client' is made to connect to 'd4server'.

    The 'images' directory contains the requested screenshots, organised into subdirectories for each question.


How to use TLDR:

    'server' and 'd3client' can be run in two terminals using
        ./server <port>
    and
        ./d3client <ip address> <port>
    this will only take one command with no flags or arguments in the client and print the output server side.

    'd4server' and 'd4client' can be run in two terminals using
        ./d4server <port>
    and
        ./d4client <ip address> <port>
    this will run a loop of accepting a command, optionally with flags and arguments, in the client, sending
    it to the server, the server runs it and sends the output to the client where it is printed,
    at which point the client will prompt you for another command. Typing 'quit' or 'q' will exit the loop.

    Both servers expect a 'SYS ' prefix to all commands, but both clients take care of this for you.



Longer version:


--------D3--------

    To run the first server client pair, open the project directory in two terminals. In one, run:
        ./server 4022
    where the input argument is a port number, note that you can choose any free port number, and not necessarily the one in this example.
    The server should be running and it will print out:
        Listening on port: 4022

    At this point go to the other terminal and run:
        ./d3client 127.0.0.1 4022
    again some caveats, the client reads an ip address and port number from the input arguments, the ip address should be
    the one corresponding to where your server is running, which in this example case is the same machine as the client will be,
    so this will run over loopback and we use localhost. The port number needs to be the same as the one used to run the server.

    If successful, the client will prompt you with:
        Enter command to send to the server: 

    and the server will also have printed:
        Client connected on server

    You can now enter some command on the client terminal, such as ls or who, and it will be sent over to the server.
    Note that the server expects commands to have SYS as a prefix, such as SYS ls or SYS who, but the client is written to
    take care of that for you, if you don't enter SYS then it will add it, if you do then it will not add it.
    The server will read the command, execute it if it is valid and print the output. On successful send, the client will
    simply exit with no further prompts, and the server will print:
        Client closed connection



--------D4--------

    The instructions for this section are in some ways identical to the D3 section, but with some occasional differences.

    To run the second client pair, open the project directory in two terminals. In one, run:
        ./d4server 4022
    where the input argument is a port number, note that you can choose any free port number, and not necessarily the
    one in this example.
    The server should be running and it will print out:
        Listening on port: 4022

    Note that 'd4server' is a modified 'server', almost entirely the same, but includes additional logic to support flags
    and arguments to the received commands, and also sends back the output of the commands to the client.

    At this point go to the other terminal and run:
        ./d4client 127.0.0.1 4022
    again some caveats, the client reads an ip address and port number from the input arguments, the ip address should be
    the one corresponding to where your server is running, which in this example case is the same machine as the client will be,
    so this will run over loopback and we use localhost. The port number needs to be the same as the one used to run the server.

    If successful, the client will prompt you with:
        Enter command to send to the server: 

    and the server will also have printed:
        Client connected on server

    You can now enter some command on the client terminal, such as ls or who, optionally along with some arguments and flags
    and it will be sent over to the server. Note that the server expects commands to have SYS as a prefix, such as
    SYS ls or SYS who, but the client is written to take care of that for you, if you don't enter SYS then it will add it,
    if you do then it will not add it.
    Once the server receives the command, it will execute it and send the output of the command back to the client. The client
    will print the output and then prompt you for another command. This will keep happening on a loop, and it will not exit
    unless it runs into a problematic error with the socket API, or if you, the user, enter 'quit' or 'q' when prompted for
    a command.


