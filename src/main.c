#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int const  LOWERCASE_MIN     = 96;
static int const  LOWERCASE_MAX     = 123;
static char const DISTANCE_TO_OTHER = 32;
static int const  UPPERCASE_MIN     = 64;
static int const  UPPERCASE_MAX     = 91;
static int const  MODE_PERMISSION   = 0644;

char  upper_filter(char c);
char  lower_filter(char c);
char  null_filter(char c);
char *transform(char *c, char (*transformation)(char));

void usage(void);

char upper_filter(char c)
{
    if(c > LOWERCASE_MIN && c < LOWERCASE_MAX)
    {
        c = (char)(c - DISTANCE_TO_OTHER);
    }
    return c;
}

char lower_filter(char c)
{
    if(c > UPPERCASE_MIN && c < UPPERCASE_MAX)
    {
        c = (char)(c + DISTANCE_TO_OTHER);
    }
    return c;
}

char null_filter(char c)
{
    return c;
}

char *transform(char *c, char (*transformation)(char))
{
    if(c != NULL)
    {
        c[0] = transformation(c[0]);
    }

    return c;
}

void usage(void)
{
    puts("Usage: ./main [-i] [-o] [-f]\nOptions:\n\t-i Input file\n\t-o Output file\n\t-f Transform function to be called\n");
}

int main(int argc, char *argv[])
{
    int chara;
    char (*transform_func)(char) = null_filter;
    char *c                      = NULL;
    int   argcount               = 0;
    char *arguments[3]           = {NULL, NULL, NULL};

    while((chara = getopt(argc, argv, "i:o:f:")) != -1)
    {
        switch(chara)
        {
            case 'i':

                arguments[0] = optarg;
                argcount++;

                if(arguments[0] == NULL)
                {
                    perror("NO INPUT ARGUMENT");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o':

                arguments[1] = optarg;
                argcount++;
                break;
            case 'f':

                argcount++;

                if(strcmp(optarg, "upper") == 0)
                {
                    transform_func = upper_filter;
                }
                else if(strcmp(optarg, "lower") == 0)
                {
                    transform_func = lower_filter;
                }
                else if(strcmp(optarg, "null") == 0)
                {
                    transform_func = null_filter;
                }
                else
                {
                    transform_func = null_filter;
                    fprintf(stderr, "Invalid function argument. Text will not be unchanged\n");
                }
                break;

            case '?':
                usage();
                exit(EXIT_FAILURE);

            default:
                exit(EXIT_FAILURE);
        }
    }

    if(argcount != 3)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    if(arguments[0] != NULL && arguments[1] != NULL)
    {
        int     fd;
        int     fd2;
        ssize_t sz;

        fd = open(arguments[0], O_RDWR | O_CLOEXEC | MODE_PERMISSION);

        if(fd == -1)
        {
            perror("FAILED TO OPEN FILE");
            exit(EXIT_FAILURE);
        }

        fd2 = open(arguments[1], O_RDWR | O_CREAT | O_TRUNC | MODE_PERMISSION | O_CLOEXEC);

        if(fd2 == -1)
        {
            perror("FAILED TO OPEN OUTPUT FILE");
            exit(EXIT_FAILURE);
        }

        c = (char *)calloc(1, sizeof(char));

        while((sz = read(fd, c, 1)) > 0)
        {
            size_t bytes_to_write;
            bytes_to_write = (size_t)sz;
            c              = transform(c, transform_func);
            write(fd2, c, bytes_to_write);
        }

        free(c);
    }

    return 0;
}
