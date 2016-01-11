#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Unit test execution sequence for myki_utils on the target device.
#===============================================================================

work_dir="/opt/test/myki_utils/work"
lockrun_dir="/opt/test/myki_utils/lockrun"

export UNITTEST_ON_TARGET=1

rm -fr ${work_dir}
mkdir -p ${work_dir}
cd ${work_dir}

scripts=$(find "${lockrun_dir}" -name "*.sh")
i=0
for s in $scripts; do

    ${s}
    ec=$?
    if [ $ec -ne 0 ]; then
        echo ""
        echo " ***** TEST FAILED *****"
        exit 1
    fi
    i=$((i+1))
done

echo ""
echo "All tests passed (${i} total)."
exit 0
