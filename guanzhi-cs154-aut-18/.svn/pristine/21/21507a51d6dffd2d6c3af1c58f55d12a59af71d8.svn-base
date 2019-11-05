#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define CHARMAX 514 

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

void printError() 
{
	char error_message[30] = "An error has occurred\n";
 	write(STDOUT_FILENO, error_message, strlen(error_message));
}

//2 = advanced red, 1 = normal red, 0 = neither, -1 = error 
int checkRedirect(char* cmd, char** args, char** file_name)
{
	int normal_red = 0; 
	int adv_red = 0;
	for (int i = 0; i < strlen(cmd); i++)
	{
		if (cmd[i] == '>')
		{
			if(cmd[i+1] == '+')
			{
				adv_red++; 
			}
			else
			{ 
				normal_red++; 
			}
		}
	}
	if (((adv_red > 0) && (normal_red > 0)) || adv_red > 1 || normal_red > 1)
	{
		return -1; 
	}
	char* red_args = strtok(cmd, ">+\n\t");
	if (red_args == NULL)
	{
		return -1; 
	}
	int s = 0; 
	char* new_line[CHARMAX]; 
	new_line[s] = red_args; 
	*args = red_args; 
	s++;
	//Potential issue: args before the redirect separated by tab space!
	while ((new_line[s++] = strtok(NULL, ">+\n\t ")) != NULL)
	{
	}
	//Check for multiple names after redirect
	if (s > 3)
	{	
		return -1;}
	if (adv_red > 0 || normal_red >0)
	{
		//Check for no file
		if (!new_line[1])
		{
			return -1; 
		}
		*file_name = new_line[1]; 
		//Check for multiple filenames
		/* 
		*file_name = strtok(new_line[1], " ");
		if (strtok(NULL, " ") != NULL)
		{
			return -1; 
		}*/
		if (adv_red > 0) 
		{
			return 2; 
		}
		else
		{
			return 1; 
		}
	}
	else 
	{
		*file_name = NULL;  
		return 0; 
	}
	
	
}

