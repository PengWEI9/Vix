########################################################################
# Copyright (C) 2008 ERG Limited, All rights reserved
#
# Module name   : PackageApp
# Module type   : Makefile system
# Compiler(s)   : n/a
# Environment(s): jats
#
# Description   : Script to be processed by the Debian Packager
#                 This script is not directly processed by JATS
#
#   This file is processed by the Debian Packager in order to construct
#   the package. It contains simple directives to construct the package
#
#......................................................................#

#
#   Specify the DEBIAN packaging files
#
PackageDescription('Basic CD for Day0 upgrade');

CopyFile    ("etc/productcfg.db", "/afc/data/config" );
CopyFile    ("etc/actionlist.db", "/afc/data/config" );

#
#   All done
#   The wrapper scripts will build and package the complete package

