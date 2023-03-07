#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

void withoutMCopy(int inFile,int outFile, int fileSize){
	ssize_t bytesRead;
	
	char *buffer = (char*) malloc( fileSize * sizeof(char));
	//Reading Input File
	
	bytesRead = read(inFile,buffer, fileSize);

	if(bytesRead == -1){
		perror("Read error: Input\n");
		exit(1);
	}
	
	//Writing to Ouput file
	if(write(outFile,buffer,bytesRead) ==-1){
		perror("Write Error: Output\n");
		exit(1);
	}

	//Freeing Allocated Memory
	free(buffer);
}

void withMCopy(int inFile, int outFile, int fileSize) {

	char *in, *out;
	
	//Mapping in File
	in = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, inFile, 0);
	if(in == MAP_FAILED)  {
		perror("Error in mapping: Input\n");
		exit(1);
	}
	//Changing Output file Size
	ftruncate(outFile, fileSize);
	
	//Mapping Out File
	out = mmap(NULL,fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, outFile,0);
	if(out == MAP_FAILED) {
		perror("Error in mapping: Input\n");
		exit(1);
	}
	//Copying
	
	memcpy(out,in, fileSize);
	
	//Deleting the memory mappings 
	munmap(in, fileSize);
	munmap(out, fileSize);
	

}

int main (int argc, char **argv)
{
	int mflag = 0;
	int index;
	int c;

	opterr = 0;

	while ((c = getopt (argc, argv, "mh:")) != -1)
		switch (c)
		{
		case 'm':
		mflag = 1;
		break;

		case '?':
			printf("Usage: \n copy [-m] <file_name> <new_file_name> \n copy [-h] \n");
		return 1;
		default:
			printf("Usage: \n copy [-m] <file_name> <new_file_name> \n copy [-h] \n");
		return 1;
		}
			
	if( (mflag == 0 && argc != 3) || (mflag == 1 && argc != 4)) {
		printf("Incorrect number of Arguments\n");
		printf("Usage: \n copy [-m] <file_name> <new_file_name> \n copy [-h] \n");
		return 1;
	}
	
	if(mflag == 0) printf("Copy Type: Read & Write \n");
	else printf("Copy Type: mmap & memcpy\n");
	
	
	for (index = optind; index < argc; index++) 
	{
		printf ("File Names - %s\n", argv[index]);
	}
	
	int outFile,inFile;
	

	//Opening Input File
	if((inFile = open(argv[optind],O_RDONLY)) == -1){
		perror("Error In File Opening: Input \n");
		return -1;
	}

	//To get Input File type and Mode as well as size
	//To Get file size to Allocate space. Stored in fileInfo.st_size
	struct stat fileInfo;
	if(fstat(inFile,&fileInfo) == -1){
		return -1;;
	}

	//Opening Output File
	if((outFile = open(argv[optind+1],O_CREAT | O_RDWR, 0666), fileInfo.st_mode) == -1){
		printf("Error In File Opening: Output");
		return -1;
	}
	

	int fileSize = fileInfo.st_size;
	
	if(mflag == 0) withoutMCopy(inFile,outFile, fileSize);
	else withMCopy(inFile, outFile, fileSize);
	
	close(inFile);
	close(outFile);


	return 0 ;
}
