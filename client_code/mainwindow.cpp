#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "widget.h"

extern void SendMessage(QTcpSocket *socket, int type, QString MyUser, QString ToUser, QString message);

MyWindow::MyWindow(QString UserName, QTcpSocket *socket, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MyWindow)
{
    //设置背景图
    QFrame *frametmp = new QFrame(this);
    frametmp->setFixedSize(859, 686);

    //设置角标
    this->setWindowIcon(QIcon(":/res/Icon.png"));

    ui->setupUi(this);

    //设置主窗口大小
    setFixedSize(859, 686);

    //用户名初始化
    MyUserName = UserName;

    //通信套接字初始化
    MySocket = socket;

    //初始化聊天窗口为空（无当前聊天窗口）
    currentListWidget = NULL;


    //在线和离线好友数量初始化
    inline_friendcnt = unline_friendcnt = 0;
    ui->inline_cnt->setText("在线好友(" + QString(itoa(inline_friendcnt, inline_tmp, 10)) + "):");
    ui->unline_cnt->setText("离线好友(" + QString(itoa(unline_friendcnt, unline_tmp, 10)) + "):");

    //发送按钮初始化
    ui->pbt_sendmess->setText("发送");
    ui->pbt_sendmess->setEnabled(false);

    //聊天窗口提示信息初始化
    ui->friend_chatname->setText("欢迎使用QT聊天室");
    ui->friend_chatname->setStyleSheet("color:mediumslateblue;background-color: rgba(255,255,255, 0%);");
    ui->Label_UserName->setText(UserName);

    //处理好友添加请求的窗口初始化
    FriendAsk = new QDialog(this);
    FriendAsk->setWindowTitle("处理好友请求");
    FriendAsk->setFixedSize(300, 400);

    //处理好友添加请求的窗口内的listwidget初始化
    FriendAskList = new QListWidget(FriendAsk);
    FriendAskList->setStyleSheet("background-color: rgba(255,255,255, 20%);");
    FriendAskList->setFixedSize(300, 400);

    //处理好友添加请求提示信息初始化
    ui->friend_message->hide();

    //处理好友添加请求的按钮初始化
    ui->pbt_friendask->setText("暂无好友添加请求");
    ui->pbt_friendask->setEnabled(false);

    //创建群聊好友列表和群聊天框
    AddFriendToList(0, "[群聊]", true);
    CreateChatListWidget("[群聊]");

    //查找好友lineEdit初始化
    ui->LocalFindFriend->setPlaceholderText("输入好友名进行列表查找");

    //查找好友无相关好友的提示信息初始化
    ui->find_inlineno->setText("查询不到相关好友，请先添加好友");
    ui->find_unlineno->setText("查询不到相关好友，请先添加好友");
    ui->find_inlineno->setStyleSheet("color:blueviolet;background-color: rgba(255,255,255, 0%);");
    ui->find_unlineno->setStyleSheet("color:blueviolet;background-color: rgba(255,255,255, 0%);");
    ui->find_inlineno->hide();
    ui->find_unlineno->hide();


    //读取服务端数据的相关变量初始化
    parse_index = line_end = 0;
    read_buffer = "";

    //服务端数据到达与处理数据绑定
    connect(MySocket, SIGNAL(readyRead()), this, SLOT(process()));
    //添加好友按钮与添加好友函数绑定
    connect(ui->pbt_addfriend, QPushButton::clicked, this, AddFriend);
    //处理好友添加请求的按钮与相关函数绑定
    connect(ui->pbt_friendask, QPushButton::clicked, this, OpenFriendAskWindow);
    //发送消息按钮与相关函数绑定
    connect(ui->pbt_sendmess, QPushButton::clicked, this, SendMessToFriend);
    //发送消息框的回车处理与相关函数绑定
    connect(ui->SendEdit, SIGNAL(returnPressed()), this, SLOT(CommitMess()));
    //查找好友lineedit与相关函数绑定
    connect(ui->LocalFindFriend, SIGNAL(textChanged(QString)), this, SLOT(FindFriend(QString)));
}
//添加好友函数
void MyWindow::AddFriend()
{
    //创建添加好友窗口与初始化
    QDialog addfriend;
    addfriend.setWindowTitle("添加好友");
    addfriend.setFixedSize(250, 75);

    //设置窗口位置
    QRect rect = this->geometry();
    int x = rect.x() + rect.width()/2 - this->width()/2;
    int y = rect.y() + rect.height()/2 - this->height()/2;
    addfriend.move(x, y);

    //窗口内的发送按钮创建与初始化
    QPushButton addfriend_send("发送", &addfriend);
    addfriend_send.setGeometry(170, 20, 60, 25);

    //窗口内用户名编辑窗口创建与初始化
    QLineEdit friend_name(&addfriend);
    friend_name.setPlaceholderText("请输入用户名");
    friend_name.setGeometry(10, 20, 150, 25);

    //发送按钮绑定
    connect(&addfriend_send, QPushButton::clicked, [&](){
        qDebug() << "发送一个好友请求包";
        SendMessage(MySocket, 3, MyUserName, friend_name.text(), QString(""));
        addfriend.close();
    });

    addfriend.exec();
}

