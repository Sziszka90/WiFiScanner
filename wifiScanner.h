#ifndef WIFISCANNER_H
#define WIFISCANNER_H

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

#include "wifi_lib.h"

#include <vector>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

class WifiScanner {
public:
    Netlink nl;
    Wifi w;

    int getWifiStatus(Netlink* nl, Wifi* w);
    int initNl80211(Netlink* nl, Wifi* w);
    void deleteNetlink(Netlink *nl);
    int doScanTrigger(Netlink* nl, Wifi* w, std::vector<Signals>* sig);

};

#endif
