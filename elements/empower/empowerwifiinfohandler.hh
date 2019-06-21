// -*- mode: c++; c-basic-offset: 2 -*-
#ifndef CLICK_EMPOWERWIFIINFOHANDLER_HH
#define CLICK_EMPOWERWIFIINFOHANDLER_HH
#include <click/config.h>
#include <click/element.hh>

CLICK_DECLS

/*
=c

EmpowerWifiInfoHandler(EL)

=s EmPOWER

Stores wifi information for later latency computation.

=d

Stores LVAP MAC, DSCP value, and Timestamp for later latency computation per Slice
and per LVAP.

=over 8

=item EL
An EmpowerLVAPManager element

=item DEBUG
Turn debug on/off

=back

=a EmpowerWifiInfoHandler
*/

class EmpowerWifiInfoHandler : public Element {
public:

    EmpowerWifiInfoHandler();
    ~EmpowerWifiInfoHandler();

    const char *class_name() const { return "EmpowerWifiInfoHandler"; }
    const char *port_count() const { return "1/1"; }
    const char *processing() const { return PULL; }

    int configure(Vector<String>&, ErrorHandler*);

    Packet* pull(int);

    void add_handlers();

private:

    class EmpowerLVAPManager *_el;

    class EmpowerLatencyInfo *_el_latency_info;

    bool _debug;

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
    static String read_handler(Element *, void *);
};

CLICK_ENDDECLS
#endif