void MyWindow::process()
{
    //解除绑定，防止解析一半又来数据
    disconnect(MySocket, SIGNAL(readyRead()), this, SLOT(process()));

    //读取缓冲区数据
    read_buffer += MySocket->readAll();

    qDebug() << read_buffer.toUtf8().data();

    //处理读取的数据
    while(true){

        //单行解析
        int ret = parse_line();

        if(ret == 0){
            //识别到不完整的一行，响应包不完整，不处理此次读取的数据
            break;
        } else if(ret == 1){
            //完整解析了一行，但还没解析完读取的所有数据
            continue;
        } else if(ret == 2){
            //完整解析了一行，并且该行为一个完整的响应包的最后一行
            DealRead();
            //处理响应报文完毕，初始化数据读取相关变量，方便下次数据的到来，但可能一次性读取了多个响应包，因此需要判断
            if(parse_index >= read_buffer.size()){
                parse_index = line_end = 0;
                read_buffer = "";
            }
        } else{
            //识别失败(理论不会出现这种情况)
            break;
        }
    }
    //继续监听数据的到来
    connect(MySocket, SIGNAL(readyRead()), this, SLOT(process()));
}
//解析行
int MyWindow::parse_line()
{
    int size = read_buffer.size();

    line_end = parse_index;
    while(line_end < size && read_buffer[line_end] != '\n') line_end ++;
    if(read_buffer[line_end] != '\n')   return 0;//响应包的这一行不完整

    QString key = "", value = "";
    line_end = parse_index;
    while(line_end < size && read_buffer[line_end] != ':') line_end ++;
    for(int i = parse_index; i < line_end; i ++) key += read_buffer[i];

    parse_index = line_end + 1;  line_end = parse_index;
    while(line_end < size && read_buffer[line_end] != '\n')    line_end ++;
    for(int i = parse_index; i < line_end; i ++) value += read_buffer[i];
    parse_index = line_end + 1;

    if(key == "Type"){
        QByteArray tmp = value.toLatin1();
        Type = atoi(tmp.data());
        return 1;
    } else if(key == "Result"){
        QByteArray tmp = value.toLatin1();
        Result = atoi(tmp.data());
        return 1;
    } else if(key == "Sname"){
        Sname = value;
        return 1;
    }else if(key == "Message"){
        Message = value;
        return 2;
    }
    //识别失败了
    return -1;
}
void MyWindow::DealRead()
{
    if(Type == 2){
        //接收到好友消息
        QListWidgetItem *Item = new QListWidgetItem(ChatListWidgetMap[Sname]);
        Item->setText(QString("您的好友(") + Sname + QString("):") + Message);
        ChatListWidgetMap[Sname]->addItem(Item);
        if(Sname != GetChatName()){
            //如果当前聊天窗口不是消息发过来的窗口，那么提醒
            FriendItemWidgetLabel[Sname]->show();
        }

    } else if(Type == 3){
        //添加好友
        if(Result == 0){
            //添加好友失败
            //显示失败原因
            ui->friend_message->setText(Message);
            ui->friend_message->setStyleSheet("color:red;background-color: rgba(255,255,255, 0%);");
            ui->friend_message->show();
            QTimer::singleShot(2000, ui->friend_message, SLOT(hide()));//2秒后提示信息消失

        } else if(Result == 1 && Message == QString("好友请求发送成功")){
            //好友请求发送成功
            ui->friend_message->setText(Message);
            ui->friend_message->setStyleSheet("color:green;background-color: rgba(255,255,255, 0%);");//设置为绿色提示
            ui->friend_message->show();
            QTimer::singleShot(2000, ui->friend_message, SLOT(hide()));//2秒后提示信息消失

        } else if(Result == 1 && Message == QString("请求添加好友")){
            //有人请求添加好友
            CreateChatListWidget(Sname);
            DealFriendAsk();

        } else if(Result == 1 && Message == QString("在线好友添加成功")){
            //添加在线好友成功,将好友加到在线好友列表中
            CreateChatListWidget(Sname);//第一次将好友加入到列表中，需要创建一个聊天listwidget
            AddFriendToList(0, Sname);
            CreateInform(QString("添加好友(") + Sname + QString(")成功！"));
            inline_friendcnt ++;

        } else if(Result == 1 && Message == QString("好友已经上线")){
            //将好友从离线好友列表变到在线好友列表
            ChangeList(1, Sname);
            CreateInform(QString("好友(") + Sname + QString(")已上线, 快去找他聊天吧~"));
            unline_friendcnt --;
            inline_friendcnt ++;

        } else if(Result == 1 && Message == QString("离线好友添加成功")){
            //添加离线好友成功,将好友加到离线好友列表中
            CreateChatListWidget(Sname);//第一次将好友加入到列表中，需要创建一个聊天listwidget
            AddFriendToList(1, Sname);
            CreateInform(QString("添加好友(") + Sname + QString(")成功！"));
            unline_friendcnt ++;

        } else if(Result == 1 && Message == QString("好友已经下线")){
            //将好友从在线好友列表变到离线好友列表中
            ChangeList(0, Sname);
            CreateInform(QString("好友(") + Sname + QString(")已下线~"));
            inline_friendcnt --;
            unline_friendcnt ++;

        } else if(Result == 1 && Message == QString("您已拒绝好友添加")){
            CreateInform(QString("您已拒绝用户(") + Sname + QString(")的好友请求"));
        } else if(Result == 1 && Message == QString("对方拒绝您的好友添加")){
            CreateInform(QString("用户(") + Sname + QString(")拒绝了您的好友添加"));
        }
        //更新一下数量
        ui->inline_cnt->setText("在线好友(" + QString(itoa(inline_friendcnt, inline_tmp, 10)) + "):");
        ui->unline_cnt->setText("离线好友(" + QString(itoa(unline_friendcnt, unline_tmp, 10)) + "):");

    } else if(Type == 4){
        //如果消息发送者是自己，那么就直接continue，因为在发送消息的时候就已经加入到listwidget中了
        if(Sname == MyUserName) return;
        //群聊消息
        QListWidgetItem *Item = new QListWidgetItem(ChatListWidgetMap["[群聊]"]);
        Item->setText(QString("用户(") + Sname + QString("):") + Message);
        ChatListWidgetMap["[群聊]"]->addItem(Item);
        if("[群聊]" != GetChatName()){
            //如果当前聊天窗口不是消息发过来的窗口，那么提醒
            FriendItemWidgetLabel["[群聊]"]->show();
        }

    } else if(Type == 5){
        //删除好友
        DelFriend(Sname);
        if(Message == "好友删除成功"){
            CreateInform(QString("删除好友(") + Sname + QString(")成功"));
        } else if(Message == "您已被删除"){
            CreateInform(QString("您已被好友(") + Sname + QString(")删除"));
        }
        if(Sname == GetChatName()){
            currentListWidget = NULL;
            ui->friend_chatname->setText("欢迎使用QT聊天室");
            ui->pbt_sendmess->setEnabled(false);
        }
        //更新一下数量
        ui->inline_cnt->setText("在线好友(" + QString(itoa(inline_friendcnt, inline_tmp, 10)) + "):");
        ui->unline_cnt->setText("离线好友(" + QString(itoa(unline_friendcnt, unline_tmp, 10)) + "):");
    }
}
//接收到一个好友请求后
void MyWindow::DealFriendAsk()
{
    //设置处理好友请求的按钮
    ui->pbt_friendask->setEnabled(true);//设置处理好友请求的按钮可按
    ui->pbt_friendask->setText("有未处理的好友添加请求");
    ui->pbt_friendask->setStyleSheet("color:red");//设置字体颜色为红色

    //创建listwidget的一个item
    QListWidgetItem *itemFriend = new QListWidgetItem(FriendAskList);
    itemFriend->setText(Sname);

    //创建item关联的窗口
    QWidget *itemFriendwidget = new QWidget(FriendAsk);
    itemFriendwidget->setFixedSize(290, 30);

    //在关联的窗口创建两个按钮
    QPushButton *itemFriendpbt1 = new QPushButton("接受", itemFriendwidget);
    QPushButton *itemFriendpbt2 = new QPushButton("拒绝", itemFriendwidget);
    itemFriendpbt1->setFixedSize(45, 20);
    itemFriendpbt2->setFixedSize(45, 20);
    itemFriendpbt1->move(155, 4);
    itemFriendpbt2->move(205, 4);

    //设置item的宽和长
    itemFriend->setSizeHint(QSize(290, 30));
    //将item加入到listwidget中
    FriendAskList->addItem(itemFriend);
    //关联item和widget
    FriendAskList->setItemWidget(itemFriend, itemFriendwidget);
    //连接两个按钮
    connect(itemFriendpbt1, QPushButton::clicked, [=](){
        //接受
        SendMessage(MySocket, 3, MyUserName, itemFriend->text(), "请求添加好友");
        FriendAskList->removeItemWidget(itemFriend);//将该项移除
        delete itemFriend;//处理后回收资源
        delete itemFriendwidget;
        if(FriendAskList->count() == 0){
            //如果没有需要处理的好友请求，设置处理好友请求的按钮
            ui->pbt_friendask->setText("暂无好友添加请求");
            ui->pbt_friendask->setStyleSheet("color:grey");
            ui->pbt_friendask->setEnabled(false);
        }
    });
    connect(itemFriendpbt2, QPushButton::clicked, [=](){
        //拒绝
        SendMessage(MySocket, 3, MyUserName, itemFriend->text(), "拒绝添加好友");
        FriendAskList->removeItemWidget(itemFriend);
        delete itemFriend;//处理后回收资源
        delete itemFriendwidget;
        if(FriendAskList->count() == 0){
            //同上
            ui->pbt_friendask->setText("暂无好友添加请求");
            ui->pbt_friendask->setStyleSheet("color:grey");
            ui->pbt_friendask->setEnabled(false);
        }
    });
}

