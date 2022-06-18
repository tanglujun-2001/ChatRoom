#ifndef QT_CONN_H
#define QT_CONN_H
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <vector>
#include <set>
#include "locker.h"
#include "sql_connection_pool.h"
#define MAX_BUFFER_SIZE 2048
#define MAX_FD 65536

class qt_conn
{
public:
    qt_conn() {}
    ~qt_conn() {}
    void init(int efd, int fd, qt_conn *usr, struct sockaddr_in);//客户端第一次连接之后进行初始化
    void init(bool einit);//客户端在处理完毕请求后进行初始化
    void easy_init();
    static void init_db_result(connection_pool *connPool);//初始化第一次读取数据库的数据存入map中，方便快速查询
    bool readall();//一次性读取客户端的请求
    bool writedata(int wfd);//一次性写入服务端生成的响应
    int parse_line();//解析行
    void DealRead();//处理客户端请求
    bool InsertMysql(int type, char *data1, char *data2);//向mysql表中插入数据
    bool DeleteMysql(int type, char *data1, char *data2);//删除mysql表中数据
    void CreateData(int destfd, int type, int res, string sname, string content);//生成服务端的响应
    void process();//每一个客户端的请求需要进行的总体步骤
    void leave();

    string UserName;
public:
    MYSQL *mysql;
    struct sockaddr_in clientaddr;

private:
    int epollfd;
    int clientfd;

    //客户端数据包解析有关信息
    int type;
    string UserPassword;
    string Message;
    char *charName;
    char *charPassword;
    char *charMessage;

    char read_buffer[MAX_BUFFER_SIZE];
    char write_buffer[MAX_BUFFER_SIZE];
    int read_index;
    int write_index;
    int NeedSend;
    int parse_index;//解析到哪里的下标
    int line_end;//每一行的结尾的下标在哪里
    qt_conn *user;//有这个东西，才能给其他客户端发包
};


#endif