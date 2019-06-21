/*
 * empowerackhandler.{cc,hh} -- handles 802.11 ack packets (EmPOWER Access Point)
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
#include <clicknet/llc.h>
#include <clicknet/ether.h>
#include <click/packet_anno.hh>
#include "empowerackhandler.hh"
#include "empowerlatencyinfo.hh"
#include "empowerlvapmanager.hh"
CLICK_DECLS


EmpowerACKHandler::EmpowerACKHandler() :
        _el(0), _el_latency_info(0), _debug(false) {
}

EmpowerACKHandler::~EmpowerACKHandler() {
}

int EmpowerACKHandler::configure(Vector<String> &conf,
        ErrorHandler *errh) {

    return Args(conf, this, errh)
            .read_m("EL", ElementCastArg("EmpowerLVAPManager"), _el)
            .read_m("EL_LATENCY_INFO", ElementCastArg("EmpowerLatencyInfo"), _el_latency_info)
            .read("DEBUG", _debug)
            .complete();
}

void EmpowerACKHandler::push(int, Packet *p){

    Timestamp rx_timestamp = Timestamp::now();

    struct click_wifi *w = (struct click_wifi *) p->data();

    if (_el_latency_info->process_incoming_packet(EtherAddress(w->i_addr1), rx_timestamp)) {
        if (_debug) {
            click_chatter("%{element} :: %s :: ACK removed successfully!",
                          this,
                          __func__);
        }
    }
    output(0).push(p);
}

enum {
    H_DEBUG
};

String EmpowerACKHandler::read_handler(Element *e, void *thunk) {
    EmpowerACKHandler *td = (EmpowerACKHandler *) e;
    switch ((uintptr_t) thunk) {
        case H_DEBUG:
            return String(td->_debug) + "\n";
        default:
            return String();
    }
}

int EmpowerACKHandler::write_handler(const String &in_s, Element *e,
                                    void *vparam, ErrorHandler *errh) {

    EmpowerACKHandler *f = (EmpowerACKHandler *) e;
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

void EmpowerACKHandler::add_handlers() {
    add_read_handler("debug", read_handler, (void *) H_DEBUG);
    add_write_handler("debug", write_handler, (void *) H_DEBUG);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(EmpowerACKHandler)
ELEMENT_REQUIRES(userlevel)