void MyWindow::OpenFriendAskWindow()
{
    FriendAsk->exec();
}

void MyWindow::SendMessToFriend()
{
    QString mess = ui->SendEdit->text();
    QString FriendName = GetChatName();
    if(FriendName != "[群聊]")    SendMessage(MySocket, 2, MyUserName, FriendName, mess);
    else    SendMessage(MySocket, 4, MyUserName, FriendName, mess);
    //将自己的消息加入到朋友的listwidget中
    QListWidget *FriendChatListWidget = ChatListWidgetMap[FriendName];//找到朋友的listwidget地址
    QListWidgetItem *FriendMess = new QListWidgetItem(FriendChatListWidget);
    FriendMess->setText(QString("您") + QString(":") + mess);
    FriendChatListWidget->addItem(FriendMess);
    ui->SendEdit->clear();//清空文字编辑框内容
}
//获取当前聊天的对象
QString MyWindow::GetChatName()
{
    QString tmp = ui->friend_chatname->text(), friendname = "";
    if(tmp == QString("欢迎使用QT聊天室")) return tmp;
    int index = 0;
    while(tmp[index] != ':') index ++;  index ++;
    while(index < tmp.size())   friendname += tmp[index ++];
    return friendname;
}

void MyWindow::CreateChatListWidget(QString DealName)
{
    //创建聊天listwidget，并用map保存该listwidget的地址
    QListWidget *ChatListWidget = new QListWidget(this);
    ChatListWidget->setStyleSheet("background-color: rgba(255,255,255, 40%);");
    ChatListWidget->move(459, 50);
    ChatListWidget->setFixedSize(391, 541);
    ChatListWidget->hide();
    ChatListWidgetMap[DealName] = ChatListWidget;
}

