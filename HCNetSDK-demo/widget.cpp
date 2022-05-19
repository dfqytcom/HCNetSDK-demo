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
    BOOL ok = NET_DVR_Init();
    if (!ok) {
        qCritical() << "NET_DVR_Init failed.";
        return;
    }
    qDebug() << "NET_DVR_GetSDKVersion: " << QString::number(NET_DVR_GetSDKVersion(), 16);

    NET_DVR_USER_LOGIN_INFO loginInfo;
    strcpy(loginInfo.sDeviceAddress, ui->lineEdit_ip->text().toLatin1().data());
    strcpy(loginInfo.sUserName, ui->lineEdit_username->text().toLatin1().data());
    strcpy(loginInfo.sPassword, ui->lineEdit_password->text().toLatin1().data());
    loginInfo.bUseAsynLogin = FALSE;
    loginInfo.wPort = ui->spinBox_port->value();
    loginInfo.pUser = (void*)this;

    NET_DVR_DEVICEINFO_V30 deviceInfo;

    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    //m_handle = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
    m_handle = NET_DVR_Login_V30(ui->lineEdit_ip->text().toLatin1().data(),
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
    BOOL ok = NET_DVR_Logout(m_handle);
    if (ok) {
        qDebug() << "Logout successful.";
    } else {
        qWarning() << "Logout failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
    ok = NET_DVR_Cleanup();
    if (ok) {
        qDebug() << "Cleanup successful.";
    } else {
        qWarning() << "Cleanup failed. Error: " << QString(NET_DVR_GetErrorMsg());
    }
}

