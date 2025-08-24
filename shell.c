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
typedef enum{PAR, SEQ, RED, PIPE, SINGLE, ERR} cmd_t;
typedef enum{false, true} bool;

bool error_flag = false;

char **cmds = NULL;
int cmd_count = 0;
void printPrompt()
{
	// This function will print the prompt in format - currentWorkingDirectory$
	char currWorkingDirectory[1024];
	getcwd(currWorkingDirectory, sizeof(currWorkingDirectory));
	printf("%s$", currWorkingDirectory);
}

void print2dStr(char **args)
{
	// This function will print debug information
	while( *args != NULL ) {
		printf("%s\n", *args);
		args++;
	}
}



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
			if( strlen(token) != 0 ) {
				cmd_count = cmd_count + 1;
				cmds = realloc(cmds, sizeof(char*) * cmd_count);
				cmds[cmd_count-1] = token;
			}

			token = strsep(&line, "&&");
		}
	}
	else if( strstr(line, "##") != NULL ) {
		*cmd_type = SEQ;
		/* separate into mutliple commands string */
		char *token = strsep(&line, "##");
		while( token != NULL ) {
			/* append to cmds */
			if( strlen(token) != 0 ) {
				cmd_count = cmd_count + 1;
				cmds = realloc(cmds, sizeof(char*) * cmd_count);
				cmds[cmd_count-1] = token;
			}
			
			token = strsep(&line, "##");
		}
	}
	else if( strstr(line, ">") != NULL ) {
		*cmd_type = RED;
		/* separate into mutliple commands string */
		char *token = strsep(&line, ">");
		while( token != NULL ) {
			/* append to cmds */
			if( strlen(token) != 0 ) {
				cmd_count = cmd_count + 1;
				cmds = realloc(cmds, sizeof(char*) * cmd_count);
				cmds[cmd_count-1] = token;
			}
			token = strsep(&line, ">");
		}
	}
	else if( strstr(line, "|") != NULL ) {
		*cmd_type = PIPE;
		/* separate into mutliple commands string */
		char *token = strsep(&line, "|");
		while( token != NULL ) {
			/* append to cmds */
			if( strlen(token) != 0 ) {
				cmd_count = cmd_count + 1;
				cmds = realloc(cmds, sizeof(char*) * cmd_count);
				cmds[cmd_count-1] = token;
			}
			else {
				/* incorrect command */
				*cmd_type = ERR;
				return;
			}
			token = strsep(&line, "|");
		}
	}
	else {
		*cmd_type = SINGLE;
		cmds = realloc(cmds, sizeof(char*));
		cmds[0] = line;
		cmd_count = 1;

	}
}
void tokenize(char *cmd, char **args, int *arg_count, const char *delim)
{
	/* parse command into program and arguments */
	/* remove leading and trailing spaces and new line */
	while( cmd != NULL && (cmd[0] == ' ' || cmd[0] == '\n') ) cmd++;
	char *end = cmd + strlen(cmd) - 1;
	while( end > cmd && (end[0] == ' ' || end[0] == '\n') ) {
		end[0] = '\0';
		end--;
	}
	
	*arg_count = 0;
	char *token = strsep(&cmd, delim);
	while( token != NULL ) {
		args[*arg_count] = token;
		*arg_count = *arg_count + 1;
		token = strsep(&cmd, delim);
	}

	if( *arg_count > 0 && strlen(args[*arg_count-1]) == 0 ) *arg_count = *arg_count - 1; // remove empty argument if any

	args[*arg_count] = NULL;	// execvp() needs NULL at the end of args array
}

void executeCommand(char *cmd, bool *exit_flag)
{
	/* parse command into program and arguments */
	char *args[100];
	int arg_count = 0;
	tokenize(cmd, args, &arg_count, " \n");

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
		printf("Shell: Incorrect command\n");
		return;
	}
	else if( pid == 0 ) {
		/* child process */
		signal(SIGINT, SIG_DFL);	// restore default SIGINT behavior in child process
		signal(SIGTSTP, SIG_DFL);	// restore default SIGTSTP behavior in child process
		/* execute command */
	
		if( execvp(args[0], args) < 0 ) {
			printf("Shell: Incorrect command\n");
			return;
		}
	}
	else {
		/* parent process */
		wait(NULL);	// wait for child to terminate
	}
}

