#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

int main(int argc, char *argv[])
{
    if (argc <= 3)
    {
        printf("usage: %s ip_address port_number filename\n", basename(argv[0]));
        return 1;
    }
    // 接受IP地址
    const char *ip = argv[1];

    // 接受端口号
    int port = atoi(argv[2]);

    // 接受文件地址
    const char *file_name = argv[3];

    // 打开文件，获取文件句柄
    int filefd = open(file_name, O_RDONLY);
    assert(filefd > 0);

    // 通过文件描述符获取文件状态
    struct stat stat_buf;
    fstat(filefd, &stat_buf);

    // 初始化IP地址结构体
    struct sockaddr_in address;
    bzero(&address, sizeof(address));

    // 设置协议族
    address.sin_family = AF_INET;
    // 将IP地址字符串表示转化为网络字节序整数表示
    inet_pton(AF_INET, ip, &address.sin_addr);
    // 赋值端口
    address.sin_port = htons(port);

    // 创建socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    // 命名绑定socket到指定的地址
    int ret = bind(sock, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    // 开启监听并设置半连接队列为5
    ret = listen(sock, 5);
    assert(ret != -1);

    // 接受来自客户端的连接，并接受地址
    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &client_addrlength);

    if (connfd < 0)
    {
        printf("errno is: %d\n", errno);
    }
    else
    {
        // 发送文件
        sendfile(connfd, filefd, NULL, stat_buf.st_size);
        close(connfd);
    }

    close(sock);
    return 0;
}
