// -*- mode: c++; c-basic-offset: 2 -*-
#ifndef CLICK_EMPOWERLATENCYINFO_HH
#define CLICK_EMPOWERLATENCYINFO_HH
#include <click/config.h>
#include <click/element.hh>
#include <click/etheraddress.hh>
#include <click/hashmap.hh>
#include <click/pair.hh>

CLICK_DECLS

/*
=c

EmpowerLatencyInfo(EL)

=s EmPOWER

Checks ACK packets for latency computation.

=d

Checks ACK packets for its sequence number and compute latency per Slice
and LVAP.

=over 8

=item EL
An EmpowerLVAPManager element

=item DEBUG
Turn debug on/off

=back

=a EmpowerLatencyInfo
*/

// Latency info format
typedef struct data_packet_info {
    int dscp;
    EtherAddress src; // ANY
    EtherAddress dst; // LVAP
    EtherAddress bssid; // AP
    Timestamp timestamp;
} PKT_INFO;

// DSCP info format
typedef struct latency_info {
    Vector<Timestamp> last_seen_vector;
    Vector<Timestamp> latency_vector;
} LAT_INFO;

// Last transmitted packets vector
typedef Vector<data_packet_info> LastTXPacketsVector;

// Keys: DSCP value (int)
typedef HashMap<int, latency_info> DSCPLatencyMap;

// Keys: LVAP EtherAddress (String)
typedef HashMap<String, DSCPLatencyMap> LVAPLatencyMap;

class EmpowerLatencyInfo : public Element {
public:

    EmpowerLatencyInfo();
    ~EmpowerLatencyInfo();

    const char *class_name() const { return "EmpowerLatencyInfo"; }
    const char *port_count() const { return "0/0"; }
    const char *processing() const { return AGNOSTIC; }

    int configure(Vector<String>&, ErrorHandler*);

    // SRC (EtherAddress), DST (EtherAddress), BSSID (EtherAddress), DSCP (int), and timestamp ms (int)
    bool process_outgoing_packet(EtherAddress, EtherAddress, EtherAddress, int, Timestamp);

    // BSSID (EtherAddress) and timestamp ms (int)
    bool process_incoming_packet(EtherAddress, Timestamp);

    void add_handlers();

private:

    class EmpowerLVAPManager *_el;

    bool _debug;

    unsigned int _max_samples;

    /* Variable to indicate if a data packet was sent and now an ACK is expected*/
    bool waiting_for_ack;

    /*
     * Structure for holding the last TX packets that are waiting ACKs to be processed by the kernel
     */
    LastTXPacketsVector last_tx_pkts_vector;

    /*
     * Structure for holding info for transmitted packets on certain LVAP/Slice
     */
    data_packet_info crr_data_packet_info;
    /*
     * Hashmap structure for holding a Vector of latencies per LVAP/Slice
     */
    LVAPLatencyMap lvap_latency_map;

    static int write_handler(const String &, Element *, void *, ErrorHandler *);
    static String read_handler(Element *, void *);
};

CLICK_ENDDECLS
#endif