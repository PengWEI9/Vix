# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

SubDir('test');
Platform('*');

AddFlags('*', '-D_REENTRANT');
AddFlags('COBRA', '-DCOBRA');

Src('*', 'printerbackupmanager.cpp', 
         'typedefs.h',
         'printerbackupmanager.h');

Lib('*', 'dc_printerbackup', @OBJS);
InstallLib('*', 'dc_printerbackup');
PackageLib('*', 'dc_printerbackup');

InstallHdr('*', @CHDRS, '--Prefix=printerbackup');
PackageHdr('*', @CHDRS, '--Prefix=printerbackup');

MakefileGenerate();
1;
