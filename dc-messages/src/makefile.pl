# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";


# Define the set of platforms that this component is built for.  By
# defining them as '*' we use the same platforms as defined in the
# build.pl file.
Platform    ( '*' );

Src			( '*', 'message_dc.h' );

PackageHdr  ( '*', @CHDRS ,"--Strip" );
PackageHdr  ( 'PPC_603E', 'message_reader.h', '--Subdir=DC', "--Strip" );

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;
