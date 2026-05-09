/*
 * Student Name: Gurleen Samra
 * Student ID: 3144680
 * Submission Date: 2026-05-10
 * File: userclean.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Purpose: this function checks if it is a reserved username.
 * Parameters: s
 * Return: 1 if the username is reserved, if not then 0
 */
int is_reserved(const char *s)
{
    return strcmp(s, "root") == 0 || strcmp(s, "admin") == 0;
}

/*
 * Purpose: this function checks if it is a valid username character.
 * Parameters: c
 * Return: 1 if the character is valid, if not then 0
 */
int valid_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || (c == '_');
}

/*
 * Purpose: this function checks if it is a whitespace character.
 * Parameters: c
 * Return: 1 if the character is whitespace, if not then 0
 */
int is_space(char c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

/*
 * Purpose: this function checks if the username is valid.
 * Parameters: s
 * Return: 1 if the username is valid, if not then 0
 */
int is_valid(const char *s)
{
    int len = strlen(s);

    if (len < 3 || len > 16)
        return 0;

    if (is_reserved(s))
        return 0;

    if (s[0] < 'a' || s[0] > 'z')
        return 0;

    for (int i = 0; i < len; i++) {
        if (valid_letter(s[i]) == 0)
            return 0;
    }

    return 1;
}

/*
 * Purpose: this function sanitizes it into a possible valid username.
 * Parameters: input
 * Return: A dynamically allocated sanitized string
 */
char *sanitize(const char *input)
{
    int len = strlen(input);
    char *temp = malloc(len + 1);
    int j = 0;


    for (int i = 0; i < len; i++) {
        char c = input[i];

        if (c >= 'A' && c <= 'Z')
            c = c + ('a' - 'A');

        if (is_space(c))
            c = '_';

        if (valid_letter(c)) {
            temp[j++] = c;
        }
    }
    temp[j] = '\0';

    char *result = malloc(j + 5);

    strcpy(result, temp);//Copy cleaned string into result

    int new_len = strlen(result);

    char *pad = "user";
    int k = 0;

    while (new_len < 3) {
        result[new_len] = pad[k];
        new_len++;
        k++;
    }
    result[new_len] = '\0';

    if (new_len > 16)
        result[16] = '\0';

    free(temp);
    return result;
}

/*
 * Purpose: Main function that reads usernames from standard input, validates them, sanitizes invalid ones, and prints results.
 * Parameters: None
 * Return: 0 if successful 
 */
int main(void)
{
    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), stdin)) {

        buffer[strcspn(buffer, "\n")] = '\0';

        if (is_valid(buffer)) {
            printf("%s\n", buffer);
        } else {
            char *clean = sanitize(buffer);

            if (is_valid(clean)) {
                printf("%s\n", clean);
            } else {
                printf("invalid and unfixable\n");
            }

            free(clean);
        }
    }

    return 0;
}