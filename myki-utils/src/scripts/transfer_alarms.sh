#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   This is a wrapper around unlocked_transfer_alarms.sh; it prevents more than
#   one instance of unlocked_transfer_alarms.sh from executing at any one time.
#
#   It also prevents unlocked_transferud.sh and unlocked_transfer_alarms.sh
#   from executing at the same time by coordinating with transfer_alarms.sh.
#
#   transfer_alarms.sh will only wait for 10 seconds before aborting the transfer
#   attempt.
#
#################################################################################

/afc/bin/lockrun.sh -f /tmp/.lock-transfer_alarms -w 10 -- /afc/bin/unlocked_transfer_alarms.sh "$@"
exit $?
