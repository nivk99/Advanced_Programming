
/** 
 @file shell.c
AUTHORS: Niv Kotek - 208236315
*/

/*INCLUDE*/
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>

/*DEFINE*/
#define MAIN_SIZE 1024
#define HELLO "hello:"

/*GLOBAL VARIABLES*/
char current_directory[1024];
char hist_filename[2000];
int status;
char hist[20][1024];
char HOST[1024], USER[1024], CWD[2048], HOME[2048];
int hist_i;
static char* args[512];
pid_t pid;
char *cmd_exec[100];
int pipe_count=0, fd;
char cwd[1024];


/*FUNCTIONS*/
void parent_directory();
void change_directory();
char* skipcomma(char* str);
static int command(int input, int first, int last, char *cmd_exec);
static int split(char *cmd_exec, int input, int first, int last);
void with_pipe_execute(char *input_buffer);
char **tokenize(char *input);
void fileprocess ();
void filewrite (char * input_buffer);
void load_history();
void signal_handler(int signum);
void _prompt();
void move(char* com2,char* prompt);
int _if(char* str);


/*************************************************************************/
void parent_directory()//pwd
{
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd );
	}
	else
		perror("getcwd() error");

}


void change_directory()//cd
{
char *h="/home";   
if(args[1]==NULL)
        chdir(h);
else if ((strcmp(args[1], "~")==0) || (strcmp(args[1], "~/")==0))
        chdir(h);
else if(chdir(args[1])<0)
    printf("bash: cd: %s: No such file or directory\n", args[1]);

}

char* skipcomma(char* str)
{
  int i=0, j=0;
  char temp[1000];
  while(str[i++]!='\0')
            {
              if(str[i-1]!='"')
                    temp[j++]=str[i-1];
            }
        temp[j]='\0';
        str = strdup(temp);
  
  return str;
}

static int command(int input, int first, int last, char *cmd_exec)
{
  int mypipefd[2], ret, input_fd, output_fd;
  ret = pipe(mypipefd);
  if(ret == -1)
      {
        perror("pipe");
        return 1;
      }
  pid = fork();
 
  if (pid == 0) 
  {
    if (first==1 && last==0 && input==0) 
    {
      dup2( mypipefd[1], 1 );
    } 
    else if (first==0 && last==0 && input!=0) 
    {
      dup2(input, 0);
      dup2(mypipefd[1], 1);
    } 
    else 
    {
      dup2(input, 0);
    }
 
    if(execvp(args[0], args)<0) printf("%s: command not found\n", args[0]);
              exit(0);
  }
  else 
  {
	waitpid(pid,&status,0);
   // waitpid(pid, 0, 0);  
   }
 
  if (last == 1)
    close(mypipefd[0]);
  if (input != 0) 
    close(input);
  close(mypipefd[1]);
  return mypipefd[0];

}

static int split(char *cmd_exec, int input, int first, int last)
{
    char *new_cmd_exec1;  
    new_cmd_exec1=strdup(cmd_exec);
      {
        int m=1;
        args[0]=strtok(cmd_exec," ");       
        while((args[m]=strtok(NULL," "))!=NULL)
              m++;
        args[m]=NULL;
        if (args[0] != NULL) 
            {

            if (strcmp(args[0], "exit") == 0) 
                    exit(0);
            if (strcmp(args[0], "echo") != 0) 
                    {
                      cmd_exec = skipcomma(new_cmd_exec1);
                      int m=1;
                      args[0]=strtok(cmd_exec," ");       
                      while((args[m]=strtok(NULL," "))!=NULL)
                                m++;
                      args[m]=NULL;

                    }
            		if(strcmp("cd",args[0])==0)
                    {
                    change_directory();
                    return 1;
                    }
            		else if(strcmp("pwd",args[0])==0)
                    {
                    parent_directory();
                    return 1;
                    }
           
            }
        }
    return command(input, first, last, new_cmd_exec1);
}


