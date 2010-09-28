# Hlt2 Inclusive Muon and Muon+track selections 06/12/09 (updated 24th may 2010)
#
# Antonio Perez-Calero (aperez@ecm.ub.es)
#
# Single muon line with IP for generic B Physics data mining, plus a HighPt line
#
# Muon+track lines designed to trigger on a muon plus any other cherged track in the decay
# Thus, it can be a muon, as for Bs->mumu, or not, as in the case of Bd->K*mumu.
#
# Then, another lifetime unbiased selection designed to trigger on JPsi->mumu, not requiring
# the second track to have been explicitely identified as a muon. Should trigger prompt JPsi as well as
# B->JPsi signal with increased robustness 

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser


class Hlt2InclusiveMuonLinesConf(HltLinesConfigurableUser) :
    
    __slots__ = { 'Prescale'   : { 'Hlt2MuonFromHLT1        '       : 1.0
                                   ,'Hlt2MuonFromHLT1Bs2MuMu'      : 1.0   
                                   ,'Hlt2SingleMuon'                : 1.0
                                   ,'Hlt2SingleHighPTMuon'          : 1.0
                                   ,'Hlt2IncMuTrack'                : 1.0
                                   ,'Hlt2IncMuTrackNoIP'            : 1.0
                                   }
                  
                  ,'SingleMuonPt'        : 1000      # MeV
                  ,'SingleMuonIP'        : 0.080     # mm
                  ,'SingleMuonHighPt'    : 10000     # MeV
                  # Mu+track cuts 
                  ,'MuTrackMuPt'         : 800       # MeV
                  ,'MuTrackTrPt'         : 600       # MeV
                  ,'MuTrackTrChi2'       : 5.0       # dimensionless
                  ,'MuTrackChi2'         : 15.0      # dimensionless
                  ,'MuTrackMuIPChi2'     : 9.0       # dimensionless
                  ,'MuTrackTrIPChi2'     : 9.0       # dimensionless
                  ,'MuTrackDoca'         : 0.200     # mm
                  ,'MuTrackFDChi2'       : 64.0      # dimensionless
                  ,'MuTrackMass'         : 1000      # MeV
                  ,'MuTrackSumPt'        : 2200      # MeV
                  ,'MuTrackCorMass'      : 7000.0    # MeV
                  ,'MuTrackDIRA'         : 0.995       # dimensionless
                  # Mu+track no IP cuts
                  ,'MuTrackNoIPMuPt'    : 1600       # MeV
                  ,'MuTrackNoIPTrPt'    : 400        # MeV
                  ,'MuTrackNoIPDoca'    : 0.100      # mm
                  ,'MuTrackNoIPMass'    : 2900       # MeV
                  }
    
    
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from HltTracking.HltPVs import PV3D
        from Configurables import HltANNSvc
        from Configurables import CombineParticles, PhysDesktop
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import ( BiKalmanFittedMuons,
                                                                    BiKalmanFittedPions )

        #some string definitions... 
        decayDescB2MuTrack = ["[B0 -> mu+ pi-]cc","[B0 -> mu+ pi+]cc"]
        decayDescB2MuTrackNoIP = ["[B0 -> mu+ pi-]cc"]
        
        MuTrackMuCut = "(TRCHI2DOF<"+str(self.getProp('MuTrackTrChi2'))+")&" \
                       "(PT>"+str(self.getProp('MuTrackMuPt'))+"*MeV)&" \
                       "(MIPCHI2DV(PRIMARY)>"+str(self.getProp('MuTrackMuIPChi2'))+")"
        
        MuTrackTrCut = "(TRCHI2DOF<"+str(self.getProp('MuTrackTrChi2'))+")&" \
                       "(PT>"+str(self.getProp('MuTrackTrPt'))+"*MeV)&" \
                       "(MIPCHI2DV(PRIMARY)>"+str(self.getProp('MuTrackTrIPChi2'))+")"
        
        MuTrackComCut = "(AM>"+str(self.getProp('MuTrackMass'))+"*MeV)&" \
                        "(ASUM(PT)>"+str(self.getProp('MuTrackSumPt'))+")&" \
                        "(AMAXDOCA('')<"+str(self.getProp('MuTrackDoca'))+"*mm)"
        
        MuTrackCut = "(VFASPF(VCHI2)<"+str(self.getProp('MuTrackChi2'))+")&" \
                     "(BPVVDCHI2>"+str(self.getProp('MuTrackFDChi2'))+")&" \
                     "(BPVDIRA>"+str(self.getProp('MuTrackDIRA'))+")&" \
                     "(MCOR<"+str(self.getProp('MuTrackCorMass'))+"*MeV)"
        
        DocaNoIP = "(AMAXDOCA('')<"+str(self.getProp('MuTrackNoIPDoca'))+"*mm)"
        MassNoIP = "(AM>"+str(self.getProp('MuTrackNoIPMass'))+"*MeV)"
        
        Hlt1UnbMuon = "HLT_PASS_RE('Hlt1(SingleMuonNoIPL0|MuTrack4JPsi)Decision')"
        Hlt1AllMuons = "HLT_PASS_RE('Hlt1.*Mu.*Decision')"
        
        ############################################################################
        #    Single muon selection lines
        ############################################################################
        
        line = Hlt2Line('MuonFromHLT1'
                        , prescale = self.prescale
                        , HLT =Hlt1AllMuons
                        , VoidFilter = '' # explicitly require NO pile up filter... 
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuonFromHLT1Decision" : 50190 } )        
        
        #--------------------------------------------

        line = Hlt2Line('Hlt2MuonFromHLT1Bs2MuMu'
                        , prescale = self.prescale
                        , HLT = "HLT_PASS_RE('Hlt1.*4BsMuMu.*Decision')"
                        , VoidFilter = '' # explicitly require NO pile up filter... 
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuonFromHLT1Bs2MuMuDecision" : 50193 } )        
        
        #--------------------------------------------
        Hlt2SelSingleMuon= Hlt2Member( FilterDesktop
                                       , "Filter"
                                       , InputLocations  = [BiKalmanFittedMuons]
                                       , Code = "((PT>"+str(self.getProp('SingleMuonPt'))+"*MeV) & (MIPDV(PRIMARY)>"+str(self.getProp('SingleMuonIP'))+"))"
                                       )
        line = Hlt2Line('SingleMuon'
                        , prescale = self.prescale 
                        , algos = [ PV3D(), BiKalmanFittedMuons, Hlt2SelSingleMuon ]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleMuonDecision" : 50191 } )
        
        #--------------------------------------------
        
        Hlt2SelSingleHighPTMuon = Hlt2Member(   FilterDesktop
                                                , "Filter"
                                                , Code = "(PT>"+str(self.getProp('SingleMuonHighPt'))+"*MeV)"
                                                , InputLocations  = [BiKalmanFittedMuons]
                                                , InputPrimaryVertices = "None"
                                                , UseP2PVRelations = False
                                                )
        line = Hlt2Line( 'SingleHighPTMuon'
                         , prescale = self.prescale 
                         , algos = [ BiKalmanFittedMuons, Hlt2SelSingleHighPTMuon]
                         , postscale = self.postscale
                         )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2SingleHighPTMuonDecision" : 50192 } )
        
        #####################################################
        #    Selections for inclusive decays with mu + track 
        #####################################################

        _preambulo = [ "PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                      "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS" ]

        
        combine = Hlt2Member( CombineParticles
                              , "CombineB"
                              , DecayDescriptors = decayDescB2MuTrack
                              , DaughtersCuts = { "mu+" : MuTrackMuCut , 
                                                  "pi+" : MuTrackTrCut }
                              , CombinationCut = MuTrackComCut
                              , MotherCut = MuTrackCut  
                              , InputLocations  = [ BiKalmanFittedMuons , BiKalmanFittedPions ]
                              , Preambulo = _preambulo
                              )
        line = Hlt2Line('MuTrack'
                        , prescale = self.prescale 
                        , algos = [ PV3D(), BiKalmanFittedMuons, BiKalmanFittedPions, combine]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuTrackDecision" : 50400 } )
        
        #---------------------------------------------------
        # mu+track NoIP
        
        combine_noip = Hlt2Member( CombineParticles
                                   , "Combine"
                                   , DecayDescriptors = decayDescB2MuTrackNoIP
                                   , DaughtersCuts = { "mu+" : "(PT>"+str(self.getProp('MuTrackNoIPMuPt'))+"*MeV)",
                                                       "pi-" : "(PT>"+str(self.getProp('MuTrackNoIPTrPt'))+"*MeV)" }
                                   , CombinationCut = DocaNoIP+" & "+MassNoIP 
                                   , MotherCut = "ALL"
                                   , InputLocations  = [ BiKalmanFittedMuons , BiKalmanFittedPions ]
                                   , InputPrimaryVertices = "None"
                                   , UseP2PVRelations = False
                                   )
        line = Hlt2Line('MuTrackNoIP'
                        , prescale = self.prescale 
                        , HLT = Hlt1UnbMuon
                        , algos = [BiKalmanFittedMuons, BiKalmanFittedPions, combine_noip]
                        , postscale = self.postscale
                        )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2MuTrackNoIPDecision" : 50401 } )
