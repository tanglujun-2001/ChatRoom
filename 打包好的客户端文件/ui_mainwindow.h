/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyWindow
{
public:
    QPushButton *pbt_addfriend;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *Label_user;
    QLabel *Label_UserName;
    QListWidget *friend_list;
    QLabel *friend_message;
    QPushButton *pbt_friendask;
    QListWidget *chatwidget;
    QPushButton *pbt_sendmess;
    QLabel *friend_chatname;
    QListWidget *unline_friend_list;
    QLabel *unline_cnt;
    QLabel *inline_cnt;
    QListWidget *friend_flag_list;
    QLineEdit *SendEdit;
    QLineEdit *LocalFindFriend;
    QLabel *find_inlineno;
    QLabel *find_unlineno;
    QLabel *iconlabel;

    void setupUi(QWidget *MyWindow)
    {
        if (MyWindow->objectName().isEmpty())
            MyWindow->setObjectName(QStringLiteral("MyWindow"));
        MyWindow->resize(859, 686);
        MyWindow->setMinimumSize(QSize(0, 0));
        MyWindow->setStyleSheet(QStringLiteral("background-color: rgba(206,239,228,60%);"));
        pbt_addfriend = new QPushButton(MyWindow);
        pbt_addfriend->setObjectName(QStringLiteral("pbt_addfriend"));
        pbt_addfriend->setGeometry(QRect(10, 50, 91, 31));
        widget = new QWidget(MyWindow);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 10, 150, 30));
        widget->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 0%);"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        Label_user = new QLabel(widget);
        Label_user->setObjectName(QStringLiteral("Label_user"));

        horizontalLayout->addWidget(Label_user);

        Label_UserName = new QLabel(widget);
        Label_UserName->setObjectName(QStringLiteral("Label_UserName"));

        horizontalLayout->addWidget(Label_UserName);

        friend_list = new QListWidget(MyWindow);
        friend_list->setObjectName(QStringLiteral("friend_list"));
        friend_list->setGeometry(QRect(220, 130, 230, 261));
        friend_list->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        friend_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        friend_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        friend_message = new QLabel(MyWindow);
        friend_message->setObjectName(QStringLiteral("friend_message"));
        friend_message->setGeometry(QRect(110, 40, 131, 31));
        friend_message->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 0%);"));
        pbt_friendask = new QPushButton(MyWindow);
        pbt_friendask->setObjectName(QStringLiteral("pbt_friendask"));
        pbt_friendask->setGeometry(QRect(170, 10, 171, 31));
        chatwidget = new QListWidget(MyWindow);
        chatwidget->setObjectName(QStringLiteral("chatwidget"));
        chatwidget->setGeometry(QRect(459, 10, 391, 581));
        chatwidget->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        pbt_sendmess = new QPushButton(MyWindow);
        pbt_sendmess->setObjectName(QStringLiteral("pbt_sendmess"));
        pbt_sendmess->setGeometry(QRect(790, 600, 61, 81));
        pbt_sendmess->setStyleSheet(QStringLiteral(""));
        friend_chatname = new QLabel(MyWindow);
        friend_chatname->setObjectName(QStringLiteral("friend_chatname"));
        friend_chatname->setGeometry(QRect(470, 20, 161, 20));
        friend_chatname->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 0%);"));
        unline_friend_list = new QListWidget(MyWindow);
        unline_friend_list->setObjectName(QStringLiteral("unline_friend_list"));
        unline_friend_list->setGeometry(QRect(220, 410, 231, 271));
        unline_friend_list->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        unline_friend_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        unline_friend_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        unline_cnt = new QLabel(MyWindow);
        unline_cnt->setObjectName(QStringLiteral("unline_cnt"));
        unline_cnt->setGeometry(QRect(220, 390, 231, 20));
        unline_cnt->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 0%);"));
        inline_cnt = new QLabel(MyWindow);
        inline_cnt->setObjectName(QStringLiteral("inline_cnt"));
        inline_cnt->setGeometry(QRect(220, 110, 101, 16));
        inline_cnt->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 0%);"));
        friend_flag_list = new QListWidget(MyWindow);
        friend_flag_list->setObjectName(QStringLiteral("friend_flag_list"));
        friend_flag_list->setGeometry(QRect(10, 90, 201, 591));
        friend_flag_list->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        SendEdit = new QLineEdit(MyWindow);
        SendEdit->setObjectName(QStringLiteral("SendEdit"));
        SendEdit->setGeometry(QRect(460, 600, 321, 81));
        SendEdit->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        LocalFindFriend = new QLineEdit(MyWindow);
        LocalFindFriend->setObjectName(QStringLiteral("LocalFindFriend"));
        LocalFindFriend->setGeometry(QRect(220, 90, 231, 20));
        LocalFindFriend->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        find_inlineno = new QLabel(MyWindow);
        find_inlineno->setObjectName(QStringLiteral("find_inlineno"));
        find_inlineno->setGeometry(QRect(230, 130, 221, 31));
        find_inlineno->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        find_unlineno = new QLabel(MyWindow);
        find_unlineno->setObjectName(QStringLiteral("find_unlineno"));
        find_unlineno->setGeometry(QRect(230, 410, 221, 31));
        find_unlineno->setStyleSheet(QStringLiteral("background-color: rgba(255,255,255, 40%);"));
        iconlabel = new QLabel(MyWindow);
        iconlabel->setObjectName(QStringLiteral("iconlabel"));
        iconlabel->setGeometry(QRect(350, -10, 111, 111));
        iconlabel->setStyleSheet(QLatin1String("border-image: url(:/res/xtu_mag.png);\n"
"background-color: rgba(255,255,255, 0%);"));

        retranslateUi(MyWindow);

        QMetaObject::connectSlotsByName(MyWindow);
    } // setupUi

    void retranslateUi(QWidget *MyWindow)
    {
        MyWindow->setWindowTitle(QApplication::translate("MyWindow", "\350\201\212\345\244\251\345\256\244", Q_NULLPTR));
        pbt_addfriend->setText(QApplication::translate("MyWindow", "\346\267\273\345\212\240\345\245\275\345\217\213", Q_NULLPTR));
        Label_user->setText(QApplication::translate("MyWindow", "\345\275\223\345\211\215\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        Label_UserName->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        friend_message->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        pbt_friendask->setText(QApplication::translate("MyWindow", "PushButton", Q_NULLPTR));
        pbt_sendmess->setText(QApplication::translate("MyWindow", "PushButton", Q_NULLPTR));
        friend_chatname->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        unline_cnt->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        inline_cnt->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        find_inlineno->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        find_unlineno->setText(QApplication::translate("MyWindow", "TextLabel", Q_NULLPTR));
        iconlabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MyWindow: public Ui_MyWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
