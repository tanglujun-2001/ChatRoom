#include "qt_conn.h"

map<string, string> user_map;
set<pair<string,string>> HaveBeenFriend;
set<pair<string,string>> friend_query;
map<string, int> user_inline;
int vct_cnt = 0, vct_cnt_query = 0;

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP;
    if (one_shot)   event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void modfd(int epollfd, int fd, int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLONESHOT | EPOLLET  | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}


void qt_conn::init(int efd, int fd, qt_conn *usr, struct sockaddr_in sockaddress){
    epollfd = efd;
    clientfd = fd;
    user = usr;
    clientaddr = sockaddress;
    init(true);
}
void qt_conn::init(bool einit){
    charName = NULL;
    charPassword = NULL;
    UserName = "";
    UserPassword = "";
    if(einit)   easy_init();
}
void qt_conn::easy_init()
{
    read_index = 0;
    write_index = 0;
    type = -1;
    charMessage = NULL;
    Message = "";
    NeedSend = 0;
    parse_index = 0;
    line_end = 0;
}

void qt_conn::init_db_result(connection_pool *connPool){
    //先从连接池中取一个连接
    MYSQL *mysql_user = NULL, *mysql_friend = NULL;
    connectionRAII mysqlcon_user(&mysql_user, connPool);
    connectionRAII mysqlcon_friend(&mysql_friend, connPool);

    //在QTdb中的users表中检索username，passwd数据
    if (mysql_query(mysql_user, "select username,passwd FROM users")){
        cout << "select失败！errno: %s" << mysql_error(mysql_user) << endl;
    }
    if(mysql_query(mysql_friend, "select username, f_usrname from friends")){
        cout << "select失败！errno: %s" << mysql_error(mysql_user) << endl;
    }
    //从表中检索完整的结果集
    MYSQL_RES *user_result = mysql_store_result(mysql_user);
    MYSQL_RES *friend_result = mysql_store_result(mysql_friend);
    //返回结果集中的列数
    int user_num_fields = mysql_num_fields(user_result);
    int friend_num_fields = mysql_num_fields(friend_result);

    //返回所有字段结构的数组
    MYSQL_FIELD *user_fields = mysql_fetch_fields(user_result);
    MYSQL_FIELD *friend_fields = mysql_fetch_field(friend_result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while(MYSQL_ROW user_row = mysql_fetch_row(user_result)){
        string tmp1(user_row[0]);
        string tmp2(user_row[1]);
        user_map[tmp1] = tmp2;
    }
    while(MYSQL_ROW friend_row = mysql_fetch_row(friend_result)){
        string tmp1(friend_row[0]);
        string tmp2(friend_row[1]);
        HaveBeenFriend.insert({tmp1, tmp2});
    }
}

bool qt_conn::readall(){
    while(true){
        int read_len = recv(clientfd, read_buffer + read_index, MAX_BUFFER_SIZE - read_index, 0);
        if(read_len == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK) break;
            return false;
        } else if(read_len == 0){
            return false;
        }
        read_index += read_len;
    }
    return true;
}
bool qt_conn::writedata(int wfd){
    if(NeedSend == 0){
        modfd(epollfd, clientfd, EPOLLIN);
        easy_init();
        return true;
    }

    while(true){
        int ret = write(wfd, write_buffer + write_index, NeedSend - write_index);
        if(ret == -1){
            if(errno == EAGAIN){
                //数据写完了
                break;
            }
            return false;
        }
        write_index += ret;
        NeedSend -= ret;
        if(NeedSend <= 0)   break;
    }
    modfd(epollfd, clientfd, EPOLLIN);
    easy_init();
    return true;
}
bool qt_conn::InsertMysql(int type, char *data1, char *data2){
    if(type == 0){
        char *mysql_ist = new char[1000];
        memset(mysql_ist, 0, sizeof mysql_ist);
        strcpy(mysql_ist, "insert into users(username, passwd) values(");
        strcat(mysql_ist, "'");
        strcat(mysql_ist, data1);
        strcat(mysql_ist, "', '");
        strcat(mysql_ist, data2);
        strcat(mysql_ist, "')");
        int ret = mysql_query(mysql, mysql_ist);
        if(ret) return true;
        return false;
    }else{
        char *mysql_ist = new char[1000];
        memset(mysql_ist, 0, sizeof mysql_ist);
        strcpy(mysql_ist, "insert into friends(username, f_usrname) values(");
        strcat(mysql_ist, "'");
        strcat(mysql_ist, data1);
        strcat(mysql_ist, "', '");
        strcat(mysql_ist, data2);
        strcat(mysql_ist, "')");
        int ret = mysql_query(mysql, mysql_ist);
        if(ret) return true;
        return false;
    }
}
bool qt_conn::DeleteMysql(int type, char *data1, char *data2){
    if(type == 0){
        //删除friend表中数据
        char *mysql_ist = new char[1000];
        memset(mysql_ist, 0, sizeof mysql_ist);
        strcpy(mysql_ist, "delete from friends where username = ");
        strcat(mysql_ist, "'");
        strcat(mysql_ist, data1);
        strcat(mysql_ist, "' and f_usrname = '");
        strcat(mysql_ist, data2);
        strcat(mysql_ist, "';");
        int ret = mysql_query(mysql, mysql_ist);
        if(ret) return true;
        return false;
    }
    return true;
}
int qt_conn::parse_line(){
    line_end = parse_index;
    string key = "", value = "";
    while(line_end < read_index && read_buffer[line_end] != '\n')   line_end ++;
    if(read_buffer[line_end] != '\n')   return 0;//不够一行   

    line_end = parse_index;
    while(line_end < read_index && read_buffer[line_end] != ':')     line_end ++;
    for(int i = parse_index; i < line_end; i ++)    key += read_buffer[i];
    read_buffer[line_end] = '\0';
    parse_index = line_end + 1;
    while(line_end < read_index && read_buffer[line_end] != '\n')    line_end ++;
    for(int i = parse_index; i < line_end; i ++)    value += read_buffer[i];
    read_buffer[line_end] = '\0';
    int value_index = parse_index;
    parse_index = line_end + 1;//转到下一行去

    if(key == "Type"){
        type = atoi(value.data());
        return 1;
    }else if(key == "UserName"){
        UserName = value;
        charName = read_buffer + value_index;
        return 1;
    }else if(key == "UserPassword"){
        UserPassword = value;
        charPassword = read_buffer + value_index;
        return 1;
    }else{
        Message = value;
        charMessage = read_buffer + value_index;
        return 2;
    }
}
void qt_conn::DealRead(){
    if(type == 0){
        if(UserName == "" || UserPassword == ""){
            CreateData(clientfd, 0, 0, "server", "用户名或密码不能为空");
            init(false);//注册或登录失败都不需要保存用户状态
            return;
        }
        //客户端注册
        if(user_map.count(UserName)){
            //存在相同的用户名，不予注册
            CreateData(clientfd, 0, 0, "server", "用户名已被注册！");
            init(false);
        }else{
            //不存在相同的用户名
            if(strlen(charName) > 30){
                CreateData(clientfd, 0, 0, "server", "注册失败！(用户名字节数大于规定的字节:30)");
                return;
            }
            user_map[UserName] = UserPassword;
            InsertMysql(0, charName, charPassword);
            CreateData(clientfd, 0, 1, "server", "注册成功");
            cout << "用户(" << UserName << ")注册成功" << endl;
            init(false);//注册或登录失败都不需要保存用户状态
        }
    }else if(type == 1){
        //客户端登陆
        if(UserName == "" || UserPassword == ""){
            CreateData(clientfd, 1, 0, "server", "用户名或密码不能为空");
            init(false);
            return;
        }
        if(user_map.count(UserName)){
            if(user_map[UserName] == UserPassword){
                //密码正确
                if(user_inline.count(UserName)){
                    //用户已经登录
                    CreateData(clientfd, 1, 0, "server", "用户已登录，请勿重复登录");
                    init(false);
                    return;
                }
                user_inline[UserName] = clientfd;
                CreateData(clientfd, 1, 1, "server", "登录成功！");//用户是看不到这个消息的，因为跳转窗口了
                cout << "用户(" << UserName << ")登录成功" << endl;   
                //将好友的信息发给客户端
                for(auto it : HaveBeenFriend){
                    if(it.first != UserName)    continue;
                    if(user_inline.count(it.second)){
                        CreateData(clientfd, 3, 1, it.second,"在线好友添加成功");
                        CreateData(user_inline[it.second], 3, 1, UserName, "好友已经上线");
                    }
                    else{
                        CreateData(clientfd, 3, 1, it.second, "离线好友添加成功");
                        //自己的好友不在线就不发了
                    }    
                }
                //将好友添加的信息发给客户端
                for(auto it : friend_query){
                    if(it.second != UserName)   continue;
                    CreateData(clientfd, 3, 1, it.first, "请求添加好友");
                }
            }else{
                //密码错误
                CreateData(clientfd, 1, 0, "server", "密码错误！");
                init(false);
            }
        }else{
            //不存在用户
            CreateData(clientfd, 1, 0, "server", "用户名不存在！");
            init(false);
        }
    }else if(type == 2){
        //客户端发送消息给好友
        if(Message == ""){
            CreateData(clientfd, 2, 0, UserPassword, "消息不能为空");
            return;
        }
        if(!HaveBeenFriend.count({UserName, UserPassword})){
            //对方不是好友
            CreateData(clientfd, 2, 0, UserPassword, "对方不是你的好友");
            return;
        }
        if(!user_inline.count(UserPassword)){
            //好友不在线
            CreateData(clientfd, 2, 0, UserPassword, "好友不在线");
            return;
        }
        CreateData(user_inline[UserPassword], 2, 1, UserName, Message);
    }else if(type == 3){
        //客户端请求添加好友
        if(UserPassword == ""){
            CreateData(clientfd, 3, 0, "server", "用户名为空");
            return;
        }
        if(UserName == UserPassword){
            CreateData(clientfd, 3, 0, "server", "不能添加自己为朋友");
            return;
        }
        if(!user_map.count(UserPassword)){
            CreateData(clientfd, 3, 0, UserPassword, "用户不存在");
            return;
        }

        if(HaveBeenFriend.count({UserName, UserPassword}) ||
           HaveBeenFriend.count({UserPassword, UserName})){
            //对方已经是好友
            CreateData(clientfd, 3, 0, UserPassword, "对方已经是你的好友");
        }else{
            if(friend_query.count({UserPassword, UserName})){
                //对方已经发起了好友请求
                if(Message == "拒绝添加好友"){
                    friend_query.erase({UserPassword, UserName});
                    CreateData(clientfd, 3, 1, UserPassword, "您已拒绝好友添加");
                    CreateData(user_inline[UserPassword], 3, 1, UserName, "对方拒绝您的好友添加");
                    return;
                }
                InsertMysql(1, charName, charPassword);
                InsertMysql(1, charPassword, charName);
                if(user_inline.count(UserPassword))   CreateData(clientfd, 3, 1, UserPassword, "在线好友添加成功");
                else    CreateData(clientfd, 3, 1, UserPassword, "离线好友添加成功");
                if(user_inline.count(UserName))       CreateData(user_inline[UserPassword], 3, 1, UserName, "在线好友添加成功");
                else    CreateData(user_inline[UserPassword], 3, 1, UserName, "离线好友添加成功");
                friend_query.erase({UserPassword, UserName});
                HaveBeenFriend.insert({UserName, UserPassword});
                HaveBeenFriend.insert({UserPassword, UserName});
            }else{
                //对方还没发起好友请求
                if(friend_query.count({UserName, UserPassword})){
                    //如果已经发送过好友请求，那么就返回一个错误
                    CreateData(clientfd, 3, 0, UserPassword, "请勿重复进行好友申请");
                    return;
                }
                friend_query.insert({UserName, UserPassword});
                CreateData(clientfd, 3, 1, UserPassword, "好友请求发送成功");
                if(user_inline.count(UserPassword)){
                    //好友在线就会接收到通知
                    CreateData(user_inline[UserPassword], 3, 1, UserName, "请求添加好友");
                }
            }
        }
    }else if(type == 4){
        //发群聊天
        if(Message == ""){
            CreateData(clientfd, 4, 0, "server", "消息不能为空");
            return;
        }
        string MessageTmp = Message;
        for(auto it : user_inline){
            CreateData(it.second, 4, 1, UserName, MessageTmp);
        }
    }else if(type == 5){
        //删除好友
        DeleteMysql(0, charName, charPassword);
        DeleteMysql(0, charPassword, charName);
        HaveBeenFriend.erase({UserName, UserPassword});
        HaveBeenFriend.erase({UserPassword, UserName});
        CreateData(clientfd, 5, 1, UserPassword, "好友删除成功");
        if(user_inline.count(UserPassword)){
            //好友在线，发送通知给好友
            CreateData(user_inline[UserPassword], 5, 1, UserName, "您已被删除");
        }
    }
}
void qt_conn::CreateData(int destfd, int type, int res, string sname, string content){
    write_index = 0;
    string tmp = "Type:";
    tmp += (char)(type + '0');
    tmp += "\n";
    tmp += "Result:";
    tmp += (char)(res + '0');
    tmp += "\n";
    tmp += "Sname:";
    tmp += sname;
    tmp += "\n";
    tmp += "Message:";
    tmp += content + "\n";
    for(int i = 0; i < (int)tmp.size(); i ++){
        write_buffer[i] = tmp[i];
    }
    NeedSend = tmp.size();
    writedata(destfd);
}
void qt_conn::process(){
    int ret = readall();
    if(!ret){
        leave();
        return;
    }

    while(1){
        int ret = parse_line();
        if(ret == 0){
            //数据没有传送完毕,报文不完整
            modfd(epollfd, clientfd, EPOLLIN);
            break;
        }else if(ret == 1){
            //完整解析了一行，但是还没解析完完整的一个请求包
            continue;
        }else{
            //完整解析了一个请求包
            DealRead();//去处理这个请求包
            break;
        }
    }
}

void qt_conn::leave(){
    cout << "客户端(ip:" << inet_ntoa(clientaddr.sin_addr);
    if(UserName == "") cout << " 未登录";
    else    cout << " 用户名:" << UserName;
    cout << ")已下线" << endl;

    if(UserName == "")  return;
    user_inline.erase(UserName);
    if(UserName != ""){
        for(auto it : HaveBeenFriend){
            if(it.first != UserName)    continue;
            if(user_inline.count(it.second)){
                CreateData(user_inline[it.second], 3, 1, UserName, "好友已经下线");
            }
        }
    }
    close(clientfd);
}
