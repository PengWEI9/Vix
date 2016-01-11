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

Src('*', 'helpers.h',
         'helpers.cpp',
         'transaction.h',
         'transaction.cpp',
         'tpursetransaction.h',
         'tpursetransaction.cpp',
         'cashtransaction.h',
         'txnbackupmanager.cpp',
         'txnbackupmanager.h',
         'txnmanager.h',
         'cartitem.h',
         'cartitem.cpp',
         'types.h',
         'topupcartitem.h',
         'topupcartitem.cpp',
         'txnmanager.cpp',
         'txnlist.h');

Lib('*', 'dc_txnlist', @OBJS);
InstallLib('*', 'dc_txnlist');
PackageLib('*', 'dc_txnlist');

InstallHdr('*', @CHDRS,'--Prefix=txnlist');
PackageHdr('*', @CHDRS,'--Prefix=txnlist');

MakefileGenerate();
1;