void MyWindow::AddFriendToList(int type, QString DealName, bool IsQ)
{
    //创建在线好友列表中的一项
    QListWidgetItem *Item;
    if(type == 0)   Item = new QListWidgetItem(ui->friend_list);
    else            Item = new QListWidgetItem(ui->unline_friend_list);
    Item->setSizeHint(QSize(230, 60));
    Item->setText(DealName);

    //创建一个与在线列表中一项相关联的窗口
    QWidget *ItemWidget = new QWidget;
    ItemWidget->setStyleSheet("background-color: rgba(255,255,255, 20%);");


    //设置在线好友的listwidget的item关联的窗口中的label
    QLabel *ItemWidgetLabel = new QLabel(QString("未读消息"), ItemWidget);
    ItemWidgetLabel->setFixedSize(100, 15);
    ItemWidgetLabel->move(90, 35);
    ItemWidgetLabel->setStyleSheet("color:red");
    ItemWidgetLabel->hide();


    //设置在线好友的listwidget的item关联的窗口中的按钮
    QPushButton *SendMess = new QPushButton("发送消息", ItemWidget);
    SendMess->setFixedSize(60, 20);
    SendMess->move(90, 12);
    connect(SendMess, QPushButton::clicked, [=](){
        //发送消息
        //先隐藏之前聊天listwidget
        if(currentListWidget != NULL)   currentListWidget->hide();
        //利用map寻找到聊天对象对应的listwidget
        currentListWidget = ChatListWidgetMap[DealName];
        //改变聊天对象标识
        ui->friend_chatname->setText(QString("当前聊天好友:") + DealName);
        //展示当前聊天listwidget
        currentListWidget->show();
        //发送按钮能按了
        ui->pbt_sendmess->setEnabled(true);
        //关闭提示信息
        FriendItemWidgetLabel[DealName]->hide();
    });
    if(!IsQ){
        QPushButton *DelFriend = new QPushButton("删除好友", ItemWidget);
        DelFriend->setFixedSize(60, 20);
        DelFriend->move(160, 12);
        connect(DelFriend, QPushButton::clicked, [=](){
            //删除好友
            SendMessage(MySocket, 5, MyUserName, DealName, "");
        });
    }

    FriendItemWidget[DealName] = ItemWidget;
    FriendItemWidgetLabel[DealName] = ItemWidgetLabel;
    //将item加入到listwidget中，并绑定窗口
    if(type == 0)   ui->friend_list->addItem(Item);
    else            ui->unline_friend_list->addItem(Item);
    if(type == 0)   ui->friend_list->setItemWidget(Item, ItemWidget);
    else            ui->unline_friend_list->setItemWidget(Item, ItemWidget);

}

