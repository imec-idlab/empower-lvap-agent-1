/*
 * empowerwifiinfohandler.{cc,hh} -- handles 802.11 ack packets (EmPOWER Access Point)
 * Pedro Heleno Isolani
 *
 * Copyright (c) 2019 IDLAB - IMEC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/wifi.h>
#include "empowerwifiinfohandler.hh"
#include "empowerlatencyinfo.hh"
CLICK_DECLS


EmpowerWifiInfoHandler::EmpowerWifiInfoHandler() :
        _el(0), _debug(false) {
}

EmpowerWifiInfoHandler::~EmpowerWifiInfoHandler() {
}

int EmpowerWifiInfoHandler::configure(Vector<String> &conf,
        ErrorHandler *errh) {

    return Args(conf, this, errh)
            .read_m("EL", ElementCastArg("EmpowerLVAPManager"), _el)
            .read_m("EL_LATENCY_INFO", ElementCastArg("EmpowerLatencyInfo"), _el_latency_info)
            .read("DEBUG", _debug)
            .complete();
}

Packet* EmpowerWifiInfoHandler::pull(int) {
    Packet* p = input(0).pull();
    if (p==0) return 0;


    /*
     * Parsing Wifi header for the wifi info
     */
    struct click_wifi *w = (struct click_wifi *) p->data();

    uint8_t type = w->i_fc[0] & WIFI_FC0_TYPE_MASK;
    uint8_t subtype = w->i_fc[0] & WIFI_FC0_SUBTYPE_MASK;
    int retry = w->i_fc[1] & WIFI_FC1_RETRY;

    /*
    * Parsing source, destination and bssid from the wifi info
    */
    uint8_t dir = w->i_fc[1] & WIFI_FC1_DIR_MASK;

    EtherAddress dst;
    EtherAddress src;
    EtherAddress bssid;

    switch (dir) {
        case WIFI_FC1_DIR_NODS:
            dst = EtherAddress(w->i_addr1);
            src = EtherAddress(w->i_addr2);
            bssid = EtherAddress(w->i_addr3);
            break;
        case WIFI_FC1_DIR_TODS:
            bssid = EtherAddress(w->i_addr1);
            src = EtherAddress(w->i_addr2);
            dst = EtherAddress(w->i_addr3);
            break;
        case WIFI_FC1_DIR_FROMDS:
            dst = EtherAddress(w->i_addr1);
            bssid = EtherAddress(w->i_addr2);
            src = EtherAddress(w->i_addr3);
            break;
        case WIFI_FC1_DIR_DSTODS:
            dst = EtherAddress(w->i_addr1);
            src = EtherAddress(w->i_addr2);
            bssid = EtherAddress(w->i_addr3);
            break;
        default:
            click_chatter("%{element} :: %s :: invalid dir %d",
                          this,
                          __func__,
                          dir);
            return 0;
    }

    //TODO:
    // Parse the IP header for the DSCP value
    // Check for the retransmissions field
    /* Example...
     * click_ether *eh = (click_ether *) p->data();
     * if (ntohs(eh->ether_type) == 0x0800) {
     *      const click_ip *ip = p->ip_header();
     *      dscp = ip->ip_tos >> 2;
     * }
     */
    int dscp = 1; //dummy data for now

    /*
    * Check and store packet info into a hashmap
    */
    Timestamp tx_timestamp = Timestamp::now();

    //TODO: Add other Packets that need ACK
    //  Beacons
    //  Prob_RESP
    //  Assoc_RESP
    if (type == WIFI_FC0_TYPE_DATA) {
        if (_el_latency_info->process_outgoing_packet(src, dst, bssid, dscp, tx_timestamp)) {
            if (_debug) {
                click_chatter("%{element} :: %s :: OUT PACKET -> TYPE: %d, SUBTYPE: %d, "
                              "SRC: %s, DST: %s, BSSID: %s, RETRY: %d, LENGTH: %d",
                              this,
                              __func__,
                              type,
                              subtype,
                              src.unparse().c_str(),
                              dst.unparse().c_str(),
                              bssid.unparse().c_str(),
                              retry,
                              p->length());
            }
        } else {
            if (_debug) {
                click_chatter("%{element} :: %s :: Error on pulling packet!",
                              this,
                              __func__);
            }
        }
    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: NON-DATA PACKET -> TYPE: %d, SUBTYPE: %d, "
                          "SRC: %s, DST: %s, BSSID: %s, RETRY: %d, LENGTH: %d",
                          this,
                          __func__,
                          type,
                          subtype,
                          src.unparse().c_str(),
                          dst.unparse().c_str(),
                          bssid.unparse().c_str(),
                          retry,
                          p->length());
        }
    }
    return p;
}


enum {
    H_DEBUG
};

String EmpowerWifiInfoHandler::read_handler(Element *e, void *thunk) {
    EmpowerWifiInfoHandler *td = (EmpowerWifiInfoHandler *) e;
    switch ((uintptr_t) thunk) {
        case H_DEBUG:
            return String(td->_debug) + "\n";
        default:
            return String();
    }
}

int EmpowerWifiInfoHandler::write_handler(const String &in_s, Element *e,
                                      void *vparam, ErrorHandler *errh) {

    EmpowerWifiInfoHandler *f = (EmpowerWifiInfoHandler *) e;
    String s = cp_uncomment(in_s);

    switch ((intptr_t) vparam) {
        case H_DEBUG: {    //debug
            bool debug;
            if (!BoolArg().parse(s, debug))
                return errh->error("debug parameter must be boolean");
            f->_debug = debug;
            break;
        }
    }
    return 0;
}

void EmpowerWifiInfoHandler::add_handlers() {
    add_read_handler("debug", read_handler, (void *) H_DEBUG);
    add_write_handler("debug", write_handler, (void *) H_DEBUG);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(EmpowerWifiInfoHandler)
ELEMENT_REQUIRES(userlevel)