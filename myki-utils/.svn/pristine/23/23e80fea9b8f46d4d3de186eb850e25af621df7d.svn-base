#!/bin/bash
#################################################################################
#
#     Vix Technology                   Licensed software
#     (C) 2015                         All rights reserved
#
#==============================================================================
#
#   This is a wrapper around unlocked_check_cfengine.sh; it prevents more than
#   one instance of unlocked_check_cfengine.sh from executing at any one time.
#
#################################################################################

/afc/bin/lockrun.sh -f /tmp/.lock-check_cfengine -- /afc/bin/unlocked_check_cfengine.sh "$@"
exit $?
