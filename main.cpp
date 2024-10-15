#define qout qDebug() << __FILE__ << " " << __LINE__ << " "
#include <QApplication>
#include <QPushButton>
#include <windows.h>
#include "mainWidget.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
//    HDC screen_dc = GetDC(nullptr);
//    // 获取设备独立像素 (DIP) 的分辨率
//    int width = GetDeviceCaps(screen_dc, DESKTOPHORZRES);
//    int height = GetDeviceCaps(screen_dc, DESKTOPVERTRES);
//    // 释放设备上下文
//    ReleaseDC(nullptr, screen_dc);
    mainWidget mainWidget;
//    mainWidget.resize(width / 1.5, height / 1.5);
    mainWidget.setWindowTitle("qxfly-ImageCompression");
    QIcon icon(":/ui/icon/logo.png");
    mainWidget.setWindowIcon(icon);
    mainWidget.show();
    return QApplication::exec();
}
