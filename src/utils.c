#include <errno.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>
#include "structs.h"
#include "utils.h"

struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
                              [NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32 },
                              [NL80211_STA_INFO_RX_BYTES] = { .type = NLA_U32 },
                              [NL80211_STA_INFO_TX_BYTES] = { .type = NLA_U32 },
                              [NL80211_STA_INFO_RX_PACKETS] = { .type = NLA_U32 },
                              [NL80211_STA_INFO_TX_PACKETS] = { .type = NLA_U32 },
                              [NL80211_STA_INFO_SIGNAL] = { .type = NLA_U8 },
                              [NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
                              [NL80211_STA_INFO_LLID] = { .type = NLA_U16 },
                              [NL80211_STA_INFO_PLID] = { .type = NLA_U16 },
                              [NL80211_STA_INFO_PLINK_STATE] = { .type = NLA_U8 },
};

struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
                            [NL80211_BSS_TSF] = { .type = NLA_U64 },
                            [NL80211_BSS_FREQUENCY] = { .type = NLA_U32 },
                            [NL80211_BSS_BSSID] = { },
                            [NL80211_BSS_BEACON_INTERVAL] = { .type = NLA_U16 },
                            [NL80211_BSS_CAPABILITY] = { .type = NLA_U16 },
                            [NL80211_BSS_INFORMATION_ELEMENTS] = {.type = NLA_NESTED },
                            [NL80211_BSS_SIGNAL_MBM] = { .type = NLA_U32 },
                            [NL80211_BSS_SIGNAL_UNSPEC] = { .type = NLA_U8 },
                            [NL80211_BSS_STATUS] = { .type = NLA_U32 },
                            [NL80211_BSS_SEEN_MS_AGO] = { .type = NLA_U32 },
                            [NL80211_BSS_BEACON_IES] = { },
};

struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
                             [NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 },
                             [NL80211_RATE_INFO_MCS] = { .type = NLA_U8 },
                             [NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
                             [NL80211_RATE_INFO_SHORT_GI] = { .type = NLA_FLAG },
};

void ctrl_c_callback() 
{
    keepRunning = 0;
}

int finish_callback(struct nl_msg *msg, void *arg)
{
    int *ret = arg;
    *ret = 0;
    return NL_SKIP;
}

int get_wifi_name_callback(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

    nla_parse(tb_msg,
              NL80211_ATTR_MAX,
              genlmsg_attrdata(gnlh, 0),
              genlmsg_attrlen(gnlh, 0),
              NULL);

    if(tb_msg[NL80211_ATTR_IFNAME])
    {
        strcpy(((Wifi*)arg)->ifname, nla_get_string(tb_msg[NL80211_ATTR_IFNAME]));
    }

    if(tb_msg[NL80211_ATTR_IFINDEX])
    {
        ((Wifi*)arg)->ifindex = nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]);
    }

    return NL_SKIP;
}

int get_wifi_info_callback(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
    struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];

    nla_parse(tb,
              NL80211_ATTR_MAX,
              genlmsg_attrdata(gnlh,0),
              genlmsg_attrlen(gnlh, 0),
              NULL);
    
    if(!tb[NL80211_ATTR_STA_INFO])
    {
        fprintf(stderr, "sta stats missing!\n");
        return NL_SKIP;
    }

    if(nla_parse_nested(sinfo, 
                        NL80211_STA_INFO_MAX, 
                        tb[NL80211_ATTR_STA_INFO],
                        stats_policy))
    {
        fprintf(stderr, "failed to parse nested attributes!\n");
        return NL_SKIP;
    }

    if(sinfo[NL80211_STA_INFO_SIGNAL])
    {
        ((Wifi*)arg)->signal = 100+(int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
    }

    if(sinfo[NL80211_STA_INFO_TX_BITRATE])
    {
        if(nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
                            sinfo[NL80211_STA_INFO_TX_BITRATE],
                            rate_policy))
        {
            fprintf(stderr, "failed to parse nested rate attributes!\n"); 
        } 
        else 
        {
            if (rinfo[NL80211_RATE_INFO_BITRATE]) 
            {
                ((Wifi*)arg)->txrate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
            } 
        }
    }
    return NL_SKIP;
}

int ack_callback(struct nl_msg *msg, void *arg) 
{
    int *ret = arg;
    *ret = 0;
    return NL_STOP;
}

