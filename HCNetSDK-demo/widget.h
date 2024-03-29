﻿#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDateTime>
#pragma warning( push )
#pragma warning( disable : 4010 )
#include "HCNetSDK.h"
#pragma warning( pop )

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class QTextEdit;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    void on_pushButton_capture_clicked();
    void on_pushButton_startRecord_clicked();
    void on_pushButton_endRecord_clicked();

private:
    Ui::Widget *ui;
    LONG m_dvrUserId = -1;
    QDateTime m_startRecordTime, m_endRecordTime;
    LONG m_playHandle = -1;
};
extern QTextEdit *g_textedit;
#endif // WIDGET_H
