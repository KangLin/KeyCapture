
#include <QLoggingCategory>
#include <QApplication>

#include "mainwindow.h"

static Q_LOGGING_CATEGORY(log, "App")
int main(int argc, char *argv[])
{
    // 注意：必须是　xcb, wayland　无法捕获
    qputenv("QT_QPA_PLATFORM", "xcb");
    qDebug(log) << "QT platform:" << QGuiApplication::platformName();
    qDebug(log) << "ENV QT_QPA_PLATFORM" << qgetenv("QT_QPA_PLATFORM");
    qDebug(log) << "ENV XDG_SESSION_TYPE:" << qgetenv("XDG_SESSION_TYPE");
    qDebug(log) << "ENV DISPLAY:" << qgetenv("DISPLAY");

    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
