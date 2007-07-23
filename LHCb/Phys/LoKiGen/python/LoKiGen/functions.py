#!/usr/bin/env python
# =============================================================================
## @file LoKiCore/functions.py
#  The full set of useful objects from LoKiGen library 
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
""" The full set of useful objects from LoKiGen library """
_author_ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

## needed since there is no autoloading of HepMC dictionaries:
import sys,PyCintex

if sys.platform == 'win32' : PyCintex.loadDict (    "HepMCRflx" )  
else                       : PyCintex.loadDict ( "libHepMCRflx" )  
    
# Namespaces:
_global  = _LoKiCore._global 
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
HepMC    = _global.HepMC

_GP = 'const HepMC::GenParticle*'
_GV = 'const HepMC::GenVertex*'

_c1 = std.vector( _GP )
_c2 = std.vector( _GV )

## ranges
GRange   = LoKi.NamedRange_ ( _c1     )
GVRange  = LoKi.NamedRange_ ( _c2     )
## range-lists: 
GRangeL  = LoKi.RangeList_  (  GRange )
GVRangeL = LoKi.RangeList_  ( GVRange )

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::GFunc
GFunc  = LoKi.Function               ( _GP ) 
## @see LoKi::Types::GCuts
GCuts  = LoKi.Predicate              ( _GP ) 
## @see LoKi::Types::GFun
GFun   = LoKi.FunctionFromFunction   ( _GP ) 
## @see LoKi::Types::GCut
GCut   = LoKi.PredicateFromPredicate ( _GP ) 
## @see LoKi::Types::GFunc
GVFunc = LoKi.Function               ( _GV ) 
## @see LoKi::Types::GCuts
GVCuts = LoKi.Predicate              ( _GV ) 
## @see LoKi::Types::GFun
GVFun  = LoKi.FunctionFromFunction   ( _GV ) 
## @see LoKi::Types::GCut
GVCut  = LoKi.PredicateFromPredicate ( _GV ) 

# =============================================================================
## All concrete types 
# =============================================================================


