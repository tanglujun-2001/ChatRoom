#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QTcpSocket>
#include <cstdlib>
#include <QKeyEvent>
#include <QEvent>
#include <QIcon>
#include "mainwindow.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    int parse_line();
    void openMainWindow();
    ~Widget();


public slots:
    //登录
    void MyLogin();
    //注册
    void MyRegister();
    //处理读取的数据
    void DealRead();
    void process();
    void CommitPass();

private:
    Ui::Widget *ui;
    QTcpSocket *NewSocket;

private:
    int Type;
    int Result;
    QString Sname;
    QString Message;
    QString read_buffer;
    int parse_index;
    int line_end;
};

#endif // WIDGET_H
