#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <mcheck.h>

#include "parser.h"
#include "shell.h"

/**
 * Program that simulates a simple shell.
 * The shell covers basic commands, including builtin commands 
 * (cd and exit only), standard I/O redirection and piping (|). 
 
 */

#define MAX_DIRNAME 100
#define MAX_COMMAND 1024
#define MAX_TOKEN 128

/* Functions to implement, see below after main */
int execute_cd(char** words);
int execute_nonbuiltin(simple_command *s);
int execute_simple_command(simple_command *cmd);
int execute_complex_command(command *cmd);


int main(int argc, char** argv) {
	
	char cwd[MAX_DIRNAME];           /* Current working directory */
	char command_line[MAX_COMMAND];  /* The command */
	char *tokens[MAX_TOKEN];         /* Command tokens (program name, 
					  * parameters, pipe, etc.) */

	while (1) {

		/* Display prompt */		
		getcwd(cwd, MAX_DIRNAME-1);
		printf("%s> ", cwd);
		
		/* Read the command line */
		fgets(command_line, MAX_COMMAND, stdin);
		/* Strip the new line character */
		if (command_line[strlen(command_line) - 1] == '\n') {
			command_line[strlen(command_line) - 1] = '\0';
		}
		
		/* Parse the command into tokens */
		parse_line(command_line, tokens);

		/* Check for empty command */
		if (!(*tokens)) {
			continue;
		}
		
		/* Construct chain of commands, if multiple commands */
		command *cmd = construct_command(tokens);
		//print_command(cmd, 0);
    
		int exitcode = 0;
		if (cmd->scmd) {
			exitcode = execute_simple_command(cmd->scmd);
			if (exitcode == -1) {
				break;
			}
		}
		else {
			exitcode = execute_complex_command(cmd);
			if (exitcode == -1) {
				break;
			}
		}
		release_command(cmd);
	}
    
	return 0;
}


/**
 * Changes directory to a path specified in the words argument;
 * For example: words[0] = "cd"
 *              words[1] = "csc209/assignment3/"
 * Your command should handle both relative paths to the current 
 * working directory, and absolute paths relative to root,
 * e.g., relative path:  cd csc209/assignment3/
 *       absolute path:  cd /u/bogdan/csc209/assignment3/
 */
int execute_cd(char** words) {
	
	/** 
	 * TODO: 
	 * The first word contains the "cd" string, the second one contains 
	 * the path.
	 * Check possible errors:
	 * - The words pointer could be NULL, the first string or the second 
	 *   string could be NULL, or the first string is not a cd command
	 * - If so, return an EXIT_FAILURE status to indicate something is 
	 *   wrong.
	 */

	//checks whether words is NULL,or the first string in words is NULL
	if(words == NULL || words[0] == NULL){
	
		return EXIT_FAILURE;
	}

	//checks whether the second string in words is NULL
	if(words[1] == NULL){

		printf("Please enter a directory\n");
	}


	/**
	 * TODO: 
	 * The safest way would be to first determine if the path is relative 
	 * or absolute (see is_relative function provided).
	 * - If it's not relative, then simply change the directory to the path 
	 * specified in the second word in the array.
	 * - If it's relative, then make sure to get the current working 
	 * directory, append the path in the second word to the current working
	 * directory and change the directory to this path.
	 * Hints: see chdir and getcwd man pages.
	 * Return the success/error code obtained when changing the directory.
	 */

	//compares first word with cd
	if(strcmp(words[0], "cd") == 0){
	 //change directory to path when path is not relative
	 if(is_relative(words[1]) == 1){

	 	if(chdir(words[1]) == 0){
	 		return 0;
	 	} else {
	 		return 1;
	 	}	

	 //change directory to path when path is relative	
	 } else if(is_relative(words[1]) == 0){

		//variable to store the absolute_path
		char absolute_path[MAX_DIRNAME];

		//variable to store the current working directory
	 	char current_working_dir[MAX_DIRNAME];

	 	//gets the current working directory and removes the first '/' from it
	 	getcwd(current_working_dir, MAX_DIRNAME-1);
	 	strncat(current_working_dir, "/", 1); 

		//determines the length of the relative path and the current working directory
	 	int length_of_path = strlen(words[1]);
	 	int length_of_cwd = strlen(current_working_dir);

		//find the difference in length
	 	int length_diff = length_of_path - length_of_cwd;

	 	//copies the relative part of the absolute path into absolute_path
	 	strncpy(absolute_path, words[1]+length_of_cwd, length_diff+1);

	 	//changes directory to absolute_path
	 	if(chdir(absolute_path) == 0){
	 		
	 		return 0;
	 	} else {
	 		return 1;
	 	}
	 }		
	}

	return -1;
}

