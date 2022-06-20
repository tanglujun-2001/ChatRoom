#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QTcpSocket>
#include <cstdlib>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QTimer>
#include <QScroller>
#include <QMap>
#include <QDateTime>
#include <QKeyEvent>

namespace Ui {
class MyWindow;
}
//主窗口的类
class MyWindow: public QWidget
{
    Q_OBJECT
public:
    explicit MyWindow(QString UserName, QTcpSocket *socket, QWidget *parent = 0);
    QString GetChatName();
    void CreateChatListWidget(QString DealName);
    void AddFriendToList(int type, QString DealName, bool IsQ = false);
    void ChangeList(int type, QString DealName);
    void DelFriend(QString DealName);
    void CreateInform(QString Inform);
    ~MyWindow();
signals:
    void ExitWin();

public slots:
    void AddFriend();
    void process();
    int parse_line();
    void DealRead();
    void DealFriendAsk();
    void OpenFriendAskWindow();
    void SendMessToFriend();
    void CommitMess();
    void FindFriend(QString findname);

private:
    Ui::MyWindow *ui;
    QTcpSocket *MySocket;
    QString MyUserName;

    int Type;
    int Result;
    QString Sname;
    QString Message;
    QString read_buffer;
    int parse_index;
    int line_end;

    QDialog *FriendAsk;
    QListWidget *FriendAskList;

    int inline_friendcnt;
    int unline_friendcnt;
    char inline_tmp[10];
    char unline_tmp[10];

    QMap<QString, QListWidget*> ChatListWidgetMap;//保存每一个聊天的listwidget地址
    QMap<QString, QWidget*> FriendItemWidget;//保存好友列表中每一个item关联的widget的地址
    QMap<QString, QLabel*> FriendItemWidgetLabel;//保存好友列表中每一个item关联的widget中的label
    QListWidget *currentListWidget;
};
#endif // MAINWINDOW_H
