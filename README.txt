WTF: Where's the File (Systems Programming Project 3)

A version control system similar to Git.

Design:
	• Client and Server
		- The server maintains a repository of projects.
		- Server also maintains all previous versions of all projects in compressed .gz files.
		- The client fetch updates to and from the server's repository.
		- The data sent between client and server is compressed using zlib
		
	• Synchronization
		- The server is a multithreaded program capable of any number of client connections at a time.
		- Mutexes are used to insure data consistency.

Usage:
	• Makefile
		- make all: builds two executables
			- WTFserver (server)
			- WTF (client)
	
	• Server
		- Start up server
			./WTFserver <port> 
	
	• Client

		- Configure: Saves the IP address/hostname and port of the server for use by later commands that connects to the server
			./WTF configure <IP/hostname> <port>
			
		- Create: Initializes a project repository with the given name on the server
			./WTF create <projectname>
			
		- Checkout: Fetches the latest version of the project.
			./WTF checkout <projectname>
	
		- Update: Fetches and outputs all differences between server and client
			./WTF update <projectname>
	
		- Upgrade: Brings client's project up to date with server's
			./WTF upgrade <projectname>
	
		- Commit: Makes note of all changes to be updated to the server
			./WTF commit <projectname>
	
		- Push: Applies changes from the commit command to the server
			./WTF push <projectname>
			
		- Add: Adds a file to be kept track of by the repository
			./WTF add <projectname> <filename>
		
		- Remove: Removes a file to be kept track of by the repository
			./WTF remove <projectname> <filename>
			
		- Destroy: Deletes the project repository on the server
			./WTF destroy <projectname>
			
		- Currentversion: Outputs the current version number of the project
			./WTF currentversion <projectname>
			
		- History: Outputs all changes to repository since the project's creation
			./WTF history <projectname>
			
		- Rollback: Reverts the server's repository to a previous version
			./WTF rollback <projectname> <version number>
