#===============================================================================
# **** Copyright (c) 2015 VIX TECHNOLOGY (AUST) LTD ****
#
#===============================================================================

# Specify the DEBIAN packaging files
PackageDescription ("myki_utils-unittest");

CopyFile    ('unittest.sh'                      ,'/opt/test/myki_utils');
SetFilePerms('a+rx'                             ,'/opt/test/myki_utils/unittest.sh');

CopyFile    ('unittest-ontarget.sh'             ,'/opt/test/myki_utils/bin');
SetFilePerms('a+rx'                             ,'/opt/test/myki_utils/bin', '--Recurse');

CopyDir     ('lockrun'                          ,'/opt/test/myki_utils/lockrun');
SetFilePerms('a+rx'                             ,'/opt/test/myki_utils/lockrun', '--Recurse');
