/*********************************************************************
Purpose/Description:

A program that simulates a shell, runs all basic unix commands with 
piping/redirection. 

Author’s Panther ID: 5159067
Certification:
I hereby certify that this work is my own and none of it is the work of
any other person.
********************************************************************/

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> 

#define MAX_ARGS 20
#define BUFSIZ 1024

int get_args(char* cmdline, char* args[]) 
{
  int i = 0;

  /* if no args */
  if((args[0] = strtok(cmdline, "\n\t ")) == NULL) 
    return 0; 

  while((args[++i] = strtok(NULL, "\n\t ")) != NULL) {
    if(i >= MAX_ARGS) {
      printf("Too many arguments!\n");
      exit(1);
    }
  }
  /* the last one is always NULL */
  return i;
}

void execute(char* cmdline) 
{
  int pid, async, oFiles = 0, iFiles = 0, apndFiles = 0, pipes = 0;
  char* args[MAX_ARGS];
  char* outputFiles[MAX_ARGS];
  char* inputFiles[MAX_ARGS];
  char* appendFiles[MAX_ARGS];
  char* pipeCmds[MAX_ARGS][MAX_ARGS];

  int nargs = get_args(cmdline, args);
  if(nargs <= 0) return;

  if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit")) {
    exit(0);
  }

  /* check if async call */
  if(!strcmp(args[nargs-1], "&")) { async = 1; args[--nargs] = 0; }
  else async = 0;

  /*
  Scan command line arguments, execute all commands and
  redirect output to files or other commands. 
  */
  int i, j = 0, k = 0, l = 0;
  // Used for i/o redirection
  int fd_in, fd_out, in = 0, out = 0, apnd = 0, pip = 0;
  char input[256], output[256];
  // Holds start and stop indices of last pipe being iterated through
  int iter = 0;
  int pipeIter = 0;
  int start = 0;
  int pipeSize = 0;
  int rowIter = 0;
  int rowSize = 0;

  // Check for pipes 
  for (i = 0; i < nargs; ++i)
  {		
  	// If the current string is a pipe...
	if(!strcmp(args[i],"|"))
	{	
		pipes += 1;
		// Copy everything to the left of the pipe
		while(iter < i)
		{	
			// Check for redirection
			if(!strcmp(args[iter],"<"))
			{
				inputFiles[0] = args[iter+1];
				// Set flag
				in = 1;
				// Avoid reading "<"
				pipeCmds[rowIter][pipeIter] = NULL;
				iter += 1;
				pipeIter += 1;
			}
			else
			{
				pipeCmds[rowIter][pipeIter] = args[iter];
				iter += 1;
				pipeIter += 1;
			}
		}
		// Copy everything to the right of the pipe
		rowIter += 1;
		pipeIter = 0;
		iter = i + 1;
		while(iter < nargs && strcmp(args[iter],"|") != 0)
		{
			// Check for redirection
			if(!strcmp(args[iter],">"))
			{
				outputFiles[0] = args[iter+1];
				// Set flag
				out = 1;
				// Avoid reading ">"
				pipeCmds[rowIter][pipeIter] = NULL;
				iter += 1;
				pipeIter += 1;
			}
			else if(!strcmp(args[iter],">>"))
			{

				appendFiles[0] = args[iter+1];
				// Set flag
				apnd = 1;
				// Avoid reading ">>"
				pipeCmds[rowIter][pipeIter] = NULL;
				iter += 1;
				pipeIter += 1;
			}
			else
			{
				pipeCmds[rowIter][pipeIter] = args[iter];
				iter += 1;
				pipeIter += 1;
			}
		}
	}
  }
  		
	// Run two commands
	if(pipes == 1)
	{	
		int pfd[2];
		pipe(pfd);
		// For the child process...
		if(fork() == 0)
		{
		  // Set up redirection first
		  if(in)
		  {
	  		// Open input file
			int fd_in = open(inputFiles[0],O_RDONLY);
			if(fd_in < 0)
			{
				perror("Unable to open file!");
				exit(1);
			}
			// Duplicate file descriptor 
			dup2(fd_in,STDIN_FILENO);
			// Close input file, this way stdin is bound to the input file!
			close(fd_in);
		  }
		  // Connect write end of pipe to STDOUT
		  dup2(pfd[1],1);
		  close(pfd[0]);
		  // Run the command
		  execvp(pipeCmds[0][0],pipeCmds[0]);

		}
		if(fork() == 0) 
		{
			if(out)
			{
				
		        /*
		        Use creat instead open to make a file.
				If the file DOES EXIST then it will be 
				overwritten.

				0644 defines file read and write permissions,
				in this case:
				User: Read and write
				Groups: Read
				Other: Read
				*/
				
				int fd_out = creat(outputFiles[0],0644);
				if(fd_out < 0)
				{
					perror("Unable to create file!");
					exit(1);
				}
				// Duplicate file descriptor of stdout to fd_out
				dup2(fd_out, STDOUT_FILENO);
				close(fd_out);
			}
			// Append to a file
			else if(apnd)
			{	
				// Open input file
				int fd_out = open(appendFiles[0],O_WRONLY|O_APPEND);
				// Unable to open file, file does not exist...
				if(fd_out < 0)
				{
					// Create a new file 
					fd_out = creat(appendFiles[k],0644);
					// Check for errors...
					if(fd_out < 0)
					{
						perror("Unable to create file!");
						exit(1);
					}
				}
				// Duplicate file descriptor 
				dup2(fd_out,STDOUT_FILENO);
				close(fd_out);
			}
		  // Connect read end of pipe to STDIN
		  dup2(pfd[0],0);
		  close(pfd[1]);
		  // Run the command
		  execvp(pipeCmds[1][0],pipeCmds[1]);
		}
		// Wait for children to die
		int status;
		close(pfd[0]);
		close(pfd[1]);
		while ((pid = wait(&status)) != -1) {}	
	}
	// Run three commands
	else if(pipes == 2)
	{
		int pfd[4];
		// Sets up first pipe 
		pipe(pfd);
		// Sets up second pipe
		pipe(pfd + 2);

		// For the first pipe...
		if(fork() == 0)
		{
		  // Set up redirection first
		  if(in)
		  {
	  		// Open input file
			int fd_in = open(inputFiles[0],O_RDONLY);
			if(fd_in < 0)
			{
				perror("Unable to open file!");
				exit(1);
			}
			// Duplicate file descriptor 
			dup2(fd_in,STDIN_FILENO);
			// Close input file, this way stdin is bound to the input file!
			close(fd_in);
		  }
		  // Connect write end of pipe to STDOUT
		  dup2(pfd[1],1);
		  // Close all pipe ends not being used
		  close(pfd[0]);
		  close(pfd[1]);
		  close(pfd[2]);
		  close(pfd[3]);
		  // Run the command
		  execvp(pipeCmds[0][0],pipeCmds[0]);
		}
		else
		{
			if(fork() == 0) 
			{

			  // Connect read end of pipe to STDIN
			  dup2(pfd[0],0);
			  // Connect write end of pipe to STDOUT
			  dup2(pfd[3],1);
			  // Close all ends of pipe not being used
			  close(pfd[0]);
			  close(pfd[1]);
			  close(pfd[2]);
			  close(pfd[3]);
			  // Run the command
			  execvp(pipeCmds[1][0],pipeCmds[1]);
			}
			else
			{
				if(fork() == 0)
				{
					if(out)
					{
						
				        /*
				        Use creat instead open to make a file.
						If the file DOES EXIST then it will be 
						overwritten.

						0644 defines file read and write permissions,
						in this case:
						User: Read and write
						Groups: Read
						Other: Read
						*/
						
						int fd_out = creat(outputFiles[0],0644);
						if(fd_out < 0)
						{
							perror("Unable to create file!");
							exit(1);
						}
						// Duplicate file descriptor of stdout to fd_out
						dup2(fd_out, STDOUT_FILENO);
						close(fd_out);
					}
					// Append to a file
					else if(apnd)
					{	
						// Open input file
						int fd_out = open(appendFiles[0],O_WRONLY|O_APPEND);
						// Unable to open file, file does not exist...
						if(fd_out < 0)
						{
							// Create a new file 
							fd_out = creat(appendFiles[k],0644);
							// Check for errors...
							if(fd_out < 0)
							{
								perror("Unable to create file!");
								exit(1);
							}
						}
						// Duplicate file descriptor 
						dup2(fd_out,STDOUT_FILENO);
						close(fd_out);
					}
				  // Connect read end of pipe to STDIN
				  dup2(pfd[2],0);
				  // Close all pipes
				  close(pfd[0]);
				  close(pfd[1]);
				  close(pfd[2]);
				  close(pfd[3]);
				  // Run the command
				  execvp(pipeCmds[2][0],pipeCmds[2]);
				}
			}
		}

		// For the parent process...
		// Close all pipes
		close(pfd[0]);
		close(pfd[1]);
		close(pfd[2]);
		close(pfd[3]);
		// Wait for children to die
		int status;
		while ((pid = wait(&status)) != -1) {}	
	}
	else
	{	

	  j = 0, k = 0, l = 0;		
	  // Check for redirection 
	  for (i = 0; i < nargs; ++i)
	  {
	  	if(!strcmp(args[i],">"))
		{	
			// Check for eof or empty output file
			if(i+1 >= nargs || !strcmp(args[i+1]," "))
			{
				perror("Redirection failed!");
				exit(1);
			}
			else
			{	
				outputFiles[j] = args[i+1];
				// Advance counters
				j += 1;
				oFiles += 1;
				// Set flag
				out = 1;
				// Avoid reading ">"
				args[i] = NULL;
			}
		}
		else if(!strcmp(args[i],"<"))
		{
			// Check for eof or empty input file
			if(i+1 >= nargs || !strcmp(args[i+1]," "))
			{
				perror("Redirection failed!");
				exit(1);
			}
			else
			{	
				inputFiles[k] = args[i+1];
				// Advance counters
				k += 1;
				iFiles += 1;
				// Set flag
				in = 1;
				// Avoid reading "<"
				args[i] = NULL;
			}
		}
		else if(!strcmp(args[i],">>"))
		{	
			// Check for eof or empty output file
			if(i+1 >= nargs || !strcmp(args[i+1]," "))
			{
				perror("Redirection failed!");
				exit(1);
			}
			else
			{	
				appendFiles[l] = args[i+1];
				// Advance counters
				l += 1;
				apndFiles += 1;
				// Set flag
				apnd = 1;
				// Avoid reading ">>"
				args[i] = NULL;
			}
	    }
	  }

	// Execute operations
	pid = fork();
	if(pid < 0)
	{
		perror("Fork failed!");
		exit(1);
	}
	// For the child process 
	else if(pid == 0)
	{	
		i = 0, j = 0, k = 0;
		while(j < oFiles || i < iFiles || k < apndFiles)
		{
			// Get input from an input file
			if(in)
			{	
				// Open input file
				int fd_in = open(inputFiles[i],O_RDONLY);
				if(fd_in < 0)
				{
					perror("Unable to open file!");
					exit(1);
				}
				// Duplicate file descriptor 
				dup2(fd_in,STDIN_FILENO);
				// Close input file, this way stdin is bound to the input file!
				close(fd_in);
				i += 1;
			}
			// Output to a file
			if(out)
			{
				
		        /*
		        Use creat instead open to make a file.
				If the file DOES EXIST then it will be 
				overwritten.

				0644 defines file read and write permissions,
				in this case:
				User: Read and write
				Groups: Read
				Other: Read
				*/
				
				int fd_out = creat(outputFiles[j],0644);
				if(fd_out < 0)
				{
					perror("Unable to create file!");
					exit(1);
				}
				// Duplicate file descriptor of stdout to fd_out
				dup2(fd_out, STDOUT_FILENO);
				close(fd_out);
				j += 1;
			}
			// Append to a file
			if(apnd)
			{	
				// Open input file
				int fd_out = open(appendFiles[k],O_WRONLY|O_APPEND);
				// Unable to open file, file does not exist...
				if(fd_out < 0)
				{
					// Create a new file 
					fd_out = creat(appendFiles[k],0644);
					// Check for errors...
					if(fd_out < 0)
					{
						perror("Unable to create file!");
						exit(1);
					}
				}
				// Duplicate file descriptor 
				dup2(fd_out,STDOUT_FILENO);
				close(fd_out);
				k += 1;
			}
		}
		// Execute command and arguments
		execvp(args[0],args);
		}
		// For the parent process...
		else
		{	
			// Wait for child process to die 
			if(!async) waitpid(pid, NULL, 0);
			// Do not wait for child to die
		    else printf("this is an async call\n");
		}
	}
}
 
int main (int argc, char* argv [])
{
  char cmdline[BUFSIZ];
  
  for(;;) {
    printf("COP4338$ ");
    if(fgets(cmdline, BUFSIZ, stdin) == NULL) {
      perror("fgets failed");
      exit(1);
    }
    execute(cmdline) ;
  }
  return 0;
}
