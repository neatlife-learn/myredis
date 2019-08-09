#include <stdio.h>
#include <zconf.h>
#include "../src/ae.h"
#include "../src/anet.h"

char myerr[ANET_ERR_LEN] = {0};
void acceptFd(struct aeEventLoop *eventLoop, int fd, void *clientdata, int mask) {
    char myerr[ANET_ERR_LEN] = {0};
    printf("获取客户端连接的fd.\n");
    char ip[20] = {0};
    int port = 0;
    int clientfd = anetTcpAccept(myerr, fd, ip, sizeof(ip), &port);

    if (clientfd == AE_ERR) {
        printf("获取客户端连接的fd异常!! \n");
        return;
    }

    printf("客户端ip %s port %d \n", ip, port);

    int ret = anetNonBlock(myerr, clientfd);
    if (ret == ANET_OK) {
        printf("客户端事件非阻塞处理设置成功\n\n");
    }
    anetEnableTcpNoDelay(myerr, clientfd);
    // 设置客户端读写事件的服务端的处理函数, 一般创建客户端fd时，设置客户端fd的数据可读事件，然后在读取客户端数据的回调中再设置客户端数据的写事件
//    if(aeCreateFileEvent(eventLoop, clientfd, AE_READABLE, readFd, NULL)==AE_ERR){
//        close(fd);
//        return;
//    }
    write(clientfd,"Hello Client!\n",14);
}

int main() {
    aeEventLoop *eventLoop = aeCreateEventLoop(1024);
    if(!eventLoop){
        return 1;
    }
    int fd = anetTcpServer(myerr, 8080, "0.0.0.0", 511);

    if (fd != ANET_ERR) {
        anetNonBlock(NULL, fd);
        if (aeCreateFileEvent(eventLoop, fd, AE_READABLE, acceptFd, NULL)) {
            printf("注册tcp服务器接收客户端连接的事件处理函数异常");
        }
    }

    printf("服务器在 0.0.0.0:8080 端口监听了! \n");
    aeMain(eventLoop);
    aeDeleteEventLoop(eventLoop);
    return 0;
}