/**
 * Executes a program, based on the tokens provided as 
 * an argument.
 * For example, "ls -l" is represented in the tokens array by 
 * 2 strings "ls" and "-l", followed by a NULL token.
 * The command "ls -l | wc -l" will contain 5 tokens, 
 * followed by a NULL token. 
 */
int execute_command(char **tokens) {
	
	/**
	 * TODO: execute a program, based on the tokens provided.
	 * The first token is the command name, the rest are the arguments 
	 * for the command. 
	 * Hint: see execlp/execvp man pages.
	 * 
	 * - In case of error, make sure to use "perror" to indicate the name
	 *   of the command that failed.
	 *   You do NOT have to print an identical error message to what would 
	 *   happen in bash.
	 *   If you use perror, an output like: 
	 *      my_silly_command: No such file of directory 
	 *   would suffice.
	 * Function returns only in case of a failure (EXIT_FAILURE).
	 */

	//executes command in tokens[0]
	if(execvp(tokens[0], tokens) == -1) {
		perror("No such file or directory");

	 	return EXIT_FAILURE;
	}
	
}
/**
 * Executes a non-builtin command.
 */
int execute_nonbuiltin(simple_command *s) {
	/**
	 * TODO: Check if the in, out, and err fields are set (not NULL),
	 * and, IN EACH CASE:
	 * - Open a new file descriptor (make sure you have the correct flags,
	 *   and permissions);
	 * - redirect stdin/stdout/stderr to the corresponding file.
	 *   (hint: see dup2 man pages).
	 * - close the newly opened file descriptor in the parent as well. 
	 *   (Avoid leaving the file descriptor open across an exec!) 
	 * - finally, execute the command using the tokens (see execute_command
	 *   function above).
	 * This function returns only if the execution of the program fails.
	 */

	 //checks whether s->in, s->out, or s->err are set
	 if(s->in != NULL || s->out != NULL || s->err != NULL){
		int fd[2];
		//opens fd[0] to be read when s->in is set
		if(s->in != NULL){

			if((fd[0] = open(s->in, O_RDONLY)) == -1){
				perror("open");
				return -1;
				exit(1);
			}
			//redirects fd[0] to standard input
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			
		}
		//opens fd[1] to be written to when s->out is set
		if(s->out != NULL){
			
			if((fd[1] = open(s->out, O_WRONLY | O_CREAT, 0666)) == -1){
				perror("open");
				return -1;
				exit(1);
			}
			//redirects fd[1] to standard output
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
		}
		//opens fd[2] to be written to when s->err is set
		if(s->err != NULL){
			 
			if((fd[2] = open(s->out, O_WRONLY | O_CREAT, 0666)) == -1){
				perror("open");
				return -1;
				exit(1);
			}
			//redires fd[2] to standard error
			dup2(fd[2], STDERR_FILENO);
			close(fd[2]);
		}

		//calls execute_command on the tokens in the simple command s, while also taking into consideration any file redirection
		if(execute_command(s->tokens) == EXIT_FAILURE){
				perror("ERROR executing command");
				return -1;
				exit(1);
		}
	} else {

		//calls execute_command on the tokens in the simple command s
		if(execute_command(s->tokens) == EXIT_FAILURE){
				perror("ERROR executing command");
				return -1;
				exit(1);
		}
	}
	
}
/**
 * Executes a simple command (no pipes).
 */
int execute_simple_command(simple_command *cmd) {

	/**
	 * TODO: 
	 * Check if the command is builtin.
	 * 1. If it is, then handle BUILTIN_CD (see execute_cd function provided) 
	 *    and BUILTIN_EXIT (simply exit with an appropriate exit status).
	 * 2. If it isn't, then you must execute the non-builtin command. 
	 * - Fork a process to execute the nonbuiltin command 
	 *   (see execute_nonbuiltin function above).
	 * - The parent should wait for the child.
	 *   (see wait man pages).
	 */

	 //checks whether the command is builtin
	 if((cmd->builtin == BUILTIN_CD) || (cmd->builtin == BUILTIN_EXIT)){

	 	//if the command is builtin execute cd, or exit depending on the command
	 	if(cmd->builtin == BUILTIN_CD){

	 		if(execute_cd(cmd->tokens) == -1){
	 			perror("execute_cd");
	 			return -1;
	 			exit(1);
	 		}

	 	} else if(cmd->builtin == BUILTIN_EXIT){

			exit(0);
		
	 	} else {

			return -1;
	 		exit(1);

	 	}
	 //if the command is not builtin
	 } else if(cmd->builtin == 0) {
		//int fd[2];
	 	int r;

	 	//error checking for fork
	 	if((r = fork()) == -1){
	 		perror("fork");
	 		return -1;
	 		exit(1);
	 	}

		//child process, executes non builtin commands
	 	if(r == 0){
	 		//close(fd[1]);
	 		if(execute_nonbuiltin(cmd) == -1){
	 			return -1;
	 			exit(1);
	 		}
	 	}
		//parent process, waits for child process to terminate
	 	if(r > 0) {

	 		int status;
	 		if(wait(&status) != -1){
	 			if(WIFEXITED(status)){
	 				if(WEXITSTATUS(status == 1)){
	 					printf("Error\n");
	 					return -1;
	 				}
	 			}
	 		}			
	 	}
	 }

	 return 0;
}