void with_pipe_execute(char *input_buffer)
{

	int i, n=1, input, first;

	input=0;
	first= 1;

	cmd_exec[0]=strtok(input_buffer,"|");

	while ((cmd_exec[n]=strtok(NULL,"|"))!=NULL)
		n++;
	cmd_exec[n]=NULL;
	pipe_count=n-1;
	for(i=0; i<n-1; i++)
	{
		input = split(cmd_exec[i], input, first, 0);
		first=0;
	}
	input=split(cmd_exec[i], input, first, 1);
	input=0;
	return;

}



// if date | grep Fri; then  echo "Shabat Shalom"; else  echo "Hard way to go"; fi
char **tokenize(char *input)
{
    char *p = strtok (input, ";");
    char **array = malloc(4 * sizeof(char *));
	if(!array)
	{
		perror("file malloc");
		exit(1);
	}

    int no = 0;
	for(int i=0;i<4;i++)
	{
		
		array[i]=malloc(MAIN_SIZE * sizeof(char));
		if(!array[i])
		{
			perror("file malloc");
			exit(1);
		}

	}
    while (p != NULL)
    {
        array[no++] = p;
        p = strtok (NULL, ";");
    }

    return array;
}


void fileprocess ()//history
{
	getcwd(current_directory, sizeof(current_directory));
 	strcat(current_directory, "/");
 	strcat(current_directory, "history.txt");
}

void filewrite (char * input_buffer)//write-history
{
	int fd_out=open(current_directory,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);
	int ret_write=write(fd_out,input_buffer,strlen(input_buffer));
	if(ret_write<0) 
    {
        printf("Error in writing file\n");
        return;
    }
	ret_write=write(fd_out,"\n",strlen("\n"));
	if(ret_write<0) 
    {
        printf("Error in writing file\n");
        return;
    }
 close(fd_out);
}

void load_history()//load-history
{
    FILE *file = fopen (current_directory, "r" );
    if( file != NULL )
    {
        char line[128]; 
        int no = 0;
        while (fgets(line, sizeof(line), file) != NULL ) 
        {
			printf("%s",line);
        }

        fclose(file);
    }
    else perror ("history.txt"); 
}


void signal_handler(int signum)//signal handler
 {
	// printf("%s ", prompt);
    char * message = "You typed Control-C!\n";
    write(1, message, strlen(message));
}


void _prompt()//print prompt
{
   gethostname(HOST, sizeof(HOST));
    getlogin_r(USER, sizeof(USER));
    char *DIR;
    if (getcwd(CWD, sizeof(CWD)) == NULL)
    {
       perror("getcwd() error");
       exit(EXIT_FAILURE);
    }

    else
    {
        char * p;
        p = strstr(CWD, HOME);
        if(p)
        {
            DIR = p + strlen(HOME);
            printf("\x1B[1;32m%s@%s\x1B[0m:\x1B[1;34m~%s\x1B[0m", USER, HOST, DIR);
        }
        else
        {
            DIR = CWD;
            printf("\x1B[1;32m%s@%s\x1B[0m:\x1B[1;34m%s\x1B[0m", USER, HOST, DIR);
        }

    }

    
}



