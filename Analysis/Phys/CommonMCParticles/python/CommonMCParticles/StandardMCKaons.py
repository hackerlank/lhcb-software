#!/usr/bin/env python
"""
Configuration file for 'Standard MC Kaons'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *
from CommonParticles.StdNoPIDsKaons    import *

## create the algorithm 
algorithm = FilterDesktop( 'StdMCKaons',
                           Inputs = ["Phys/StdAllNoPIDsKaons/Particles"],
                           Code = "(mcMatch('[K+]cc'))",
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdMCKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
