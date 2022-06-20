/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_zc;
    QPushButton *btn_dl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_user;
    QLineEdit *LineEdit_user;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_pass;
    QLineEdit *LineEdit_pass;
    QLabel *Label_web;
    QLabel *Label_warn;
    QLabel *mag_label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QStringLiteral("Widget"));
        Widget->resize(400, 300);
        Widget->setStyleSheet(QStringLiteral(""));
        widget = new QWidget(Widget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(80, 230, 231, 41));
        widget->setStyleSheet(QStringLiteral(""));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btn_zc = new QPushButton(widget);
        btn_zc->setObjectName(QStringLiteral("btn_zc"));
        btn_zc->setMinimumSize(QSize(75, 30));

        horizontalLayout->addWidget(btn_zc);

        btn_dl = new QPushButton(widget);
        btn_dl->setObjectName(QStringLiteral("btn_dl"));
        btn_dl->setMinimumSize(QSize(75, 30));

        horizontalLayout->addWidget(btn_dl);

        widget_2 = new QWidget(Widget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(80, 120, 231, 41));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_user = new QLabel(widget_2);
        label_user->setObjectName(QStringLiteral("label_user"));
        QFont font;
        font.setPointSize(11);
        label_user->setFont(font);

        horizontalLayout_2->addWidget(label_user);

        LineEdit_user = new QLineEdit(widget_2);
        LineEdit_user->setObjectName(QStringLiteral("LineEdit_user"));
        LineEdit_user->setMinimumSize(QSize(130, 25));

        horizontalLayout_2->addWidget(LineEdit_user);

        widget_3 = new QWidget(Widget);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        widget_3->setGeometry(QRect(80, 170, 231, 61));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_pass = new QLabel(widget_3);
        label_pass->setObjectName(QStringLiteral("label_pass"));
        label_pass->setFont(font);

        horizontalLayout_3->addWidget(label_pass);

        LineEdit_pass = new QLineEdit(widget_3);
        LineEdit_pass->setObjectName(QStringLiteral("LineEdit_pass"));
        LineEdit_pass->setMinimumSize(QSize(130, 25));
        LineEdit_pass->setEchoMode(QLineEdit::Password);
        LineEdit_pass->setReadOnly(false);

        horizontalLayout_3->addWidget(LineEdit_pass);

        Label_web = new QLabel(Widget);
        Label_web->setObjectName(QStringLiteral("Label_web"));
        Label_web->setGeometry(QRect(300, 270, 91, 21));
        Label_warn = new QLabel(Widget);
        Label_warn->setObjectName(QStringLiteral("Label_warn"));
        Label_warn->setGeometry(QRect(90, 100, 211, 20));
        mag_label = new QLabel(Widget);
        mag_label->setObjectName(QStringLiteral("mag_label"));
        mag_label->setGeometry(QRect(290, 0, 111, 111));
        mag_label->setStyleSheet(QStringLiteral("image: url(:/res/xtu_mag.png);"));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "\347\231\273\345\275\225\347\252\227\345\217\243", Q_NULLPTR));
        btn_zc->setText(QApplication::translate("Widget", "\346\263\250\345\206\214", Q_NULLPTR));
        btn_dl->setText(QApplication::translate("Widget", "\347\231\273\345\275\225", Q_NULLPTR));
        label_user->setText(QApplication::translate("Widget", "\347\224\250\346\210\267\345\220\215\357\274\232", Q_NULLPTR));
        label_pass->setText(QApplication::translate("Widget", "\345\257\206  \347\240\201\357\274\232", Q_NULLPTR));
        LineEdit_pass->setText(QString());
        Label_web->setText(QApplication::translate("Widget", "\346\234\215\345\212\241\345\231\250\350\277\236\346\216\245\345\244\261\350\264\245\357\274\201", Q_NULLPTR));
        Label_warn->setText(QApplication::translate("Widget", "\345\257\206\347\240\201\351\224\231\350\257\257", Q_NULLPTR));
        mag_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
