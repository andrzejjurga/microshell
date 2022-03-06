#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <regex.h>

#define MAX_INPUT 256
#define ASCII_ESC 27


int gethostname(char *name, size_t namelen);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
int tolower(int c);

int getTerminaHeight()
{
    struct winsize window;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);

    return window.ws_row;
}

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
        if(strcmp(text,"\0") && !strstr(text, "history"))
            fprintf(file, "%s\n", text);
        fclose(file);
    }
}

int checkRegex(char *text, char *userRegex, int R)
{
    regex_t regex;
    int regex_status = 0;

    regex_status = regcomp(&regex, userRegex, R);
    if(regex_status)
    { 
        return -1; 
    }

    regex_status = regexec(&regex, text, 0, NULL, 0);
    if( !regex_status )
    {
        regfree(&regex);
        return 1;
    }
    else if( regex_status == REG_NOMATCH )
    {
        regfree(&regex);
        return 0;
    }
    else
    {
        regfree(&regex);
        return -1;
    }

}

void historyPrint()
{
    char historyFile[MAX_INPUT];
    memset(historyFile,0,MAX_INPUT);
    char *line = NULL;
    size_t length = 0;
    ssize_t get;
    strcpy(historyFile, getenv("HOME"));
    strcat(historyFile, "/.microshellHistory");
    FILE * file;
    file = fopen(historyFile, "r");
    while ((get = getline(&line, &length, file)) != -1) 
    {
        printf("%s", line);
    }
    free(line);
    fclose(file);
}

void historySearch(char *search)
{
    char historyFile[MAX_INPUT];
    memset(historyFile,0,MAX_INPUT);
    char *line = NULL;
    size_t length = 0;
    ssize_t get;
    strcpy(historyFile, getenv("HOME"));
    strcat(historyFile, "/.microshellHistory");
    FILE * file;
    file = fopen(historyFile, "r");
    while ((get = getline(&line, &length, file)) != -1) 
    {
        if(strstr(line, search))
        {
            printf("%s", line);
        }
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
    remove(historyFile);
    historyAdd("--History--");
}

/*Count lines in file*/
int linesCount(char *filePath)
{   
    int counter = 0;
    FILE * file;
    int temp;
    file = fopen(filePath, "r");
    while(!feof(file))
    {
        temp = fgetc(file);
        if(temp == '\n')
            {
                counter++;
            }
    }
    fclose(file);
    return counter;
}

/*Checki if file is dire or not*/
int is_dir(const char *path)
{
    struct stat file;
    stat(path, &file);
    return S_ISDIR(file.st_mode);
}

/*Make all carracter lower case*/
char* lowerCase(char * text)
{
    int i = 0;
    while(text[i] != '\n' && text[i] != '\0')
    {
        text[i] = tolower(text[i]);
        i++;
    }
    return text;
}

/*Simple ls inmplementation*/
void ls()
{
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) 
    {
        while ((dir = readdir(d)) != NULL) 
        {
            if(dir->d_name[0]!='.')
            {
                if (!is_dir(dir->d_name))
                    printf("\033[34m%s\033[0m\n", dir->d_name);
                else
                    printf("%s/\n", dir->d_name);
            }
        }
    }
    closedir(d);
}

