#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Verifies that when a single instance of a shell script is running under lock
# protection that another instance cannot start.
#===============================================================================
. ../unittest.sh

${lockrun} -f ${lockfile1} -- /bin/sh -c "sleep 3 ; touch ${tagfile1} ; exit 7" &
pid=$!
sleep 1

${lockrun} -f ${lockfile1} -- /bin/sh -c "sleep 1 ; touch ${tagfile2} ; exit 3"
assert_exitcode 201

wait ${pid}
assert_exitcode 7

assert_file_exists ${tagfile1}
assert_file_does_not_exist ${tagfile2}