## @see LoKi::Cuts::G3Q
G3Q      = LoKi.GenParticles.ThreeCharge() 
## @see LoKi::Cuts::GABSID
GABSID    = LoKi.GenParticles.AbsIdentifier()
## @see LoKi::Cuts::GALL
GALL      = LoKi.BooleanConstant ( _GP ) ( True ) 
## @see LoKi::Cuts::GANCESTOR
GANCESTOR = LoKi.GenParticles.IsAnAncestor
## @see LoKi::Cuts::GBAR
GBAR      = LoKi.GenParticles.BarCode ()
## @see LoKi::Cuts::GBARCODE 
GBARCODE  = LoKi.GenParticles.BarCode ()
## @see LoKi::Cuts::GBARYON
GBARYON  = LoKi.GenParticles.IsBaryon() 
## @see LoKi::Cuts::GBEAUTY
##GBEAUTY   = GQUARK( LHCb.ParticleID.bottom  )
GBEAUTY   = LoKi.GenParticles.HasQuark ( 5 )
## @see LoKi::Cuts::GCHARGED
GCHARGED  = LoKi.GenParticles.IsCharged ()
## @see LoKi::Cuts::GCHARM
##GCHARM  = GQUARK( LHCb.ParticleID.charm  )
GCHARM    = LoKi.GenParticles.HasQuark ( 4 )
## @see LoKi::Cuts::GCOUNTER
GCOUNTER  = LoKi.Monitoring.Counter ( _GP )
## @see LoKi::Cuts::GCTAU
GCTAU     = LoKi.GenParticles.ProperLifeTime ()
## @see LoKi::Cuts::GDELTAR2
GDELTAR2  = LoKi.GenParticles.DeltaR2   
## @see LoKi::Cuts::GDETA
GDETA     = LoKi.GenParticles.DeltaEta  
## @see LoKi::Cuts::GDPHI
GDPHI     = LoKi.GenParticles.DeltaPhi  
## @see LoKi::Cuts::GDR2
GDR2      = LoKi.GenParticles.DeltaR2   
## @see LoKi::Cuts::GE
GE        = LoKi.GenParticles.Energy  ()
## @see LoKi::Cuts::GETA
GETA      = LoKi.GenParticles.PseudoRapidity ()
## @see LoKi::Cuts::GFALSE
GFALSE    = LoKi.BooleanConstant( _GP )(False)
## @see LoKi::Cuts::GFAEVX
GFAEVX   = LoKi.GenParticles.AdapterToEndVertex
## @see LoKi::Cuts::GFAPVX
GFAPVX   = LoKi.GenParticles.AdapterToProductionVertex
## @see LoKi::Cuts::GFROMTREE
GFROMTREE = LoKi.GenParticles.FromHepMCTree
## @see LoKi::Cuts::GHADRON
GHADRON  = LoKi.GenParticles.IsHadron   () 
## @see LoKi::Cuts::GID
GID      = LoKi.GenParticles.Identifier ()
## @see LoKi::Cuts::GINTREE
GINTREE  = LoKi.GenParticles.InTree
## @see LoKi::Cuts::GLEPTON
GLEPTON   = LoKi.GenParticles.IsLepton  () 
## @see LoKi::Cuts::GM
GM       = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GM0
GM0      = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GMASS
GMASS    = LoKi.GenParticles.Mass       () 
## @see LoKi::Cuts::GMESON
GMESON   = LoKi.GenParticles.IsMeson    () 
## @see LoKi::Cuts::GMOMDIST
GMOMDIST  = LoKi.GenParticles.MomentumDistance
## @see LoKi::Cuts::GNEUTRAL
GNEUTRAL  = LoKi.GenParticles.IsNeutral () 
## @see LoKi::Cuts::GNINTREE
GNINTREE  = LoKi.GenParticles.NInTree
## @see LoKi::Cuts::GNLT
GNLT     = LoKi.GenParticles.NominalLifeTime () 
## @see LoKi::Cuts::GNONE
GNONE    = LoKi.BooleanConstant( _GP )(False)
## @see LoKi::Cuts::GNUCLEUS
GNUCLEUS = LoKi.GenParticles.IsNucleus () 
## @see LoKi::Cuts::GONE
GONE      = LoKi.Constant ( _GP )( 1.0 )
## @see LoKi::Cuts::GP
GP       = LoKi.GenParticles.Momentum () 
## @see LoKi::Cuts::GPHI
GPHI      = LoKi.GenParticles.Phi     ()
## @see LoKi::Cuts::GPLOT
GPLOT     = LoKi.Monitoring.Plot ( _GP )
## @see LoKi::Cuts::GPT
GPT      = LoKi.GenParticles.TransverseMomentum     () 
## @see LoKi::Cuts::GPT0
GPT0     = LoKi.GenParticles.TransverseMomentum     () 
## @see LoKi::Cuts::GPTDIR
GPTDIR    = LoKi.GenParticles.TransverseMomentumRel
## @see LoKi::Cuts::GPTREL
GPTREL    = LoKi.GenParticles.TransverseMomentumRel
## @see LoKi::Cuts::GPX
GPX      = LoKi.GenParticles.MomentumX () 
## @see LoKi::Cuts::GPY
GPY      = LoKi.GenParticles.MomentumY () 
## @see LoKi::Cuts::GPZ
GPZ      = LoKi.GenParticles.MomentumZ () 
## @see LoKi::Cuts::GQUARK
GQUARK    = LoKi.GenParticles.HasQuark
## @see LoKi::Cuts::GSSWITCH
GSSWITCH  = LoKi.SimpleSwitch    ( _GP )
## @see LoKi::Cuts::GSTAT
GSTAT     = LoKi.Monitoring.Stat ( _GP )
## @see LoKi::Cuts::GSTATUS
GSTATUS   = LoKi.GenParticles.Status ( )
## @see LoKi::Cuts::GSTRANGE
## GSTRANGE  = LoKi.GenParticles.HasQuark ( LHCb.ParticleID.strange )
GSTRANGE  = LoKi.GenParticles.HasQuark ( 3 )
## @see LoKi::Cuts::GSWITCH
GSWITCH   = LoKi.Switch          ( _GP )
## @see LoKi::Cuts::GTHETA
GTHETA    = LoKi.GenParticles.Theta ()
## @see LoKi::Cuts::GTIME
GTIME    = LoKi.GenParticles.ProperLifeTime () 
## @see LoKi::Cuts::GTOP
## GTOP  = LoKi.GenParticles.HasQuark ( LHCb.ParticleID.top )
GTOP     = LoKi.GenParticles.HasQuark ( 6 )
## @see LoKi::Cuts::GTRUE
GTRUE    = LoKi.BooleanConstant ( _GP )( True )
## @see LoKi::Cuts::GVALID
GVALID   = LoKi.Valid           ( _GP ) ()
## @see LoKi::Cuts::GVEV
GVEV     = LoKi.GenParticles.ValidEndVertex()
## @see LoKi::Cuts::GZERO
GZERO     = LoKi.Constant       ( _GP )( 0.0 )    



## @see LoKi::Cuts::GVTRUE
GVTRUE     = LoKi.BooleanConstant( _GV )(True)
## @see LoKi::Cuts::GVFALSE
GVFALSE    = LoKi.BooleanConstant( _GV )(False)
## @see LoKi::Cuts::GVALL
GVALL      = GVTRUE
## @see LoKi::Cuts::GVNONE
GVNONE     = GVFALSE
## @see LoKi::Cuts::GVONE
GVONE      = LoKi.Constant ( _GV )(1.0)
## @see LoKi::Cuts::GVZERO
GVZERO     = LoKi.Constant ( _GV )(0.0)
## @see LoKi::Cuts::GVMIN
GVMIN      = LoKi.Min          ( _GV )
## @see LoKi::Cuts::GVMAX
GVMAX      = LoKi.Max          ( _GV )
## @see LoKi::Cuts::GVSWITCH
GVSWITCH   = LoKi.Switch       ( _GV )
## @see LoKi::Cuts::GVSSWITCH
GVSSWITCH  = LoKi.SimpleSwitch ( _GV )
## @see LoKi::Cuts::GVVALID
GVVALID    = LoKi.Valid        ( _GV )()
## @see LoKi::Cuts::GVSAME
GVSAME     = LoKi.TheSame      ( _GV )
## @see LoKi::Cuts::GVBAR
GVBAR      = LoKi.GenVertices.BarCode()
## @see LoKi::Cuts::GVBARCODE 
GVBARCODE  = GVBAR
## @see LoKi::Cuts::GVTIME
GVTIME     = LoKi.GenVertices.PositionT()
## @see LoKi::Cuts::GVCOUNT
GVCOUNT    = LoKi.GenVertices.CountIF
## @see LoKi::Cuts::GVSUM
GVSUM      = LoKi.GenVertices.SumIF 


# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o
        
# =============================================================================
# The END
# =============================================================================
