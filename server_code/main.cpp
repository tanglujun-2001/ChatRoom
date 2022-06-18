#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <string.h>
#include "sql_connection_pool.h"
#include "threadpool.h"
#include "qt_conn.h"
using namespace  std;

#define MAX_EVENT_NUMBER 10000
#define MAX_FD 65536
int epollfd;
int stop_server;
qt_conn *user;
extern int setnonblocking(int fd);
//将需要监听的事件添加到EPOLL树中
extern void addfd(int epollfd, int fd, bool one_shot);
extern void modfd(int epollfd, int fd, int ev);

int main(int argc, char *argv[])
{
    if(argc == 1){
        cout << "未指定端口！" << endl;
        return 0;
    }
    //指定端口
    uint16_t port = atoi(argv[1]);
    
    //创建数据库连接池
    connection_pool *connPool = connection_pool::GetInstance();
    connPool->init("localhost", "root", "Qq431126", "QTdb", 3306, 8);

    threadpool<qt_conn> *pool = NULL;
    pool = new threadpool<qt_conn>(connPool);

    
    int ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int flag = 1;
    //设置端口复用，防止服务端重启导致time_wait的端口占用
    setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(bind(ListenSocket, (struct sockaddr *)&address, sizeof address) == -1){
        cout << "绑定失败，原因可能是端口已被占用！" << endl;
        return 0;
    }
    if(listen(ListenSocket, 5) == -1){
        cout << "监听失败！" << endl;
        return 0;
    }

    //创建内核事件表
    epoll_event events[MAX_EVENT_NUMBER];
    epollfd = epoll_create(5);
    if(epollfd == -1){
        cout << "创建EPOLL树根节点失败！" << endl;
        return 0;
    }
    addfd(epollfd, ListenSocket, false);

    //预先创建好所有的qt_conn，每一个表示相应fd的客户端连接
    user = new qt_conn[MAX_FD];
    user->init_db_result(connPool);//初始化数据库中的数据

    while(!stop_server){
        int num = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        for(int i = 0; i < num; i ++){
            int sockfd = events[i].data.fd;
            if(sockfd == ListenSocket){
                //新的客户端来了
                struct sockaddr_in clientaddress;
                socklen_t clientaddr_len = sizeof clientaddress;
                int clientfd = accept(ListenSocket, (struct sockaddr *)&clientaddress, &clientaddr_len);
                cout << "一个新的客户端已连接(ip:" << inet_ntoa(clientaddress.sin_addr) << ")" << endl;
                user[clientfd].init(epollfd, clientfd, user, clientaddress);//初始化客户端连接
                addfd(epollfd, clientfd, false);

            } else if(events[i].events & (EPOLLRDHUP | EPOLLHUP)){
                //客户端下线了
                user[sockfd].leave();
    
            } else if(events[i].events & (EPOLLIN)){
                //客户端消息来了
                pool->append(user + sockfd);//reactor模式下让工作线程去读写数据
            }
        }
    }
    return 0;
}
