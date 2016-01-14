###############################################################################
#
#   Makefile for generic libraries
#
###############################################################################

#
#   Check arguments
#

die "Usage: Makefile.pl rootdir Makelib.pl\n"
    unless( $#ARGV+1 >= 2 );
require "$ARGV[1]";

#
#   Platforms
#

Platform( '*' );

#
#   Compiler flags (use of these should be minimised)
#

CompileOptions ( 'WIN32', "subsystem:console" );

#
#   Convenience definitions (not used so far in Myki BR)
#

AddCFlags( 'COBRA'              , '-DCOBRA'             , '-DSERPENT' );
AddCFlags( 'PPC_603E'           , '-DVIPER'             , '-DSERPENT' );
AddCFlags( 'UBUNTU12'           , '-DLINUX_DESKTOP'     , '-DDESKTOP'       , '-DBR_TEST_ENABLED=1' );
AddCFlags( 'UBUNTU12_INSTRUMENT', '-DLINUX_DESKTOP'     , '-DDESKTOP'       , '-DBR_TEST_ENABLED=1' );

AddCFlags( '*', '-DLDT_SUPPORTED' );

#
#   Enable code that mimics KAMCO features that otherwise are suspected to
#   be incorrect according to KEANE specification.
#

#AddCFlags( '*', '-DOPT_MIMIC_KAMCO_READER_CHANGE_FIXED_EPASS_DATES' );

###############################################################################
#
#   Source files
#
###############################################################################

#
#   Platform-independent modules
#

Src(    '*',

        #   API

        'myki_br.c',
        'myki_br.h',
        'myki_br_rules.h',
        'myki_br_context_data.h',

        #   Internal

        'myki_br_card_constants.h',     # Shouldn't really be here

        'BR_Common.c',
        'BR_Common.h',

        'BR_VIX_0_1.c',
        'BR_VIX_0_2.c',
        'BR_VIX_0_3.c',
        'BR_VIX_0_4.c',
        'BR_VIX_0_5.c',

        'BR_LLSC_0_1.c',
        'BR_LLSC_0_A.c',
        'BR_LLSC_0_B.c',
        'BR_LLSC_10_1.c',
        'BR_LLSC_10_2.c',
        'BR_LLSC_10_3.c',
        'BR_LLSC_10_4.c',
        'BR_LLSC_10_5.c',
        'BR_LLSC_10_6.c',
        'BR_LLSC_1_1.c',
        'BR_LLSC_1_10.c',
        'BR_LLSC_1_2.c',
        'BR_LLSC_1_3.c',
        'BR_LLSC_1_4.c',
        'BR_LLSC_1_5.c',
        'BR_LLSC_1_6.c',
        'BR_LLSC_1_7.c',
        'BR_LLSC_1_8.c',
        'BR_LLSC_1_9.c',
        'BR_LLSC_2_1.c',
        'BR_LLSC_2_10.c',
        'BR_LLSC_2_11.c',
        'BR_LLSC_2_12.c',
        'BR_LLSC_2_13.c',
        'BR_LLSC_2_14.c',
        'BR_LLSC_2_2.c',
        'BR_LLSC_2_3.c',
        'BR_LLSC_2_4.c',
        'BR_LLSC_2_5.c',
        'BR_LLSC_2_6.c',
        'BR_LLSC_2_7.c',
        'BR_LLSC_2_8.c',
        'BR_LLSC_2_9.c',
        'BR_LLSC_3_1.c',
        'BR_LLSC_3_2.c',
        'BR_LLSC_4_1.c',
        'BR_LLSC_4_10.c',
        'BR_LLSC_4_11.c',
        'BR_LLSC_4_13.c',
        'BR_LLSC_4_14.c',
        'BR_LLSC_4_15.c',
        'BR_LLSC_4_16.c',
        'BR_LLSC_4_17.c',
        'BR_LLSC_4_18.c',
        'BR_LLSC_4_19.c',
        'BR_LLSC_4_20.c',
        'BR_LLSC_4_2.c',
        'BR_LLSC_4_3.c',
        'BR_LLSC_4_4.c',
        'BR_LLSC_4_5.c',
        'BR_LLSC_4_6.c',
        'BR_LLSC_4_7.c',
        'BR_LLSC_4_8.c',
        'BR_LLSC_4_9.c',
        'BR_LLSC_5_1.c',
        'BR_LLSC_5_2.c',
        'BR_LLSC_5_3.c',
        'BR_LLSC_5_4.c',
        'BR_LLSC_5_5.c',
        'BR_LLSC_5_6.c',
        'BR_LLSC_5_7.c',
        'BR_LLSC_5_8.c',
        'BR_LLSC_5_9.c',
        'BR_LLSC_6_1.c',
        'BR_LLSC_6_10.c',
        'BR_LLSC_6_11.c',
        'BR_LLSC_6_12.c',
        'BR_LLSC_6_13.c',
        'BR_LLSC_6_14.c',
        'BR_LLSC_6_15.c',
        'BR_LLSC_6_16.c',
        'BR_LLSC_6_17.c',
        'BR_LLSC_6_18.c',
        'BR_LLSC_6_19.c',
        'BR_LLSC_6_2.c',
        'BR_LLSC_6_20.c',
        'BR_LLSC_6_21.c',
        'BR_LLSC_6_22.c',
        'BR_LLSC_6_23.c',
        'BR_LLSC_6_24.c',
        'BR_LLSC_6_3.c',
        'BR_LLSC_6_7.c',
        'BR_LLSC_6_8.c',
        'BR_LLSC_6_9.c',
        'BR_LLSC_7_1.c',
        'BR_LLSC_7_2.c',
        'BR_LLSC_7_3.c',
        'BR_LLSC_7_4.c',
        'BR_LLSC_7_5.c',
        'BR_LLSC_9_1.c',
        'BR_LLSC_99_1.c',

        'BR_OSC_0_A.c',
        'BR_OSC_0_B.c',
        'BR_OSC_1_1.c',
        'BR_OSC_1_2.c',
        'BR_OSC_1_3.c',
        'BR_OSC_1_4.c',

        #   VIX specific operator business rules
        #
        'BR_VIX_OSC_2_1.c',
        'BR_VIX_OSC_2_2.c',
        'BR_VIX_OSC_2_3.c',
        'BR_VIX_OSC_2_5.c',

        'BR_VIX_OSC_3_1.c',
        'BR_VIX_OSC_3_2.c',
        'BR_VIX_OSC_3_3.c',
        'BR_VIX_OSC_3_4.c',
        'BR_VIX_OSC_3_5.c',

        'BR_VIX_LLSC_20_1.c',
        'BR_VIX_LLSC_20_2.c',
        'BR_VIX_LLSC_21_1.c',
        'BR_VIX_LLSC_21_2.c',
);

################################################################################
#
#   Build the Library
#
################################################################################

Lib( '*', 'Myki_BR', @OBJS );

################################################################################
#
#   Package the Library
#
################################################################################

#   Package-internal libraries and headers

InstallLib( '*', @LIBS );
InstallHdr( '*', @CHDRS );

#   Package-external (ie, API) libraries and headers

PackageLib( '*', @LIBS );
PackageHdr( '*', 'myki_br.h', 'myki_br_context_data.h', 'myki_br_rules.h' );    # myki_br_rules.h should be removed from the API when time permits.

################################################################################
#
#   Generate the makefile and exit
#
################################################################################

MakefileGenerate();
1;

