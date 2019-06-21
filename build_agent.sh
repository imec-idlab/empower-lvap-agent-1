#!/usr/bin/env bash

AP="192.168.2.11"
DATE="20190519"
LEDE="/home/empower/empower/sdn_wifi_lede"
TARGET="target-x86_64_musl-1.1.16"

cp elements/empower/* $LEDE/build_dir/$TARGET/empower-lvap-agent-$DATE/elements/empower/

cd $LEDE

# make package/empower-lvap-agent/clean (optional)
make package/empower-lvap-agent/install V=99

# later copy to usr/bin/click
scp build_dir/$TARGET/empower-lvap-agent-$DATE/ipkg-install/usr/bin/empower-lvap-agentclick root@$AP:/root

# On the WTP run:
# /etc/init.d/empower restart