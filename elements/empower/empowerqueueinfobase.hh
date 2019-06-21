// -*- mode: c++; c-basic-offset: 2 -*-
#ifndef CLICK_EMPOWERQUEUEINFOBASE_HH
#define CLICK_EMPOWERQUEUEINFOBASE_HH
#include <click/config.h>
#include <click/element.hh>
#include <click/hashmap.hh>
#include <click/timer.hh>

CLICK_DECLS

/*
=c

EmpowerQueueInfoBase(EL)

=s EmPOWER

Holds queue delay per slice.

=d

Holds average/median queue delay per slice.

=over 8

=item EL
An EmpowerLVAPManager element

=item DEBUG
Turn debug on/off

=back

=a EmpowerQueueInfoBase
*/

// Key: DSCP value (int)
typedef HashMap<int, Vector<Timestamp> > DSCPTimestampEnqueuedPacketsMap;

// Key: DSCP value (int)
typedef HashMap<int, Vector<Timestamp> > DSCPDelayPacketsMap;

// Key: DSCP value (int)
typedef HashMap<int, Timestamp> DSCPQueueDelayMap;

class EmpowerQueueInfoBase : public Element {
public:

    EmpowerQueueInfoBase();
    ~EmpowerQueueInfoBase();

    const char *class_name() const { return "EmpowerQueueInfoBase"; }
    const char *port_count() const { return "0/0"; }
    const char *processing() const { return AGNOSTIC; }

    int initialize(ErrorHandler *);
    int configure(Vector<String>&, ErrorHandler*);

    // SRC (EtherAddress), DST (EtherAddress), BSSID (EtherAddress), DSCP (int), and timestamp (int)
    void process_packet_enqueue(int, Timestamp);

    // DSCP (int) and timestamp (int)
    void process_packet_dequeue(int, Timestamp);

    // DSCP (int) returns the queue delay in Timestamp format
    Timestamp get_queue_delay(int);

    void add_handlers();

    void run_timer(Timer *);

private:

    class EmpowerLVAPManager *_el;

    bool _debug;

    int _period;

    Timer _timer;

    /*
     * Structure for holding the last TX packets timestamp according to its DSCP values
     */
    DSCPTimestampEnqueuedPacketsMap dscp_timestamp_enqueued_pkts_map;

    /*
     * Structure for holding delay info per DSCP (Slice)
     */
    DSCPQueueDelayMap dscp_queue_delay_map;

    /*
     * Structure for holding last transmitted packet delays per DSCP (Slice)
     */
    DSCPDelayPacketsMap dscp_delay_packets_map;

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
    static String read_handler(Element *, void *);
};

CLICK_ENDDECLS
#endif