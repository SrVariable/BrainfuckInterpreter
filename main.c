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
    int status;

    buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
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

    buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
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
    free(buffer);
    return (size);
}

int generate_file(const char *str_size)
{
    int fd;
    const char *header = "#include <stdio.h>\n#include <stdlib.h>\n";
    const char *main = "int main(void){";
    const char *ptr_declaration = "unsigned char*ptr = (unsigned char*)calloc((";
    const char *ptr_declaration_end = "+ 1), sizeof(unsigned char));";
    const char *temp = "unsigned char*temp = ptr;";

    fd = open("temp.c", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0)
        return (-1);
    if (write(fd, header, strlen(header)) < 0)
        return (-1);
    if (write(fd, main, strlen(main)) < 0)
        return (-1);
    if (write(fd, ptr_declaration, strlen(ptr_declaration)) < 0)
        return (-1);
    if (write(fd, str_size, strlen(str_size)) < 0)
        return (-1);
    if (write(fd, ptr_declaration_end, strlen(ptr_declaration_end)) < 0)
        return (-1);
    if (write(fd, temp, strlen(temp)) < 0)
        return (-1);
    close(fd);
    return (0);
}

int append_content(char *buffer)
{
    int fd;

    fd = open("temp.c", O_WRONLY | O_APPEND, 0622);
    if (fd < 0)
        return (-1);
    for (int i = 0; buffer[i]; i++)
    {
        if (buffer[i] == '>')
        {
            if (write(fd, "++ptr;", 6) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == '<')
        {
            if (write(fd, "--ptr;", 6) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == '+')
        {
            if (write(fd, "++*ptr;", 7) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == '-')
        {
            if (write(fd, "--*ptr;", 7) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == '.')
        {
            if (write(fd, "putchar(*ptr);", 14) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == ',')
        {
            if (write(fd, "*ptr=getchar();", 15) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == '[')
        {
            if (write(fd, "while (*ptr) {", 14) < 0)
            {
                close(fd);
                return (-1);
            }
        }
        else if (buffer[i] == ']')
        {
            if (write(fd, "}", 1) < 0)
            {
                close(fd);
                return (-1);
            }
        }
    }
    close(fd);
    return (0);
}

int append_end(void)
{
    int fd;
    const char *end = "free(temp);return (0);}";

    fd = open("temp.c", O_WRONLY | O_APPEND, 0622);
    if (fd < 0)
        return (-1);
    if (write(fd, end, strlen(end)) < 0)
    {
        close(fd);
        return (-1);
    }
    close(fd);
    return (0);
}

int get_result(void)
{
    int status;

    status = system("gcc temp.c -o temp");
    if (status < 0)
        return (-1);
    status = system("./temp");
    if (status < 0)
        return (-1);
    if (remove("temp.c") < 0)
        return (-1);
    if (remove("temp") < 0)
        return (-1);
    return (0);
}

int brainfuck_interpreter(const char *size, int fd)
{
    int bytesread;
    char *buffer;

    buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
    if (!buffer)
        return (-1);
    bytesread = 1;
    if (generate_file(size) < 0)
    {
        free(buffer);
        return (-1);
    }
    while (bytesread != 0)
    {
        bytesread = read(fd, buffer, BUFFER_SIZE);
        if (bytesread < 0)
        {
            free(buffer);
            return (-1);
        }
        buffer[bytesread] = '\0';
        if (append_content(buffer) < 0)
        {
            free(buffer);
            return (-1);
        }
    }
    free(buffer);
    if (append_end() < 0)
        return (-1);
    if (get_result() < 0)
        return (-1);
    return (0);
}

static int	number_length(int number)
{
	long	temp;
	int		counter;

	if (number < 0)
		temp = (long)number * -1;
	else
		temp = (long)number;
	counter = 1;
	while ((temp / 10) > 0)
	{
		temp /= 10;
		counter++;
	}
	return (counter);
}

static char	*fixed_malloc(int number, size_t bytes)
{
	char	*str;

	if (number < 0)
		str = (char *)malloc((bytes + 2) * sizeof(char));
	else
		str = (char *)malloc((bytes + 1) * sizeof(char));
	if (!str)
		return (NULL);
	return (str);
}

static void	number_to_string(char *str, int number, int numberlength)
{
	long	temp;

	if (number < 0)
		temp = (long)number * -1;
	else
		temp = (long)number;
	str[numberlength] = '\0';
	while (numberlength-- > 0)
	{
		str[numberlength] = temp % 10 + '0';
		temp /= 10;
	}
	return ;
}

char	*ft_itoa(int number)
{
	char	*str;
	int		numberlength;

	numberlength = number_length(number);
	str = fixed_malloc(number, numberlength);
	if (!str)
		return (NULL);
	if (number < 0)
	{
		str[0] = '-';
		number_to_string(str + 1, number, numberlength);
	}
	else
		number_to_string(str, number, numberlength);
	return (str);
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
    int fd = open(argv[1], O_RDONLY, 0644);
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
    // Reset the pointer of fd
    if (lseek(fd, 0, SEEK_SET) < 0)
    {
        printf("Error resetting the pointer of fd\n");
        close(fd);
        return (1);
    }
    // Compiles the file
    char *str_size = ft_itoa(size);
    if (!str_size)
    {
        printf("Error converting size to string\n");
        close(fd);
        return (1);
    }
    if (brainfuck_interpreter(str_size, fd) < 0)
    {
        printf("Error compiling\n");
        free(str_size);
        close(fd);
        return (1);
    }
    free(str_size);
    // Close the file
    close(fd);
    return (0);
}