/**
 * Executes a complex command.  A complex command is two commands chained 
 * together with a pipe operator.
 */
int execute_complex_command(command *c) {
	
	/**
	 * TODO:
	 * Check if this is a simple command, using the scmd field.
	 * Remember that this will be called recursively, so when you encounter
	 * a simple command you should act accordingly.
	 * Execute nonbuiltin commands only. If it's exit or cd, you should not 
	 * execute these in a piped context, so simply ignore builtin commands. 
	 */

	 //checks whether a command is a simple command
	 if(c->scmd != NULL){
	 	//checks whether the command is builtin (cd, exit) or not builitin
	 	if(is_builtin(c->scmd->tokens[0]) == 0){

			//if it is not builtin executes the command
			if(execute_nonbuiltin(c->scmd) == -1){
				return -1;
				exit(1);
			}
				

	 	//otherwise exit	
	 	} else {
	 		return -1;
	 		exit(1);
	 	}
	 }
	 	
	/** 
	 * Optional: if you wish to handle more than just the 
	 * pipe operator '|' (the '&&', ';' etc. operators), then 
	 * you can add more options here. 
	 */

	if (!strcmp(c->oper, "|")) {
		
		/**
		 * TODO: Create a pipe "pfd" that generates a pair of file 
		 * descriptors, to be used for communication between the 
		 * parent and the child. Make sure to check any errors in 
		 * creating the pipe.
		 */
		int fd[2];

		//variables used in fork
		int r, s;
		if(pipe(fd) == -1){
			perror("pipe");
			return -1;
			exit(1);
		}
			
		/**
		 * TODO: Fork a new process.
		 * In the child:
		 *  - close one end of the pipe pfd and close the stdout 
		 * file descriptor.
		 *  - connect the stdout to the other end of the pipe (the 
		 * one you didn't close).
		 *  - execute complex command cmd1 recursively. 
		 * In the parent: 
		 *  - fork a new process to execute cmd2 recursively.
		 *  - In child 2:
		 *     - close one end of the pipe pfd (the other one than 
		 *       the first child), and close the standard input file 
		 *       descriptor.
		 *     - connect the stdin to the other end of the pipe (the 
		 *       one you didn't close).
		 *     - execute complex command cmd2 recursively. 
		 *  - In the parent:
		 *     - close both ends of the pipe. 
		 *     - wait for both children to finish.
		 */

		 //error checking for fork
		 if((r = fork()) == -1){
		 	perror("fork");
		 	return -1;
		 	exit(1);
		 }

		 //first child process, closes the read pipe, redirects write pipe to standard output
		 if(r == 0){
		 	close(fd[0]);
		 	if(dup2(fd[1], STDOUT_FILENO) == -1){
		 		perror("dup2");
		 		return -1;
		 		exit(1);
		 	}

			//handles execution of multiple complex commands using recursion
		 	if(execute_complex_command(c->cmd1) == -1){
		 		perror("execute_complex_command");
		 		return -1;
		 		exit(1);
		 	}
		 }
		 //first parent process, forks a new process
		 if(r > 0){
		 int status;
		 	if((s = fork()) == -1){	
				perror("fork");
				return -1;
				exit(1);
		 	}
			//child process, closes the write pipe, redirects read pipe to standard input 
		 	if(s == 0){
		 		close(fd[1]);
		 		if(dup2(fd[0], STDIN_FILENO) == -1) {
		 			perror("dup2");
		 			return -1;
		 			exit(1);
		 		}

		 		//handles execution of multiple complex commands using recursion
		 		if(execute_complex_command(c->cmd2) == -1){
		 			perror("execute_complex_command");
		 			return -1;
		 			exit(1);
		 		}
		 	}

			//second parent process, closes both file descriptors, waits for child to exit
		 	if(s > 0){
		 		int status;
		 		close(fd[0]);
		 		close(fd[1]);
		 		if(wait(&status) == -1){
		 			return -1;
					exit(1);	
		 		}
		 	}

		 	//wait call for first parent process
			if(wait(&status) == -1){
				return -1;
				exit(1);
			}
		 }	 			
	}
	return 0;
}
