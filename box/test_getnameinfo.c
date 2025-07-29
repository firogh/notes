#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(void) {
    struct sockaddr_in sa;
        struct sockaddr_in6 s6;
    char node[NI_MAXHOST];
    memset(&sa, 0, sizeof sa);
    memset(&s6, 0, sizeof s6);
    sa.sin_family = AF_INET;
        s6.sin6_family = AF_INET6;
    inet_pton(AF_INET, "8.8.8.8", &sa.sin_addr);
    /* google-public-dns-a.google.com */
        inet_pton(AF_INET6, "::ffff:147.178.32.252", &s6.sin6_addr);

    int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa),
                          node, sizeof(node),
                          NULL, 0, NI_NAMEREQD);

    if (res) {
        printf("error: %d\n", res);
        printf("%s\n", gai_strerror(res));
    }
    else
        printf("node=%s\n", node);
    res = getnameinfo((struct sockaddr*)&s6, sizeof(s6),
                          node, sizeof(node),
                          NULL, 0, NI_NAMEREQD);

    if (res) {
        printf("error: %d\n", res);
        printf("%s\n", gai_strerror(res));
    }
    else
        printf("node=%s\n", node);
       return 0;
}
