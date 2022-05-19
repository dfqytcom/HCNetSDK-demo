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
    // 录像类型：0- 手动，1- 报警，2- 回传，3- 信号，4- 移动，5- 遮挡
    LONG recordType = 0;
    BOOL ok = NET_DVR_StartDVRRecord(m_dvrUserId, channel, recordType);
    if (ok) {
        m_startRecordTime = QDateTime::currentDateTime();
        qInfo() << "Start record.  " << m_startRecordTime.toString("yyyy-MM-dd HH:mm:ss");
    } else {
        qCritical() << "Start record failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
    ui->pushButton_startRecord->setEnabled(false);
    ui->pushButton_endRecord->setEnabled(true);
}

void Widget::on_pushButton_endRecord_clicked()
{
    LONG channel = 1;
    BOOL ok = NET_DVR_StopDVRRecord(m_dvrUserId, channel);
    if (ok) {
        m_endRecordTime = QDateTime::currentDateTime();
        qInfo() << "End record." << m_endRecordTime.toString("yyyy-MM-dd HH:mm:ss");
        std::thread th([this, channel](){
            NET_DVR_TIME start, end;
            start.dwYear = m_startRecordTime.date().year();
            start.dwMonth = m_startRecordTime.date().month();
            start.dwDay = m_startRecordTime.date().day();
            start.dwHour = m_startRecordTime.time().hour();
            start.dwMinute = m_startRecordTime.time().minute();
            start.dwSecond = m_startRecordTime.time().second();
            end.dwYear = m_endRecordTime.date().year();
            end.dwMonth = m_endRecordTime.date().month();
            end.dwDay = m_endRecordTime.date().day();
            end.dwHour = m_endRecordTime.time().hour();
            end.dwMinute = m_endRecordTime.time().minute();
            end.dwSecond = m_endRecordTime.time().second();
            QString filename = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                    + QDir::separator() + QUuid::createUuid().toString() + ".mp4";
            LONG r = NET_DVR_GetFileByTime(m_dvrUserId, channel, &start, &end, filename.toLocal8Bit().data());
            if (r != -1) {
                qInfo() << "The video is saved to " << filename << ". " << NET_DVR_GetLastError();
                while (true) {
                    int pos = NET_DVR_GetDownloadPos(r);
                    if (pos == -1) {
                        qWarning() << "Download video from dvr failed. Error code: " << NET_DVR_GetLastError()
                                   << ". Message: "<< QString(NET_DVR_GetErrorMsg());
                        return;
                    }
                    qInfo() << "Download progress: " << pos;
                    if (pos == 100) {
                        qInfo() << "Download done!";
                        return;
                    }
                    if (pos > 100) {
                        qWarning() << "Download video from dvr failed. Error code: " << NET_DVR_GetLastError()
                                   << ". Message: " << QString(NET_DVR_GetErrorMsg());
                        return;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            } else {
                qWarning() << "Download video from dvr failed. Error code: " << NET_DVR_GetLastError();
            }
        });
        th.detach();

    } else {
        qCritical() << "End record failed. Error: " << QString(NET_DVR_GetErrorMsg());
        return;
    }
    ui->pushButton_startRecord->setEnabled(true);
    ui->pushButton_endRecord->setEnabled(false);
}

