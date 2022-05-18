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
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_connect_clicked()
{

}

void Widget::on_pushButton_disconnect_clicked()
{

}

