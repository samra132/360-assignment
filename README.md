# Student Name: Gurleen Samra, Omar Ghzeil
# Student ID: 3144680, 3123197
# Submission Date: 2026-05-10
# File: README.md

# CMPT 360 Assignment 1 - Username Sanitizer Utility

## Description

This program reads possible usernames from standard input and checks if each username is valid. A valid username must not be `root` or `admin`, must start with a lowercase letter, must only contain lowercase letters, numbers, or underscores, and must be between 3 and 16 characters long.

If the username is not valid, the program tries to fix it. It changes uppercase letters to lowercase, changes whitespace into underscores, removes invalid characters, pads short usernames with letters from the word `user`, and cuts long usernames to 16 characters. After that, the program checks the username again. If it is still not valid, it prints that the username is invalid and unfixable.

## Files Included

- `userclean.c` - The C source file for the program
- `Makefile` - Used to compile, clean, and run valgrind
- `README.md` - Explanation and instructions for the assignment

## How to Compile

To compile the program, run:

```bash
make

## Academic Integrity Statement
I certify that this submission represents entirely my own wor

I certify that this submission represents entirely my own work.

