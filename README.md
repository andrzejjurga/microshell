## Microshell
Welcome to Microshell! This is a simple shell program written in C that allows you to run basic Unix commands.

### How to Use
To use Microshell, simply compile the microshell.c file using the C compiler of your choice. For example, you can use the following command:

```
gcc -o microshell microshell.c
```
After you have compiled the program, you can run it by typing the following command:
```
./microshell
```

Once Microshell is running, you can enter Unix commands just like you would in a regular shell. For example, you can type ls to list the files in the current directory, or cd to change to a different directory.

### Commands implemented by me:
* help
* cd
* exit
* history
    * -d|--delete - delete history
    * -s|--search [text] - search history 
* grep
    * -c - count appearances 
    * -h - hide file name
    * -b - show line numeber
    * -N - show lines where the phrase does not appear
    * -i - case insensitive
    * -r - use simple regex
    * -R - use advanced regex
* more
    * -s - open the file on the first pattern encountered

### Acknowledgments
This project was developed as part of the Operating Systems course at Adam Mickiewicz University.

### License
This project is licensed under the MIT License. You are free to use, modify, and distribute the code as you wish. Please refer to the LICENSE file for more information.
