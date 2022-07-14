#include "UI/QtFFplay.h"
#include <QtWidgets/QApplication>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "SDL2.lib")

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtFFplay w;
    w.show();
    return a.exec();
}
