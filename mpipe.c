/*
    COMP3511 Spring 2022 
    PA1: Simplified Linux Shell (Multi-level Pipe)

    Your name: Tsui Ho Tin
    Your ITSC email:httsuiac@connect.ust.hk 

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks. 

*/

// Note: Necessary header files are included
// Do not add extra header files
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
// Assume that each command line has at most 256 characters (including NULL)
#define MAX_CMDLINE_LEN 256

// Assume that we have at most 8 pipe segments
#define MAX_PIPE_SEGMENTS 8

// Assume that we have at most 8 arguments for each segment
// We also need to add an extra NULL item to be used in execvp
// Thus: 8 + 1 = 9
//
// Example: 
//   echo a1 a2 a3 a4 a5 a6 a7 
//
// The execvp system call needs to store an extra NULL to represent the end of the parameter list
//
//   char *arguments[MAX_ARGUMENTS_PER_SEGMENT]; 
//
// For the above example, 9 strings are stored: echo a1 a2 a3 a4 a5 a6 a7 NULL
//
#define MAX_ARGUMENTS_PER_SEGMENT 9

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// The pipe character
#define PIPE_CHAR "|"

// Define the  Standard file descriptors here
#define STDIN_FILENO    0       // Standard input
#define STDOUT_FILENO   1       // Standard output 
#define STDERR_FILENO   2       // Standard error output 

// Helper: parse_tokens function is given
// This function helps you parse the command line
//
// Suppose the following variables are defined:
//
// char *pipe_segments[MAX_PIPE_SEGMENTS]; // character array buffer to store the pipe segements
// int num_pipe_segments; // an output integer to store the number of pipe segment parsed by this function
// cmdline // the input argument of the process_cmd function
//
// Sample usage:
//
//  parse_tokens(pipe_segments, cmdline, &num_pipe_segments, PIPE_CHAR);
//
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}



// The function prototype of process_cmd, to be implemented below
void process_cmd(char *cmdline);

/* The main function implementation */
int main()
{
    char cmdline[MAX_CMDLINE_LEN];
    fgets(cmdline, MAX_CMDLINE_LEN, stdin);
    process_cmd(cmdline);
    return 0;
}

// TODO: implementation of process_cmd
void process_cmd(char *cmdline)
{
  //printf("%s\n", cmdline);
  char *cmd[MAX_PIPE_SEGMENTS];
  int cmd_num;
  parse_tokens(cmd,cmdline,&cmd_num, PIPE_CHAR);

if (cmd_num == 1) {
        char* arg[MAX_ARGUMENTS_PER_SEGMENT];
	char* temp[MAX_ARGUMENTS_PER_SEGMENT];
        int numStrings;
        //printf("%s\n", cmd[0]);
        parse_tokens(arg, cmd[0], &numStrings, SPACE_CHARS);
	//printf("%s\n", arg[1]);
        for(int j=0;j<numStrings;++j){
		temp[j]=arg[j];
		temp[j+1]=NULL;
	}
		
        execvp(temp[0], temp);
}


    int i, in, out = dup(1);   
    for (i = 0; i < cmd_num; i++)
    {
        int ps[2];
        if (i < cmd_num-1) pipe(ps);  
        pid_t pid = fork();
        if (pid == 0)
        {
  
            if (i) {
		dup2(in, 0);
		close(in);
	    }

            if (i < cmd_num-1){
		 dup2(ps[1], 1);
		 close(ps[1]);
	    }
            else {
		dup2(out, 1);
		close(out);
	    }
            char* arg[MAX_ARGUMENTS_PER_SEGMENT];
	    char* temp[MAX_ARGUMENTS_PER_SEGMENT];
            int numStrings;
            parse_tokens(arg, cmd[i], &numStrings, SPACE_CHARS);
            for(int j=0;j<numStrings;++j){
		    temp[j]=arg[j];
		    temp[j+1]=NULL;
	    }
		
            execvp(temp[0], temp);
        }
        if (i) close(in);
        close(ps[1]);
        in = ps[0]; // the current pipe's read end is the new input
    }
    close(out);
    wait(0);
    


   
}
