#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

// error handling function ; print error and abort
static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main()
{
    // get fd for client socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket()");
    }

    // setting up server address
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = ntohs(1234); // set port number
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
    // connect to server
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("connect");
    }

    // send message to server
    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64] = {};
    // read reply from server
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        die("read");
    }
    printf("server says: %s\n", rbuf);
    close(fd);
    return 0;
}
