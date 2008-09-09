##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                              $DDDBROOT/options/lhcb-2008.py
# Syntax is:
#   gaudirun.py Brunel-2008.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().applyConf()


##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
