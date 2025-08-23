/********************************************************************************************
This is a template for assignment on writing a custom Shell. 

Students may change the return types and arguments of the functions given in this template,
but do not change the names of these functions.

Though use of any extra functions is not recommended, students may use new functions if they need to, 
but that should not make code unnecessorily complex to read.

Students should keep names of declared variable (and any new functions) self explanatory,
and add proper comments for every logical step.

Students need to be careful while forking a new process (no unnecessory process creations) 
or while inserting the single handler code (should be added at the correct places).

Finally, keep your filename as myshell.c, do not change this name (not even myshell.cpp, 
as you not need to use any features for this assignment that are supported by C++ but not by C).
*********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>			// exit()
#include <unistd.h>			// fork(), getpid(), exec()
#include <sys/wait.h>		// wait()
#include <signal.h>			// signal()
#include <fcntl.h>			// close(), open()

/* extra functions being used and their dependencies:
	getcwd() : unistd.h
*/
typedef enum{PAR, SEQ, RED, SINGLE} cmd_t;
typedef enum{false, true} bool;

char **cmds = NULL;
int cmd_count = 0;
void parseInput(char *line, cmd_t *cmd_type)
{

	cmds = NULL;
	cmd_count = 0;
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).

	if( strstr(line, "&&") != NULL ) {
		*cmd_type = PAR;
		/* separate into mutliple commands string */
		char *token = strsep(&line, "&&");
		while( token != NULL ) {
			/* append to cmds */
			cmd_count = cmd_count + 1;
			cmds = realloc(cmds, sizeof(char*) * cmd_count);
			cmds[cmd_count-1] = token;

			token = strsep(&line, "&&");
		}
	}
	else if( strstr(line, "##") != NULL ) {
		*cmd_type = SEQ;
		/* separate into mutliple commands string */
		char *token = strsep(&line, "##");
		while( token != NULL ) {
			/* append to cmds */
			cmd_count = cmd_count + 1;
			cmds = realloc(cmds, sizeof(char*) * cmd_count);
			cmds[cmd_count-1] = token;
			token = strsep(&line, "##");
		}
	}
	else if( strstr(line, ">") != NULL ) {
		*cmd_type = RED;
		/* separate into mutliple commands string */
		char *token = strsep(&line, ">");
		while( token != NULL ) {
			/* append to cmds */
			cmd_count = cmd_count + 1;
			cmds = realloc(cmds, sizeof(char*) * cmd_count);
			cmds[cmd_count-1] = token;
			token = strsep(&line, ">");
		}
	}
	else {
		*cmd_type = SINGLE;
		cmds = realloc(cmds, sizeof(char*));
		cmds[0] = line;
		cmd_count = 1;

	}
}
void tokenize(char *cmd, char **args, int *arg_count)
{
	/* parse command into program and arguments */
	*arg_count = 0;
	char *token = strsep(&cmd, " \n");
	while( token != NULL ) {
		args[*arg_count] = token;
		*arg_count = *arg_count + 1;
		token = strsep(&cmd, " \n");
	}

	if( *arg_count > 0 && strlen(args[*arg_count-1]) == 0 ) *arg_count = *arg_count - 1; // remove empty argument if any

	args[*arg_count] = NULL;	// execvp() needs NULL at the end of args array
}

void executeCommand(bool *exit_flag)
{
	// This function will fork a new process to execute a command
	if( cmd_count != 1 ) {
		printf("Error: executeCommand() should be called for single command only.\n");
		return;
	}
	char *cmd = cmds[0];
	/* parse command into program and arguments */
	char *args[100];
	int arg_count = 0;
	tokenize(cmd, args, &arg_count);

	/* check for exit command */
	if( strcmp(args[0], "exit") == 0 ) {
		*exit_flag = true;
		return;
	}
	/* else */

	/* handle cd command using chdir() */
	if( strcmp(args[0], "cd") == 0 ) {
		if( arg_count < 2 ) {
			/* navigate to home directory */
			chdir(getenv("HOME"));
		}
		else {
			if( chdir(args[1]) != 0 ) {
				printf("Shell: Incorrect command\n");
			}
		}
		return;
	}

	/* else fork a new process to execute the command */
	pid_t pid = fork();
	if( pid < 0 ) {
		printf("Fork failed.\n");
		return;
	}
	else if( pid == 0 ) {
		/* child process */
		signal(SIGINT, SIG_DFL);	// restore default SIGINT behavior in child process
		signal(SIGTSTP, SIG_DFL);	// restore default SIGTSTP behavior in child process
		/* execute command */
	
		if( execvp(args[0], args) < 0 ) {
			// printf("Error executing command %s\n", args[0]);
			exit(1);
		}
	}
	else {
		/* parent process */
		wait(NULL);	// wait for child to terminate
	}
}

void executeParallelCommands()
{
	// This function will run multiple commands in parallel
}

void executeSequentialCommands()
{	
	// This function will run multiple commands in parallel
}

void executeCommandRedirection()
{
	// This function will run a single command with output redirected to an output file specificed by user
}

void printPrompt()
{
	// This function will print the prompt in format - currentWorkingDirectory$
	char currWorkingDirectory[1024];
	getcwd(currWorkingDirectory, sizeof(currWorkingDirectory));
	printf("%s$ ", currWorkingDirectory);
}

void debugStop(bool *exit_flag)
{
	*exit_flag = true;
}

int main()
{
	// Initial declarations
	
	signal(SIGINT, SIG_IGN);	// ignore SIGINT in the shell process
	signal(SIGTSTP, SIG_IGN);	// ignore SIGTSTP in the shell process
	bool exit_flag = false;

	int debug_cntr = 0; // debug

	while(1)	// This loop will keep your shell running until user exits.
	{
		// Print the prompt in format - currentWorkingDirectory$
		printPrompt();

		// accept input with 'getline()'
        unsigned long int n = 100;
        char *line = (char*)malloc(sizeof(char)*(n));
        getline(&line, (unsigned long int*)&n, stdin);

		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		cmd_t cmd_type;
		parseInput(line, &cmd_type);		
		

		
		if(cmd_type == PAR)
			executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(cmd_type == SEQ)
			executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(cmd_type == RED)
			executeCommandRedirection();	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand(&exit_flag);		// This function is invoked when user wants to run a single commands
				
		if(exit_flag)	// When user uses exit command.
		{
			printf("Exiting shell...");
			break;
		}

		if( ++debug_cntr == 4 ) debugStop(&exit_flag); // debug


	}
	
	return 0;
}