int family_callback(struct nl_msg *msg, void *arg) 
{
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[CTRL_ATTR_MAX + 1]; 
    struct nlattr *mcgrp;
    struct handler_args *grp = arg;
    
    int rem_mcgrp;

    nla_parse(tb,
              CTRL_ATTR_MAX, 
              genlmsg_attrdata(gnlh, 0), 
              genlmsg_attrlen(gnlh, 0), 
              NULL);

    if (!tb[CTRL_ATTR_MCAST_GROUPS]) return NL_SKIP;

    nla_for_each_nested(mcgrp, tb[CTRL_ATTR_MCAST_GROUPS], rem_mcgrp) 
    { 
        struct nlattr *tb_mcgrp[CTRL_ATTR_MCAST_GRP_MAX + 1];

        nla_parse(tb_mcgrp,
                  CTRL_ATTR_MCAST_GRP_MAX, 
                  nla_data(mcgrp), 
                  nla_len(mcgrp), 
                  NULL);

        if (!tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME] || !tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]) 
        {
            continue;
        }

        if (strncmp(nla_data(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME]), grp->group,
            nla_len(tb_mcgrp[CTRL_ATTR_MCAST_GRP_NAME]))) 
        {
            continue;
        }

        grp->id = nla_get_u32(tb_mcgrp[CTRL_ATTR_MCAST_GRP_ID]);
        break;
    }

    return NL_SKIP;
}

int error_callback(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
{
    printf("error_handler() called.\n");
    int *ret = arg;
    *ret = err->error;
    return NL_STOP;
}

int no_seq_check_callback(struct nl_msg *msg, void *arg) 
{
    return NL_OK;
}

int dump_callback(struct nl_msg* msg, void* arg)
{
    struct nlattr *bss[NL80211_BSS_MAX + 1];
    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    char mac_addr[20];
    struct nlattr *tb[NL80211_ATTR_MAX + 1];

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
    
    if (!tb[NL80211_ATTR_BSS]) 
    {
        printf("bss info missing!\n");
        return NL_SKIP;
    }

    if (nla_parse_nested(bss, NL80211_BSS_MAX, tb[NL80211_ATTR_BSS], bss_policy)) 
    {
        printf("failed to parse nested attributes!\n");
        return NL_SKIP;
    }

    if (!bss[NL80211_BSS_BSSID])
    {
        return NL_SKIP;
    }

    if (!bss[NL80211_BSS_INFORMATION_ELEMENTS])
    {
        return NL_SKIP;
    }

    mac_addr_n2a(mac_addr, nla_data(bss[NL80211_BSS_BSSID]));
    printf("%s, ", mac_addr);
    printf("%d MHz, ", nla_get_u32(bss[NL80211_BSS_FREQUENCY]));
    printf("-%d dBm, ", 100+(int)nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]) / 100);
    print_ssid(nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]), nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]));
    printf("\n");

    return NL_SKIP;
}

int scan_callback(struct nl_msg* msg, void* arg) 
{

    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct trigger_results *results = arg;

    if (gnlh->cmd == NL80211_CMD_SCAN_ABORTED) 
    {
        printf("Got NL80211_CMD_SCAN_ABORTED.\n");
        results->done = 1;
        results->aborted = 1;
    } 
    else if (gnlh->cmd == NL80211_CMD_NEW_SCAN_RESULTS) 
    {
        printf("Got NL80211_CMD_NEW_SCAN_RESULTS.\n");
        results->done = 1;
        results->aborted = 0;
    }  

    return NL_SKIP;
}

//----------------------------------------------------------------------------------------------//

