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
#include <vector>

int WifiScanner::doScanTrigger(Netlink* nl, Wifi* w, std::vector<Signals>* sig)
{
    return do_scan_trigger(nl, w, sig);
}


int WifiScanner::getWifiStatus(Netlink* nl, Wifi* w)
{
    return get_wifi_status(nl, w);

}

int WifiScanner::initNl80211(Netlink* nl, Wifi* w)
{
    return init_nl80211(nl, w);
}

void WifiScanner::deleteNetlink(Netlink *nl)
{
    delete_netlink(nl);
    return;
}



