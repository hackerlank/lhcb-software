##############################################################################
# File for running Brunel on real data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-2008.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().SpecialData = ["fieldOff"] # All 2008 real data was taken with Magnet OFF
##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-TED_Data.py
##############################################################################
