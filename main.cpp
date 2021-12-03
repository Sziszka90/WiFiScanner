#include "mainwindow.h"
#include "wifi_lib.h"
#include "wifiScanner.h"
#include "chart.h"

#include<QApplication>

int main(int argc, char **argv)
{

    QApplication a(argc, argv);
    MainWindow window;

    window.show();

    int returnValue = a.exec();

    std::cout<<"\nExiting gracefully... "<<std::endl;

    std::cout<<"OK"<<std::endl;
    return returnValue;

}