void executeParallelCommands(bool *exit_flag)
{
	// This function will run multiple commands in parallel
	if( cmd_count < 2 ) {
		printf("Shell: Incorrect command\n");
		return;
	}
	pid_t pids[cmd_count];
	pid_t shell_pid = getpid();

	for( int i = 0; i < cmd_count; i++ ) {
		char *cmd = cmds[i];
		
		/* call executeCommand() in a new process */
		/* ensure that child process doesn't fork */
		if( getpid() != shell_pid ) return;

		pids[i] = fork();
		if( pids[i] < 0 ) {
			return;
		}
		else if( pids[i] == 0 ) {
			/* child process */
			executeCommand(cmd, exit_flag);
			/* assuming no exit command in parallel commands */
		}
		else {
			/* parent process */
			// do nothing
		}
	}
	if( getpid() == shell_pid ) {
		/* parent process */
		for( int i = 0; i < cmd_count; i++ ) {
			waitpid(pids[i], NULL, 0);	// wait for all child processes to terminate
		}
	}
}

void executeSequentialCommands(bool *exit_flag)
{	
	// This function will run multiple commands sequentially
	if( cmd_count < 2 ) {
		printf("Shell: Incorrect command\n");
		return;
	}
	for( int i = 0; i < cmd_count; i++ ) {
		char *cmd = cmds[i];
		executeCommand(cmd, exit_flag);
		if( *exit_flag ) return;
	}
}

void executeCommandRedirection(bool *exit_flag)
{
	// This function will run a single command with output redirected to an output file specificed by user
	if( cmd_count != 2 ) {
		printf("Shell: Incorrect command\n");
		return;
	}
	char *cmd = cmds[0];
	char *outfile = cmds[1];
	/* parse command into program and arguments */
	char *args[100];
	int arg_count = 0;
	tokenize(cmd, args, &arg_count, " \n");

	/* parse outfile to remove leading/trailing spaces and new line */
	while( outfile != NULL && (outfile[0] == ' ' || outfile[0] == '\n') ) outfile++;
	char *end = outfile + strlen(outfile) - 1;
	while( end > outfile && (end[0] == ' ' || end[0] == '\n') ) {
		end[0] = '\0';
		end--;
	}

	if( outfile == NULL || strlen(outfile) == 0 ) {
		printf("Shell: Incorrect command\n");
		return;
	}

	/* assuming no exit command in redirection */
	/* execute the command in a new process with output redirected to outfile */
	pid_t pid = fork();
	if( pid < 0 ) {
		return;
	}
	else if( pid == 0 ) {
		/* child process */
		signal(SIGINT, SIG_DFL);	// restore default SIGINT behavior in child process
		signal(SIGTSTP, SIG_DFL);	// restore default SIGTSTP behavior in child process

		/* open outfile for writing */
		int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if( fd < 0 ) {
			printf("Shell: Incorrect command\n");
			return;
		}
		/* redirect stdout to fd */
		dup2(fd, STDOUT_FILENO);
		close(fd);

		/* execute command */
		if( execvp(args[0], args) < 0 ) {
			printf("Shell: Incorrect command\n");
			return;
		}
	}
	else {
		/* parent process */
		wait(NULL);	// wait for child to terminate
	}
}