void grep(char *argv[])
{
    DIR *d;
    FILE * file;
    char text[MAX_INPUT];
    char *line = NULL;
    char path[MAX_INPUT];
    char search[MAX_INPUT];
    char *match;
    int matchBool;
    strcpy(path, ".");
    strcpy(search, "");
    size_t length = 0;
    ssize_t get;
    struct dirent *dir;

    int c = 0, h = 0, n = 0, N = 0, i = 0, r = 0, R = 0, I = 0, j = 1, valid = 1;
    int count = 0, index = 0, omitedCounter=0;
    /*Recognize all flags*/
    while(argv[j] != NULL && strchr(argv[j], '-')!=NULL && valid != 0)
    {
        valid = 0;
        if(strchr(argv[j], 'c') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            c = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'n') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            n = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'N') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            N = 1;
            valid = 1;
            printf("tutaj\n");
        }
        else if(strchr(argv[j], 'h') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            h = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'r') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            r = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'R') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            R = 1;
            r = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'i') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            i = 1;
            valid = 1;
        }
        else if(strchr(argv[j], 'I') == strchr(argv[j], '-')+1 && strlen(argv[j]) == 2)
        {    
            i = 1;
            I = 1;
            valid = 1;
        }
        j++;
    }

    /*Recognize pattern*/
    if(argv[j] != NULL && valid == 1)
    {
        strcpy(search, argv[j]);
    }
    else
    {
        valid = 0;
    }
    j++;
    /*Path from user*/
    if(argv[j] != NULL && valid == 1)
    {
        strcpy(path, argv[j]);
    }
    d = opendir(path);
    if (d != NULL && valid == 1) 
    {

        while ((dir = readdir(d)) != NULL) 
        {
            if(access(dir->d_name, F_OK)==0 && dir->d_name[0]!='.')
            {
                file = fopen(dir->d_name, "r");
                while ((get = getline(&line, &length, file)) != -1) 
                {
                    index++;
                    if (strlen(line) < MAX_INPUT || i == 0)
                    {
                        if(i)
                        {
                            strcpy(text, line);
                            lowerCase(line);
                            lowerCase(search);
                        }
                        if(r)
                        {
                            matchBool = checkRegex(line, search, R);

                        }
                        else
                        {
                            match = strstr(line, search);
                            if(match != NULL)
                            {
                                matchBool = 1;
                            }
                            else
                            {
                                matchBool = 0;
                            }
                        }
                        if(matchBool==1 && N==0)
                        {
                            if(!c)
                            {
                                if(!h)
                                {
                                    printf("\033[33m%s:\033[0m",dir->d_name);
                                    if(n)
                                    {
                                        printf("\033[32m%d:\033[0m",index);
                                    }
                                }
                                if(i)
                                {
                                printf(" %s", text);
                                }
                                else
                                {
                                    printf(" %s", line);
                                }

                            }
                            count++;
                        }
                        else if(matchBool == 0 && N==1)
                        {
                            if(!c)
                            {
                                if(!h)
                                {
                                    printf("\033[33m%s:\033[0m",dir->d_name);
                                    if(n)
                                    {
                                        printf("\033[32m%d:\033[0m",index);
                                    }
                                }
                                if(i)
                                {
                                printf(" %s", text);
                                }
                                else
                                {
                                    printf(" %s", line);
                                }

                            }
                            count++;
                        }
                    }
                    else if(strlen(line) >= MAX_INPUT)
                    {
                        omitedCounter++;
                        if(I)
                            printf("\033[31mLine:%d from file:%s omitted because it is too long\033[0m\n",index, dir->d_name); 
                    }
                }
                if(c && count)
                {
                  printf("\033[33m%s: %d\033[0m\n",dir->d_name, count);  
                    count = 0;
                }
            }
            index = 0;
        }
        if(i && !I && omitedCounter > 0)
        printf("\033[31m%d lines have been omitted. To see more information, use -I flag\033[0m\n", omitedCounter);
        if(matchBool == -1)
            printf("\033[31mCould not compile regex\033[0m\n");

    }
    else if(access(path, F_OK)==0)
    {
        file = fopen(path, "r");
        while ((get = getline(&line, &length, file)) != -1) 
        {
            index++;
                    if (strlen(line) < MAX_INPUT || i == 0)
                    {
                        if(i)
                        {
                            strcpy(text, line);
                            lowerCase(line);
                            lowerCase(search);
                        }
                        if(r)
                        {
                            matchBool = checkRegex(line, search, R);
                        }
                        else
                        {
                            match = strstr(line, search);
                            if(match != NULL)
                            {
                                matchBool = 1;
                            }
                            else
                            {
                                matchBool = 0;
                            }
                        }
                        if(matchBool==1 && N==0)
                        {
                            if(!c)
                            {
                                if(n)
                                {
                                    printf("\033[32m%d:\033[0m",index);
                                }
                                if(i)
                                {
                                printf(" %s", text);
                                }
                                else
                                {
                                    printf(" %s", line);
                                }

                            }
                            count++;
                        }
                        else if(matchBool == 0 && N==1)
                        {
                            if(!c)
                            {
                                if(n)
                                {
                                    printf("\033[32m%d:\033[0m",index);
                                }
                                if(i)
                                {
                                printf(" %s", text);
                                }
                                else
                                {
                                    printf(" %s", line);
                                }

                            }
                            count++;
                        }
                    }
                    else if(strlen(line) >= MAX_INPUT)
                    {
                        omitedCounter++;
                        if(I)
                            printf("\033[31mLine:%d omitted because it is too long\033[0m\n",index); 
                    }
        }
        free(line);
        fclose(file);
    }
    else
    {
        valid = 0;
    }
    closedir(d);
    if(valid==0)
        printf("\033[31merror while executing 'grep' command\033[0m\n");
}   

