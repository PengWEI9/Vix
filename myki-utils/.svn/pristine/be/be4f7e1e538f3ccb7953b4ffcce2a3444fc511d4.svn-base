#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that when a single instance of a shell script holds a lock, another
# instance that waits for that lock first can run once the first process
# finishes.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1} -- /bin/sh -c "sleep 4 ; touch ${tagfile1} ; exit 7" &
pid=$!
t=$(date +%s)
sleep 1

${lockrun} -f ${lockfile1} -w 6 -F ${lockfile2} -- /bin/sh -c "sleep 1 ; touch ${tagfile2} ; exit 3"
assert_exitcode 3
assert_elapsed ${t} 3 6

wait ${pid}
assert_exitcode 7

assert_file_exists ${tagfile1}
assert_file_exists ${tagfile2}

