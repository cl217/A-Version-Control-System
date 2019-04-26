To Test
	Move WTFserver executable to server folder

IP (localhost): 127.0.0.1

Port: Any between 8000-65535
	

sending data structure
	char*: 
		-delimeter to split tokens is '\t'
		-first token will always be the command
		-2nd token (if any) will be type of data:
			-Project: sending project name only (create, etc)
			-ProjectFile: sending projectName with fileName, no content (remove,destroy,etc)
			-ProjectFileContent: sending projectName with fileNames and content 
		-3rd token 
			-if files are sent- num of files being sent
		
		-For ProjectFileContent
			<command><dataType><bytesPname><projectName>
				<numFile><bytesfName><fName><bytefContent><fContent>...
				

splitdata() is used to parse char* data into struct node*
		struct node{
			char* nodeType; //command, dataType, project, numFile, fileNode
			char* name; //also used for numFile
			char* content;	
			struct node* next;
		};

	For ProjectFileContent
		<NODE 1>
			nodeType = "command"
			name = (command)
			content = NULL
		<NODE 2>
			nodeType = "dataType"
			name = (dataType- Project, ProjectFile, ProjectFileContent)
			content = NULL
		<Node 3> 
			nodeType = project
			name = (projectpath)
			content = NULL
		<Node 4>
			nodeType = numFile
			name = (num of files)
			content = NULL
		<Node 5>
			nodeType = fileNode	
			name = (filepath)
			content = (content of files
		<Node ...>
			nodeType = fileNode	
			name = (filepath)
			content = (content of files)
	
	Note: can prob remove "nodeType" and numFile node and condense first 3 nodes into 1
	
			



