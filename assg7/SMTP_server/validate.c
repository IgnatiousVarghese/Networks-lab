#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

int starts_with(char *full, char *sub)
{
    int m = strlen(full);
    int n = strlen(sub);

    if (n > m)
        return 0;

    for (int i = 0; i < n; i++)
        if (full[i] != sub[i])
            return 0;

    return n;
}

int has_char(char *str, char c)
{
    int n = strlen(str);
    for (int i = 0; i < n; i++)
        if (str[i] == c)
            return i;
    return -1;
}

void get_line(char *data, int *index, char *out)
{
    int n = strlen(data);
    int i = 0;

    while (data[*index] != '\n' && *index < n)
    {

        out[i] = data[*index];
        *index += 1;
        i++;
    }

    out[i] = '\0';

    if (data[*index] == '\n')
        *index += 1;
}

int get_field(char *mail, char *field, char *out)
{
    char buff[500];
    if (strcmp(field, "From") == 0)
    {
        sscanf(mail, "%[^\n]%*c", buff);
        if (strncmp(buff, "From: ", 6) == 0)
        {
            strcpy(out, buff + 6);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "To") == 0)
    {
        sscanf(mail, "%[^\n]%*c%[^\n]%*c", buff, buff);
        if (strncmp(buff, "To: ", 4) == 0)
        {
            strcpy(out, buff + 4);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "Subject") == 0)
    {
        sscanf(mail, "%[^\n]%*c%[^\n]%*c%[^\n]%*c", buff, buff, buff);
        if (strncmp(buff, "Subject: ", 9) == 0)
        {
            strcpy(out, buff + 9);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "Body") == 0)
    {
        int n = 0;
        sscanf(mail, "%[^\n]%*c%[^\n]%*c%[^\n]%*c%n", buff, buff, buff, &n);
        strcpy(out, mail + n);
        return 0;
    }
    return -1;
}
int get_field_pop3(char *mail, char *field, char *out)
{
    char buff[500];
    if (strcmp(field, "From") == 0)
    {
        sscanf(mail, "%[^\n]%*c", buff);
        if (strncmp(buff, "From: ", 6) == 0)
        {
            strcpy(out, buff + 6);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "To") == 0)
    {
        sscanf(mail, "%[^\n]%*c%[^\n]%*c", buff, buff);
        if (strncmp(buff, "To: ", 4) == 0)
        {
            strcpy(out, buff + 4);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "Subject") == 0)
    {
        sscanf(mail, "%[^\n]%*c%[^\n]%*c%[^\n]%*c", buff, buff, buff);
        if (strncmp(buff, "Subject: ", 9) == 0)
        {
            strcpy(out, buff + 9);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "Received") == 0)
    {
        sscanf(mail, "%[^\n]%*c%[^\n]%*c%[^\n]%*c%[^\n]%*c", buff, buff, buff, buff);
        if (strncmp(buff, "Received: ", 10) == 0)
        {
            strcpy(out, buff + 10);
            return 0;
        }
        else
            return -1;
    }
    else if (strcmp(field, "Body") == 0)
    {
        int n = 0;
        sscanf(mail, "%[^\n]%*c%[^\n]%*c%[^\n]%*c%[^\n]%*c%n", buff, buff, buff, buff, &n);
        strcpy(out, mail + n);
        return 0;
    }
    return -1;
}
int verify_email(char *email)
{
    int n = strlen(email);
    int x = 0;
    int y = 0;
    int flag = 0;
    for (int i = 0; i < n; i++)
    {
        if (email[i] == '@')
        {
            flag = 1;
            continue;
        }

        if (flag == 0)
            x++;
        else
            y++;

        if (y >= 1)
            break;
    }

    return x > 0 && y > 0;
}