void MyWindow::ChangeList(int type, QString DealName)
{
    int row = 0;
    while((type == 0 && row < ui->friend_list->count()) || (type == 1 && row < ui->unline_friend_list->count())){
        if(type == 0){
            if(DealName == ui->friend_list->item(row)->text()){
                //删除listwidget中的item
                ui->friend_list->takeItem(row);
                //回收item绑定的widget
                delete FriendItemWidget[DealName];
                //erase掉map中保存的数据
                FriendItemWidget.erase(FriendItemWidget.find(DealName));
                AddFriendToList(1, DealName);
            }
        }
        else{
            if(DealName == ui->unline_friend_list->item(row)->text()){
                //删除listwidget中的items
                ui->unline_friend_list->takeItem(row);
                //回收item绑定的widget
                delete FriendItemWidget[DealName];
                //erase掉map中保存的数据
                FriendItemWidget.erase(FriendItemWidget.find(DealName));
                AddFriendToList(0, DealName);
            }
        }
        row ++;
    }
}

void MyWindow::DelFriend(QString DealName)
{
    //删除好友列表中的相关数据
    int row = 0;
    while(row < ui->friend_list->count()){
        if(DealName == ui->friend_list->item(row)->text()){
            //删除好友列表中的item
            ui->friend_list->takeItem(row);
            //删除好友列表中的item绑定的widget
            delete FriendItemWidget[DealName];
            //erase掉map中的数据
            FriendItemWidget.erase(FriendItemWidget.find(DealName));
            //再删除聊天的listWidget
            delete ChatListWidgetMap[DealName];
            //erase掉map中的数据
            ChatListWidgetMap.erase(ChatListWidgetMap.find(DealName));
            //更新在线好友数量
            inline_friendcnt --;
            return;
        }
        row ++;
    }
    row = 0;
    while(row < ui->unline_friend_list->count()){
        if(DealName == ui->unline_friend_list->item(row)->text()){
            //删除离线好友列表的item
            ui->unline_friend_list->takeItem(row);
            //删除好友列表中的item绑定的widget
            delete FriendItemWidget[DealName];
            //erase掉map中的数据
            FriendItemWidget.erase(FriendItemWidget.find(DealName));
            //再删除聊天的listWidget
            delete ChatListWidgetMap[DealName];
            //erase掉map中的数据
            ChatListWidgetMap.erase(ChatListWidgetMap.find(DealName));
            //更新离线好友数量
            unline_friendcnt --;
            return;
        }
        row ++;
    }
}
//写通知
void MyWindow::CreateInform(QString Inform)
{
    QListWidgetItem *Item = new QListWidgetItem(ui->friend_flag_list);
    Item->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + ":\n" + Inform);
    ui->friend_flag_list->addItem(Item);
}

