## UID: 405695663

## Pipe Up

Emulate the pipe operator (|) with this C program.

## Building

To build the program, run the following command:

```bash
make
```
This should build an executable called `pipe`.
## Running

To run the program, do the following command:\

```bash
./pipe [command1] [command2] ...
```

Where `[command1]` and `[command2]` are the commands you want to pipe together.\
```
./pipe ls cat wc
```
Should output the number of lines, words, and characters in the output of `ls | cat`.\
For instance, the output could look like this:
```
  1       1       5
```
This executable can be run with 1+ commands, separated by spaces. The output of the first command will be piped into the input of the second command, and so on.\
## Cleaning up

To clean up the build files, run the following command:
```bash
make clean
```
