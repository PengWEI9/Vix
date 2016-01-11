###############################################################################
#
#   Myki_CD     Business rules library for Myki (Melbourne)
#
###############################################################################

###############################################################################
#
#   Build system
#
###############################################################################

$MAKELIB_PL     = "$ENV{ GBE_TOOLS }/makelib.pl";
$BUILDLIB_PL    = "$ENV{ GBE_TOOLS }/buildlib.pl";

require         "$BUILDLIB_PL";
require         "$MAKELIB_PL";

###############################################################################
#
#   Product configuration
#
###############################################################################

BuildPlatforms ( 'PPC_603E' );       # Viper
BuildPlatforms ( 'COBRA' );          # Cobra
BuildPlatforms ( 'UBUNTU12' );
#daf_myki_cardservices doesn't support LINUX_I386
#BuildPlatforms ( 'LINUX_I386' );

BuildName      ( 'myki_cd 3.8.25000 mbu' );

BuildSnapshot   ();
BuildAccessPerms();
BuildInterface  ( 'local' );
BuildInterface  ( 'interface' );

###############################################################################
#
#   External Packages
#
###############################################################################

LinkPkgArchive ( 'basetypes', '25.5.7006.cr' );
LinkPkgArchive ( 'crypto-api', '3.6.0000.cr' );
LinkPkgArchive ( 'cs', '26.2.12000.cr' );
LinkPkgArchive ( 'daf_ct_api', '27.4.0000.cr' );
LinkPkgArchive ( 'daf_desfire', '1.0.9012.cr' );
LinkPkgArchive ( 'daf_myki_api', '4.10.0000.cr' );
LinkPkgArchive ( 'daf_myki_cardservices', '3.15.10009.cr' );
LinkPkgArchive ( 'daf_retcodes', '25.4.11000.cr' );
LinkPkgArchive ( 'datertswis', '25.2.13074.cr' );
LinkPkgArchive ( 'jsoncpp', '0.1.1000.cots' );
LinkPkgArchive ( 'link_api', '6.1.0000.cr' );
LinkPkgArchive ( 'llcomms_api', '3.0.3000.cr' );
LinkPkgArchive ( 'myki_ldt', '2.2.15002.mbu' );
LinkPkgArchive ( 'myki_ud', '2.0.4018.mbu' );
BuildPkgArchive ( 'pugixml', '1.2.0000.cots' );
LinkPkgArchive ( 'solidbasetypes', '25.4.0100.cr' );
BuildPkgArchive ( 'sqlite', '3.8.4.2.cots' );
BuildPkgArchive ( 'sqlite_wrapper', '1.0.2003.cr' );
LinkPkgArchive ( 'utf', '25.5.6002.cr' );

###############################################################################
#
#   Specify subdirectories to process
#
###############################################################################

BuildSubDir    ( 'src' );

###############################################################################
#
#   Generate Files
#
###############################################################################

BuildDescpkg   ();
BuildVersion   ();
BuildMake      ();
