#include <QApplication>
#include <QMessageBox>
#include <Windows.h>
#include "MainWindow.hpp"

bool IsSupportedOS()
{
    OSVERSIONINFO osInfo = {};
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osInfo);
    return (osInfo.dwMajorVersion >= 6);
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (!IsSupportedOS()) {
        QMessageBox::critical(0, "Fatal error", "This program runs only in Vindows Vista or newer");
        QApplication::exit(1);
    }

    MainWindow window;

    return app.exec();
}
