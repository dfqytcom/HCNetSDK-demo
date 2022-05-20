#include "widget.h"
#include "ui_widget.h"
#include <thread>
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
    ui->pushButton_startRecord->setEnabled(true);
    ui->pushButton_endRecord->setEnabled(false);
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
        qCritical() << "Capture failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
}

void Widget::on_pushButton_startRecord_clicked()
{
    if (m_dvrUserId < 0) {
        qWarning() << "User doesn't login. plz login first.";
        return;
    }
    LONG channel = 1;
    NET_DVR_PREVIEWINFO StruPlayInfo;
    StruPlayInfo.hPlayWnd = NULL;
    StruPlayInfo.lChannel = channel;
    StruPlayInfo.dwStreamType = 0;
    StruPlayInfo.dwLinkMode = 0;
    StruPlayInfo.bBlocked = 1;
    m_playHandle = NET_DVR_RealPlay_V40(m_dvrUserId, &StruPlayInfo, NULL, NULL);
    if (m_playHandle == -1) {
        qCritical() << "NET_DVR_RealPlay_V40 failed. Error code: " << NET_DVR_GetLastError();
        return;
    }
    m_startRecordTime = QDateTime::currentDateTime();
    qInfo() << "start NET_DVR_RealPlay_V40. " << m_startRecordTime.toString("yyyy-MM-dd HH:mm:ss");
    QString filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                + QDir::separator() + "record_" + m_startRecordTime.toString("yyyyMMdd_HHmmss") + ".mp4";
    if (!NET_DVR_SaveRealData(m_playHandle, filename.toLocal8Bit().data())) {
        qCritical() << "NET_DVR_SaveRealData failed. Error code: " << NET_DVR_GetLastError();
        return;
    }
    qInfo() << "start NET_DVR_SaveRealData";
    qInfo() << "The video will be saved to " << filename;
    ui->pushButton_startRecord->setEnabled(false);
    ui->pushButton_endRecord->setEnabled(true);
}

void Widget::on_pushButton_endRecord_clicked()
{
    if (m_playHandle < 0) {
        qWarning() << "m_playHandle < 0";
        return;
    }
    if (!NET_DVR_StopSaveRealData(m_playHandle)) {
        qWarning() << "NET_DVR_StopSaveRealData failed. Error code: " << NET_DVR_GetLastError();
        return;
    }
    m_endRecordTime = QDateTime::currentDateTime();
    qInfo() << "NET_DVR_StopSaveRealData." << m_endRecordTime.toString("yyyy-MM-dd HH:mm:ss");
    ui->pushButton_startRecord->setEnabled(true);
    ui->pushButton_endRecord->setEnabled(false);
}

