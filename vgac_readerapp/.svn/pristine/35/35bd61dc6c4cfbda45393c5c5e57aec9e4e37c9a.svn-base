#!/bin/sh
#
# post day zero processing
#
# this script goes in with the day-zero upgrade in the manifest
#
#

#disable old TZ
if [ -f /etc/TZ ]; then
    mv -f /etc/TZ /etc/TZ.bak
fi

# disable wireless drivers don't have any (speeds up boot)
if [  -f /etc/init.d/S14wireless ]; then
    mv -f /etc/init.d/S14wireless /etc/init.d/disabled.S14wireless
fi

# get rid of the old network stuff
#if [  -f /etc/init.d/S40network ]; then
#    mv -f /etc/init.d/S40network /etc/init.d/disabled.S40network
#fi

# disable day-zero and DAMS
touch /afc/day-zero/done
touch /afc/dams/done

sync

# here we should reboot
reboot
# EOF

