# Tiny Shell

A very tiny shell by Sokhountea Sy

## Brief Into

As part of an assignment in my course Operating Systems, I developed a tiny shell using C. It has very minimal functionality. Implemented chdir and history as internal commands. The chdir would change the current working directory of the process and history would list the last 100 commands that were executed in the tiny shell. SIGNINT signal is handled differently by asking the user for confirmation to terminate the shell, and SIGTSTP signal is ignored.

## Files
* README.md - This file
* tiny_shell.c - The heart of this project
