/* kompiluj
gcc -ansi -Wall -o microshell/microshell microshell/microshell.c
./microshell/microshell
*/

/* informacje o autorze programu oraz jego funkcjonalnosci 
void help(){

}
chdir("..");
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_INPUT 256


void help(){
    printf("\n+--------------------------------------------------+\n|                                                  |\n|               ### MicroShellSO ###               |\n|               Andrzej Jurga 477601               |\n|                                                  |\n|   help - displays help                           |\n|   cd [PATH] - allows to change working directory |\n|   exit - ends with MicroShell                    |\n|                                                  |\n+--------------------------------------------------+\n\n");
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
void prepareArgsArray(char *argv[],char *text, char *temp){
        int i = 0;
        int j = 0;
        int k = 0;
        printf("%s\n",text);
        while(text[i]!='\0')
        {
            if(text[i]=='\'' || text[i]=='\"')
            {
                printf("WARN: %c\n",text[i]);
                i++;
                k++;
                while(text[i]!='\'' && text[i]!='\"')
                {
                    printf("WHILE: %c\n",text[i]);
                    temp[i-k] = text[i];
                    i++;
                }
            temp[i-1] = '\0';
            }
            else
            {
                while(text[i]!=' ' && text[i]!='\0')
                {
                    temp[i-k] = text[i];
                    i++;
                }
            temp[i] = '\0';
            }
            /*argv[j] = temp;*/
            strcpy(argv[j], temp);
            /*printf("%s\n",temp);
            printf("%s\n",argv[j]);*/
            memset(temp,0,MAX_INPUT);
            
            i++;
            k=i;
            j++;
        }
        argv[j] = NULL;
}

/*Try to recognize and run commend from user*/
void recoCommand(char *builtIn[], int length, char* argv[]){ 
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
        chdir(argv[1]);
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
            printf("Command '%s' not found.\n", argv[0]);
        }
        wait(NULL);
    }
}


int main() {
   char text[MAX_INPUT];
   char path[MAX_INPUT];
   char temp[MAX_INPUT];
   int spaceCount;
   /*List of native commends*/
   char **argv;
   char *builtIn[] = {"cd", "help", "exit"}; 
   /*Number of supported commends*/
   int builtInCount = sizeof(builtIn)/sizeof(builtIn[0]); 


    while(1){
        getcwd(path, sizeof(path));
        printf("[%s] $", path); /*Prompt*/
        fgets(text, MAX_INPUT, stdin); /*Get line*/
        text[strlen(text)-1] = '\0'; /*Replace new line with end of line*/
        /*now input is ready*/
        if(checkQuoats(text)){

            spaceCount = 0;
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
printf("Space count: %d\n",spaceCount);
            argv = malloc((spaceCount + 2) * sizeof(char*));
            for (i = 0; i < (spaceCount + 2); i++)
                argv[i] = malloc((MAX_INPUT+2) * sizeof(char));
            /*array of args is no created*/
            
            prepareArgsArray(argv, text, temp);

            for(i = 0; i<spaceCount + 2; i++)
                printf("|%s",argv[i]);
            printf("|\n");
        
        
            recoCommand(builtIn, builtInCount, argv);
        }
        else
        {
            printf("ERROR: please check \' and \" placement\n");
        }
    }
   

   return 0;
}