#ifndef __UTILS_H__
#define __UTILS_H__

extern int keepRunning;

int init_nl80211(Netlink *nl, Wifi *w);
int get_wifi_status(Netlink *nl, Wifi *w);
void mac_addr_n2a(char *mac_addr, unsigned char *arg);
void print_ssid(unsigned char *ie, int ielen);
int do_scan_trigger(Netlink* nl, Wifi* w);


int finish_callback(struct nl_msg *msg, void *arg);
int get_wifi_name_callback(struct nl_msg *msg, void *arg);
int get_wifi_info_callback(struct nl_msg *msg, void *arg);
void ctrl_c_callback();
int ack_callback(struct nl_msg *msg, void *arg);
int family_callback(struct nl_msg *msg, void *arg);
int error_callback(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
int no_seq_check_callback(struct nl_msg *msg, void *arg);
int dump_callback(struct nl_msg* msg, void* arg);
int scan_callback(struct nl_msg* msg, void* arg);

#endif