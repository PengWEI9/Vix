###############################################################################
#
#   Myki_BR     Business rules library for Myki (Melbourne)
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
BuildPlatforms ( 'UBUNTU12_INSTRUMENT' );

BuildName      ( 'myki_br 1.2.49004 mbu' );

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
LinkPkgArchive ( 'daf_ct_viper_mcr_comms', '1.0.3009.cr' );
LinkPkgArchive ( 'daf_desfire', '1.0.9012.cr' );
LinkPkgArchive ( 'daf_myki_api', '4.10.0000.cr' );
LinkPkgArchive ( 'daf_myki_cardservices', '3.15.10009.cr' );
LinkPkgArchive ( 'daf_myki_fs', '1.9.1029.cr' );
LinkPkgArchive ( 'daf_retcodes', '25.4.11000.cr' );
LinkPkgArchive ( 'datertswis', '25.2.13074.cr' );
LinkPkgArchive ( 'Dinkumware_STL', '2.0.0000.cots' );
LinkPkgArchive ( 'link', '15.1.0013.cr' );
LinkPkgArchive ( 'link_api', '6.1.0000.cr' );
LinkPkgArchive ( 'llcomms_api', '3.0.3000.cr' );
LinkPkgArchive ( 'myki_cd', '3.8.29000.mbu' );
LinkPkgArchive ( 'myki_ldt', '2.2.15002.mbu' );
LinkPkgArchive ( 'myki_ud', '2.0.4018.mbu' );
LinkPkgArchive ( 'solidbasetypes', '25.4.0100.cr' );

###############################################################################
#
#   Specify subdirectories to process
#
###############################################################################

BuildSubDir    ( 'src' );
BuildSubDir    ( 'test' );

###############################################################################
#
#   Generate Files
#
###############################################################################

BuildDescpkg   ();
BuildVersion   ( "myki_br_", 'array', 'defs' );
BuildMake      ();
