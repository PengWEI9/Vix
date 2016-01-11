# Boiler-plate for the build system, do not modify.
die "Usage: Makefile.pl rootdir Makelib.pl/n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

# build for cobra and pc platforms
Platform    ( '*' );

# These are flags passed to the C/C++ compiler.
AddFlags    ( 'COBRA',  '-DCOBRA' );
AddFlags    ( 'COBRA',  '-DHAVE_UNISTD' );
AddFlags    ( 'COBRA',  '-DHAVE_TCGETATTR' );

Src         ( '*',      'cli.cpp',          'cli.h'     );
Src         ( '*',      'LDT_cmd.cpp',      'LDT_cmd.h' );
Src         ( '*',      'VCard.cpp',        'VCard.h'   );
Src         ( '*',      'envcmd.cpp',       'envcmd.h'  );
Src         ( '*',      'Common.cpp',       'Common.h'  );
Src         ( '*',      'utcmd.cpp',        'utcmd.h'   );

InstallHdr  ( '*',      'cli.h' );

Lib         ( '*',      'cli',  @OBJS );

InstallLib  ( '*',      'cli'   );

# Boiler-plate for the build system, do not modify.
MakefileGenerate();
1;
