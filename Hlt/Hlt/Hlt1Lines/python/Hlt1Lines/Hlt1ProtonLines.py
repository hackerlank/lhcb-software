from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import *

class Hlt1ProtonLinesConf( HltLinesConfigurableUser ):
    __slots__ = {
          'DiProton_SpdMult'    :   600.   # dimensionless, Spd Multiplicy cut 
        , 'DiProton_PT'         :     1.9  # GeV
        , 'DiProton_P'          :    10.   # GeV  
        , 'DiProton_TrChi2'     :     5. 
        , 'DiProton_MassMin'    :  2900.   # MeV, after Vtx fit
        , 'DiProton_MassMax'    :  4100.   # MeV, after Vtx fit
        , 'DiProton_VtxDOCA'    :     0.2    
        , 'DiProton_VtxChi2'    :     9.   # dimensionless

        , 'DiProton_TrNTHits'   :    15.  # From Track lines
        , 'DiProton_VeloNHits'  :     9.   # From Track lines
        , 'DiProton_VeloQcut'   :     3.   # From Track lines

        , 'SpdMultExclusive'    :    20.


        , 'DiProtonLowMult_SpdMult'    :    20.   # dimensionless, Spd Multiplicy cut 
        , 'DiProtonLowMult_PT'         :     0.6  # GeV, same as LooseForward
        , 'DiProtonLowMult_P'          :     6.   # GeV, same as LooseForward  
        , 'DiProtonLowMult_TrChi2'     :     5. 
        , 'DiProtonLowMult_MassMin'    :  2900.   # MeV, after Vtx fit
        , 'DiProtonLowMult_MassMax'    :  4100.   # MeV, after Vtx fit
        , 'DiProtonLowMult_VtxDOCA'    :     0.2    
        , 'DiProtonLowMult_VtxChi2'    :    25.   # dimensionless

        , 'DiProtonLowMult_TrNTHits'   :    15.  # From Track lines
        , 'DiProtonLowMult_VeloNHits'  :     9.   # From Track lines
        , 'DiProtonLowMult_VeloQcut'   :     3.   # From Track lines
          
          }
        
    def DiProton_preambulo( self ):
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                TightForward,
                                                FitTrack )

        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( "DiProton" ),                      
                      TightForward,                      
                      FitTrack,                      
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiProton_VtxDOCA)s * mm, \
                                                           %(DiProton_VtxChi2)s )" % self.getProps(),
                      "MakeDiProtons = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo


    def DiProtonLowMult_preambulo( self ):
        from HltTracking.Hlt1Streamers import ( VeloCandidates,
                                                LooseForward,
                                                FitTrack )
        
        ## define some "common" preambulo 
        Preambulo = [ VeloCandidates( "DiProtonLowMult" ),                      
                      LooseForward,                      
                      FitTrack,                      
                      ## helpers
                      "VertexConf = LoKi.Hlt1.VxMakerConf( %(DiProtonLowMult_VtxDOCA)s * mm, \
                                                           %(DiProtonLowMult_VtxChi2)s )" % self.getProps(),
                      "MakeDiProtonsLowMult = TC_VXMAKE4( '', VertexConf )",
                      "from LoKiPhys.decorators import RV_MASS"
                      ]
        return Preambulo
    
    
    def DiProton_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        DiProton_unit = HltUnit(
            'DiProtonStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.DiProton_preambulo(),
            Code = """
            VeloCandidates
            >>  ( ( TrIDC('isVelo') > %(DiProton_VeloNHits)s ) & ( TrNVELOMISS < %(DiProton_VeloQcut)s ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass Velo Hits', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE    , 'nVeloHits' , LoKi.Monitoring.ContextSvc ) )
            >>  execute( decodeIT )
            >>  TightForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass tight forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nTightForward' , LoKi.Monitoring.ContextSvc ) )
            >>  (TrTNORMIDC > %(DiProton_TrNTHits)s )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass NHits', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nNHits' , LoKi.Monitoring.ContextSvc ) )            
            >>  ( ( TrPT > %(DiProton_PT)s * GeV ) & ( TrP  > %(DiProton_P)s * GeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  MakeDiProtons
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'p+', 'p~-' ) > %(DiProton_MassMin)s )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass min', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMin' , LoKi.Monitoring.ContextSvc ) )   
            >>  ( RV_MASS ( 'p+', 'p~-' ) < %(DiProton_MassMax)s )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass max', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMax' , LoKi.Monitoring.ContextSvc ) )            
            >>  SINK( 'Hlt1DiProtonDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return DiProton_unit

    
    def DiProtonLowMult_streamer( self ):
        from Configurables import LoKi__HltUnit as HltUnit
        DiProtonLowMult_unit = HltUnit(
            'DiProtonLowMultStreamer',
            ##OutputLevel = 1 ,
            Preambulo = self.DiProtonLowMult_preambulo(),
            Code = """
            VeloCandidates
            >>  execute( decodeIT )
            >>  LooseForward
            >>  tee  ( monitor( TC_SIZE > 0, '# pass loose forward', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nLooseForward' , LoKi.Monitoring.ContextSvc ) )
            >>  ( ( TrPT > %(DiProtonLowMult_PT)s * GeV ) & ( TrP  > %(DiProtonLowMult_P)s * GeV ) )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass PT', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nPT' , LoKi.Monitoring.ContextSvc ) )               
            >>  MakeDiProtonsLowMult
            >>  tee  ( monitor( TC_SIZE > 0, '# pass vertex', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'nVertices' , LoKi.Monitoring.ContextSvc ) )
            >>  ( RV_MASS ( 'p+', 'p~-' ) > %(DiProtonLowMult_MassMin)s )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass min', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMin' , LoKi.Monitoring.ContextSvc ) )   
            >>  ( RV_MASS ( 'p+', 'p~-' ) < %(DiProtonLowMult_MassMax)s )
            >>  tee  ( monitor( TC_SIZE > 0, '# pass mass max', LoKi.Monitoring.ContextSvc ) )
            >>  tee  ( monitor( TC_SIZE , 'massMax' , LoKi.Monitoring.ContextSvc ) )            
            >>  SINK( 'Hlt1DiProtonLowMultDecision' )
            >> ~TC_EMPTY
            """ % self.getProps()
            )
        return DiProtonLowMult_unit

    
    
    
    def __apply_configuration__( self ) : 
        from HltLine.HltLine import Hlt1Line
        Hlt1Line(
            'DiProton',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "L0_CHANNEL( 'Hadron' ) & ( L0_DATA('Spd(Mult)') < %(DiProton_SpdMult)s )" % self.getProps(),   
            ##
            algos     = [ self.DiProton_streamer() ] 
            )

        
        Hlt1Line(
            'DiProtonLowMult',
            ##
            prescale  = self.prescale,
            postscale = self.postscale,
            L0DU = "( L0_DATA('Spd(Mult)') < %(DiProtonLowMult_SpdMult)s )" % self.getProps(),   
            ##
            algos     = [ self.DiProtonLowMult_streamer() ] 
            )
