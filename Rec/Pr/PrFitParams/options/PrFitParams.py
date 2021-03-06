from Configurables import Brunel, NTupleSvc, TrackSys, RecMoniConf

Brunel().InputType = "DIGI" # implies also Brunel().Simulation = True
Brunel().WithMC    = True   # implies also Brunel().Simulation = True
Brunel().OutputType   = "NONE"
Brunel().Histograms   = "Expert"
TrackSys().TrackPatRecAlgorithms = [ "Truth", "FastVelo" ]
Brunel().RecoSequence = ["Decoding", "Truth", "VELO" ]
RecMoniConf().MoniSequence = [ ]
Brunel().MCLinksSequence = [ "Tr" ]
Brunel().MCCheckSequence = [ "Fit" ]

from Configurables import DDDBConf
DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/static_DDDB_FT_v4/lhcb.xml"
Brunel().DDDBtag   = "MC11-20111102"
Brunel().CondDBtag = "sim-20111111-vc-md100"

NTupleSvc().Output = ["FILE1 DATAFILE='~/w0/track.root' TYP='ROOT' OPT='NEW'"]

from Configurables import LHCbApp

from Gaudi.Configuration import * 
from GaudiConf import IOHelper

IOHelper().inputFiles(['PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ocallot/Bs_mumu_v4_nu50.digi?svcClass=default'])
Brunel().EvtMax     = 1000

def doMyChanges():
   from Configurables import GaudiSequencer
   GaudiSequencer("CaloBankHandler").Members = []
   GaudiSequencer("RecoDecodingSeq").Members = [ "DecodeVeloRawBuffer/DecodeVeloClusters", "FTRawBankDecoder" ]
   GaudiSequencer("RecoTruthSeq").Members = [ "UnpackMCParticle", "UnpackMCVertex", "PrLHCbID2MCParticle" ]
   GaudiSequencer("MCLinksTrSeq").Members = ["PrTrackAssociator"] 
appendPostConfigAction( doMyChanges )


from Configurables import PrFitFwdParams, PrFitSeedParams, PrFitKsParams

PrFitFwdParams().VeloFromMC = True

GaudiSequencer("CheckFitSeq").Members += [ PrFitFwdParams() ]
#                                           PrFitSeedParams(),
#                                           PrFitKsParams() ]

PrFitKsParams().zMagnetParams = [ 5372.42, -3372.25, 370.484 ];
PrFitKsParams().momentumParams = [ 1226.32, 479.859, 2859.75 ];


PrFitSeedParams().InitialArrowParams  = [ 4.21826e-09, -8.93796e-08, 0.372981 ]
PrFitSeedParams().MomentumScaleParams = [ 40.3751, 1163.24, -682850 ];
PrFitSeedParams().zMagnetParams       = [ 5373.86, -3283.82, 379.141 ];
PrFitSeedParams().dRatioParams        = [ -3.77e-4, 4.7 ]
PrFitSeedParams().yCorrectionParams   = [ 1.25e-14 ]

#====  final Fwd fit
PrFitFwdParams().ZVelo      = 1640 ;
PrFitFwdParams().ZReference = 8520 ;

PrFitFwdParams().zMagnetParams = [ 5212.38, 406.609, -1102.35, -498.039 ]
PrFitFwdParams().xParams = [ 18.6195, -5.55793 ]
PrFitFwdParams().byParams = [ -0.667996 ]
PrFitFwdParams().cyParams = [ -3.68424e-05 ]
PrFitFwdParams().momentumParams = [ 1.21014, 0.637339, -0.200292, 0.632298, 3.23793, -27.0259 ]