void move(char* com2,char* prompt)//move up and down
{
	int up=0;
	char cm[MAIN_SIZE];
	strcpy(cm,com2);
	while (cm[0]=='\033')
	{
        if(strlen(cm)%3 == 0)
        {
			bool bo=true;
			int i=0;
			if(up<0)
			{
				up =0;
			}
            while(cm[3*i]!= '\0')
            {
                if(cm[3*i] == '\033' && cm[3*i+1] =='[' && cm[3*i+2] == 'A')//up
				{
					up++, i++;

				}  //checking for up key
                else 
                {
                    bo=false;
                    break;
                }
            }

            if(bo)
            {
				printf("\033[1A");//line up
				printf("\x1b[2K");//delete line
				_prompt();
		   		 printf("%s ", prompt);
                printf("%s",hist[(hist_i - up) % 20]);
				cm[MAIN_SIZE] ;
				fgets(cm, MAIN_SIZE, stdin);//reading data
				cm[strlen(cm) - 1] = '\0';
            }
        }


		if(strlen(cm)%3 == 0)
		{
			bool bo=true;
			int i=0;
			if(up<0)
			{
				up =0;
			}
			while(cm[3*i]!= '\0')
			{
				if(cm[3*i] == '\033' && cm[3*i+1] =='[' && cm[3*i+2] == 'B')//down
				{
					up--, i++;

				}  //checking for up key
	
				else 
				{
					bo=false;
					break;
				}
			}

			if(bo)
			{
				printf("\033[1A");//line up
				printf("\x1b[2K");//delete line
				_prompt();
				printf("%s ", prompt);
				printf("%s",hist[(hist_i - up) % 20]);
				cm[MAIN_SIZE];
				fgets(cm, MAIN_SIZE, stdin);//reading data
				cm[strlen(cm) - 1] = '\0';
			}
		}

	}
	if(up > 0)
	{
		strcpy(com2,hist[(hist_i - up) % 20]);
	}
}


int _if(char* str)
{
	str+=3;
	return system(str);
}




