#ifndef WIFI_LIB_H
#define WIFI_LIB_H

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <errno.h>

struct trigger_results {
    int done;
    int aborted;
};

struct handler_args 
{
    const char *group;
    int id;
};

typedef struct {
    int id;
    struct nl_sock *socket;
    struct nl_cb *cb1, *cb2, *cb3;
    int result1, result2;
} Netlink;

typedef struct {
    char ifname[30];
    int ifindex;
    int signal;
    int txrate;

} Wifi;

typedef struct {
    uint8_t *name;
    int signalStrength;
} Signals;

int init_nl80211(Netlink *nl, Wifi *w);

int get_wifi_status(Netlink *nl, Wifi *w);

void mac_addr_n2a(char *mac_addr, unsigned char *arg);

uint8_t* print_ssid(unsigned char *ie, int ielen);

int do_scan_trigger(Netlink* nl, Wifi* w, std::vector<Signals>* sig);

int finish_callback(struct nl_msg *msg, void *arg);

int get_wifi_name_callback(struct nl_msg *msg, void *arg);

int get_wifi_info_callback(struct nl_msg *msg, void *arg);

int ack_callback(struct nl_msg *msg, void *arg);

int family_callback(struct nl_msg *msg, void *arg);

int error_callback(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);

int no_seq_check_callback(struct nl_msg *msg, void *arg);

int dump_callback(struct nl_msg* msg, void* arg);

int scan_callback(struct nl_msg* msg, void* arg);

void delete_netlink(Netlink* nl);

#endif
