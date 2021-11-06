#ifndef __STRUCTS__H__
#define __STRUCTS__H__

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

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

#endif  //!__STRUCTS__H__