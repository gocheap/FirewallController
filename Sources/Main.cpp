#include <QApplication>
#include <QMessageBox>
#include <Windows.h>
#include "MainWindow.hpp"

int main(int argc, char** argv)
{
    OSVERSIONINFO osInfo = {};
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osInfo);

    if (osInfo.dwMajorVersion >= 6) {
        QApplication app(argc, argv);
        MainWindow window;
        return app.exec();
    } else {
        QMessageBox::critical(0, "Fatal error", "This program runs only in Vindows Vista or newer");
        return 1;
    }
}
