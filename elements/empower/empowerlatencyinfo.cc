/*
 * empowerlatencyinfo.{cc,hh} -- handles 802.11 packets info for latency
 * computation (EmPOWER Access Point)
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
#include "empowerlatencyinfo.hh"
CLICK_DECLS


EmpowerLatencyInfo::EmpowerLatencyInfo() :
        _el(0), _max_samples(100), _debug(false), waiting_for_ack(false) {
}

EmpowerLatencyInfo::~EmpowerLatencyInfo() {
}

int EmpowerLatencyInfo::configure(Vector<String> &conf,
                                 ErrorHandler *errh) {

    return Args(conf, this, errh)
            .read_m("EL", ElementCastArg("EmpowerLVAPManager"), _el)
            .read("MAX_SAMPLES", _max_samples)
            .read("DEBUG", _debug)
            .complete();
}

bool EmpowerLatencyInfo::process_outgoing_packet(EtherAddress src, EtherAddress dst, EtherAddress  bssid, int dscp, Timestamp timestamp){

    // Assigning values
    crr_data_packet_info.dscp = dscp;
    crr_data_packet_info.src = src;
    crr_data_packet_info.dst = dst;
    crr_data_packet_info.bssid = bssid;
    crr_data_packet_info.timestamp = timestamp;

    // TODO: Add to the vector
    //last_tx_pkts_vector.push_back(crr_data_packet_info);

    if (waiting_for_ack){
        click_chatter("%{element} :: %s :: THERE IS A PACKET WAITING FOR ACK!",
                      this,
                      __func__);
    } else {
        if (_debug) {
            // Now waiting for ACK
            waiting_for_ack = true;

            if (_debug){
                click_chatter("%{element} :: %s :: Waiting for ACK: TRUE",
                              this,
                              __func__);
            }
        }
    }

    waiting_for_ack = true;
    return true;
}

bool EmpowerLatencyInfo::process_incoming_packet(EtherAddress bssid_ethernet_address, Timestamp timestamp){

    /*if (_debug) {
        click_chatter("%{element} :: %s :: Comparing BSSIDs: %s with %s",
                      this,
                      __func__,
                      crr_timestamp_info.bssid.unparse().c_str(),
                      bssid_ethernet_address.unparse().c_str());
    }*/

    //TODO:
    // check if the vector is not empty then...
    //      if type is data...
    //          calculate rtt over the first entry of the vector
    //          remove the first entry of the vector
    //      else...
    //          remove packet and do not calculate latency for it
    if (crr_data_packet_info.bssid == bssid_ethernet_address) {

        if (_debug) {
            click_chatter("%{element} :: %s :: IN ACK -> MAC: %s!",
                          this,
                          __func__,
                          bssid_ethernet_address.unparse().c_str());
        }

        if (waiting_for_ack) {
            // Releasing waiting for ACK
            waiting_for_ack = false;
            return true;
        } else {
            click_chatter("%{element} :: %s :: NOT WAITING FOR ACK!",
                          this,
                          __func__);
        }
    }
    return false;


    // Checking if lvap_timestamp_map has lvap_ethernet_address and compute the latency later
    //LVAPTimestampMap::Pair * crr_timestamp_pair = lvap_timestamp_map.find_pair(lvap_ethernet_address.unparse().c_str());
    //assert(crr_timestamp_pair);

    // Current Round-Trip Time (ms)
    //Timestamp crr_rtt = timestamp - crr_timestamp_pair->value.timestamp;

    //TODO Review
    //Timestamp crr_rtt = timestamp - crr_timestamp_info.timestamp;

    //TODO: Cleanup
    /*if (_debug) {
        click_chatter("%{element} :: %s :: IN packet LVAP: %s, timestamp.unparse: %s, unparse_interval: %s, and crr_rtt.msec(): %d",
                      this,
                      __func__,
                      lvap_ethernet_address.unparse().c_str(),
                      crr_rtt.unparse().c_str(),
                      crr_rtt.unparse_interval().c_str(),
                      crr_rtt.msec());
    }*/

    // Check whether LVAP is in the map
    //TODO Review
    //LVAPLatencyMap::Pair * lvap_info_pair = lvap_latency_map.find_pair(lvap_ethernet_address.unparse().c_str());

    // If LVAP and DSCP are not present: create key
    //TODO Review
    //if (lvap_info_pair){

        //TODO: Cleanup
        //if (_debug) {
        //    click_chatter("%{element} :: %s :: if (lat_info_pair)",
        //                  this,
        //                  __func__);
        //}

        // Verifying if DSCP is present
        //TODO Review
        //DSCPLatencyMap::Pair * dscp_info_pair = lvap_info_pair->value.find_pair(crr_timestamp_info.dscp);

        // Checking the vector size for rotating the last measurements (until MAX_SAMPLES)
        //TODO Review
        //if (dscp_info_pair->value.latency_vector.size() == _max_samples) {
        //    dscp_info_pair->value.last_seen_vector.pop_front();
        //    dscp_info_pair->value.latency_vector.pop_front();
        //}

        // Inserting Current Time and Current RTT into them
        //TODO Review
        //dscp_info_pair->value.last_seen_vector.push_back(timestamp);
        //dscp_info_pair->value.latency_vector.push_back(crr_rtt);

        //TODO: Cleanup
        //if (_debug) {
        //    click_chatter("%{element} :: %s :: VECTOR: dscp_info_pair: (key, size) (%d, %d)",
        //                  this,
        //                  __func__,
        //                  dscp_info_pair->key,
        //                  dscp_info_pair->value.latency_vector.size());
        //}

        //TODO Review
        //Timestamp sum = Timestamp();
        //for (Vector<Timestamp>::const_iterator i = dscp_info_pair->value.latency_vector.begin(); i!= dscp_info_pair->value.latency_vector.end(); i++) {
        //    sum = sum + *i;
            /*if (_debug) {
                click_chatter("%{element} :: %s :: For my vector i: %d, i*:%d, and sum: %d",
                              this,
                              __func__,
                              i,
                              *i,
                              sum);
            }*/
        //}
        //TODO Review
        //Timestamp avg = sum/int(dscp_info_pair->value.latency_vector.size());

        //TODO: Cleanup
        //if (_debug) {
        //    click_chatter("%{element} :: %s :: SUM: %d, unparse: %s, unparse_interval: %s, SIZE: %d, AVG: %d, unparse: %s, unparse_interval: %s",
        //                  this,
        //                  __func__,
        //                  sum,
        //                  sum.unparse().c_str(),
        //                  sum.unparse_interval().c_str(),
        //                  dscp_info_pair->value.latency_vector.size(),
        //                  avg,
        //                  avg.unparse().c_str(),
        //                  avg.unparse_interval().c_str());
        //}

    //} else {

        //TODO: Cleanup
        //if (_debug) {
        //    click_chatter("%{element} :: %s :: else",
        //                  this,
        //                  __func__);
        //}

        // Create HashMap and Vector for the DSCP
        //TODO Review
        //DSCPLatencyMap crr_dscp_map;
        //latency_info crr_lat_info;

        // Inserting Current Time and Current RTT into them
        //TODO Review
        //crr_lat_info.last_seen_vector.push_back(timestamp);
        //crr_lat_info.latency_vector.push_back(crr_rtt);

        // Inserting the Latency Info into DSCP hashmap
        //TODO Review
        //crr_dscp_map.insert(crr_timestamp_info.dscp, crr_lat_info);

        // Inserting the DSCP hashmap into the LVAP hashmap
        //TODO Review
        //lvap_latency_map.insert(lvap_ethernet_address.unparse().c_str(), crr_dscp_map);
    //}
    //TODO: Cleanup
    /*if (_debug){
        click_chatter("%{element} :: %s :: Check incoming packet from LVAP: %s and timestamp: %d, crr_rtt.msec(): %d, lat_info_pair: %d",
                      this,
                      __func__,
                      lvap_ethernet_address.unparse().c_str(),
                      timestamp,
                      crr_rtt.msec(),
                      lvap_info_pair);
    }*/
}

