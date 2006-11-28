#!/usr/bin/env python2.4
# =============================================================================
# $Id: Phi.py,v 1.18 2006-11-28 18:26:45 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
# =============================================================================
## The simple Bender-based example: plot dikaon mass peak
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
#  @date 2006-10-12
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The simple Bender-based example plot dikaon mass peak """
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================

## import everything form bender 
from bendermodule import * 

#gaudi.config()
#import gaudimodule
#gaudimodule.loaddict('EventAssocDict')

## Simple class to plot dikaon mass peak
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2006-10-13
class Phi(Algo) :
    """ simple class to plot dikaon mass peak """
    
    ## standard constructor
    def __init__ ( self , name = 'Phi' ) :
        """ standard constructor """ 
        return Algo.__init__ ( self , name )

    ## standard mehtod for analyses
    def analyse( self ) :
        """ standard method for analyses """

        ## select all kaons 
        kaons = self.select( 'kaons'  , 'K+'  == ABSID )
        
        self.select( "K+" , kaons , 0 < Q )
        self.select( "K-" , kaons , 0 > Q )
        
        dikaon = self.loop( "K+ K-" , "phi(1020)" )
        for phi in dikaon :
            m12 = phi.mass(1,2) / 1000 
            if 0 > m12 or 1.1 < m12 : continue
            self.plot( M(phi)/1000 , "K+K- mass " , 1. , 1.1 ) 
            chi2 = VCHI2( phi )
            if 0 > chi2 or 49 < chi2 : continue
            self.plot( M(phi)/1000 , "K+K- mass chi2<49  " , 1. , 1.1 ) 
            
        self.setFilterPassed( True ) 
        return SUCCESS
    
## configure the job
def configure ( **args ) :
    """ Configure the job """
    
    ## read external configruation files
    gaudi.config ( files = [ 
        '$DAVINCIROOT/options/DaVinciCommon.opts'         ,
        '$COMMONPARTICLESROOT/options/StandardKaons.opts'
        ] )
    
    ## I am old-fashioned person - I like HBOOK 
    if not 'HbookCnv' in gaudi.DLls : gaudi.DLLs += ['HbookCnv']
    gaudi.HistogramPersistency = "HBOOK"
    hps = gaudi.service('HistogramPersistencySvc')
    hps.OutputFile = args.get('histos','PhiMC.hbook')

    ## StagerSvc at CERN
    if 'CERN' == os.environ.get('CMTPATH',None) and \
           os.environ.has_key('GaudiSiteSvcShr') :
        stager = gaudi.service('GaudiSiteSvc')
        stager.BlockSize    = 20
        stager.InitialStage =  5 
        if not 'GaudiSiteSvc' in gaudi.DLLs   : gaudi.DLLs   += [ 'GaudiSiteSvc']
        if not 'StagerSvc'    in gaudi.ExtSvc : gaudi.ExtSvc += [ 'StagerSvc'   ]
        
    ## create local algorithm:
    alg = Phi()

    gaudi.addAlgorithm( alg )
    ## add to main sequence in Davinci
    #davinci = gaudi.algorithm('GaudiSequencer/DaVinciMainSeq')
    #davinci.Members += ['Phi']
    
    ## configure the desktop
    desktop = gaudi.tool ( 'Phi.PhysDesktop' )
    desktop.InputLocations = [ '/Event/Phys/StdLooseKaons' ]
    
    ## add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True
    
    ## get the input data
    import data_Bs2Jpsiphi_mm as input 
    
    ## get input data 
    evtSel = gaudi.evtSel()    
    evtSel.open ( input.PFNs ) 
    evtSel.PrintFreq = 100
    
    return SUCCESS 
    
## report about what is imported
if __name__ == '__main__' :

    ## configure the job:
    configure()

    ## run the job
    gaudi.run(5000)
    
# =============================================================================
# $Log: not supported by cvs2svn $
# 
# =============================================================================
# The END 
# =============================================================================
