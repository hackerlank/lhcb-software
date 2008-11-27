#!/usr/bin/env python
# =============================================================================
## @file LoKiMC/decorators.py
#  The set of basic decorator for objects from LoKiMC library
#  The file is a part of LoKi and Bender projects
#   
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiMC library

      This file is a part of LoKi project - 
'C++ ToolKit  for Smart and Friendly Physics Analysis'

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiMC.functions   import *
import LoKiMC.MC

if not 'Bender' in dir() : Bender =  LoKi.Dicts

# =============================================================================
## decorate MC-finder object:
def _valid_(s)         :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if finder.valid() : 
    
    """
    return LoKi.Dicts.MCFinderDicts.valid      (s)
## decorate MC-finder object:
def _not_valid_(s)      :
    """
    Check the validity of the MC-finder object and
    the underlying pointer to IMCDecayFinder tool

    >>> finder = ...
    >>> if not finder : ...
    
    """
    return not s.valid()
# =============================================================================
## update the documentation 
_valid_.__doc__     += LoKi.Dicts.MCFinderDicts.valid.__doc__
# =============================================================================
## find the decays, see IMCDecayFinder::
def _findDecays_(s,decay,*a) :
    """
    Find the decay according t the decay descriptor
    [see IMCDecayFinder interface]

    >>> finder=...
    >>> decays = finder.findDecay( 'B0-> pi+ pi-')
    >>> 

    @param decay the decay descriptor
    @return the Range of found decay trees
    
    """
    return LoKi.Dicts.MCFinderDicts.findDecays ( s , decay , *a )
# =============================================================================
## update the documentation 
_findDecays_.__doc__ += LoKi.Dicts.MCFinderDicts.findDecays.__doc__
# =============================================================================
LoKi.MCFinder.find       = _findDecays_
LoKi.MCFinder.findDecay  = _findDecays_
LoKi.MCFinder.findDecays = _findDecays_
LoKi.MCFinder.valid      = _valid_ 
LoKi.MCFinder.__invert__ = _not_valid_ 


_name = __name__ 
# =============================================================================
## Make the decoration of all objects from this module
def _decorate ( name = _name ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    _mcp = 'const LHCb::MCParticle*'
    _mcv = 'const LHCb::MCVertex*'
    #_vp  = std.vector( _mcp )
    #_vv  = std.vector( _mcv )
    #_vd  = std.vector( 'double' )
    _vp  = 'std::vector<const LHCb::MCParticle*>' ## std.vector( _mcp )
    _vv  = 'std::vector<const LHCb::MCVertex*>'   ## std.vector( _mcv )
    _vd  = 'std::vector<double>'                  ## std.vector( 'double' )
    #
    
    # MCParticle -> double
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_mcp,'double')         ) ## the base
    _decorated  = _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (LHCb.MCParticle)  ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_mcp,_mcp)        ) ## operators&operations
    
    # MCVertex -> double
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_mcv,'double')         ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunCalls (LHCb.MCVertex)  ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FuncOps  (_mcv,_mcv)        ) ## operators&operations
    
    # MCParticle -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_mcp,bool)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (LHCb.MCParticle)  ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_mcp,_mcp)        ) ## operators&operations

    # MCVertex -> bool
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_mcv,bool)             ) ## the base
    _decorated |= _LoKiCore.decorateCalls       (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutCalls (LHCb.MCVertex)    ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutsOps  (_mcv,_mcv)        ) ## operators&operations

    ## functional part:
    
    # vector<T> -> vector<double>
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_vp,_vd)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_mcp)               ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_mcp)               ) ## call-traits
    
    _functions = _LoKiCore.getInherited (
        name                                   , ## moduel name  
        LoKi.Functor   (_vv,_vd)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_mcv)               ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.MapsOps(_mcv)               ) ## call-traits

    # vector<T> -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,_vp)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_mcp,_mcp)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_mcp,_mcp)          ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_vv)               ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_mcv,_mcv)          ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.PipeOps(_mcv,_mcv)          ) ## call-traits

    # vector<T> -> double

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcp)             ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,'double')          ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcv)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcv)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.FunValOps(_mcv)             ) ## call-traits

    # vector<T> -> bool

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.decoratePredicateOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcp)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcp)             ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,bool)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcv)             ) ## call-traits
    _decorated |= _LoKiCore.decorateFunctionOps (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcv)             ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.CutValOps(_mcv)             ) ## call-traits

    # vector<T> -> T

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vp,_mcp)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_mcp,_mcp)       ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_mcp,_mcp)       ) ## call-traits


    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   (_vv,_mcv)              ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_mcv,_mcv)       ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.ElementOps(_mcv,_mcv)       ) ## call-traits

     #sources : void -> vector<T>

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vp)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_mcp,_mcp)        ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_mcp,_mcp)        ) ## call-traits

    _functions = _LoKiCore.getInherited (
        name                                   , ## module name  
        LoKi.Functor   ('void',_vv)            ) ## the base
    _decorated |= _LoKiCore.decorateCalls  (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_mcv,_mcv)        ) ## call-traits
    _decorated |= _LoKiCore.decorateMaps   (
        _functions                             , ## list of functor types
        LoKi.Dicts.SourceOps(_mcv,_mcv)        ) ## call-traits

   
    ## decorate pids (Comparison with strings, integers and ParticleID objects:
    for t in ( MCID , MCABSID ) :
        t = type ( t ) 
        _LoKiCore.decoratePID ( t , LoKi.Dicts.PIDOps ( t ) )
        _decorated.add ( t )
    return _decorated
# =============================================================================


# =============================================================================
## Perform the actual decoration:
_decorated = _decorate() 
# =============================================================================

# =============================================================================
if '__main__' == __name__ :
    print 'Number of properly decorated types: %s' % len(_decorated)
    for o in _decorated : print o

# =============================================================================
# The END 
# =============================================================================
