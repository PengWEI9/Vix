#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that when two locks are held a script that needs them will run once
# both locks are released.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1} -- /bin/sh -c "sleep 4 ; touch ${tagfile1} ; exit 7" &
pid1=$!
${lockrun} -f ${lockfile2} -- /bin/sh -c "sleep 6 ; touch ${tagfile2} ; exit 3" &
pid2=$!
t=$(date +%s)
sleep 1

${lockrun} -f ${lockfile1} -w 6 -F ${lockfile2} -W 4 -- /bin/sh -c "sleep 1 ; touch ${tagfile3} ; exit 3"
assert_exitcode 3
assert_elapsed ${t} 5 8

wait ${pid1}
assert_exitcode 7
wait ${pid2}
assert_exitcode 3

assert_file_exists ${tagfile1}
assert_file_exists ${tagfile2}
assert_file_exists ${tagfile3}

