# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the
# QT tutorial application.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

SubDir( 'cli' );

# build for cobra and pc platforms
Platform    ( '*');

AddFlags    ('*', '-D_REENTRANT' );
AddFlags    ('*', '-DCSDEBUG=9' );


Src ( '*' , 'version.h');
Src ( '*' , 'version.c' );

Src ('*' , 'jsonParser.cpp');
Src ('*' , 'jsonParser.h');

Lib ('*' , "jsonParser", "jsonParser.cpp");
InstallLib( '*' , "jsonParser");
InstallHdr( '*' , @CHDRS );

Src ( '*' , 'MykiPrinter.h' );
Src ( '*' , 'MykiPrinter.cpp' );
Src ( '*' , 'main.cpp');


Prog        ( '*', 'dcprinter', 'MykiPrinter.cpp', 'main.cpp', 'version.c',
             '-Lpthread',
             '-Lstdc++',
             '-lreader_common',
             '-lcli',
             '-lepson_printer_manager',
             '-ljsonParser',
             '-ljsoncpp',
             '-lcsf',
             '-lcsx',
             '-lcs',
             '-losa',
             '--MultiScan'
             );

Prog        ( '*', 'testipc', 'test.cpp',
             '-Lpthread',
             '-Lstdc++',
             '-lreader_common',
             '-lcsf',
             '-lcsx',
             '-lcs',
             '-losa',
             '--MultiScan'
             );

PackageProg( '*', 'dcprinter' );
PackageProg( '*', 'testipc' );

MakeDebianPackage ('*', '--Script=PackageApp.pl' );

MakefileGenerate();
1;