int init_nl80211(Netlink *nl, Wifi *w)
{
    nl->socket = nl_socket_alloc(); 
    if(!nl->socket)
    {
        fprintf(stderr, "Failed to allocate netlink socket.\n");
        return -ENOMEM;
    }

    nl_socket_set_buffer_size(nl->socket, 8192, 8192);

    if(genl_connect(nl->socket)) 
    {
        fprintf(stderr, "Failed to connect to netlink socket.\n");
        nl_close(nl->socket);
        nl_socket_free(nl->socket);
        return -ENOLINK;
    }

    nl->id = genl_ctrl_resolve(nl->socket, "nl80211");
    if(nl->id<0)
    {
        fprintf(stderr, "Nl80211 interface not found.\n");
        nl_close(nl->socket);
        nl_socket_free(nl->socket);
        return -ENOENT;
    }

    nl->cb1 = nl_cb_alloc(NL_CB_DEFAULT);
    nl->cb2 = nl_cb_alloc(NL_CB_DEFAULT);
    if((!nl->cb1) || (!nl->cb2)) 
    {
        fprintf(stderr, "Failed to allocate netlink callback.\n");
        nl_close(nl->socket);
        nl_socket_free(nl->socket);
        return ENOMEM;
    }

    nl_cb_set(nl->cb1, NL_CB_VALID, NL_CB_CUSTOM, get_wifi_name_callback, w);
    nl_cb_set(nl->cb1, NL_CB_FINISH, NL_CB_CUSTOM, finish_callback, &(nl->result1));

    nl_cb_set(nl->cb2, NL_CB_VALID, NL_CB_CUSTOM, get_wifi_info_callback, w);
    nl_cb_set(nl->cb2, NL_CB_FINISH, NL_CB_CUSTOM, finish_callback, &(nl->result2));

    return nl->id;
}

int get_wifi_status(Netlink* nl, Wifi* w) 
{
    nl->result1 = 1;
    nl->result2 = 1;
    
    struct nl_msg* msg1 = nlmsg_alloc();

    if (!msg1) 
    {
        fprintf(stderr, "Failed to allocate netlink message.\n");
        return -ENOMEM;
    }
  
    genlmsg_put(msg1,
                NL_AUTO_PORT,
                NL_AUTO_SEQ,
                nl->id,
                0,
                NLM_F_DUMP,
                NL80211_CMD_GET_INTERFACE,
                0);

    nl_send_auto(nl->socket, msg1);
  
    while (nl->result1 > 0)
    { 
        nl_recvmsgs(nl->socket, nl->cb1); 
    }

    nlmsg_free(msg1);

    if (w->ifindex < 0)
    { 
        return -1; 
    }

    struct nl_msg* msg2 = nlmsg_alloc();

    if (!msg2) 
    {
        fprintf(stderr, "Failed to allocate netlink message.\n");
        return -ENOMEM;
    }
  
    genlmsg_put(msg2,
                NL_AUTO_PORT,
                NL_AUTO_SEQ,
                nl->id,
                0,
                NLM_F_DUMP,
                NL80211_CMD_GET_STATION,
                0);
              
    nla_put_u32(msg2, NL80211_ATTR_IFINDEX, w->ifindex); 
    nl_send_auto(nl->socket, msg2); 

    while (nl->result2 > 0) 
    {
        nl_recvmsgs(nl->socket, nl->cb2); 
    }

    nlmsg_free(msg2);
  
    return 0;
}


void mac_addr_n2a(char *mac_addr, unsigned char *arg) 
{
    int i, l;

    l = 0;
    for (i = 0; i < 6; i++) {
        if (i == 0) {
            sprintf(mac_addr+l, "%02x", arg[i]);
            l += 2;
        } else {
            sprintf(mac_addr+l, ":%02x", arg[i]);
            l += 3;
        }
    }
}

void print_ssid(unsigned char *ie, int ielen) 
{
    uint8_t len;
    uint8_t *data;
    int i;

    while (ielen >= 2 && ielen >= ie[1]) {
        if (ie[0] == 0 && ie[1] >= 0 && ie[1] <= 32) {
            len = ie[1];
            data = ie + 2;
            for (i = 0; i < len; i++) {
                if (isprint(data[i]) && data[i] != ' ' && data[i] != '\\') printf("%c", data[i]);
                else if (data[i] == ' ' && (i != 0 && i != len -1)) printf(" ");
                else printf("\\x%.2x", data[i]);
            }
            break;
        }
        ielen -= ie[1] + 2;
        ie += ie[1] + 2;
    }
}

int nl_get_multicast_id(struct nl_sock *sock, const char *family, const char *group) 
{
    struct nl_msg *msg;
    struct nl_cb *cb;
    int ret, ctrlid;
    struct handler_args grp = { .group = group, .id = -ENOENT, };

    msg = nlmsg_alloc();
    if (!msg) return -ENOMEM;

    cb = nl_cb_alloc(NL_CB_DEFAULT);
    if (!cb) 
    {
        ret = -ENOMEM;
        goto out_fail_cb;
    }

    ctrlid = genl_ctrl_resolve(sock, "nlctrl");

    genlmsg_put(msg, 0, 0, ctrlid, 0, 0, CTRL_CMD_GETFAMILY, 0);

    ret = -ENOBUFS;
    nla_put_string(msg, CTRL_ATTR_FAMILY_NAME, family);

    ret = nl_send_auto_complete(sock, msg);
    if (ret < 0) goto out;

    ret = 1;

    nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_callback, &ret);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, family_callback, &grp);

    while (ret > 0) nl_recvmsgs(sock, cb);

    if (ret == 0) ret = grp.id;

    nla_put_failure:
        out:
            nl_cb_put(cb);
        out_fail_cb:
            nlmsg_free(msg);
            return ret;
}

