#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that a single lock is not inherited by any child processes that are
# launched as part of the main script.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1} -- /bin/sh -c "(sleep 10 &) ; touch ${tagfile1} ; exit 7"
assert_exitcode 7
assert_file_exists ${tagfile1}

${lockrun} -f ${lockfile1} -- /bin/sh -c "touch ${tagfile2} ; exit 3"
assert_exitcode 3
assert_file_exists ${tagfile2}

