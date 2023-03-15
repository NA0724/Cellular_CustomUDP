# Cellular_CustomUDP-Client using customized protocol on top of UDP protocol for requesting identification from server for access permission to the cellular network.
Pre-requisite:
Install gcc for C program compiler

How to compile and run in Linux:

1. Copy the files 'client2.c', 'server2.c', 'input.txt' and 'Verification_Database.txt' to the desired location.
2. Run the below commands to compile the C programs:
	gcc Server2.c -o serverExe
	gcc Client2.c -o clientExe
3. First the server should be started. To start the server, run:
	./serverExe
4. In a new terminal window, run below for running the client program:
	./clientExe
5. Packets would start transmitting and output would be visible
