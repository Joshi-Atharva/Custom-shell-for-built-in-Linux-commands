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


parseInput()
{
	// This function will parse the input string into multiple commands or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).
}

void executeCommand()
{
	// This function will fork a new process to execute a command
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

int main()
{
	// Initial declarations
	
	while(1)	// This loop will keep your shell running until user exits.
	{
		// Print the prompt in format - currentWorkingDirectory$
		
		// accept input with 'getline()'

		// Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
		parseInput(); 		
		
		if(/*condition*/)	// When user uses exit command.
		{
			printf("Exiting shell...");
			break;
		}
		
		if(/*condition*/)
			executeParallelCommands();		// This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
		else if(/*condition*/)
			executeSequentialCommands();	// This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
		else if(/*condition*/)
			executeCommandRedirection();	// This function is invoked when user wants redirect output of a single command to and output file specificed by user
		else
			executeCommand();		// This function is invoked when user wants to run a single commands
				
	}
	
	return 0;
}