void createRedFile(char** args, char* file, int red_status)
{	
	int fdescrip; 
	if (red_status == 1)
	{
		fdescrip = open(file, O_WRONLY | O_EXCL | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	}
	else if (red_status == 2)
	{
		fdescrip = open(file, O_EXCL | O_RDWR | O_CREAT, S_IRUSR | O_TRUNC, 0777); 
	}
	if (fdescrip <= 0)
	{
		printError(); 
		exit(0); 
		return;
	} 
	else 
	{
		if (dup2(fdescrip, STDOUT_FILENO) < 0)
		{
			printError(); 
		}
		close(fdescrip);
		return; 
	}
}

void exec(char* cmd)
{
	char* saved_cmd = strdup(cmd); 
	char* parse_sym[CHARMAX]; 
	int i = 0;
	char* token = strtok(cmd, " \n\t");   
	//Check if empty line
	if(!token)
	{
		return; 
	}
	parse_sym[i++] = token;
	while(token != NULL)
	{
		token = strtok(NULL, " \n\t"); 
		parse_sym[i++] = token; 
	} 

	//Check redirect error for EXIT, CD, PWD
	char* save = strdup(parse_sym[0]); 
	char* nored = strtok(save, ">");
	//Check if only > 
	if (nored == NULL)
	{
		printError(); 
		return; 
	} 
	if ((!strcmp(nored, "cd") || !strcmp(nored, "exit") || !strcmp(nored, "pwd")) && (strlen(nored) != strlen(parse_sym[0])))
	{
			printError();
			free(save); 
			return; 
	}
	free(save); 
	//PWD 
	if (!strcmp(parse_sym[0], "pwd"))
	{	
		if (parse_sym[1])
		{
			printError(); 
			return; 
		}
		char wd[1024]; 
		getcwd(wd, 1024); 
		myPrint(wd); 
		myPrint("\n"); 
 	}
	//Exit
	else if (!strcmp(parse_sym[0], "exit"))
	{
		if(parse_sym[1])
		{
			printError(); 
			return;
		}
		exit(0); 
	}
	//cd
	else if (!strcmp(parse_sym[0], "cd"))
	{
		if (parse_sym[2])
		{
			printError(); 
			return;
		}
		else if (parse_sym[1])
		{
			if (chdir(parse_sym[1]) < 0)
			{
				printError(); 
				return;
			}
		}
		else
		{
			chdir(getenv("HOME")); 
		}
	}
	else
	{
		char* file; 
		char* args; 
		int redirect_status = checkRedirect(saved_cmd, &args, &file); 
		if (redirect_status < 0)
		{
			printError(); 
			return; 
		}
		char* red_parse[CHARMAX]; 
		char* token; 	
		int i = 0; 					
		while((token = strtok_r(args, " \n\t", &args)))
        {
        	red_parse[i++] = token; 
        }
		red_parse[i] = NULL; 
		pid_t pid = fork(); 
		
		//Do all child processes first, in order
		if (pid == 0)
		{
			//Normal redirect 
			if (redirect_status == 1)
			{		
				createRedFile(red_parse, file, redirect_status); 
				if (execvp(red_parse[0], red_parse) < 0)
				{
					printError(); 
					exit(0); 
				}
			}
			else if (redirect_status == 2)
			{
				FILE* test = fopen(file, "r"); 
				if (!test)
				{
					createRedFile(red_parse, file, 1); 
	                if (execvp(red_parse[0], red_parse) < 0)
	                {
                	    printError();
                    	exit(0);
    	            }
					fclose(test); 
					exit(0); 
				}	
				//If already exists, then create new file with new output, and write in existing data from old file
				//Then copy new file into old file 
				pid_t pid2 = fork(); 
				if (pid2 == 0)
				{
					createRedFile(red_parse, "new", redirect_status);
					if (execvp(red_parse[0], red_parse) < 0 )
					{
						printError(); 
					} 
					exit(0);
				}
				else
				{
					wait(NULL); 
				}
				FILE* new = fopen("new", "r");
				FILE* full = fopen("full", "w");  
				char c = fgetc(new);
				while (c != EOF)
				{
					fputc(c, full);
					c = fgetc(new); 
				}		
				c = fgetc(test); 
				while (c != EOF)
				{
					fputc(c, full); 
					c = fgetc(test); 
				}
				fclose(new); 
				fclose(test);
				fclose(full); 
				FILE* old = fopen(file, "w");  
				full = fopen("full", "r"); 
				c = fgetc(full); 
				while (c != EOF)
				{
					fputc(c, old); 
					c = fgetc(full); 
				}
				fclose(full); 
				fclose(old);
				remove("new");
				remove("full"); 
				exit(0); 
			}
			else
			{
				if (execvp(red_parse[0], red_parse) < 0)
                {
                    printError();
                    exit(0);
                }
				exit(0); 	
			}
		}
		else
		{ wait(NULL); }
	}	
	free(saved_cmd); 
}

int main(int argc, char *argv[]) 
{
    char *pinput;
	if (argc == 1)
	{
		char cmd_buff[1024];  
    	while (1) 
		{
	    	myPrint("myshell> ");
        	pinput = fgets(cmd_buff, 1024, stdin);
			//Empty command line, continue 
        	if (!pinput) {
            	continue;
        	}
			
			if (strlen(cmd_buff) >= CHARMAX)
			{
				myPrint(cmd_buff); 
				myPrint("\n"); 
				printError(); 
				continue; 
			}
			
			char* cur_command; 
			char* save = cmd_buff; 
			while((cur_command = strtok_r(save, ";\n\t", &save)))
			{
				exec(cur_command); 
			}
    	}	
	}
	else if(argc == 2)
	{
		FILE* fp = fopen(argv[1], "r"); 
		char cmd_buff[2000];
		if (fp == NULL)
		{
			printError(); 
			return 0; 
		}
		while(fgets(cmd_buff, 2000, fp) != NULL)
		{
			cmd_buff[strlen(cmd_buff)-1] = '\0';
			//Check for blank line 
			char* checkline = strdup(cmd_buff); 
			char* stripped = strtok(checkline, " \n\t");
			if (stripped == NULL)
			{
				continue; 
			}
			myPrint(cmd_buff); 
			myPrint("\n"); 
			if (strlen(cmd_buff) >= CHARMAX) 
			{
       	 		printError();
        		continue;
      		}	
			
			char* cmds[CHARMAX] = {0}; 
			char* cur_command;
			char* save;
			int i = 0; 
			cur_command = strtok_r(cmd_buff, ";\n\t", &save); 
			cmds[i++] = cur_command;
			while((cur_command = strtok_r(NULL, ";\n\t", &save)))
			{
				cmds[i++] = cur_command; 
			}
			for (int j = 0; j < i; j++)
			{
				exec(cmds[j]); 
			}
		}
		fclose(fp);  
	}
	else 
	{
		printError(); 
		exit(0); 
	}
}
