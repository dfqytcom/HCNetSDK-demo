#include "widget.h"
#include "ui_widget.h"
#include <QtWidgets>

QTextEdit *g_textedit = nullptr;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    g_textedit = ui->textEdit_output;

    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_connect_clicked()
{
    qDebug() << "NET_DVR_GetSDKVersion: " << QString::number(NET_DVR_GetSDKVersion(), 16);

    NET_DVR_DEVICEINFO_V30 deviceInfo;
    m_dvrUserId = NET_DVR_Login_V30(ui->lineEdit_ip->text().toLatin1().data(),
                                 ui->spinBox_port->value(),
                                 ui->lineEdit_username->text().toLatin1().data(),
                                 ui->lineEdit_password->text().toLatin1().data(),
                                 &deviceInfo);
    if (NET_DVR_GetLastError()) {
        qCritical() << "Login failed. Error : " << QString(NET_DVR_GetErrorMsg());
        return;
    }
    qDebug() << "Login successful.";
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
}

void Widget::on_pushButton_disconnect_clicked()
{
    BOOL ok = NET_DVR_Logout(m_dvrUserId);
    if (ok) {
        qDebug() << "Logout successful.";
    } else {
        qWarning() << "Logout failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
}