enum {
    H_DEBUG,
    H_MAX_SAMPLES
};

String EmpowerLatencyInfo::read_handler(Element *e, void *thunk) {
    EmpowerLatencyInfo *td = (EmpowerLatencyInfo *) e;
    switch ((uintptr_t) thunk) {
        case H_DEBUG:
            return String(td->_debug) + "\n";
        case H_MAX_SAMPLES:
            return String(td->_max_samples) + "\n";
        default:
            return String();
    }
}

int EmpowerLatencyInfo::write_handler(const String &in_s, Element *e,
                                     void *vparam, ErrorHandler *errh) {

    EmpowerLatencyInfo *f = (EmpowerLatencyInfo *) e;
    String s = cp_uncomment(in_s);

    switch ((intptr_t) vparam) {
        case H_DEBUG: {    //debug
            bool debug;
            if (!BoolArg().parse(s, debug))
                return errh->error("debug parameter must be boolean");
            f->_debug = debug;
            break;
        }
        case H_MAX_SAMPLES: {
            int max_samples;
            if (!IntArg().parse(s, max_samples))
                return errh->error("max samples parameter must integer");
            if (max_samples < 2)
                return errh->error("max samples parameter has to be bigger than 2 samples");
            f->_max_samples = max_samples;
            break;
        }
    }
    return 0;
}

void EmpowerLatencyInfo::add_handlers() {
    add_read_handler("debug", read_handler, (void *) H_DEBUG);
    add_write_handler("debug", write_handler, (void *) H_DEBUG);
    add_read_handler("max_samples", read_handler, (void *) H_MAX_SAMPLES);
    add_write_handler("max_samples", write_handler, (void *) H_MAX_SAMPLES);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(EmpowerLatencyInfo)
ELEMENT_REQUIRES(userlevel)