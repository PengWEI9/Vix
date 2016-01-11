# Copyright (C) 2013 Vix Technology Pty Ltd, All Rights Reserved
#

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
BuildPlatforms( 'PPC_603E', 'UBUNTU12');

# The name of this package along with its version number.  Version numbers
# must be of the form x.y.zzzz cr.
BuildName( 'myki_dc 1.3.107000 mbu' );

# Boiler-plate for the build system, do not modify.
BuildSnapshot();
BuildAccessPerms();
BuildInterface( 'local' );
BuildInterface( 'interface' );

LinkPkgArchive ( 'basetypes', '25.5.7006.cr' );
LinkPkgArchive ( 'cce', '0.4.2003.mbu' );
LinkPkgArchive ( 'cmdutil', '25.2.4080.cr' );
LinkPkgArchive ( 'cs', '26.2.17000.cr' );
LinkPkgArchive ( 'datertswis', '25.2.13082.cr' );
LinkPkgArchive ( 'dc-messages', '1.2.19000.mbu' );
LinkPkgArchive ( 'debian_dpkg', '1.13.25.1000.cots' );
LinkPkgArchive ( 'debian_packager', '1.20.0000.cr' );
LinkPkgArchive ( 'jsoncpp', '0.1.1000.cots' );
LinkPkgArchive ( 'libWebsockets', '1.22.3000.cots' );
LinkPkgArchive ( 'mbu-gate-x', '0.7.8000.mbu' );
LinkPkgArchive ( 'myki-varop-messages', '1.1.0000.mbu' );
LinkPkgArchive ( 'myki_alarm', '1.5.23003.mbu' );
LinkPkgArchive ( 'myki_avl', '1.1.21002.mbu' );
LinkPkgArchive ( 'myki_cd', '3.8.30000.mbu' );
LinkPkgArchive ( 'myki_common', '1.7.0000.mbu' );
LinkPkgArchive ( 'myki_schedule', '1.0.22001.mbu' );
LinkPkgArchive ( 'reader-common', '1.4.4000.mbu' );
LinkPkgArchive ( 'support_serpent', '7.9.8000.cr' );
LinkPkgArchive ( 'support_serpent-api', '1.4.6000.cr' );
LinkPkgArchive('utf', '25.7.1002.cr' );
BuildPkgArchive('crc', '26.8.5054.cr');
LinkPkgArchive('commonlib', '1.0.15000.cr');
BuildPkgArchive('boost', '1.58.0.0001.cots');

# Boiler-plate for the build system, do not modify.
BuildDescpkg();
BuildVersion();

BuildMake();
