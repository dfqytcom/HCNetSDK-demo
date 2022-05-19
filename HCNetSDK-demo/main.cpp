#include "widget.h"
#include <QApplication>
#include <QtWidgets>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (g_textedit != nullptr) {
        switch (type) {
        case QtDebugMsg:
            g_textedit->append("[DEBUG]<font color='gray'>" +msg+"</font>");
            break;
        case QtInfoMsg:
            g_textedit->append("[INFO]<font color='gray'>" +msg+"</font>");
            break;
        case QtWarningMsg:
            g_textedit->append("[WARN]<font color='red'>" +msg+"</font>");
            break;
        case QtCriticalMsg:
            g_textedit->append("[ERROR]<font color='red'>" +msg+"</font>");
            break;
        case QtFatalMsg:
            abort();
        }
    } else {
        QByteArray localMsg = msg.toLocal8Bit();
        switch (type) {
        case QtDebugMsg:
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
            abort();
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);
    a.setApplicationDisplayName("HCNetSDK demo");

    if (!NET_DVR_Init()) {
        return -1;
    }
    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);
    NET_DVR_SetLogToFile(3, QCoreApplication::applicationDirPath().toLocal8Bit().data(), TRUE);

    Widget w;
    w.show();
    int r = a.exec();

    NET_DVR_Cleanup();

    return r;
}
