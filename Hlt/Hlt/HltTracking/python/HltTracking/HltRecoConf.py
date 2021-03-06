#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: $
# =============================================================================
## @file
#  Configuration of Hlt Reconstruction Algorithms
#  @author Sebastian Neubert, sebastian.neubert@cern.ch
#  @date 2014-11-24
# =============================================================================
"""
 Parameters for the HLT reconstruction algorithms
 Note: the actual definition of the tracking sequences can be found in
 HltSharedTracking.py
 Hlt1Tracking.py
 and
 Hlt2Tracking.py
 Line-specific parameters should go into the respectiv HltLine configurables
"""
# =============================================================================
__author__  = "Sebastian Neubert, sebastian.neubert@cern.ch"
__version__ = ""
# =============================================================================

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from GaudiKernel.SystemOfUnits import MeV, mm, m

class HltRecoConf(LHCbConfigurableUser):
    __slots__ = { "Forward_HPT_MinPt"           :  500. * MeV
                , "Forward_HPT_MinP"            : 3000. * MeV
                , "Forward_LPT_Muon_MinPt"      :  300. * MeV
                , "Forward_LPT_Muon_MinP"       : 3000. * MeV
                , "Forward_ULPT_Muon_MinPt"     :  80. * MeV
                , "Forward_ULPT_Muon_MinP"      : 3000. * MeV
                , "Forward_LPT_MinPt"           :   80. * MeV  # was 200
                , "Forward_LPT_MinP"            : 1000. * MeV # was 3000
                , "Forward_MaxOTHits"           : 15000
                , "MatchVeloMuon_MinP"          : 4000. * MeV
                , "GoodTrCHI2PDOF"              : 4.0  # This TrCHI2PDOF is used in the sequence to mark hits of good tracks.
                , "MaxTrCHI2PDOF"               : 4.0  # This TrCHI2PDOF is used in the making of fitted tracks.
                , "ApplyGHOSTPROBCut"           : False  # Enable cut on ghost probability in the making of protoparticles.
                , "ApplyGHOSTPROBCutInTBTC"     : True  # Enable cut on ghost probability in the creation of fitted tracks.
                , "ApplyGHOSTPROBCutInHLT1"     : True  # Enable cut on ghost probability in the fitting of Hlt1 tracks
                , "MaxTrGHOSTPROB"              : 0.4  # Cut value of ghost probability for above options.
                , "VeloSelectionCut"            : "(~TrBACKWARD) & ( TrNVELOMISS < 100 )"
                , "FitVelo"                     : True
                , "FastFitVelo"                 : True
                , "OfflineSeeding"              : True
                , "OfflineRich"                 : True
                , "CalculateProbNN"             : True
                , "AddGhostProb"                : True
                , "InitFits"                    : True
                , "CacheStatesInStateProvider"  : False
                , "BeamGasMode"                 : False
                , "VeloTrackingZMin"            : -2000.   #minimum velo tracking range
                , "VeloTrackingZMax"            : +2000.   #maximum velo tracking range
                , "MoreOfflineLikeFit"          : True   # This runs a fit in HLT1 and HLT2 which uses the default offline fit
                                                         # with a simplified material description
                , "FitIterationsInHltFit"       : 1      # Increases the number of fit iterations in the current Hlt fit.
                , "NewMSinFit"                  : True   # Use the new description of the multiple scattering term
                , "PVOptions"                   : { "UseBeamSpotRCut"       : True,
                                                    "BeamSpotRCut"          : 0.2 * mm,
                                                    "TrackErrorScaleFactor" : 1.,
                                                    "ResolverBound"         : 5 * mm,
                                                    "MinTracks"             : 4.,
                                                    "trackMaxChi2"          : 12.,
                                                    "BeamSpotRHighMultiplicityCut": 0.4*mm,
                                                    "BeamSpotRMultiplicityTreshold": 10}
                , 'Hlt1TrackOption'             : 'Rerun'
                }

    # Options for dealing with HLT1 reconstructed objects
    __known_hlt1trackoptions__ = ["Decode", "Rerun"]
    #                             ^ take from HLT1Track raw banks
    #                                       ^ Recreate tracks from scratch

    def getConfiguredForward(Name) :
        pass

    def __apply_configuration__(self):
        """
        Apply Hlt configuration
        """
        if self.getProp("Hlt1TrackOption") not in self.__known_hlt1trackoptions__:
            raise ValueError("You must choose a valid Hlt1TrackOption for the input of Hlt2, you chose "+self.getProp("Hlt1TrackOption")+" which is not one of: "+self.__known_hlt1trackoptions__.__str__())
        log.debug("HltReco configuration")
        import GaudiKernel.ProcessJobOptions
        GaudiKernel.ProcessJobOptions.PrintOff()
        # Configure the PV refitting correctly
        from Configurables import LoKi__PVReFitter
        LoKi__PVReFitter("LoKi::PVReFitter").CheckTracksByLHCbIDs = True


