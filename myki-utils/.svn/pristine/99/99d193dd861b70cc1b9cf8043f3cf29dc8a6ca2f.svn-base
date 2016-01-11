# Copyright (C) 1998 ERG Limited, All rights reserved
#
# Module name   : Makefile.pl
# Module type   : Build system
# Compiler(s)   : ANSI C
# Environment(s): Solaris
#
# Description:
#
#   Perl Makefile for src directory
#
# Include the makefile library

die "Usage:Makefile.pl rootdir makelib.pl\n"
   unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

# Subdirectories to recurse (if any).


Platform ('*' );

Src( '*', 'descpkg' );
PackageFile( '*', 'descpkg' );


# Headers to be installed
#   eg  InstallHdr( '*', 'public.h'  );
#..


# Libraries to be made:
#   eg  Lib( 'P386', 'lib', 'src1', 'src2', '-args' );
#       Lib( '*', 'lib', @OBJS );
#..



# Packaging installation
#..
#Src( '*',            'InstallPkg.sh', 1 );
#PackageFile( '*',    'InstallPkg.sh', '--Set=B' );

MakeDebianPackage ('*', '--Script=PackageApp.pl' );

#--- Finally generate the makefile
MakefileGenerate ();
