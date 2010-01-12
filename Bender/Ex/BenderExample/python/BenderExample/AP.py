#!/usr/bin/env python
# =============================================================================
# $Id: AP.py,v 1.3 2010-01-12 10:58:34 ibelyaev Exp $ 
# =============================================================================
## @file BenderExample/AP.py
#
# The simple Bender-based example: show " L0 <-> K0S " reflection
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date 2009-09-01
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""

The simple Bender-based example: show ' L0 <-> K0S ' reflection

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
# =============================================================================
## import everything form bender
import GaudiKernel.SystemOfUnits as Units 
from   Bender.Awesome            import *
import LoKiMC.trees              as Trees  
import PartProp.Nodes            as Nodes
from   LoKiCore.functions        import *
from   LoKiPhys.Phys             import *
# =============================================================================
## Simple class for access MC-truth 
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class AP(AlgoMC) :
    """
    Simple class for access MC-truth 
    """
    ## standard constructor
    def __init__ ( self , name = 'AP' , **args ) :
        """
        Standard constructor
        """ 
        AlgoMC.__init__ ( self , name , **args )
            
            
    ## standard method for analyses
    def analyse( self ) :
        """
        Standard method for analyses
        """
        
        # K0s -> pi+ pi0 
        ks   = Trees.MCExclusive  ( Nodes.Pid('KS0') )
        ks  += 'pi+'
        ks  += 'pi-'
        st   = ks.validate  ( self.ppSvc() )
        if st.isFailure()  : return st
        
        ## Lambda0
        l0   = Trees.MCExclusive ( Nodes.Pid('Lambda0')  )
        l0  += 'pi-'
        l0  += 'p+'
        st   = l0.validate  ( self.ppSvc() )
        if st.isFailure()  : return st

        ## Lambda0~
        l0b  = Trees.MCExclusive ( Nodes.Pid('Lambda~0')  )
        l0b += 'p~-'
        l0b += 'pi+'
        st   = l0b.validate ( self.ppSvc() )
        if st.isFailure()  : return st


        cutks  = MCDECTREE( ks  ) 
        mcks   = self.mcselect ( 'ks'  , cutks )
        
        cutl1  = MCDECTREE( l0 ) 
        mcl01  = self.mcselect ( 'l01' , cutl1 )
        
        cutl2  = MCDECTREE( l0b ) 
        mcl02  = self.mcselect ( 'l02' , cutl2 )
                
        if mcks.empty() and mcl01.empty()  and mcl02.empty() : 
            return self.Warning ( 'No mc-trees are found' , SUCCESS )        

        if mcks.empty() :
            mcKS  = NONE
        else :
            mcKS  = MCTRUTH ( self.mcTruth() , mcks  )

        if mcl01.empty () :
            mcL01 = NONE
        else:
            mcL01 = MCTRUTH ( self.mcTruth() , mcl01 )
            
        if mcl02.empty () :
            mcL02 = NONE
        else:
            mcL02 = MCTRUTH ( self.mcTruth() , mcl02 )
        
        pions = self.select ( 'pi' ,
                              ( 'pi+' == ABSID   ) &
                              ( mcKS | mcL01 | mcL02 ) )
        
        self.select ( 'pi+' , pions , Q > 0 )
        self.select ( 'pi-' , pions , Q < 0 )

        tup = self.nTuple ( 'my N-tuple' ) 
        
        ## construct dipions 
        dipion = self.loop ( 'pi+ pi-' , 'KS0' )
        for pi2 in dipion :
            
            m12 = pi2.mass(1,2) / Units.GeV
            if m12 > 1 : continue
            
            chi2 = VCHI2 ( pi2 )
            if not 0.0 <= chi2 < 25 : continue
            
            mc1 = mcKS  ( pi2 )
            mc2 = mcL01 ( pi2 )
            mc3 = mcL02 ( pi2 )
            if mc1 or mc2 or mc3 :  
                self.plot ( M(pi2) / Units.GeV , 'MASS pi pi ' , 0.3 , 0.8 , 500 )
            else: continue

            tup.column ( 'm'     , M(pi2) / Units.GeV )
            tup.column ( 'mc1'   , mc1       )
            tup.column ( 'mc2'   , mc2       )
            tup.column ( 'mc3'   , mc3       )
            tup.column ( 'vchi2' , chi2      )
            tup.column ( 'tr1'   , TRTYPE ( pi2(1) ) ) 
            tup.column ( 'tr2'   , TRTYPE ( pi2(2) ) ) 
            tup.column ( 'lv02'  , LV02(pi2) )

            tup.write()


        return SUCCESS


# =============================================================================
## configure the job
def configure ( **args ) :
    """
    Configure the job
    """
    
    from Configurables import DaVinci, HistogramPersistencySvc,EventSelector, NTupleSvc
    
    daVinci = DaVinci (
        DataType   = 'MC09' , 
        Simulation = True
        ) 
    
    HistogramPersistencySvc ( OutputFile = 'AP_Histos.root' )
    
    EventSelector (
        PrintFreq = 100 , 
        Input = [
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000001_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000002_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000003_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000004_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000005_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005329/0000/00005329_00000006_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000001_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000002_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000003_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000004_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000005_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
        "   DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/MC09/DST/00005341/0000/00005341_00000006_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'"]
        )
    NTupleSvc ( Output = [ "AP DATAFILE='AP_Tuples.root' TYPE='ROOT' OPT='NEW'"] )
    
    gaudi = appMgr() 
    
    ## create local algorithm:
    alg = AP (
        'AP'  ,
        ## MC-relations 
        PP2MCs         = [ 'Relations/Rec/ProtoP/Charged' ] ,
        ## print histograms ? 
        HistoPrint     = True  ,
        ## LUN for N-tuples 
        NTupleLUN      = 'AP'  ,
        ## input particles : 
        InputLocations = [ 'StdNoPIDsPions' ]
        )
    
    ##gaudi.addAlgorithm ( alg ) 
    gaudi.setAlgorithms( [alg] )
   
    return SUCCESS 

    
# =============================================================================
## job steering 
if __name__ == '__main__' :

    ## make printout of the own documentations 
    print __doc__
    print __author__
    print __version__

    ## configure the job:
    configure()
    
    ## run the job
    run(10000)
    
# =============================================================================
# The END 
# =============================================================================
