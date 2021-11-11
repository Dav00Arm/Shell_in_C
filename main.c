#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SHELL_RL_BUFSIZE 1024
#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIM "\t\r\n\a"


char *shell_read_line(void)
{
    int pos = 0;
    int bufsize = SHELL_RL_BUFSIZE;
    char *buffer = malloc(sizeof(char) * bufsize);
    
    // Check if character is in buffer size
    if (!buffer)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        int c = getchar();
        
        if(c == EOF || c == '\n')
        {
            buffer[pos] = '\0';
            return buffer;
        }
        else
        {
            buffer[pos] = c;
        }
        pos++;
        
        if(pos >= bufsize)
        {
            bufsize += SHELL_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer)
            {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
                
            }
        }
    }
}


char **shell_split_line(char *line)
{
    int bufsize = SHELL_TOK_BUFSIZE, pos = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;
    
    if(!tokens)
    {
        fprintf(stderr, "allocaiton error\n");
        exit(EXIT_FAILURE);
    }
    
    token = strtok(line, SHELL_TOK_DELIM);
    
    while(token != NULL)
    {
        tokens[pos] = token;
        pos++;
    
    
    if(pos >= bufsize)
    {
        bufsize += SHELL_TOK_BUFSIZE;
        tokens = realloc(tokens, bufsize * sizeof(char*));
        if(!tokens)
        {
            fprintf(stderr, "allocaiton error\n");
            exit(EXIT_FAILURE);
            
        }
    }
    
    token = strtok(NULL,SHELL_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
    
}



int shell_launch(char **args)
{
    pid_t pid , wpid;
    int status;
    
    pid = fork();
    if (pid == 0) 
    {
        if(execvp(args[0], args) == -1)
        {
            perror("Shell error");
        }
        exit(EXIT_FAILURE);
        
    }
    else if(pid < 0)
    {
        perror("Shell error");
    }
    else
    {
        while(!WIFEXITED(status) && !WIFSIGNALED(status)){
            
            wpid = waitpid(pid, &status, WUNTRACED);
        }
    }
    return 1;
}


int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char *builtins[] = {"cd", "help", "exit"};

int (*builtins_func[]) (char **) = {&shell_cd, &shell_help, &shell_exit};
    
    

int shell_num_builtins() 
{
    return sizeof(builtins) / sizeof(char*);
    
}


int shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "expected argument to 'cd'.\n");
    }
    else 
    {
        if(chdir(args[1]) != 0)
        {
            perror("Shell error");
        }
    }
    return 1;
}


int shell_help(char **args)
{

    printf("Type program from built-ins with arguments to continue.\n");
    printf("Here are built-ins:\n");
    
    for(int i = 0; i < shell_num_builtins();i++)
    {
        printf("%s\n", builtins[i]);
    }
    
    return 1;
}


int shell_exit(char **args)
{
    return 0;
}


int shell_execute(char **args)
{
    
    if(args[0] == NULL)
    {
        return 1;
    }
    
    for(int i = 0; i < shell_num_builtins(); i++)
    {
        if(strcmp(args[0], builtins[i]) == 0)
        {
            return (*builtins_func[i])(args);
        }
    }
    
    return shell_launch(args);
}

void shell_loop(void)
{
   
    char *line;
    char  **args;
    int status;
    
    while (status) {
        printf("> ");
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);
        
        free(line);
        free(args);
    }
    
}


int main(int argc, char **argv)
{
  shell_loop();

  return EXIT_SUCCESS;
}
