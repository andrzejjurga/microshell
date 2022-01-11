/* kompiluj 
gcc -ansi -Wall -o microshell/microshell microshell/microshell.c
./microshell/microshell 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>

#define MAX_INPUT 256

void historyAdd(char *text)
{
    if(strcmp(text, "history"))
    {
        char historyFile[MAX_INPUT];
        memset(historyFile,0,MAX_INPUT);
        strcpy(historyFile, getenv("HOME"));
        strcat(historyFile, "/.microshellHistory");
        FILE * file;
        file = fopen(historyFile, "a");
        fprintf(file, "%s\n", text);
        fclose(file);
    }
}

void historyPrint()
{
    char historyFile[MAX_INPUT];
    memset(historyFile,0,MAX_INPUT);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    strcpy(historyFile, getenv("HOME"));
    strcat(historyFile, "/.microshellHistory");
    FILE * file;
    file = fopen(historyFile, "r");
    
    while ((read = getline(&line, &len, file)) != -1) {
            printf("%s", line);
        }
    free(line);
    fclose(file);
}

void historyDelete()
{
    char historyFile[MAX_INPUT];
    memset(historyFile,0,MAX_INPUT);
    strcpy(historyFile, getenv("HOME"));
    strcat(historyFile, "/.microshellHistory");
    FILE * file;
    file = fopen(historyFile, "a");
    remove(historyFile);
}

void help(){
    printf("\n+--------------------------------------------------+\n|                                                  |\n|               ### MicroShellSO ###               |\n|               Andrzej Jurga 477601               |\n|                                                  |\n|   help - displays help                           |\n|   cd [PATH] - allows to change working directory |\n|   exit - ends with MicroShell                    |\n|                                                  |\n+--------------------------------------------------+\n\n");
}

int gethostname(char *name, size_t namelen);

int countSpaces(char *text){
    int spaceCount = 0;
    int inQuotation = 0;
    int i;
    for(i = 0; i < strlen(text); i++)
    {
        if(text[i]=='\'' || text[i]=='\"')
        {
            if(inQuotation == 0)
            {
                inQuotation = 1;
            }
            else
            {
                inQuotation = 0;
            }
        }
        if(text[i] == ' ' && inQuotation == 0)
            spaceCount++;
    }
    return spaceCount;
}
int checkQuoats(char *text){
    int apostrophe = 0;
    int quote = 0;
    int i = 0;
    int bool = 1;
    while(text[i]!='\0')
    {
        if(text[i]=='\'')
        {
            if(quote % 2 == 0)
                apostrophe++;
            else
                bool = 0;
        }
        if(text[i]=='\"')
        {
            if(apostrophe % 2 == 0)
                quote++;
            else
                bool = 0;
        }
        i++;
    }
    if(quote % 2 != 0 || apostrophe % 2 != 0)
        bool = 0;
    return bool;
}

/*Convert input string to array of arguments*/
void prepareArgsArray(char *argv[], char *text, char *temp){
        int i = 0;
        int j = 0;
        int k = 0;
        /*printf("%s\n",text);*/
        while(text[i]!='\0')
        {
            if(text[i]=='\'' || text[i]=='\"')
            {
                printf("WARN: %c\n",text[i]);
                i++;
                k++;
                while(text[i]!='\'' && text[i]!='\"')
                {
                    temp[i-k] = text[i];
                    i++;
                }
            temp[i-1] = '\0';
            }
            else
            {
                if(text[i]==' ')
                {
                    i++;
                    k++;
                }
                while(text[i]!=' ' && text[i]!='\0')
                {
                    temp[i-k] = text[i];
                    i++;
                }
            temp[i] = '\0';
            }
            printf("%s\n", temp);
            strcpy(argv[j], temp);
            memset(temp,0,MAX_INPUT);
            
            i++;
            k=i;
            j++;
        }
        argv[j] = NULL;
}

/*Try to recognize and run command from user*/
void recoCommand(char *builtIn[], int length, char* argv[], char *prevDirectory){ 
    int i;
    for(i = 0; i<length; i++)
    {
        if(strcmp(argv[0], builtIn[i]) == 0)
        {
            printf("%s\n", builtIn[i]);    
            break;
        }
    }

    if(i==0)
    {
        if(!strcmp(argv[1], "-"))
        {
            chdir(prevDirectory);
        }
        else
        {
            getcwd(prevDirectory, MAX_INPUT);
            chdir(argv[1]);
        }
        perror("ERROR");
    }
    else if(i==1)
    {
     help();   
    }
    else if(i==2)
    {
    exit(0);
    }
    else if(i==3)
        {
            printf("in 3 else\n");
            if(argv[1] != NULL && (strcmp(argv[1], "-d") || strcmp(argv[1], "--delete")))
            {
                historyDelete();
            }
            else
            {
                historyPrint();
            }
        }
    else if(i == length)
    {
        pid_t childPid = fork();
        if(childPid<0)
        {
            /*ERROR*/
        }
        else if(childPid==0)
        {
            /*execlp(argv[0], argv[0], NULL);*/
            execvp(argv[0], argv);
            printf("\033[31mCommand '%s' not found.\n\033[0m", argv[0]);
        }
        wait(NULL);
    }
}


int main() {
   char text[MAX_INPUT];
   char path[MAX_INPUT];
   char temp[MAX_INPUT];
   char *name;
   char prevDirectory[MAX_INPUT];
   getcwd(prevDirectory, MAX_INPUT);
   char host[MAX_INPUT];
   
   int spaceCount;
   /*List of native commends*/
   char **argv;
   char *builtIn[] = {"cd", "help", "exit", "history"}; 
   /*Number of supported commends*/
   int builtInCount = sizeof(builtIn)/sizeof(builtIn[0]); 


    while(1){
        getcwd(path, sizeof(path)); 
        name = getlogin();
        gethostname(host, MAX_INPUT);
        printf("\033[32m%s\033[0m@\033[34m%s\033[0m:\033[36m[%s]\033[0m$", name, host, path); /*Prompt*/
        fgets(text, MAX_INPUT, stdin); /*Get line*/
        text[strlen(text)-1] = '\0'; /*Replace new line with end of line*/
        /*now input is ready*/

        historyAdd(text);

        if(checkQuoats(text)){
            int i;
            spaceCount = countSpaces(text);
            
printf("Space count: %d\n",spaceCount);

            argv = malloc((spaceCount + 2) * sizeof(char*));
            for (i = 0; i < (spaceCount + 2); i++)
                argv[i] = malloc((MAX_INPUT+2) * sizeof(char));
            /*array of args is no created*/
            
            prepareArgsArray(argv, text, temp);

            for(i = 0; i<spaceCount + 2; i++)
                printf("|%s",argv[i]);
            printf("|\n");
        
        
            recoCommand(builtIn, builtInCount, argv, prevDirectory);
        }
        else
        {
            printf("\033[31mERROR: please check \' and \" placement\n\033[0m");
        }
    }
   

   return 0;
}