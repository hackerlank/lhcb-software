# =============================================================================
# $Id: HltBeamGasLines.py,v 1.1 2009-02-24 20:39:18 panmanj Exp $
# =============================================================================
## @file
#  Configuration of BeamGas Lines
#  @author Jaap Panman jaap.panman@cern.ch
#  @date 2009-02-25
# =============================================================================
"""
 script to configure BeamGas trigger lines
"""
# =============================================================================
__author__  = "Jaap Panman jaap.panman@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.1 $"
# =============================================================================

from Gaudi.Configuration import * 
from LHCbKernel.Configuration import *

from Configurables import GaudiSequencer
from Configurables import LoKi__ODINFilter  as ODINFilter

from HltConf.HltLine import Hlt1Line   as Line
from HltConf.HltLine import bindMembers
from HltConf.HltLine import Hlt1Member as Member
from HltConf.HltLine import Hlt1Tool   as Tool
from HltConf.HltLine import hlt1Lines  
from HltConf.HltL0Candidates import *


class HltBeamGasLinesConf(LHCbConfigurableUser) :
    #--------------------------------
    #
    # For the moment set up to select a single hadron Et candidate in the calorimeter
    # matching with a velo track, and with a minimum Pt

    # steering variables
    __slots__ = { 'L0ChannelLeft'           : "Hadron" # assume the L0 puts the special trigger here
                , 'BeamGas_EtCut'           : 500.     # low EtCut 
                , 'BeamGas_PtCut'           : 500.     # low PtCut 
                }

    def __apply_configuration__(self) : 
        importOptions('$HLTCONFROOT/options/HltRecoSequence.py')

        # depending variables
        #------------------------------------
        L0ChannelLeft = self.getProp('L0ChannelLeft')
        BeamGas_EtCut = self.getProp('BeamGas_EtCut')

        # SingleBeamLeft BeamGas Alley
        #-----------------------------------
        Line ( 'BeamGasLeft' 
               , ODIN  = " ODIN_BXTYP == LHCb.ODIN.SingleBeamLeft "
               , L0DU  = "L0_CHANNEL('"+L0ChannelLeft+"')"
               , algos = [
            convertL0Candidates('AllHadron')
            , Member("TF","L0BeamGas", FilterDescriptor = ["L0ET,>,"+str(BeamGas_EtCut)])
            , GaudiSequencer('Hlt1RecoRZVeloSequence')
            , Member ( 'TF' ,'RZVelo'
                       , InputSelection = 'RZVelo'
                       , FilterDescriptor = ['Calo2DChi2_%TFL0BeamGas,<,4'])
            , Member ( 'TU', 'Velo',  RecoName = 'Velo')
            , Member ( 'TF', '1Velo', FilterDescriptor = [ 'Calo3DChi2_%TFL0BeamGas,<,4' ])
            , Member ( 'TM' , 'VeloCalo'
                       , InputSelection1 = '%TF1Velo'
                       , InputSelection2 = '%TFL0BeamGas'
                       , MatchName = 'VeloCalo' , MaxQuality = 4.
                       )
            , Member ( 'TU', 'GuidedForward'
                       , InputSelection  = '%TMVeloCalo'
                       , RecoName = 'GuidedForward'
                       , HistogramUpdatePeriod = 1
                       )
            , Member ( 'TF' , 'GuidedForward'
                       , OutputSelection = '%Decision'
                       , FilterDescriptor = ['PT,>,'+str(self.getProp('BeamGas_PtCut'))])
            ])