MatchVeloMuonOptions = {"MaxChi2DoFX" : 10.,
                        "XWindow" : 200.,
                        "YWindow" : 400.
                        }

MatchVeloTTMuonOptions = {"MaxChi2DoFX" : 2.,
                          "HistoProduce" : False,
                          "Window" : {1 : (100, 200),
                                      2 : (200, 400),
                                      3 : (300, 500),
                                      4 : (400, 600)},
                          "ErrorCorrections" : {'mx' : [9.55, -0.5, -143],
                                                'my' : [7.46, -0.5, -112],
                                                'x'  : [1., 1., 1., 1.],
                                                'y'  : [1., 1., 1., 1.]}
                          }

VeloTTToolOptions = {"minMomentum" : 0.0,
                     "minPT" : 100.0 ,
                     "PassTracks" : True,
                     "PassHoleSize" : 40 * mm,  # Update to new default from 45 mm.
                     }

VeloTTOptions = { }

CommonForwardOptions = { "MaxITHits" : 999999 ,
                         "MaxNVelo" : 999999 }

#Sascha: Rename this to tool,
CommonForwardTrackingOptions = { "MinHits" : 12,
                                 "MinOTHits" : 14,
                                 "AddTTClusterName" : "PatAddTTCoord" }

#Sascha: Rename this to tool and hlt1
ForwardTrackingOptions_MomEstimate = { "UseMomentumEstimate" : True,
                                       "UseWrongSignWindow"  : True,
                                       "WrongSignPT"         : 2000.,
                                       "Preselection"        : True,
                                       "PreselectionPT"      : 400.,
                                       "SecondLoop"          : False,
                                       # optimized settings for 2016 Data taking
                                       "MaxXCandidateSize"   : [50,24],
                                       "MaxSpreadX"          : 0.65,
                                       "MaxSpreadSlopeX"     : 0.0075,
                                       "MaxChi2X"            : 32,
                                       "MaxChi2Y"            : 24,
                                       "NNAfterStereoFit"    : True,
                                       "NNAfterStereoFitCut" : 0.1,
                                       "NextBestResponse"    : 0.7 }

ComplementForwardToolOptions = { "SecondLoop"          : True,
                                 "MaxXCandidateSize"   : [70,26,12],
                                 "MaxSpreadX"          : 1.5,
                                 "MaxSpreadSlopeX"     : 0.007,
                                 "MaxChi2X"            : 52,
                                 "MaxChi2Y"            : 42,
                                 "NNBeforeXFit"        : True,
                                 "NNAfterStereoFit"    : True,
                                 "NNBeforeXFitCut"     : 0.12,
                                 "NNAfterStereoFitCut" : 0.09,
                                 "NextBestResponse"    : 0.75
                                 }

OnlineSeedingToolOptions = { "NDblOTHitsInXSearch" : 2,
                             "MinMomentum" : 1000.}

OfflineSeedingToolOptions = { "NDblOTHitsInXSearch" : 0,
                              "MinMomentum" : 500.}


DownstreamOptions = { "MinMomentum": 0.,
                      "MinPt": 0.}

MuonTTOptions = {
               "SeedStation"  : 2,
               "YTolSlope"    : 40000.,
               "XTolSlope"    : 40000.,
               "XTol"         : 25.,
               "MaxChi2Tol"   : 7.,
               "MinAxProj"    : 5.5,
               "MaxAxProj"    : 25.
               }

#
# The default RICH options
#
MaxChi2 = HltRecoConf().getProp("MaxTrCHI2PDOF")
HltRichDefaultHypos			= ["pion","kaon"]
HltRichDefaultRadiators			= ["Rich1Gas","Rich2Gas"]
HltRichDefaultTrackCuts     = { tt : { "Chi2Cut" : [0.,MaxChi2], "PCut" : [2,100], "PtCut" : [0.8,100]} for tt in ['Forward','Match']}


OfflineRichDefaultHypos	       = ["electron","muon","pion","kaon","proton","deuteron","belowThreshold"]
OfflineRichDefaultRadiators    = ["Rich1Gas","Rich2Gas"]
OfflineRichDefaultTrackCuts    = { tt : {'Chi2Cut' : [0,MaxChi2], 'PCut' : [0,1e9], 'PtCut' : [0,1e9] } for tt in ['Forward','Match'] }
OfflineRichDefaultDownTrackCuts    = { tt : {'Chi2Cut' : [0,MaxChi2], 'PCut' : [0,1e9], 'PtCut' : [0,1e9] } for tt in ['KsTrack'] }