void MyWindow::CommitMess()
{
    qDebug() << "yes";
    if(ui->pbt_sendmess->isEnabled() == true)   SendMessToFriend();
}

void MyWindow::FindFriend(QString findname)
{
    QString SearchTarget = QString("*") + findname + QString("*");
    QList<QListWidgetItem*> NewInlineList, NewUnlineList;
    NewInlineList = ui->friend_list->findItems(SearchTarget, Qt::MatchWildcard);
    NewUnlineList = ui->unline_friend_list->findItems(SearchTarget, Qt::MatchWildcard);
    for(int i = 0; i < ui->friend_list->count(); i ++){
        ui->friend_list->setItemHidden(ui->friend_list->item(i), true);
    }
    for(int i = 0; i < ui->unline_friend_list->count(); i ++){
        ui->unline_friend_list->setItemHidden(ui->unline_friend_list->item(i), true);
    }
    for(int i = 0; i < (int)NewInlineList.size(); i ++){
        ui->friend_list->setItemHidden(NewInlineList[i], false);
    }
    for(int i = 0; i < (int)NewUnlineList.size(); i ++){
        ui->unline_friend_list->setItemHidden(NewUnlineList[i], false);
    }
    if(NewInlineList.size() == 0 && findname != QString(""))   ui->find_inlineno->show();
    else    ui->find_inlineno->hide();
    if(NewUnlineList.size() == 0 && findname != QString(""))   ui->find_unlineno->show();
    else    ui->find_unlineno->hide();
}

MyWindow::~MyWindow()
{
    ExitWin();
    delete ui;
}
