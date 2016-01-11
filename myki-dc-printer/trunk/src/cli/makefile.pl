# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

Platform    ( '*' );

# These are flags passed to the C/C++ compiler.

Src( '*', 'cli.cpp' );
Src( '*', 'cli.h' );

InstallHdr( '*', 'cli.h' );

Lib( '*', 'cli', 'cli.cpp' );

InstallLib( '*', 'cli' );

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;

