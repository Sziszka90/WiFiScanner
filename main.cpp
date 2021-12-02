#include "mainwindow.h"
#include "wifi_lib.h"
#include "wifiScanner.h"
#include "chart.h"

#include<QApplication>

#include <memory>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv)
{

    QApplication a(argc, argv);
    MainWindow window;

    window.show();

    return a.exec();

    cout<<"\nExiting gracefully... "<<endl;

    cout<<"OK"<<endl;
    return 0;

}
