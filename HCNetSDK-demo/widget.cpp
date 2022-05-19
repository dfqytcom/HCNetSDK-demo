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
    qDebug() << "Login successful. User ID: " << m_dvrUserId;
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

void Widget::on_pushButton_capture_clicked()
{
    if (m_dvrUserId < 0) {
        qWarning() << "User doesn't login. plz login first.";
        return;
    }
    LONG channel = 1;
    NET_DVR_JPEGPARA param;
    param.wPicSize = 0xff;
    param.wPicQuality = 0;
    QString filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QDir::separator() + QUuid::createUuid().toString() + ".jpg";
    BOOL ok = NET_DVR_CaptureJPEGPicture(m_dvrUserId,  channel, &param, filename.toLocal8Bit().data());
    if (ok) {
        qInfo() << "Capture successful. The picture is saved to " << filename;
        QPixmap p(filename);
        p = p.scaled(ui->label_capture->width(), ui->label_capture->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->label_capture->setPixmap(p);
    } else {
        qWarning() << "Capture failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
}

