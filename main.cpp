#include "mainwindow.h"
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>
#include "wifi_lib.h"
#include "wifiScanner.h"
#include <memory>
#include <iostream>
#include <QApplication>
#include <thread>
#include <vector>

using namespace std;

std::atomic_int runApp(1);

int runScanner(WifiScanner* wifiScanner, Netlink* nl, Wifi* w)
{

    do
    {
        std::vector<Signals>* sig = new std::vector<Signals>();

        wifiScanner->getWifiStatus(nl, w);

        int err = wifiScanner->doScanTrigger(nl, w, sig);
        if (err != 0) {
            cout<<"do_scan_trigger() failed with"<< err <<endl;
            return err;
        }

        for( int i = 0; i < sig->size(); ++i)
            cout<<(*sig)[i].name<<endl;

        sleep(4);
    }
    while(runApp);

    return 0;
}

int main(int argc, char **argv)
{

    Netlink nl;
    Wifi w;

    WifiScanner* wifiScanner = new WifiScanner();


    nl.id = wifiScanner->initNl80211(&nl, &w);

    if (nl.id < 0)
    {
        fprintf(stderr, "Error initializing netlink 802.11\n");
        return -1;
    }

    std::thread thread1(runScanner, wifiScanner, &nl, &w);

    QApplication a(argc, argv);
    MainWindow* win = new MainWindow();
    win->show();

    runApp = a.exec();

    thread1.join();

    cout<<"Thread joined"<<endl;


    cout<<"\nExiting gracefully... "<<endl;
    wifiScanner->deleteNetlink(&nl);
    cout<<"OK"<<endl;
    return 0;




}
