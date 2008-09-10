########################################################################
#
# Options for exercise 6
#
# @author Patrick Koppenburg
# @date 2008-06-03
#
########################################################################
from os import environ
import GaudiKernel.SystemOfUnits as Units 
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from Configurables import LoKi__Hybrid__PlotTool as PlotTool 
from Configurables import TutorialChecker

importOptions( "$L0DUROOT/options/ReplaceL0DUBankWithEmulated.opts" )
#
importOptions( "$HLTCONFROOT/options/Hlt1.opts" )
importOptions( "$HLTCONFROOT/options/Hlt2.opts" )
importOptions( "$ANALYSISROOT/solutions/DaVinci4/DVTutorial_4b.py" )
# B tagging options
importOptions( "$FLAVOURTAGGINGROOT/options/BTaggingTool.opts" )
#
# Print Reconstructed Bs
#
checker = TutorialChecker() 
checker.addTool( PhysDesktop() )
checker.PhysDesktop.InputLocations = [ "Phys/Bs2JpsiPhi" ]
GaudiSequencer("TutorialSeq").Members.append(checker)

HistogramPersistencySvc().OutputFile = "DVHistos_6.root"
NTupleSvc().Output = ["FILE1 DATAFILE='Tuple.root' TYP='ROOT' OPT='NEW'"]

ApplicationMgr().EvtMax = 1000 
EventSelector().PrintFreq = 1 
