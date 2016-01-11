# Copyright (C) 2009-2010 Vix Products Pty Ltd, All Rights Reserved
#
# This it the makefile that describes the build instructions for the 
# BusinessRules libary.

# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

SubDir( 'test' );
SubDir( 'etc' );
SubDir( 'utf' );

# Define the set of platforms that this component is built for.  By 
# defining them as '*' we use the same platforms as defined in the 
# build.pl file.
Platform    ( '*' );

AddFlags    ('COBRA', '-DCOBRA' );

# List all the source files that make up the application.
Src( '*', "Actionlist.cpp" ); 
Src( '*', "DeviceHotlist.cpp" );
Src( '*', "DeviceParameters.cpp" ); 
Src( '*', "DifferentialPrices.cpp" );
Src( '*', "DifferentialPricesProvisional.cpp" );
Src( '*', "FaresEpass.cpp" ); 
Src( '*', "FaresProvisional.cpp" ); 
Src( '*', "HMIMessaging.cpp" ); 
Src( '*', "FaresStoredValue.cpp" ); 
Src( '*', "Locations.cpp" ); 
Src( '*', "MasterRoutes.cpp" );
Src( '*', "myki_actionlist.cpp" );
Src( '*', "myki_cd.cpp" ); 
Src( '*', "PassengerTypes.cpp" ); 
Src( '*', "ProductConfigurationUtils.cpp" ); 
Src( '*', "ProductIssuers.cpp" ); 
Src( '*', "Products.cpp" ); 
Src( '*', "Routes.cpp" );
Src( '*', "RouteSections.cpp" );
Src( '*', "RouteStops.cpp" );
Src( '*', "ServiceProviders.cpp" ); 
Src( '*', "StationLines.cpp" ); 
Src( '*', "Stations.cpp" ); 
Src( '*', "Zones.cpp" ); 

Src( '*', "myki_actionlist.h" );
Src( '*', "myki_cd.h" ); 
Src( '*', "Actionlist.h" );
Src( '*', "CdCache.h" );
Src( '*', "DeviceHotlist.h" );
Src( '*', "DeviceParameters.h" ); 
Src( '*', "DifferentialPrices.h" );
Src( '*', "DifferentialPricesProvisional.h" );
Src( '*', "FaresEpass.h" ); 
Src( '*', "FaresProvisional.h" ); 
Src( '*', "HMIMessaging.h" ); 
Src( '*', "FaresStoredValue.h" ); 
Src( '*', "Locations.h" ); 
Src( '*', "MasterRoutes.h" );
Src( '*', "PassengerTypes.h" ); 
Src( '*', "ProductConfigurationUtils.h" ); 
Src( '*', "ProductIssuers.h" ); 
Src( '*', "Products.h" ); 
Src( '*', "Routes.h" );
Src( '*', "RouteSections.h" );
Src( '*', "RouteStops.h" );
Src( '*', "ServiceProviders.h" ); 
Src( '*', "StationLines.h" ); 
Src( '*', "Stations.h" ); 
Src( '*', "Zones.h" ); 


#
#   Build and install the library
Lib          ( '*'    , "Myki_CDbuild", @OBJS  );
MergeLibrary ( '*', , "Myki_CD", "Myki_CDbuild", "sqlite_wrapper,--Interface", "sqlite,--Interface", "pugixml_static,--Interface" );
InstallLib   ( '*'    , "Myki_CD" );
InstallHdr   ( '*'    , @CHDRS ,"--Strip" );

# Build the program that creates a new CD database
Prog( '*', 'ProductConfiguration',
        'XMLUpdateMain.cpp',
        'ProductConfiguration.cpp',
        'version.c',
        '-lMyki_CD',
        '-ldaf_myki_cardservices',
        '-ljsoncpp',
        '-ldatertswis',
        '-lcsf',
        '-lcsx' );
Prog( '!WIN32', 'ProductConfiguration', '-Lstdc++', '-Lpthread', '-Ldl' );
Prog( 'UBUNTU12', 'ProductConfiguration', '-Lm' );

# Build the program that creates a new actionlist database
Prog( '*', 'ActionlistUpdate',
        'XMLUpdateMain.cpp',
        'ActionlistUpdate.cpp',
        'version.c',
        '-lMyki_CD',
        '-ldaf_myki_cardservices',
        '-ljsoncpp',
        '-ldatertswis',
        '-lcsf',
        '-lcsx' );
Prog( '!WIN32', 'ActionlistUpdate', '-Lstdc++', '-Lpthread', '-Ldl' );
Prog( 'UBUNTU12', 'ActionlistUpdate', '-Lm' );

#.............................................................................
# Packaging definitions
#

PackageLib ( '*', 'Myki_CD' );
PackageHdr ( '*', 'myki_cd.h', 'myki_actionlist.h' );
PackageProg( '*', 'ProductConfiguration' );
PackageProg( '*', 'ActionlistUpdate' );

#..
#
PackageFile ( '*', 'descpkg' );

# un-comment the next line to build a deployable basic CD package
#MakeDebianPackage ('COBRA', '--Script=PackageApp.pl' );

#.............................................................................
# Finally generate the makefile
#
MakefileGenerate();

#..  Successful termination
1;

