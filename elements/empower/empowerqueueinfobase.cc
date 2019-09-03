/*
 * empowerqueueinfobase.{cc,hh} -- handles 802.11 packets info for queue
 * delay computation (EmPOWER Access Point)
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
#include "empowerqueueinfobase.hh"
CLICK_DECLS


EmpowerQueueInfoBase::EmpowerQueueInfoBase() :
        _el(0), _timer(this), _period_timer_queue_delay(500), _period_timer_deficit(500), _debug(false) {
}

EmpowerQueueInfoBase::~EmpowerQueueInfoBase() {
}

int EmpowerQueueInfoBase::initialize(ErrorHandler *) {

    _timer_queue_delay.initialize(this);
    _timer_deficit.initialize(this);

    _timer_queue_delay.schedule_now();
    _timer_deficit.schedule_now();
    return 0;
}

int EmpowerQueueInfoBase::configure(Vector<String> &conf,
                                  ErrorHandler *errh) {
    return Args(conf, this, errh)
            .read_m("EL", ElementCastArg("EmpowerLVAPManager"), _el)
            .read("DEBUG", _debug)
            .complete();
}

void EmpowerQueueInfoBase::run_timer_queue_delay(Timer *) {

    // Computing the average queue delay
    for (DSCPDelayPacketsMap::iterator i = dscp_delay_packets_map.begin(); i!= dscp_delay_packets_map.end(); i++) {

        Timestamp sum = Timestamp();
        Timestamp avg = Timestamp();

        // Iterate over the vector of computed delays
        for (Vector<Timestamp>::const_iterator j = i.value().begin(); j!= i.value().end(); j++) {
            sum += *j;
        }

        // If there was sum
        if (sum){
            avg = sum / i.value().size();
        }

        // Update the avg delay
        dscp_queue_delay_map.insert(i.key(), avg);
    }

    // Reset delay packets map
    for (DSCPDelayPacketsMap::iterator i = dscp_delay_packets_map.begin(); i!= dscp_delay_packets_map.end(); i++) {
        i.value().clear();
    }

    _timer_queue_delay.schedule_after_msec(_period_timer_queue_delay);
}

void EmpowerQueueInfoBase::run_timer_deficit(Timer *) {
    // Computing the average deficit
    for (DSCPDeficitPacketsMap::iterator i = dscp_deficit_packets_map.begin(); i!= dscp_deficit_packets_map.end(); i++) {

        int sum = 0;
        int avg = 0;

        // Iterate over the vector of computed delays
        for (Vector<int>::const_iterator j = i.value().begin(); j!= i.value().end(); j++) {
            sum += *j;
        }

        // If there was sum
        if (sum){
            avg = sum / i.value().size();
        }

        // Update the avg delay
        dscp_deficit_map.insert(i.key(), avg);
    }

    // Reset deficit packets map
    for (DSCPDeficitPacketsMap::iterator i = dscp_deficit_packets_map.begin(); i!= dscp_deficit_packets_map.end(); i++) {
        i.value().clear();
    }

    _timer_deficit.schedule_after_msec(_period_timer_deficit);
}

void EmpowerQueueInfoBase::process_packet_deficit(int dscp, int deficit) {

    if (_debug) {
        click_chatter("%{element} :: %s :: process packet Deficit for DSCP: %d and Deficit: %d!",
                      this,
                      __func__,
                      dscp,
                      deficit);
    }

    DSCPDeficitPacketsMap :: Pair * crr_dscp_deficit_pkts_pair = dscp_deficit_packets_map.find_pair(dscp);

    // if DSCP exists in deficit packets map
    if (crr_dscp_deficit_pkts_pair){
        if (_debug){
            click_chatter("%{element} :: %s :: Deficit packets vector size is: %d",
                          this,
                          __func__,
                          crr_dscp_deficit_pkts_pair->value.size());
        }

        // Include packet timestamp at the end of the vector
        crr_dscp_deficit_pkts_pair->value.push_back(deficit);

    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: Initializing Deficit vector for DSCP: %d",
                          this,
                          __func__,
                          dscp);
        }

        // Create vector for packet deficits
        Vector<int> deficit_pkts_vector;

        // Inserting current deficit into it
        deficit_pkts_vector.push_back(deficit);

        // Inserting the vector of deficits into DSCP hashmap
        dscp_deficit_packets_map.insert(dscp, deficit_pkts_vector);
    }

}

int EmpowerQueueInfoBase::get_deficit(int dscp) {
    // Getting the average deficit from the map..
    DSCPDeficitMap :: Pair * crr_dscp_deficit_pair = dscp_deficit_map.find_pair(dscp);

    if (crr_dscp_deficit_pair){
        return crr_dscp_deficit_pair->value;
    } else {
        return 0; // Returns empty deficit
    }
}

void EmpowerQueueInfoBase::process_packet_enqueue(int dscp, Timestamp timestamp){

    if (_debug){
        click_chatter("%{element} :: %s :: process packet ENqueue for DSCP: %d and Timestamp: %s!",
                      this,
                      __func__,
                      dscp,
                      timestamp.unparse().c_str());
    }

    DSCPTimestampEnqueuedPacketsMap :: Pair * crr_dscp_timstamp_enqueued_pkts_pair = dscp_timestamp_enqueued_pkts_map.find_pair(dscp);

    // if DSCP exists in timestamp enqueued packets map
    if (crr_dscp_timstamp_enqueued_pkts_pair){
        if (_debug){
            click_chatter("%{element} :: %s :: Enqueued packets vector size is: %d",
                          this,
                          __func__,
                          crr_dscp_timstamp_enqueued_pkts_pair->value.size());
        }

        // Include packet timestamp at the end of the vector
        crr_dscp_timstamp_enqueued_pkts_pair->value.push_back(timestamp);

    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: Initializing Timestamp vector for DSCP: %d",
                          this,
                          __func__,
                          dscp);
        }

        // Create vector for packet timestamps
        Vector<Timestamp> timestamp_enqueued_pkts_vector;

        // Inserting current time into it
        timestamp_enqueued_pkts_vector.push_back(timestamp);

        // Inserting the vector of timestamps into DSCP hashmap
        dscp_timestamp_enqueued_pkts_map.insert(dscp, timestamp_enqueued_pkts_vector);
    }
}

Timestamp EmpowerQueueInfoBase::get_queue_delay(int dscp) {
    // Getting the average latency from the map..
    DSCPQueueDelayMap :: Pair * crr_dscp_queue_delay_pair = dscp_queue_delay_map.find_pair(dscp);

    if (crr_dscp_queue_delay_pair){
        return crr_dscp_queue_delay_pair->value;
    } else {
        return Timestamp(); // Returns empty timestamp
    }
}

void EmpowerQueueInfoBase::process_packet_dequeue(int dscp, Timestamp timestamp) {

    if (_debug){
        click_chatter("%{element} :: %s :: process packet DEqueue for DSCP: %d and Timestamp: %s!",
                      this,
                      __func__,
                      dscp,
                      timestamp.unparse().c_str());
    }

    DSCPTimestampEnqueuedPacketsMap :: Pair * crr_dscp_timstamp_enqueued_pkts_pair = dscp_timestamp_enqueued_pkts_map.find_pair(dscp);
    assert(crr_dscp_timstamp_enqueued_pkts_pair);

    // Check if the DSCP exists in the hashmap
    if (crr_dscp_timstamp_enqueued_pkts_pair){

        // Check if there is at least one packet in the vector
        if (crr_dscp_timstamp_enqueued_pkts_pair->value.size() > 0) {

            Vector<Timestamp>::const_iterator first_timestamp = crr_dscp_timstamp_enqueued_pkts_pair->value.begin();

            // Calculate queue delay for the DSCP
            Timestamp crr_queue_delay = timestamp - *first_timestamp;

            if (_debug){
                click_chatter("%{element} :: %s :: First timestamp: %s vs current: %s, calculated: %s, unparse_interval: %s!",
                              this,
                              __func__,
                              first_timestamp->unparse().c_str(),
                              timestamp.unparse().c_str(),
                              crr_queue_delay.unparse().c_str(),
                              crr_queue_delay.unparse_interval().c_str());
            }

            // Add delay into the DSCP delay packets map
            DSCPDelayPacketsMap :: Pair * crr_dscp_delay_pkts_pair = dscp_delay_packets_map.find_pair(dscp);

            if (crr_dscp_delay_pkts_pair){

                // Adding the crr delay into de map
                crr_dscp_delay_pkts_pair->value.push_back(crr_queue_delay);

                if (_debug){
                    click_chatter("%{element} :: %s :: DSCP: %d and size(): %d!",
                                  this,
                                  __func__,
                                  dscp,
                                  crr_dscp_delay_pkts_pair->value.size());
                }

            } else {
                // Creating vector and adding the delay into it
                Vector<Timestamp> crr_delays_vector;
                crr_delays_vector.push_back(crr_queue_delay);

                // Inserting it in the map!
                dscp_delay_packets_map.insert(dscp, crr_delays_vector);
            }

            // Remove first timestamp from the vector
            crr_dscp_timstamp_enqueued_pkts_pair->value.pop_front();
        } else {
            click_chatter("%{element} :: %s :: error: skipping packet DEqueue because timestamp vector is empty or not valid!", this, __func__);
        }
    } else {
        if (_debug){
            click_chatter("%{element} :: %s :: error: packet DEqueue hashmap pair not valid!", this, __func__);
        }
    }
}

enum {
    H_DEBUG
};

String EmpowerQueueInfoBase::read_handler(Element *e, void *thunk) {
    EmpowerQueueInfoBase *td = (EmpowerQueueInfoBase *) e;
    switch ((uintptr_t) thunk) {
        case H_DEBUG:
            return String(td->_debug) + "\n";
        default:
            return String();
    }
}

int EmpowerQueueInfoBase::write_handler(const String &in_s, Element *e,
                                      void *vparam, ErrorHandler *errh) {

    EmpowerQueueInfoBase *f = (EmpowerQueueInfoBase *) e;
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

void EmpowerQueueInfoBase::add_handlers() {
    add_read_handler("debug", read_handler, (void *) H_DEBUG);
    add_write_handler("debug", write_handler, (void *) H_DEBUG);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(EmpowerQueueInfoBase)