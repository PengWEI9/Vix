###############################################################################
#
#   Reader Common  - Common Libs and headers for card reader projects
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

#BuildPlatforms ( 'COBRA' );          # Cobra
BuildPlatforms ( 'DEVLINUX' );     # Desktop Linux
BuildPlatforms ( 'WIN32' );          # Desktop Windows
BuildPlatforms ( 'LINUX_I386' );

BuildName      ( 'reader-common 1.3.0023 mbu' );

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
LinkPkgArchive ( 'cs', '26.1.23003.cr' );
LinkPkgArchive ( 'utf', '25.5.6002.cr' );
LinkPkgArchive ( 'solidbasetypes', '25.4.0100.cr' );
LinkPkgArchive ( 'zlib_linux', '1.2.5.8002.cots' );
BuildPkgArchive ( 'linux_drivers_serpent-api', '3.4.1000.cr' );
BuildPkgArchive ( 'support_serpent', '7.9.8000.cr' );
BuildPkgArchive ( 'support_serpent-api', '1.4.6000.cr' );


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
BuildMake      ();
