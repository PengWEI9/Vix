#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that when a single instance of a shell script holds a lock, another
# instance that takes that lock second cannot start.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile2} -- /bin/sh -c "sleep 3 ; touch ${tagfile1} ; exit 7" &
pid=$!
sleep 1

${lockrun} -f ${lockfile1} -F ${lockfile2} -- /bin/sh -c "sleep 1 ; touch ${tagfile2} ; exit 3"
assert_exitcode 202

wait ${pid}
assert_exitcode 7

${lockrun} -f ${lockfile2} -- /bin/sh -c "touch ${tagfile3} ; exit 8"
assert_exitcode 8

assert_file_exists ${tagfile1}
assert_file_does_not_exist ${tagfile2}
assert_file_exists ${tagfile3}