int main()
{
	printf("\033[1;35m\n *** Welcome to Shell ***\n\033[0m");
	fileprocess();
	//handler - Ctrl+C
	signal(SIGINT, signal_handler);
	char command[MAIN_SIZE];
	char save_command[MAIN_SIZE];
	char *prompt=(char*)calloc(strlen(HELLO),sizeof(char));
	strcpy(prompt,HELLO);

	if(!prompt)
	{
		perror("file calloc");
		exit(1);
	}

	while (true)
	{
		_prompt();
		printf("%s ", prompt);
		char command[MAIN_SIZE];
		fgets(command, MAIN_SIZE, stdin);//reading data
		command[strlen(command) - 1] = '\0';

		move(command,prompt);

		//hist
		strcpy(hist[hist_i % 20], command);
   		 hist_i++;
		filewrite(command);


		if (!strcmp(command, "quit")) // quit [7]
		{ 
			printf("\033[1;35m\n*** Goodbye, %s ***\n\033[0m", HOST);
			 free(prompt);
            break;
        }

		else if(!strcmp(command, "!!"))//A command that repeats the last command [6]
		{
			 strcpy(command, save_command);

		}
		else
		{
			strcpy(save_command, command);
		}

		if ((strstr(command,"if")&& strstr(command,"else")&& strstr(command,"then")&& strstr(command,"fi")))
		{
			char **arr=tokenize(command);
			if((strstr(arr[0],"if")&& strstr(arr[2],"else")&& strstr(arr[1],"then")&& strstr(arr[3],"fi")))
			{
				if(strstr(arr[0],"if"))
				{
					int i=_if(arr[0]);
					if(i&&strstr(arr[2],"else"))
					{
						arr[2]+=5;
						strcpy(command,arr[2]);
					}
					else if(i==0&&strstr(arr[1],"then"))
					{
						arr[1]+=5;
						strcpy(command,arr[1]);

					}

				}
			}
			else
			{
				printf("syntax error");
			}
			free(arr);
		}

		if(strchr(command, '|')&&(!strstr(command, "if")))//piping
		{
			with_pipe_execute(command);
			waitpid(pid,&status,0);
			pid=0;
			pipe_count=0;
			fd=0;

		}

		else if(strchr(command,'&'))// and
		{
			if (fork() == 0)
			{
			int i = 0;
			char *argv[MAIN_SIZE];
   			char *token = strtok (command," ");
    		while (token != NULL)
    		{
       		 argv[i] = token;
        	token = strtok (NULL, " ");
       		 i++;
    		}
   			argv[i] = NULL;
			execvp(argv[0], argv);
    		}

		}
		else if(strchr(command,'>')&&!strstr(command,">>"))
		{
			if (fork() == 0)
			{
			int i = 0;
			char *argv[MAIN_SIZE];
   			char *token = strtok (command," ");
    		while (token != NULL)
    		{
       		 argv[i] = token;
        	token = strtok (NULL, " ");
       		 i++;
    		}
   			argv[i] = NULL;
			argv[i - 2] = NULL;
			int fd = creat(argv[i - 1], 0660); 
            dup2(fd, 1);
			argv[i - 2] = argv[i - 1] = NULL;
			execvp(argv[0], argv);

			}
		else
		{
 			wait(&status);
		}

		}
		else if(strstr(command,">>") )
		{

			if (fork() == 0)
			{
			int i = 0;
			char *argv[MAIN_SIZE];
   			char *token = strtok (command," ");
    		while (token != NULL)
    		{
       		 argv[i] = token;
        	token = strtok (NULL, " ");
       		 i++;
    		}
   			argv[i] = NULL;
			int fd = open(argv[i - 1], O_CREAT | O_APPEND | O_RDWR, 0660);
            dup2(fd, 1);
			argv[i - 2] = argv[i - 1] = NULL;
			execvp(argv[0], argv);
			}
		else
		{
			wait(&status);
		}
		}
		else if(strchr(command,'<') )
		{
			if (fork() == 0)
			{
			int i = 0;
			char *argv[MAIN_SIZE];
   			char *token = strtok (command," ");
    		while (token != NULL)
    		{
       		 argv[i] = token;
        	token = strtok (NULL, " ");
       		 i++;
    		}
   			argv[i] = NULL;
            int fd = open(argv[i - 1], O_RDONLY, 0660);
            dup2(fd, 0);
			argv[i - 2] = argv[i - 1] = NULL;
			execvp(argv[0], argv);

			
			}

		else
		{

			wait(&status);
		}
		}
		
		else
		{

			int i = 0;
			char *argv[MAIN_SIZE];
   			char *token = strtok (command," ");
    		while (token != NULL)
    		{
       		 argv[i] = token;
        	token = strtok (NULL, " ");
       		 i++;
    		}
   			 argv[i] = NULL;

			 if(!strcmp(argv[0], "cd"))//cd
			 {
				chdir(argv[1]);

			 }
			 else if(!strcmp(argv[0], "prompt")&&!strcmp(argv[1], "="))//prompt
			 {
				free(prompt);
				prompt=(char*)calloc(strlen( argv[2]),sizeof(char));
				if(!prompt)
				{
					perror("file calloc");
					exit(1);
				}
				strcpy(prompt,argv[2]);
			 }
			 else if(!strcmp(argv[0], "echo") && !strcmp(argv[1], "$?"))//[4]
			 {
				printf("%d\n", status);
			 }
			else  if(!strcmp(argv[0], "echo") && argv[1][0] == '$')
			 {
				char * variable_name = argv[1] + 1;
       			 variable_name[strlen(variable_name)] = '\0';
        		char * result = getenv(variable_name);
       			 printf("%s\n", result);

			 }
			 else if(!strcmp(argv[0], "read"))
			 {
				char buffer[MAIN_SIZE];
				fgets(buffer, MAIN_SIZE, stdin);
				buffer[strlen(buffer) - 1] = '\0';
				int st=setenv(argv[1], buffer, 1);
				    if(st)  // error
					{
        				perror("Error setenv");
					}

			 }
			 else if(argv[0][0] == '$' && argv[1][0] == '=')
			 {
				char * variable_name = argv[0] + 1;
        		variable_name[strlen(variable_name)] = '\0';
				int st=setenv(variable_name, argv[2], 1);
				if(st)  // error
				{
        			perror("Error setenv");
				}
			 }
			else  if(fork() == 0)
			 {
				execvp(argv[0], argv);
			 }
			 else
			 {
				wait(&status);

			 }


		}



	}
	




	return 0;
}



