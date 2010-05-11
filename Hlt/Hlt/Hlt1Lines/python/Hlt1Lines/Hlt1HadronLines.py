# =============================================================================
## @file
#  Configuration of Hadron Lines
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure Hadron trigger lines
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.22 $"
# =============================================================================

import Gaudi.Configuration 

def histosfilter(name,xlower=0.,xup=100.,nbins=100):
    """ return the dictonary with the booking of the histograms associated to a filter
    @param filter name i.e 'PT
    """
    histosfs = { name : ( name,xlower,xup,nbins),
                 name+"Best" : (name+"Best",xlower,xup,nbins) }
    return histosfs


from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
class Hlt1HadronLinesConf(HltLinesConfigurableUser) :
    #--------------------------------
    #
    # (J.A. Hernando 17/12/08)
    # External selection to be provided
    #    1) AllL0Hadrons (L0 Candidates above a given threshold i.e 2000 MeV)
    #    2) Velo, PV3D, Velo tracks
    # Replace opts-file to set the opts for the fast fit by configurables

    # Note: the 3D reconstruction is now done upfront by adding
    #
    # [Velo,PV3D().ignoreOutputSelection()]
    #
    # to the line. This replaces the old
    #
    # [RZVelo,PV2D().ignoreOutputSelection()]
    #     
    # It requires the import of Velo,PV3D instead of RZVelo,PV2D.    

    # steering variables
    #------------------------
    #   Single Hadron Line
    #     SingleHadron_PtCut
    #   DiHadron Line
    #     HadMain_IPCut
    #     HadMain_PtCut
    #     HadCompanion_IPCut
    #     HadCompanion_PtCut
    #
    __slots__ = { 'L0Channel'               : "Hadron" 
                , 'HadSingle_IPCut'         : 0.1
                , 'HadDi_IPCut'             : 0.1
                , 'HadMain_PTCut'           : 2500.
                , 'HadMain_TrackFitChi2Cut' : 10.
                , 'HadCompanion_DOCACut'    : 0.2
                , 'HadCompanion_DZCut'      : 1.5
                , 'HadCompanion_PTCut'      : 1000.
                , 'HadCompanion_PointingCut': 0.4
                , 'SingleHadron_PTCut'      : 5000.
                , 'Postscale'               : { 'Hlt1HadronMonConf1' : 0.000001,
                                                'Hlt1HadronMonConf2' : 0.000001,
                                                'Hlt1HadronMonComp'  : 0.000001
                                              }  
                }
    
    def __apply_configuration__(self) : 
        from HltLine.HltLine import Hlt1Line   as Line
        from HltLine.HltLine import bindMembers
        from HltLine.HltLine import Hlt1Member as Member
        from HltLine.HltLine import Hlt1Tool   as Tool
        from HltLine.HltLine import hlt1Lines  
        from Hlt1Lines.HltFastTrackFit import setupHltFastTrackFit
        from HltTracking.HltReco import Velo
        from HltTracking.HltPVs  import PV3D
        from Configurables import HltTrackUpgradeTool, PatForwardTool, HltGuidedForward
        from Hlt1Lines.HltConfigurePR import ConfiguredPR
        
        # confirmed track
        #------------------------
        def confirmationl0part(type=""):
            prefix = 'HadronConfL0'+type
            from Hlt1Lines.HltL0Candidates import convertL0Candidates
            L0Channel = self.getProp("L0Channel")
            # get the L0 candidates (all or L0)
            l0 = bindMembers(prefix, [ convertL0Candidates(L0Channel)] )
            return l0

        def confirmationpreip(type=""):
            prefix = 'HadronConfPreIP'+type
            OutputOfL0 = confirmationl0part(type).outputSelection()
            conf = [Velo,PV3D().ignoreOutputSelection()]

            return bindMembers(prefix, conf)

        def veloipcut(type=""):
            prefix = "HadronConfVeloIPCut"+type
            OutputOfConf = confirmationpreip(type).outputSelection()
            
            if   type.find('Di') > -1     : 
                 cutvalue = self.getProp("HadDi_IPCut")
            elif type.find('Single') > -1 : 
                 cutvalue = self.getProp("HadSingle_IPCut")
            else                  : 
                 return None # Not an allowed value!

            conf = [Member ( 'TF', '2Velo', InputSelection = '%s' %OutputOfConf
                           , FilterDescriptor = [ 'IP_PV3D,||>,%s' %cutvalue ]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor = histosfilter('IP_PV3D_'+type,-0.3,3.,2000))]

            return bindMembers(prefix,conf)

        def confirmationpostip(type=""):
            if type.find('Mon') > -1 :
                type = type.strip('Mon')
                OutputOfVeloIP = confirmationpreip(type).outputSelection()
            else :
                OutputOfVeloIP = veloipcut(type).outputSelection()
            
            OutputOfL0     = confirmationl0part(type).outputSelection()
            prefix = 'HadronConfPostIP'+type
            from HltLine.HltDecodeRaw import DecodeIT
            conf =                  [ Member ( 'TM' , 'VeloCalo'
                                             , InputSelection1 = '%s' %OutputOfVeloIP
                                             , InputSelection2 = '%s' %OutputOfL0
                                             , MatchName = 'VeloCalo' , MaxQuality = 4.
                                             )
                                    , DecodeIT
                                    , Member ( 'TU', 'GuidedForward'
                                             , RecoName = 'GuidedForward'
                                             , tools = [ Tool( HltTrackUpgradeTool
                                                               , tools = [ Tool( HltGuidedForward
                                                                                 ,tools = [ConfiguredPR( "Forward" )] )] )]
                                               , HistogramUpdatePeriod = 1
                                             )         
                                    ]
            if type.find('Single') < 0 :
                if   type.find("Di") > -1     : cutvalue = self.getProp("HadMain_PTCut")
                else                          : return None # Not an allowed value!
                conf += [Member ( 'TF' , 'DiHadronPT1' 
                                       , FilterDescriptor = ['PT,>,%s'%cutvalue]
                                       , HistogramUpdatePeriod = 1
                                       , HistoDescriptor  = histosfilter('PT_'+type,0.,8000.,200)
                                )]
            return bindMembers(prefix, conf)
        
        # simple hadron cut
        #---------------
        def singlehadron():
            OutputOfConfirmation = confirmationpostip('Single').outputSelection()
            sh = [ PV3D().ignoreOutputSelection(),
                   Member ( 'TF' , 'SingleHadronPT' ,
                            InputSelection = '%s' %OutputOfConfirmation,
                            FilterDescriptor = ['PT,>,%(SingleHadron_PTCut)s'%self.getProps()],
                            HistogramUpdatePeriod = 1,
                            HistoDescriptor  = histosfilter('PT',0.,8000.,200)
                            )
                   ]
            return sh


        # companion track - dihadron part
        #------------------------------------
        def companion(type=""):
            prefix = "HadCompanion"+type

            comp = [ Velo , PV3D().ignoreOutputSelection()]

            if (type.find('Mon') > -1) :
                return bindMembers(prefix,comp)

            if type.find("Di") > -1       : cutvalue = self.getProp("HadDi_IPCut")
            else                          : return None # Not an allowed value!
       
            OutputOfConfirmation = confirmationpostip(type).outputSelection()
 
            comp += [ Member ( 'TF', '1UVelo'
                               , FilterDescriptor = ['MatchIDsFraction_%s,<,0.9' %OutputOfConfirmation ]
                               , HistogramUpdatePeriod = 1
                               , HistoDescriptor  = histosfilter('MatchIDsFraction_%s' %OutputOfConfirmation,0.,8000.,200)
                               )
                    ]

            comp += [ Member ( 'TF', 'Companion'
                           , FilterDescriptor = [ 'IP_PV3D,||>,%s'%cutvalue]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('IP_PV3D_'+type,-0.3,30.,200)
                           )
                    ]
            return bindMembers(prefix,comp)

        #
        # dihadron part
        #---------------------
        def dihadron(type=""):
            prefix = "HadDi"
            
            OutputOfConfirmation = confirmationpostip(type).outputSelection()
            OutputOfCompanion    = companion(type).outputSelection() 
            
            if type.find("Di") > -1       : cutvalue = self.getProp("HadMain_PTCut")
            else                          : return None # Not an allowed value!
            
            from HltLine.HltDecodeRaw import DecodeIT
            dih = [ PV3D().ignoreOutputSelection()
                , Member ( 'VM2', 'UVelo'
                         , InputSelection1 = '%s' %OutputOfConfirmation 
                         , InputSelection2 = '%s' %OutputOfCompanion
                         , FilterDescriptor = [ 'DOCA,<,'+str(self.getProp('HadCompanion_DOCACut'))]
                         , HistoDescriptor  = histosfilter('DOCA_'+type,0.,1.,200)
                           )
                , Member ( 'VF', '1UVelo'
                           , FilterDescriptor = [ 'VertexDz_PV3D,>,%s'%self.getProp('HadCompanion_DZCut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('VertexDx_PV3D_'+type,1.,12.,200)                       
                           )
                , DecodeIT
                , Member ( 'VU', 'Forward'
                           , RecoName = 'Forward'
                           , tools = [ Tool( HltTrackUpgradeTool
                                             ,tools = [ConfiguredPR( "Forward" )] )]
                           )
                , Member ( 'VF', 'DiHadronPT2',
                           FilterDescriptor = [ 'VertexMinPT,>,%s'%self.getProp("HadCompanion_PTCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexMinPT_'+type,0.,5000.,200)                       
                           )
                , Member ( 'VF', 'DiHadron',
                           FilterDescriptor = [ 'VertexPointing_PV3D,<,%s'%self.getProp("HadCompanion_PointingCut")],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('VertexPointing_PV3D_'+type,0.1,1.1,200)                       
                           )
                ]
            return bindMembers(prefix,dih)
        
        # afterburn of a line with tracks
        #--------------------------------
        def afterburn():
            after = [ PV3D().ignoreOutputSelection()
                , Member ( 'TU' , 'FitTrack' , RecoName = "FitTrack", callback = setupHltFastTrackFit )
                , Member ( 'TF' , '1FitTrack' ,
                           FilterDescriptor = ["FitIP_PV3D,||>,%s"%self.getProp('HadSingle_IPCut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitIP_PV3D',-.3,3.,200)
                           )
                , Member ( 'TF' , '2FitTrack'
                           , OutputSelection = '%Decision'
                           , FilterDescriptor = ["FitChi2OverNdf,<,%s"%self.getProp('HadMain_TrackFitChi2Cut')],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitChi2OverNdf',0.,100.,100)
                           )
                ]
            return after

        # afterburn of a line with vertices
        #-------------------
        def vafterburn(type=""):
            OutputOfDiHadron = dihadron(type).outputSelection()

            if type.find("Di") > -1       : cutvalue = self.getProp("HadDi_IPCut")
            else                          : return None # Not an allowed value!

            vafter =  [ PV3D().ignoreOutputSelection()
                , Member ( 'VU', 'FitTrack', InputSelection='%s' %OutputOfDiHadron
                           , RecoName = 'FitTrack', callback = setupHltFastTrackFit )
                , Member ( 'VF', '1FitTrack',
                           FilterDescriptor = [ 'FitVertexMinIP_PV3D,||>,%s'%cutvalue],
                           HistogramUpdatePeriod = 1,
                           HistoDescriptor  = histosfilter('FitVertexMinIP_PV3D_'+type,-0.3,3.,200)                       
                           )
                , Member ( 'VF', '2FitTrack'
                           , OutputSelection = "%Decision"
                           , FilterDescriptor = ['FitVertexMaxChi2OverNdf,<,%s'%self.getProp('HadMain_TrackFitChi2Cut')]
                           , HistogramUpdatePeriod = 1
                           , HistoDescriptor  = histosfilter('FitVertexMaxChi2OverNdf_'+type,0,50.,200)
                           )
                ]
            return vafter
           
        def mondecision(type=""):
            if   type.find('Conf') > -1 :
                return [Member('TF','MonConf',OutputSelection = "%Decision", FilterDescriptor = ['IsBackward,<,0.5'])]
            elif type.find('Comp') > -1 :
                from HltLine.HltDecodeRaw import DecodeIT
                return [ DecodeIT
                       , Member( 'TU',
                                'MonForward',
                                RecoName = 'Forward',
                                tools = [ Tool( HltTrackUpgradeTool,
                                                tools = [ConfiguredPR( "Forward" )]
                                              )
                                        ]
                              )
                       ]+mondecision('Conf')
            else                :
                return None # Not an allowed type
 
        from Hlt1Lines.HltL0Candidates import L0Channels
        if self.getProp('L0Channel') in L0Channels() :
            # Single Hadron Line with 3D reco upfront
            #-----------------------------------
            Line ( 'SingleHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part('Single')]+\
                            [confirmationpreip('Single')]+\
                            [veloipcut('Single')]+\
                            [confirmationpostip('Single')]+\
                            singlehadron()+\
                            afterburn()
                 )

            # DiHadron Line with 3D upfront
            #-----------------------------------
            Line ( 'DiHadron'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU  = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos =  [confirmationl0part('Di')]+\
                            [confirmationpreip('Di')]+\
                            [veloipcut('Di')]+\
                            [confirmationpostip('Di')]+\
                            [companion('Di')]+\
                            [dihadron('Di')]+\
                            vafterburn('Di')
                 )

            # Monitoring line : pre-confirmation reconstruction
            #-----------------------------------
            Line ( 'HadronMonConf1'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part('Single')]+\
                           [confirmationpreip('Single')]+\
                           mondecision('Conf') 
                 )
            
            # Monitoring line : post-confirmation reconstruction
            #-----------------------------------
            Line ( 'HadronMonConf2'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [confirmationl0part('Single')]+\
                           [confirmationpreip('Single')]+\
                           [confirmationpostip('SingleMon')]+\
                           mondecision('Conf')
                 )
            
            # Monitoring line : companion reconstruction
            #-----------------------------------
            Line ( 'HadronMonComp'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , L0DU = "L0_CHANNEL('%(L0Channel)s')"%self.getProps()
                 , algos = [companion('DiMon')]+\
                           mondecision('Comp')
                 )