void executeCommandPipe(bool *exit_flag) {
	/* flow : 
		> i = 1
		> while( i < cmds_count) char *cmd1 = cmds[i-1], *cmd2 = cmds[i]
		> parse cmd1 and cmd2 into args1 and args2
		> create a pipe
		> fork a new process
		> in child process :
			- redirect stdout to write end of pipe
			- close read end of pipe
			- exec cmd1
		> in parent process :
			- wait for child to terminate
			- redirect stdin to read end of pipe
			- close write end of pipe
			- exec cmd2
		> i = i + 1
	*/

	if( cmd_count < 2 ) {
		printf("Shell: Incorrect command\n");
		return;
	}
	pid_t shell_pid = getpid();
	char *cmd1, *cmd2;
	int i = 0;
	char *args1[100], *args2[100];
	int arg_count1 = 0, arg_count2 = 0;

	while( i < cmd_count - 1 ) {
		cmd1 = cmds[i];
		cmd2 = cmds[i+1];
		tokenize(cmd1, args1, &arg_count1, " \n");
		tokenize(cmd2, args2, &arg_count2, " \n");

		int pipefd[2];
		if( pipe(pipefd) < 0 ) {
			error_flag = true;
			return;
		}

		if( getpid() != shell_pid ) return; // ensure that child process doesn't loop if exec fails

		pid_t subshell_pid = fork(); // for executing pipes, so that shell parent process is not affected
		if( subshell_pid < 0 ) {
			error_flag = true;
			return;
		}
		else if( subshell_pid == 0 ) {
			/* subshell process */
			/* now fork to create child process for cmd1 */
			pid_t pid = fork();
			if( pid < 0 ) {
				error_flag = true;
				return;
			}
			else if( pid == 0 ) {
				/* child process - we will write in this one */
				signal(SIGINT, SIG_DFL);	// restore default SIGINT behavior in child process
				signal(SIGTSTP, SIG_DFL);	// restore default SIGTSTP behavior in child process

				/* redirect stdout to write end of pipe */
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[0]); // close read end of pipe

				/* execute cmd1 */
				if( execvp(args1[0], args1) < 0 ) {
					printf("Shell: Incorrect command\n");
					error_flag = true;
					return;
				}
			}
			else {
				/* parent process */
				wait(NULL); // wait for child to terminate

				/* redirect stdin to read end of pipe */
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[1]); // close write end of pipe

				/* execute cmd2 */
				if( execvp(args2[0], args2) < 0 ) {
					printf("Shell: Incorrect command\n");
					error_flag = true;
					return;
				}
			}
		}
		else {
			/* parent process */
			waitpid(subshell_pid, NULL, 0); // wait for subshell to terminate
		}

		i = i + 1;
	}
}



int main()
{
	// Initial declarations
	
	signal(SIGINT, SIG_IGN);	// ignore SIGINT in the shell process
	signal(SIGTSTP, SIG_IGN);	// ignore SIGTSTP in the shell process
	bool exit_flag = false;

	pid_t shell_pid = getpid(); 

	while(!exit_flag)	// This loop will keep your shell running until user exits.
	{
		error_flag = false; // debug // so that error can be inspected after each command execution if required

		if( getpid() != shell_pid ) return 0; // ensure that any child process doesn't loop
		// Print the prompt in format - currentWorkingDirectory$
		printPrompt();

		// accept input with 'getline()'
        unsigned long int n = 100;
        char *line = (char*)malloc(sizeof(char)*(n));
        getline(&line, (unsigned long int*)&n, stdin);

		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		cmd_t cmd_type;
		parseInput(line, &cmd_type);		
		

		
		if(cmd_type == PAR){
			executeParallelCommands(&exit_flag); // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		}		
		else if(cmd_type == SEQ) {
			executeSequentialCommands(&exit_flag);	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		}
		else if(cmd_type == RED) {
			executeCommandRedirection(&exit_flag);	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		}
		else if( cmd_type == PIPE ) {
			executeCommandPipe(&exit_flag);
		}
		else if( cmd_type == SINGLE ) {
			char *cmd = cmds[0];
			if( cmd != NULL && cmd_count ) { // empty command, ignore
				executeCommand(cmd, &exit_flag);		// This function is invoked when user wants to run a single commands
			}
		}
		else if( cmd_type == ERR ) {
			printf("Shell: Incorrect command\n");
		}
				
		if(exit_flag)	// When user uses exit command.
		{
			printf("Exiting shell...\n");
		}

		free(line);
		free(cmds);
		cmds = NULL;
		cmd_count = 0;

	}
	
	return 0;
}
