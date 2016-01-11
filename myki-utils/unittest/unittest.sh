#!/bin/sh
#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
# Unit test execution framework for lockrun.sh.
#===============================================================================

test_name=$(basename $0 | sed "s|.sh$||")

export UNITTEST=1

# Obtain the environment and command line arguments.
base_dir=$(pwd)
work_dir=${base_dir}/${test_name}
tag_ext=ut_tag
lock_ext=ut_lock

unset bin_dir
while [ "$1" != "" ]; do
    param=$(echo $1 | awk -F= '{print $1}')
    value=$(echo $1 | awk -F= '{print $2}')
    case "${param}" in
        -LocalBinDir)   bin_dir="${value}";;
        *)
            echo "ERROR: Unexpected argument \"$1\""
            exit 1
            ;;
    esac
    shift
done




# Asserts that the file $1 does not exist.
assert_file_does_not_exist()
{
    if [ -e "$1" ]; then
        echo " * FAILED: The file $1 must not exist"
        exit 1
    fi
    echo " - File $1 does not exist as expected"
        return 0
}

# Asserts that the file $1 does exist.
assert_file_exists()
{
    if [ ! -e "$1" ]; then
        echo " * FAILED: The file $1 must exist"
        exit 1
    fi
    echo " - File $1 exists as expected"
    return 0
}

# Asserts that the current exit code is $1
assert_exitcode()
{
    local ec=$?
    if [ $ec -ne $1 ]; then
        echo " * FAILED: The exit code must be $1 but $ec was found"
        exit 1
    fi
    echo " - Exit code $ec as expected"
    return 0
}

# Asserts that the time elapsed from $1 is in the range of $2 to $3 seconds.
assert_elapsed()
{
    local t=$1
    local n=$(date +%s)
    local elapsed=$((n-t))
    if [ ${elapsed} -lt $2 ] || [ ${elapsed} -gt $3 ]; then
        echo " * FAILED: The elapsed time must be in the range [$2, $3] but was ${elapsed}"
        exit 1
    fi
    echo " - Elapsed time of ${elapsed} was valid in the range [$2, $3]"
    return 0
}

# Delete all the files from the work directory that have the specified match.
rm -fr ${work_dir}/*.${tag_ext}
rm -fr ${work_dir}/*.${lock_ext}
mkdir -p ${work_dir}

lockfile1="${work_dir}/lockfile_1.${lock_ext}"
lockfile2="${work_dir}/lockfile_2.${lock_ext}"
lockfile3="${work_dir}/lockfile_3.${lock_ext}"
lockfile4="${work_dir}/lockfile_4.${lock_ext}"

tagfile1="${work_dir}/tag_1.${tag_ext}"
tagfile2="${work_dir}/tag_2.${tag_ext}"
tagfile3="${work_dir}/tag_3.${tag_ext}"
tagfile4="${work_dir}/tag_4.${tag_ext}"
tagfile5="${work_dir}/tag_5.${tag_ext}"

if [ "${UNITTEST_ON_TARGET}" = "" ]; then
    lockrun="${base_dir}/../../src/scripts/lockrun.sh"
else
    lockrun="/afc/bin/lockrun.sh"
fi
chmod +x ${lockrun}

echo ""
echo "================================================================================"
echo "Unit Test Started: ${test_name}"
echo "  --> Working Directory       = \"${work_dir}\""
echo "  --> Binaries Directory      = \"${bin_dir}\""
echo "  --> Run Single Instance     = \"${lockrun}\""
echo ""
echo ""
