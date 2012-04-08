#!/usr/bin/env python
# =============================================================================
# @file
#
# Simple script to get the acceptance of generator cuts
#
# The actual code has been kindly provided by Greig COWAN
#
# The algorithm gets the generator-files (withotu cuts)
# and (re)applies the GenCut-tool
#
# @attention one needs to build package Gen/GenCuts! 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author Vanya  BELYAEV Ivan.Belyaev@cern.ch
# @thanks Greig  COWAN 
# @date   2011-06-18
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""

Simple script to get the acceptance of generator cuts 

The actual code has been kindly provided by Greig COWAN

The algorithm gets the generator-files (withotu cuts)
and (re)applies the GenCut-tool

ATTENTION: one needs to build package Gen/GenCuts! 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

Usage:

def configure ( datafiles         ,
                catalogs  = []    ,
                castor    = False ) :
    
    from Bender.GenCuts import configure_decay
    
    return configure_decay (
        'Alg' , 
        'Upsilon(1S) => mu+ mu-' ,
        datafiles ,
        catalogs  ,
        castor 
        )

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@cern.ch "
__date__    = " 2011-06-18 "
__version__ = " $Revision: 1.2 $ "
# =============================================================================
## import everything from bender
from   Bender.MainMC                 import *
from   GaudiKernel.SystemOfUnits     import GeV
from   GaudiKernel.PhysicalConstants import c_light
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger(__name__)
# =============================================================================
HepMC.GenParticle.Vector = std.vector('HepMC::GenParticle*')            
# ==============================================================================
## @class GenCuts
#  Simple class to check generator cuts efficiency
#
#  The algorithm gets the generator-files (without cuts)
#  and (re)applies the GenCut-tool
#
#  The actual code has been kindly provided by Greig COWAN
# 
#  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
#  @thanks Greig COWAN 
#  @date 2011-06-17
class GenCuts(AlgoMC) :
    """
    Simple class to check Generator Level Cuts
    """
    def __init__ ( self  ,
                   name  ,
                   decay ,
                   tool  , **kwargs ) :
        """
        """
        AlgoMC.__init__ ( self , name , **kwargs )
        
        ## define the actual decay
        self._decay = decay
        self._tool  = tool

    ## standard method for analyses
    def initialize( self ):
        """
        Initialization of algorithm
        """
        
        sc = AlgoMC.initialize ( self )
        if sc.isFailure() : return sc

        #
        ## the tool itself 
        self.inLHCb  = self.tool    ( cpp.IGenCutTool, self._tool )
        self._cnt    = self.counter ( "multiplicity" )
        
        self._gv     = HepMC.GenEvent    ()
        self._gc     = LHCb.GenCollision () 
        
        return SUCCESS
    
    
    ## the standard method for analyse 
    def analyse ( self ) :
        """
        Standard method for analyses
        """
        
        ## check the presence of beauty particles 
        beauty = self.gselect ( 'beauty' , GBEAUTY )
        
        gen    = self.gselect ( 'gen0' ,   self._decay )
        if     gen.empty() :
            return self.Warning('No proper (HepMC)-decays are found', SUCCESS )
        
        self._cnt += len ( gen )

        ## algorithm decision: 
        self.setFilterPassed ( not gen.empty() ) 
        tup = self.nTuple ( 'Gen' )
        
        n = 0 
        for p in gen :

            #
            ## tool have a bit strange interface... :-( 
            v   = HepMC.GenParticle.Vector()
            v.push_back ( p )
            ## use tool! 
            acc = self.inLHCb.applyCut ( v , self._gv , self._gc )
            
            ## take care about multiple candidates 
            n += 1
            
            tup.column ( 'st'   , int ( GTATUS ( p ) ) )
            tup.column ( 'y'    ,  GY ( p )            )
            tup.column ( 'pt'   , GPT ( p ) / GeV      )
            tup.column ( 'hasB' , not beauty.empty() , 0  , 1 ) 
            tup.column ( 'n'    , n , 1 , 7 ) 
            tup.column ( 'acc'  , acc , 0 , 1 )
            
            tup.write ()
            
        return SUCCESS                                           # RETURN
        
    ## standard method for finalization 
    def finalize ( self ):
        """
        print cuts 
        """

        print  100*'*'
        
        print  'DECAY  : ' , self._decay
        
        print  100*'*'
        
        self.inLHCb  = None
        
        return AlgoMC.finalize ( self ) 

    
# ==============================================================================
## commmon configuration
#  @param name algorithm name
#  @param decay the decay pattern or functor 
#  @param datafiles the data files
#  @param tool the tool to be applied  
def configure_decay ( name               ,
                      decay              ,                      
                      datafiles          ,
                      catalogs   = []    ,
                      castor     = False ,
                      tool       = 'DaughtersInLHCb' ) :
    """
    Configure the job:

    def configure ( datafiles , catalogs = [] , castor = False ) :
        
        from Bender.GenCuts import configure_decay
        
        return configure_decay (
            'Alg' , 
            'Upsilon(1S) => mu+ mu-' ,
            datafiles ,
            catalogs  ,
            castor 
            )
    """
    from Configurables import DaVinci
    daVinci = DaVinci (
        DataType      = '2011' ,
        Simulation    = True   ,
        HistogramFile = 'GenCuts_Histo.root' ,
        TupleFile     = 'GenCuts_Tuple.root' ,
        ) 
    
    from Configurables import EventClockSvc
    EventClockSvc (
        EventTimeDecoder = "FakeEventTime"
        )
        
    ## define the proper postconfig action 
    def  postconfig () :
        
        from Configurables import EventClockSvc, CondDB 
        EventClockSvc (
            EventTimeDecoder = "FakeEventTime"
            )
        CondDB  ( IgnoreHeartBeat = True )
        
    ## Important: use Post Config action! 
    from Gaudi.Configuration import appendPostConfigAction
    appendPostConfigAction ( postconfig )
    
    ## define/set the input data 
    setData ( datafiles , catalogs , castor )
    
    ## get the actual application manager (create if needed)
    gaudi = appMgr()
    
    ## create local algorithm:
    alg = GenCuts(
        name                             ,
        decay                            ,
        tool                             , 
        PropertiesPrint      = True      , 
        HistoPrint           = True      ,
        PP2MCs               = []        ,
        InputPrimaryVertices = "None" 
        )
    
    ## finally inform Application Manager about our algorithm
    gaudi.setAlgorithms ( [ alg ] )
    
    logger.info ( 'Algorithm name  : %s' % name  ) 
    logger.info ( 'The decay       : %s' % decay ) 
    logger.info ( 'The IGenCutTool : %s' % tool  ) 
    
    return SUCCESS

# =============================================================================
## job steering 
if __name__ == '__main__' :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120
        
# =============================================================================
# The END 
# =============================================================================