int do_scan_trigger(Netlink* nl, Wifi* w)
{
    struct nl_msg* msg;
    struct trigger_results results = { .done=0, .aborted=0 };
    struct nl_msg* ssids_to_scan;
    int err;
    int ret;
    int mcid = nl_get_multicast_id(nl->socket, "nl80211", "scan");
    nl_socket_add_memberships(nl->socket, mcid);

    msg = nlmsg_alloc();

    if (!msg) 
    {
        fprintf(stderr, "Failed to allocate netlink message.\n");
        return -ENOMEM;
    }

    ssids_to_scan = nlmsg_alloc();

    if (!ssids_to_scan) 
    {
        fprintf(stderr, "Failed to allocate netlink message.\n");
        nlmsg_free(msg);
        return -ENOMEM;
    }

    nl->cb3 = nl_cb_alloc(NL_CB_DEFAULT);
    if(!nl->cb3)
    {
        fprintf(stderr, "Failed to allocate netlink callbacks.\n");
        nlmsg_free(msg);
        nlmsg_free(ssids_to_scan);
        return -ENOMEM;
    }

    genlmsg_put(msg,
                NL_AUTO_PORT,
                NL_AUTO_SEQ,
                nl->id,
                0,
                0,
                NL80211_CMD_TRIGGER_SCAN,
                0);
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, w->ifindex); 
    nla_put(ssids_to_scan, 1, 0, "");
    nla_put_nested(msg, NL80211_ATTR_SCAN_SSIDS, ssids_to_scan);
    nlmsg_free(ssids_to_scan);

    nl_cb_set(nl->cb3, NL_CB_VALID, NL_CB_CUSTOM, scan_callback, &results);
    nl_cb_err(nl->cb3, NL_CB_CUSTOM, error_callback, &err); 
    nl_cb_set(nl->cb3, NL_CB_FINISH, NL_CB_CUSTOM, finish_callback, &err);
    nl_cb_set(nl->cb3, NL_CB_ACK, NL_CB_CUSTOM, ack_callback, &err);
    nl_cb_set(nl->cb3, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check_callback, NULL);

    err = 1;
    ret = nl_send_auto(nl->socket, msg);
    
    printf("NL802111_CMD_TRIGGER_SCAN sent %d bytes to the kernel.\n", ret);
    
    printf("Waiting for scan to complete...\n");

    while (err > 0)
    {
        ret = nl_recvmsgs(nl->socket, nl->cb3);
    }

    if (err < 0) 
    {
        printf("WARNING: err has a value of %d.\n", err);
    }

    if (ret < 0) 
    {
        printf("ERROR: nl_recvmsgs() returned %d (%s).\n", ret, nl_geterror(-ret));
        return ret;
    }

    while (!results.done) 
    {
        nl_recvmsgs(nl->socket, nl->cb3);
    }

    if (results.aborted) 
    {
        printf("ERROR: Kernel aborted scan.\n");
        return 1;
    }

    printf("Scan is done.\n");

    genlmsg_put(msg, 0, 0, nl->id, 0, NLM_F_DUMP, NL80211_CMD_GET_SCAN, 0); 
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, w->ifindex); 
    nl_socket_modify_cb(nl->socket, NL_CB_VALID, NL_CB_CUSTOM, dump_callback, NULL);

    ret = nl_send_auto(nl->socket, msg);  

    printf("NL80211_CMD_GET_SCAN sent %d bytes to the kernel.\n", ret);

    ret = nl_recvmsgs_default(nl->socket); 

    if (ret < 0) {
        printf("ERROR: nl_recvmsgs_default() returned %d (%s).\n", ret, nl_geterror(-ret));
        return ret;
    } 

    nlmsg_free(msg);
    nl_cb_put(nl->cb3);
    nl_socket_drop_membership(nl->socket, mcid);

    return 0;
}

