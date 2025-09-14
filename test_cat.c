#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 131072 // pareil que dans le strace

int main(int argc, char **argv) {
    if (argc < 2) {
        write(2, "Usage: mycat <file>\n", 21);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char *buf = malloc(BUF_SIZE);
    if (!buf) {
        perror("malloc");
        close(fd);
        return 1;
    }

    ssize_t r;
    while ((r = read(fd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, r) != r) {
            perror("write");
            free(buf);
            close(fd);
            return 1;
        }
    }

    free(buf);
    close(fd);
    return 0;
}
