#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that when a single instance of a shell script is running under lock
# protection that another instance will wait for it to finish before starting.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1} -- /bin/sh -c "sleep 4 ; touch ${tagfile1} ; exit 8" &
pid=$!
t=$(date +%s)

sleep 1
${lockrun} -f ${lockfile1} -w 6 -- /bin/sh -c "touch ${tagfile2} ; exit 6"
assert_exitcode 6
assert_elapsed ${t} 3 6

wait ${pid}
assert_exitcode 8

assert_file_exists ${tagfile1}
assert_file_exists ${tagfile2}


