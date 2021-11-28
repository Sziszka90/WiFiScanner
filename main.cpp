#include "mainwindow.h"
#include "wifi_lib.h"
#include "wifiScanner.h"

#include<QApplication>

#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

std::mutex mtx;

std::atomic_int runApp(1);

int runScanner(WifiScanner* wifiScanner, Netlink* nl, Wifi* w, std::vector<Signals>* sig)
{
    std::vector<Signals>* signal = new std::vector<Signals>();

    do
    {
        signal->clear();

        wifiScanner->getWifiStatus(nl, w);

        int err = wifiScanner->doScanTrigger(nl, w, signal);
        if (err != 0) {
            cout<<"do_scan_trigger() failed with"<< err <<endl;
            return err;
        }

        mtx.lock();


        std::sort (signal->begin(), signal->end(), ([](Signals sig1, Signals sig2) { return sig1.signalStrength > sig2.signalStrength; }));
        *sig = *signal;

        mtx.unlock();
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

    std::vector<Signals> sig;

    nl.id = wifiScanner->initNl80211(&nl, &w);

    if (nl.id < 0)
    {
        cout<<"Error initializing netlink 802.11\n"<<endl;
        return -1;
    }

    std::thread threadScanner(runScanner, wifiScanner, &nl, &w, &sig);

    QApplication a(argc, argv);
    MainWindow window;
    window.setSignals(&sig);
    window.show();

    runApp = a.exec();

    threadScanner.join();

    cout<<"Thread joined"<<endl;

    cout<<"\nExiting gracefully... "<<endl;
    wifiScanner->deleteNetlink(&nl);
    cout<<"OK"<<endl;
    return 0;




}