void more(char *argv[])
{
    char *line = NULL;
    int s = 0;
    if(argv[1] != NULL && access(argv[1], R_OK) == 0)
    {
        if(argv[2] != NULL && argv[3] != NULL && strstr(argv[2], "-s") != NULL)
        {
            s = 1;
        } 
        char filePath[MAX_INPUT];
        char c;
        strcpy(filePath, argv[1]);
        int i = 0, lineCounter = linesCount(filePath), printedLines = 0;
        if(!lineCounter) printf("\033[31mError encountered!\n\033[0m");
        else{
            FILE * file;
            file = fopen(filePath, "r");
            size_t length = 0;
            ssize_t get;
            int height = getTerminaHeight();
            while ((get = getline(&line, &length, file)) != -1 && i<getTerminaHeight()) 
            {
                c = 'z';
                printf("%s", line);
                if(argv[2] != NULL && argv[3] != NULL && strstr(line, argv[3]) != NULL)
                    s = 0;
                printedLines++;
                i++;       
                if(i == height)
                {
                    if(s == 1)
                    {
                        i = 0;
                    }
                    else
                    {
                        printf("\033[0;42m--More--(%.1f%%)\033[0;0m", (float)printedLines/lineCounter*100);
                        c = getchar();
                        getchar();
                    }
                    if(c!='q')
                    {
                        i = 0;
                    }
                }
            }
            free(line);
            fclose(file);
        }
    }
    else
    {
        if(argv[1] == NULL)
            printf("\033[31mNot enough arguments\n\033[0m");
        else if(access(argv[1], R_OK))
            printf("\033[31mInvalid file\n\033[0m");        
    }
}

/*Display user manual*/
void help(){
    printf("\n+--------------------------------------------------------------+\n|                                                              |\n|                     ### MicroShellSO ###                     |\n|                     Andrzej Jurga 477601                     |\n|                                                              |\n|   help - displays help                                       |\n|   cd [PATH] - allows to change working directory             |\n|   exit - ends with MicroShell                                |\n|   history - displays command history for current user        |\n|      -d|--delete - delete history                            | \n|      -s|--search [text] - search history                     |\n|   grep [Flags] pattern [file/dir] - search file/dir for      |\n|       matching phrases                                       |\n|      -c - count appearances                                  |\n|      -h - hide file name                                     |\n|      -b - show line numeber                                  |\n|      -N - show lines where the phrase does not appear        |\n|      -i - case insensitive                                   |\n|      -r - use simple regex                                   |\n|      -R - use advanced regex                                 |\n|   more file [-s pattern]- view a file page by page           |\n|      -s - open the file on the first pattern encountered     |\n|                                                              |\n|                                                              |\n+--------------------------------------------------------------+\n\n");
}

/*Count spaces in user input*/
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

