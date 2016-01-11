#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   This is a wrapper around unlocked_transferud.sh; it prevents more than one
#   instance of unlocked_transferud.sh from executing at any one time.
#
#   It also prevents unlocked_transferud.sh and unlocked_transfer_alarms.sh
#   from executing at the same time by coordinating with transfer_alarms.sh.
#
#################################################################################

# NOTE: we must always take the transferud lock first to prevent possible
#       deadlock.  We will wait for up to 1 hours for transfer alarms to finish
#       as by observation no run of transfer alarms should take this long.
/afc/bin/lockrun.sh -f /tmp/.lock-transferud -F /tmp/.lock-transfer_alarms -W 3600 -- /afc/bin/unlocked_transferud.sh "$@"
exit $?
