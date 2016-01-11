#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that a single instance of a shell script can execute under lock
# protection.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1}/a/b/c/d/f.lock -- /bin/sh -c "sleep 1 ; touch ${tagfile1} ; exit 7"
assert_exitcode 2
assert_file_does_not_exist ${tagfile1}

