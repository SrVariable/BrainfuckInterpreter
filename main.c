/* Tokens
> | ++ptr;
< | --ptr;
+ | ++*ptr;
- | --*ptr;
. | putchar(*ptr);
, | *ptr=getchar();
[ | while (*ptr) {
] | }
*/
#define BUFFER_SIZE 10

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int check_closed(char *buffer, int bytesread)
{
    static int opened[2];

    if (bytesread == 0 && opened[0] < 0)
        return (-1);
    else if (bytesread == 0 && opened[1] < 0)
        return (-2);
    for (int i = 0; i < bytesread; i++)
    {
        if (buffer[i] == '>')
            opened[0]++;
        else if (buffer[i] == '<')
            opened[0]--;
        else if (buffer[i] == '[')
            opened[1]++;
        else if (buffer[i] == ']')
            opened[1]--;
        if (opened[0] < 0)
            return (-1);
        else if (opened[1] < 0)
            return (-2);
    }
    return (0);
}

int parse_file(int fd)
{
    char *buffer;
    int bytesread;
    int less_than_opened = 0;
    int status;

    buffer = (char*)calloc(BUFFER_SIZE, sizeof(unsigned char));
    if (!buffer)
        return (-1);
    bytesread = 1;
    while (bytesread != 0)
    {
        bytesread = read(fd, buffer, BUFFER_SIZE);
        if (bytesread < 0)
        {
            free(buffer);
            return (-1);
        }
        buffer[bytesread] = '\0';
        status = check_closed(buffer, bytesread);
        if (status < 0)
        {
            free(buffer);
            return (status);
        }
    }
    free(buffer);
    return (0);
}

int parse_buffer(char *buffer, int bytesread)
{
    static int less_than_opened;
    static int amount;
    static bool stop;

    if (bytesread == 0 && less_than_opened < 0)
        return (-1);
    for (int i = 0; i < bytesread; i++)
    {
        if (buffer[i] == '>')
        {
            if (less_than_opened == amount)
                stop = false;
            less_than_opened++;
            if (!stop)
                amount++;
        }
        else if (buffer[i] == '<')
        {
            stop = true;
            less_than_opened--;
        }
    }
    return (amount);
}

int pointer_size(int fd)
{
    char *buffer;
    int bytesread;
    int size;

    buffer = (char*)calloc(BUFFER_SIZE, sizeof(unsigned char));
    if (!buffer)
        return (-1);
    bytesread = 1;
    size = 1;
    while (bytesread != 0)
    {
        bytesread = read(fd, buffer, BUFFER_SIZE);
        if (bytesread < 0)
        {
            free(buffer);
            return (-1);
        }
        buffer[bytesread] = '\0';
        if (parse_buffer(buffer, bytesread) < 0)
        {
            free(buffer);
            return (-1);
        }
    }
    size += parse_buffer(buffer, bytesread);
    if (size < 0)
    {
        free(buffer);
        return (-1);
    }
    printf("%d\n", size);
    free(buffer);
    return (size);
}

int brainfuck_compiler(int size)
{
    return (0);
}

int main(int argc, char **argv)
{
    // Check that the user has provided a file
    if (argc != 2)
    {
        printf("Usage :%s <brainfuck_file>", argv[0]);
        return (1);
    }
    // Check if the extension is correct
    char *extension = strchr(argv[1], '.') + 1;
    if (strncmp(extension, "bf", 2) != 0)
    {
        printf("Invalid file! Filename extension must be '.bf'\n");
        return (1);
    }
    // Open the file
    int fd = open(argv[1], O_RDONLY, 0622);
    if (fd < 0)
    {
        printf("Error opening file\n");
        return (1);
    }
    // Parse file
    int parsestatus = parse_file(fd);
    if (parsestatus == -1)
    {
        printf("More '<' than '>'\n");
        close(fd);
        return (1);
    }
    else if (parsestatus == -2)
    {
        printf("Mismatch '[' and ']'\n");
        close(fd);
        return (1);
    }
    // Reset the pointer of fd
    if (lseek(fd, 0, SEEK_SET) < 0)
    {
        printf("Error resetting the pointer of fd\n");
        close(fd);
        return (1);
    }
    // Get the size of the pointer
    int size = pointer_size(fd);
    if (size < 0)
    {
        printf("Error parsing file\n");
        close(fd);
        return (1);
    }
    // Compiles the file
    if (brainfuck_compiler(size) < 0)
    {
        printf("Error compiling\n");
        close(fd);
        return (1);
    }
    // Close the file
    close(fd);
    return (0);
}