/*Check if quoats are pleaced correctly*/
int checkQuoats(char *text){
    int apostrophe = 0;
    int quote = 0;
    int i = 0;
    int check = 1;
    while(text[i]!='\0')
    {
        if(text[i]=='\'')
        {
            if(quote % 2 == 0)
                apostrophe++;
            else
                check = 0;
        }
        if(text[i]=='\"')
        {
            if(apostrophe % 2 == 0)
                quote++;
            else
                check = 0;
        }
        i++;
    }
    if(quote % 2 != 0 || apostrophe % 2 != 0)
        check = 0;
    return check;
}

/*Convert input string to array of arguments*/
void prepareArgsArray(char *argv[], char *text, char *temp){
        int i = 0;
        int j = 0;
        int k = 0;
        while(text[i]!='\0')
        {
            if(text[i]=='\'' || text[i]=='\"')
            {
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
            strcpy(argv[j], temp);
            memset(temp,0,MAX_INPUT); 
            i++;
            k=i;
            j++;
        }
        argv[j] = NULL;
}

/*Try to recognize and run command from user*/
void recoCommand(char *builtIn[], int length, char *argv[], char *prevDirectory){ 
    int i;
    for(i = 0; i<length; i++)
    {
        if(strcmp(argv[0], builtIn[i]) == 0)
        {    
            break;
        }
    }

    if(i==0)
    {/*cd getcwd(path, sizeof(path));*/
        if(argv[1] != NULL)
        {
            if(!strcmp(argv[1], "-"))
            {
                if(chdir(prevDirectory)==-1)
                    perror("ERROR");
            }
            else if(!strcmp(argv[1], "~"))
            {
                getcwd(prevDirectory, MAX_INPUT);
                if(chdir(getenv("HOME"))==-1)
                    perror("ERROR");
            }
            else
            {
                getcwd(prevDirectory, MAX_INPUT);
                if(chdir(argv[1])==-1)
                    perror("ERROR");
            }
        }
        else
        {
            printf("\033[31mNot enough arguments\n\033[0m");
        }
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
    {/*History*/
        if(argv[1] != NULL && (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--delete")))
        {
            historyDelete();
        }
        else if(argv[1] != NULL && argv[2] != NULL && (!strcmp(argv[1], "-s") || !strcmp(argv[1], "--search")))
        {
            historySearch(argv[2]);
        }
        else
        {
            historyPrint();
        }
    }
    else if(i==4)
    {
    more(argv);
    }
    else if(i==5)
    {
    ls(argv);
    }
    else if(i==6)
    {
    grep(argv);
    }
    else if(i == length)
    {
        pid_t childPid = fork();
        if(childPid==0)
        {
            execvp(argv[0], argv);
            printf("\033[31mCommand '%s' not found.\n\033[0m", argv[0]);
            exit(0);
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
   char **argv;
   /*List of native commends*/
   char *builtIn[] = {"cd", "help", "exit", "history", "more", "ls", "grep"}; 
   /*Number of supported commends*/
   int builtInCount = sizeof(builtIn)/sizeof(builtIn[0]); 


    while(1){
        fflush(stdout);
        getcwd(path, sizeof(path)); 
        name = getlogin();
        gethostname(host, MAX_INPUT);
        printf("\033[32m%s\033[0m@\033[34m%s\033[0m:\033[36m[%s]\033[0m$ ", name, host, path); /*Prompt*/
        fgets(text, MAX_INPUT, stdin); /*Get line*/
        text[strlen(text)-1] = '\0'; /*Replace new line with end of line*/
        /*now input is ready*/

        historyAdd(text);

        if(checkQuoats(text) && strcmp(text, "")){
            int i;
            spaceCount = countSpaces(text);

            argv = malloc((spaceCount + 2) * sizeof(char*));
            for (i = 0; i < (spaceCount + 2); i++)
                argv[i] = malloc((MAX_INPUT+2) * sizeof(char));
            /*array of args is now created*/
            
            prepareArgsArray(argv, text, temp);
            recoCommand(builtIn, builtInCount, argv, prevDirectory);
        }
        else if(strcmp(text, ""))
        {
            printf("\033[31mERROR: please check \' and \" placement\n\033[0m");
        }
    }
   return 0;
}