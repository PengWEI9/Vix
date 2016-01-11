#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   This is a wrapper around unlocked_update_stats.sh; it prevents more than one
#   instance of unlocked_update_stats.sh from executing at any one time.
#
#################################################################################

/afc/bin/lockrun.sh -f /tmp/.lock-update_stats -w 10 -- /afc/bin/unlocked_update_stats.sh "$@"
exit $?
