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

int keepRunning = 1;

void ctrl_c_callback(int dummy) 
{
    keepRunning = 0;
}

int main(int argc, char **argv) 
{
    Netlink nl;
    Wifi w;
    
    signal(SIGINT, ctrl_c_callback);
  
    nl.id = init_nl80211(&nl, &w);
    if (nl.id < 0) 
    {
        fprintf(stderr, "Error initializing netlink 802.11\n");
        return -1;
    }

    get_wifi_status(&nl, &w);

    int err = do_scan_trigger(&nl, &w);
    if (err != 0) {
        printf("do_scan_trigger() failed with %d.\n", err);
        return err;
    }
  
    do 
    {
        get_wifi_status(&nl, &w);  
        printf("Interface: %s | InterfaceIdx: %d signal: -%d dBm | txrate: %.1f MBit/s\n",
           w.ifname, w.ifindex, w.signal, (float)w.txrate/10);
        sleep(5);
    } 
    while(keepRunning);

    printf("\nExiting gracefully... ");
    delete_netlink(&nl);
    printf("OK\n");
    return 0;
}