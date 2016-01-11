# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the main build file for the qt_tutorial_vdk demonstration
# application that shows how to build QT applications using the iVal
# development kit.

# These entries are required by the build system and must not be changed.
$MAKELIB_PL     = "$ENV{ GBE_TOOLS }/makelib.pl";
$BUILDLIB_PL    = "$ENV{ GBE_TOOLS }/buildlib.pl";
require         "$BUILDLIB_PL";
require         "$MAKELIB_PL";

# The platform on which this application builds - PPC_603E is used for the
# iVal product range as it is the PowerPC core type on the Viper processor
# card.
#
# The --OnlyProd option specifies that only production binaries are built,
# instead of both production and debug binaries.
BuildPlatforms( 'COBRA', 'UBUNTU12', 'PPC_603E', '--Uses=DC' ); #UBUNTU12 build requires daf libraries to be built first

# The name of this package along with its version number.  Version numbers
# must be of the form x.y.zzzz cr.
BuildName( 'vgac_readerapp 2.2.92001 mbu' );

# Boiler-plate for the build system, do not modify.
BuildSnapshot();
BuildAccessPerms();
BuildInterface( 'local' );
BuildInterface( 'interface' );

# This requests a source code tgz archive be built for this demonstartion
# package for redistribution.  If you do not plan to redistribute the source
# code to the package this line can be omitted.
# BuildSrcArchive();

# Specifies the set of packages that this application depends on for its
# build.
BuildPkgArchive ( 'basetypes', '25.5.7006.cr' );
BuildPkgArchive('boost', '1.58.0.0001.cots');
LinkPkgArchive ( 'cmdutil', '25.2.4080.cr' );
LinkPkgArchive ( 'commonlib', '1.0.15000.cr' );
BuildPkgArchive ( 'crc', '26.8.5054.cr' );
BuildPkgArchive ( 'crypto-ansi-data-storage', '1.3.2113.cr' );
BuildPkgArchive ( 'crypto-api', '3.6.0000.cr' );
BuildPkgArchive ( 'crypto-lock', '1.0.3060.cr' );
BuildPkgArchive ( 'cs', '26.2.17000.cr' );
LinkPkgArchive ( 'curl', '7.30.0.0006.cots' );
LinkPkgArchive ( 'daf_bertlv', '2.0.1029.cr' );
LinkPkgArchive ( 'daf_ct_api', '27.4.0000.cr' );
LinkPkgArchive ( 'daf_ct_crypto_stubs', '2.3.1039.cr' );
LinkPkgArchive ( 'daf_ct_desfire_virtual', '26.7.0018.cr' );
LinkPkgArchive ( 'daf_ct_unified', '26.3.5070.cr' );
LinkPkgArchive ( 'daf_ct_unified_mcr_driver', '26.8.0053.cr' );
LinkPkgArchive ( 'daf_ct_viper_mcr_comms', '1.0.3009.cr' );
LinkPkgArchive ( 'daf_desfire', '1.0.9012.cr' );
LinkPkgArchive ( 'daf_myki_api', '4.10.0000.cr' );
LinkPkgArchive ( 'daf_myki_cardservices', '3.15.10009.cr' );
LinkPkgArchive ( 'daf_myki_crypto', '1.4.1032.cr' );
LinkPkgArchive ( 'daf_myki_fs', '1.9.1029.cr' );
LinkPkgArchive ( 'daf_osa_mos_lib', '25.11.2018.cr' );
LinkPkgArchive ( 'daf_retcodes', '25.4.11000.cr' );
LinkPkgArchive ( 'daf_utils_bitstream', '25.0.13000.cr' );
LinkPkgArchive ( 'daf_utils_date_time', '25.5.4010.cr' );
LinkPkgArchive ( 'daf_utils_serial_comms', '25.7.13027.cr' );
LinkPkgArchive ( 'daf_utils_timer', '26.0.4001.cr' );
LinkPkgArchive ( 'daf_utils_timingpoint', '26.2.0029.cr' );
LinkPkgArchive ( 'datertswis', '25.2.13082.cr' );
LinkPkgArchive ( 'dc-messages', '1.2.17000.mbu' );
BuildPkgArchive ( 'debian_dpkg', '1.13.25.1000.cots' );
BuildPkgArchive ( 'debian_packager', '1.20.0000.cr' );
BuildPkgArchive ( 'Dinkumware_STL', '2.0.0000.cots' );
LinkPkgArchive ( 'jsoncpp', '0.1.1000.cots' );
BuildPkgArchive ( 'link', '15.1.0013.cr' );
BuildPkgArchive ( 'link_api', '6.1.0000.cr' );
BuildPkgArchive ( 'linux_types', '1.3.2000.cr' );
BuildPkgArchive ( 'llcomms', '32.1.0039.cr' );
BuildPkgArchive ( 'llcomms_api', '3.0.3000.cr' );
BuildPkgArchive ( 'mcrfirmware', '14.13.0000.cr' );
BuildPkgArchive ( 'mcrfirmware-api', '8.0.0000.cr' );
LinkPkgArchive ( 'mcr_host_protocol', '31.5.0067.cr' );
LinkPkgArchive ( 'myki-tmi', '3.0.60000.mbu' );
LinkPkgArchive ( 'myki_alarm', '1.5.23003.mbu' );
LinkPkgArchive ( 'myki_avl', '1.1.19004.mbu' );
LinkPkgArchive ( 'myki_br', '1.2.54000.mbu' );
LinkPkgArchive ( 'myki_cd', '3.8.30000.mbu' );
LinkPkgArchive ( 'myki_common', '1.7.0000.mbu' );
LinkPkgArchive ( 'myki_ldt', '2.2.15003.mbu' );
LinkPkgArchive ( 'myki_ud', '2.0.4024.mbu' );
LinkPkgArchive ( 'myki_xml2ini', '1.0.21027.mbu' );
LinkPkgArchive ( 'openssl', '1.0.1j.0000.cots' );
LinkPkgArchive ( 'reader-common', '1.4.4000.mbu' );
LinkPkgArchive ( 'reader-messages', '1.0.11001.mbu' );
LinkPkgArchive ( 'readerapp', '2.2.200000.mbu' );
BuildPkgArchive ( 'solidbasetypes', '25.4.0100.cr' );
BuildPkgArchive ( 'support_serpent', '7.9.8000.cr' );
BuildPkgArchive ( 'support_serpent-api', '1.4.6000.cr' );
BuildPkgArchive ( 'zlib_linux', '1.2.5.8002.cots' );
BuildPkgArchive ( 'utf', '25.7.1002.cr' );

# This is the list of subdirectories to search for 'makefile.pl' build 
# instruction files.
BuildSubDir( 'src' );

# Boiler-plate for the build system, do not modify.
BuildDescpkg();
BuildVersion();

BuildMake();
# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
