// -*- mode: c++; c-basic-offset: 2 -*-
#ifndef CLICK_EMPOWERACKHANDLER_HH
#define CLICK_EMPOWERACKHANDLER_HH
#include <click/config.h>
#include <click/element.hh>

CLICK_DECLS

/*
=c

EmpowerACKHandler(EL)

=s EmPOWER

Checks ACK packets for latency computation.

=d

Checks ACK packets to compute latency per Slice and LVAP.

=over 8

=item EL
An EmpowerLVAPManager element

=item DEBUG
Turn debug on/off

=back

=a EmpowerACKHandler
*/

class EmpowerACKHandler : public Element {
public:

    EmpowerACKHandler();
    ~EmpowerACKHandler();

    const char *class_name() const { return "EmpowerACKHandler"; }
    const char *port_count() const { return "1/1"; }
    const char *processing() const { return PUSH; }

    int configure(Vector<String>&, ErrorHandler*);

    void push(int, Packet*);

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