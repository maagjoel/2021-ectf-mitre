#!/bin/bash

# 2021 Collegiate eCTF
# Launch a test echo deployment
# Ben Janis
#
# (c) 2021 The MITRE Corporation
#
# This source file is part of an example system for MITRE's 2021 Embedded System CTF (eCTF).
# This code is being provided only for educational purposes for the 2021 MITRE eCTF competition,
# and may not meet MITRE standards for quality. Use this code at your own risk!

set -e
set -m

if [ ! -d ".git" ]; then
    echo "ERROR: This script must be run from the root of the repo!"
    exit 1
fi

export DEPLOYMENT=echo
export SOCK_ROOT=$PWD/socks
export SSS_SOCK=sss.sock
export FAA_SOCK=faa.sock
export MITM_SOCK=mitm.sock
export START_ID=10
export END_ID=17
export SC_PROBE_SOCK=sc_probe.sock
export SC_RECVR_SOCK=sc_recvr.sock

# create deployment
make create_deployment
make add_sed SED=attack_c2 SCEWL_ID=10 NAME=c2 CUSTOM=' "PRNG_SEED=PRNGSD ATCK_DZ_ID=11 ATCK_DZ_X=0x1337 ATCK_DZ_Y=0x1337 TRST_DZ_ID=12 PACKAGE_FLAG=PKGFL" '
make add_sed SED=attack_dz SCEWL_ID=12 NAME=dz
make add_sed SED=attack_uav SCEWL_ID=37 NAME=uav1 CUSTOM='"PACKAGE_FLAG=PKGFL RECOVERY_FLAG=RCVFL DROP_FLAG=DRPFL UAVID_FLAG=UAVFL NO_FLY_ZONE_FLAG=NFZFL C2_ID=10 ALT_FLOOR=100 ALT_CEIL=200"'
make add_sed SED=attack_uav SCEWL_ID=29 NAME=uav2 CUSTOM='"PACKAGE_FLAG=PKGFL RECOVERY_FLAG=RCVFL DROP_FLAG=DRPFL UAVID_FLAG=UAVFL NO_FLY_ZONE_FLAG=NFZFL C2_ID=10 ALT_FLOOR=100 ALT_CEIL=200"'
make add_sed SED=attack_uav SCEWL_ID=22 NAME=uav3 CUSTOM='"PACKAGE_FLAG=PKGFL RECOVERY_FLAG=RCVFL DROP_FLAG=DRPFL UAVID_FLAG=UAVFL NO_FLY_ZONE_FLAG=NFZFL C2_ID=10 ALT_FLOOR=100 ALT_CEIL=200"'

# launch deployment
make deploy

# launch transceiver in background
python3 tools/faa.py $SOCK_ROOT/$FAA_SOCK &

# launch seds detatched
make launch_sed_d NAME=echo_server SCEWL_ID=10
sleep 1
make launch_sed_d NAME=echo_server SCEWL_ID=11
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=12
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=13
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=14
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=15
sleep 1
make launch_sed_d NAME=echo_client SCEWL_ID=16

# bring transceiver back into foreground
fg

echo "Killing docker containers..."
docker kill $(docker ps -q) 2>/dev/null
