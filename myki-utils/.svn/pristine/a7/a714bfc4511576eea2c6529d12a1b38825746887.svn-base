#!/bin/perl
# Copyright (C) 1998-2001 ERG Limited, All rights reserved
#
# Module name   : build.pl
# Module type   : Makefile system
# Environment(s): n/a
#
# Description:    Configuration file for EMBEDDED development
#
# $Name:  $
# $Source: /export/repository/cvs/core/switch/build.pl,v $
# $Revision: 1.16.2.13 $ $Date: 2002/05/09 06:47:30 $ $State: Exp $
# $Author: sashav $ $Locker:  $
#.........................................................................#

#.. Build system
#
$MAKELIB_PL     = "$ENV{ GBE_TOOLS }/makelib.pl";
$BUILDLIB_PL    = "$ENV{ GBE_TOOLS }/buildlib.pl";

require         "$BUILDLIB_PL";
require         "$MAKELIB_PL";

#.. Toolset configuration
#

#.. Product configuration
#
BuildPlatforms  ( 'COBRA', 'UBUNTU12', 'PPC_603E' );
BuildName       ( 'myki_utils 1.0.123001 mbu' );
BuildSnapshot   ();
BuildAccessPerms();
#BuildReleaseFile( "$BUILDNAME", 'myki_switch 1.0.0000 MBU' );
BuildInterface  ( 'local' );
BuildInterface  ( 'interface' );

#..    Imported package archives
#
#      Note:    !current expands firstly to 'current_$USER'
#               then 'current'.
#

LinkPkgArchive ( 'cmdutil', '25.2.4080.cr' );
LinkPkgArchive ( 'cs', '26.2.17000.cr' );
LinkPkgArchive ( 'debian_dpkg', '1.13.25.1000.cots' );
LinkPkgArchive ( 'debian_packager', '1.20.0000.cr' );
LinkPkgArchive ( 'myki_alarm', '1.5.23003.mbu' );
LinkPkgArchive ( 'reader-common', '1.4.4000.mbu' );

BuildSubDir    ('src');
BuildSubDir    ('unittest');

#.. Build source tree makefile(s)
#
BuildDescpkg    ();
BuildVersion    ("myki_utils", "array");
BuildMake       ();

