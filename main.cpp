#include <QCoreApplication>
#include "discovery.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    printf("Hello world!\n");

    Discovery dis;
    dis.Init();

    while(true)
    {
        dis.Start();

        sleep(5);

        dis.Stop();

        sleep(5);
    }

    return a.exec();
}
