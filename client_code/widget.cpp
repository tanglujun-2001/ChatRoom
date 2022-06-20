#include "widget.h"
#include "ui_widget.h"
//发送包
void SendMessage(QTcpSocket *socket, int type, QString MyUser, QString ToUser, QString message)
{
    QString NeedSend;
    NeedSend += QString("Type:") + QString((char)(type + '0')) + QString("\n");
    NeedSend += QString("UserName:") + MyUser + QString("\n");
    NeedSend += QString("UserPassword:") + ToUser + QString("\n");
    NeedSend += QString("Message:") + message + QString("\n");
    socket->write(NeedSend.toUtf8());
    qDebug() << NeedSend << endl;
}

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //设置背景图
    QFrame *frametmp = new QFrame(this);
    frametmp->setFixedSize(400, 300);
    frametmp->setStyleSheet("border-image:url(:/res/bottombg2.png);");


    ui->setupUi(this);
    setFixedSize(400, 300);//设置固定登录页固定大小
    ui->Label_warn->hide();//设置警告标志默认隐藏

    //设置角标
    this->setWindowIcon(QIcon(":/res/Icon.png"));

    //建立服务器连接
    NewSocket = new QTcpSocket();
    NewSocket->connectToHost("175.178.119.39", 5051);//建立连接
    bool connected = NewSocket->waitForConnected(1000);//等待一秒看完成没
    if(!connected){//连接失败
        ui->Label_web->setStyleSheet("color:red");//设置为红色提示
        ui->Label_web->setText("服务器连接失败！");//提示
        ui->btn_dl->setEnabled(false);//将登录和注册按钮失能
        ui->btn_zc->setEnabled(false);
    }else{//连接成功
        ui->Label_web->setStyleSheet("color:green");
        ui->Label_web->setText("服务器连接成功！");
        connect(ui->LineEdit_pass, SIGNAL(returnPressed()), this, SLOT(CommitPass()));
        connect(ui->LineEdit_user, SIGNAL(returnPressed()), this, SLOT(CommitPass()));
        read_buffer = "";
        parse_index = line_end = 0;
        connect(ui->btn_dl, QPushButton::clicked, this, MyLogin);
        connect(ui->btn_zc, QPushButton::clicked, this, MyRegister);
        connect(NewSocket, SIGNAL(readyRead()), this, SLOT(process()));
    }
}
//注册
void Widget::MyRegister()
{
    QString UserName = ui->LineEdit_user->text();
    QString UserPassword = ui->LineEdit_pass->text();
    SendMessage(NewSocket, 0, UserName, UserPassword, QString(""));
}
//登录
void Widget::MyLogin()
{
    QString UserName = ui->LineEdit_user->text();
    QString UserPassword = ui->LineEdit_pass->text();
    SendMessage(NewSocket, 1, UserName, UserPassword, QString(""));
}
//解析行
int Widget::parse_line()
{
    int size = read_buffer.size();

    line_end = parse_index;
    while(line_end < size && read_buffer[line_end] != '\n') line_end ++;
    if(read_buffer[line_end] != '\n')   return 0;//响应报包的这一行不完整

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

//处理收到的数据

void Widget::process()
{
    //防止解析一半又来数据了
    disconnect(NewSocket, SIGNAL(readyRead()), this, SLOT(process()));
    read_buffer += NewSocket->readAll();//读取缓冲区响应数据
    qDebug() << read_buffer.toUtf8().data();
    while(true){
        int ret = parse_line();
        if(ret == 0){
            //响应包不完整
            break;
        } else if(ret == 1){
            //解析了一行
            continue;
        } else if(ret == 2){
            //完整响应包解析完毕，需要处理响应报文
            DealRead();
            //处理响应报文完毕，恢复read_buffer，方便下次数据的到来
            read_buffer = "";
            parse_index = line_end = 0;
            //如果登录成功直接return，就不会进行到下面的监听数据的连接了，改为主窗口进行数据读写
            if(Type == 1 && Result == 1)    return;
        } else{
            //识别失败(理论不会出现这种情况)
            break;
        }
    }
    //继续监听数据的到来
    connect(NewSocket, SIGNAL(readyRead()), this, SLOT(process()));
}
void Widget::DealRead()
{
    if(Type == 0){
        //注册
        if(Result == 0){
            //注册失败
            ui->Label_warn->setStyleSheet("color:red");
            ui->Label_warn->setText(Message);
            QTimer::singleShot(2000, ui->Label_warn, SLOT(hide()));
            ui->Label_warn->show();
        }else{
            //注册成功
            qDebug() << "成功";
            ui->Label_warn->setStyleSheet("color:green");
            ui->Label_warn->setText(Message);
            QTimer::singleShot(2000, ui->Label_warn, SLOT(hide()));
            ui->Label_warn->show();
        }
    }else if(Type == 1){
        if(Result == 0){
            //登录失败
            ui->Label_warn->setStyleSheet("color:red");
            ui->Label_warn->setText(Message);
            QTimer::singleShot(2000, ui->Label_warn, SLOT(hide()));
            ui->Label_warn->show();
        }else{
            //登录成功
            ui->Label_warn->hide();
            openMainWindow();

        }
    }
}

void Widget::openMainWindow()
{
    //传一个NewSocket是为了主窗口和服务器的通信
    MyWindow *MainWindow = new MyWindow(ui->LineEdit_user->text(), NewSocket);
    //主窗口叉掉就关闭本窗口，其实不用这个也可以，直接把this隐藏就行了，因为如果一个界面都没有show的话就会直接关闭
    connect(MainWindow, SIGNAL(ExitWin()), this, SLOT(close()));
    this->hide();//将登录页隐藏
    MainWindow->show();//显示主窗口
}

void Widget::CommitPass()
{
    ui->btn_dl->click();
}

Widget::~Widget()
{
    delete ui;
}
