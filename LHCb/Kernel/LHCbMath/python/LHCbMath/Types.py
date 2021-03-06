#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#
#  Simple file to provide "easy" access in python for
#  the basic ROOT::Math classes
#  @see GaudiKernel/Point3DTypes.h
#  @see GaudiKernel/Vector3DTypes.h
#  @see GaudiKernel/Vector4DTypes.h
#  @see GaudiKernel/GenericVectorTypes.h
#
#  The usage is fairly trivial:
#
#  @code
#
#  import LHCbMath.Types
#
#  @endcode
#
#  Important: All types are defined in corresponding C++ namespaces
#
#  @code
#
#  import LHCbMath.Types
#
#  from GaudiPython.Bindings import gbl as cpp ## get global C++ namespace
#  Gaudi = cpp.Gaudi                           ## get C++ namespace Gaudi
#
#  p3 = Gaudi.XYZPoint(0,1,2)               ## use C++ type Gaudi::XYZPoint
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
#
#  Last modification $Date$
#                 by $Author$
#
#
# =============================================================================
"""
Simple file to provide 'easy' access in python for the basic ROOT::Math classes

  see $GAUDIKERNELROOT/GaudiKernel/Point3DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/Vector3DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/Vector4DTypes.h
  see $GAUDIKERNELROOT/GaudiKernel/GenericVectorTypes.h

  see $LHCBMATHROOT/LHCbMath/Line.h

  The lines and planes are decorated:
     see $LHCBMATHROOT/LHCbMath/GeomFun.h


  The usage is fairly trivial:

  >>> import LHCbMath.Types

  Important: All types are defined in corresponding
               C++ namespaces: Gaudi & Gaudi::Math

  >>> import LHCbMath.Types
  >>> from GaudiPython.Bindings import gbl as cpp ## get global C++ namespace
  >>> Gaudi = cpp.Gaudi                           ## get C++ namespace Gaudi
  >>> p3 = Gaudi.XYZPoint(0,1,2)                  ## use C++ type Gaudi::XYZPoint

  >>> dir( Gaudi.Math )
  >>> dir( Gaudi      )

  Last modification $Date$
                 by $Author$

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2009-09-12"
__version__ = "Version$Revision$"
# =============================================================================
__all__     = () ## nothing to be imported !
# =============================================================================
import ROOT

try:
    import cppyy
except ImportError:
    # FIXME: backward compatibility
    print "# WARNING: using PyCintex as cppyy implementation"
    import PyCintex as cppyy
    import sys
    sys.modules['cppyy'] = cppyy

## get global C++ namespace
cpp = cppyy.makeNamespace('')

## C++ namespace Gaudi
std   = cpp.std

Gaudi = cpp.Gaudi 
## ROOT::Math namespace
_RM = ROOT.ROOT.Math

## Geomemtry vectors 
cpp.Gaudi.XYZPoint            = _RM.PositionVector3D     ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
cpp.Gaudi.XYZVector           = _RM.DisplacementVector3D ('ROOT::Math::Cartesian3D<double>,ROOT::Math::DefaultCoordinateSystemTag')
cpp.Gaudi.LorentzVector       = _RM.LorentzVector        ('ROOT::Math::PxPyPzE4D<double>')
cpp.Gaudi.Plane3D             = _RM.Plane3D

cpp.Gaudi.Math.XYZPoint       = cpp.Gaudi.XYZPoint
cpp.Gaudi.Math.XYZVector      = cpp.Gaudi.XYZVector
cpp.Gaudi.Math.LorentzVector  = cpp.Gaudi.LorentzVector
cpp.Gaudi.Math.Plane3D        = cpp.Gaudi.Plane3D

cpp.Gaudi.Point3D             = cpp.Gaudi.XYZPoint
cpp.Gaudi.Math.Point3D        = cpp.Gaudi.XYZPoint

cpp.Gaudi.Vector3D            = cpp.Gaudi.XYZVector
cpp.Gaudi.Math.Vector3D       = cpp.Gaudi.XYZVector

# =============================================================================
## try to pickup the vector
@staticmethod
def _vector_ ( i , typ = 'double' ) :
    """
    Pick up the vector of correspoding size

    >>>  V3   = Gaudi.Math.Vector(3)
    >>>  vct  = V3 ()

    """
    v = _RM.SVector ( typ , i )
    return deco_vector ( v ) 

# =============================================================================
## try to pickup the matrix
@staticmethod
def _matrix_ ( i , j , typ = 'double' ) :
    """
    Pick up the matrix of correspoding size

    >>>  M3x4   = Gaudi.Math.Matrix(3,4)
    >>>  matrix = M3x4 ()
    
    """
    m = _RM.SMatrix ( "%s,%d,%d" % ( typ , i , j ) )
    return deco_matrix( m )  


# =============================================================================
## try to pickup the symmeric matrix
@staticmethod
def _sym_matrix_ ( i , typ = 'double' ) :
    """
    Pick up the symmetric matrix of correspoding size

    >>>  SymM3  = Gaudi.Math.SymMatrix(3)
    >>>  matrix = SymM3 ()

    """
    m = _RM.SMatrix('%s,%d,%d,ROOT::Math::MatRepSym<%s,%d>' %  ( typ , i , i , typ , i ) )
    return deco_symmatrix  ( m ) 

cpp.Gaudi.Vector         =     _vector_
cpp.Gaudi.Math.Vector    =     _vector_
cpp.Gaudi.Matrix         =     _matrix_
cpp.Gaudi.Math.Matrix    =     _matrix_
cpp.Gaudi.SymMatrix      = _sym_matrix_
cpp.Gaudi.Math.SymMatrix = _sym_matrix_

## Gaudi::Math
cpp.Gaudi.Math.XYZLine           = cpp.Gaudi.Math.Line( cpp.Gaudi.XYZPoint, cpp.Gaudi.XYZVector)
cpp.Gaudi.Math.SVector2WithError = cpp.Gaudi.Math.SVectorWithError(2,'double')
cpp.Gaudi.Math.SVector3WithError = cpp.Gaudi.Math.SVectorWithError(3,'double')
cpp.Gaudi.Math.SVector4WithError = cpp.Gaudi.Math.SVectorWithError(4,'double')
cpp.Gaudi.Math.SVector5WithError = cpp.Gaudi.Math.SVectorWithError(5,'double')
cpp.Gaudi.Math.SVector6WithError = cpp.Gaudi.Math.SVectorWithError(6,'double')
cpp.Gaudi.Math.SVector8WithError = cpp.Gaudi.Math.SVectorWithError(8,'double')

cpp.Gaudi.XYZLine                = cpp.Gaudi.Math.XYZLine
cpp.Gaudi.Line3D                 = cpp.Gaudi.Math.XYZLine
cpp.Gaudi.Math.Line3D            = cpp.Gaudi.Math.XYZLine


cpp.Gaudi.Math.frac              = cpp.Gaudi.Math.Functions.frac
cpp.Gaudi.Math.asym              = cpp.Gaudi.Math.Functions.asym
cpp.Gaudi.Math.binomEff          = cpp.Gaudi.Math.Functions.binomEff

## vectors of vectors
cpp.Gaudi.Vectors2       = std.vector ( cpp.Gaudi.Vector2 )
cpp.Gaudi.Vectors3       = std.vector ( cpp.Gaudi.Vector3 )
cpp.Gaudi.Vectors4       = std.vector ( cpp.Gaudi.Vector4 )
cpp.Gaudi.Math.Vectors2  = cpp.Gaudi.Vectors2
cpp.Gaudi.Math.Vectors3  = cpp.Gaudi.Vectors3
cpp.Gaudi.Math.Vectors4  = cpp.Gaudi.Vectors4

cpp.Gaudi.Math.ValueWithError.Vector = cpp.std.vector( cpp.Gaudi.Math.ValueWithError)
cpp.Gaudi.Math.ValueWithError.Vector .__str__   = lambda s : str( [ i for i in s ])
cpp.Gaudi.Math.ValueWithError.Vector .__repr__  = lambda s : str( [ i for i in s ])





## Sum the contents of the vector
def _ve_sum_ ( s ) :
    """

    >>> v = ...
    >>> s = v.sum()

    """
    return Gaudi.Math.sum ( s )


## Sum the contents of the vector
def _ve_asum_ ( s ) :
    """

    >>> v = ...
    >>> s = v.abssum()

    """
    return Gaudi.Math.abssum ( s )

_ve_sum_  . __doc__ += '\n' + cpp.Gaudi.Math.sum    .__doc__
_ve_asum_ . __doc__ += '\n' + cpp.Gaudi.Math.abssum .__doc__

_VVE = std.vector( cpp.Gaudi.Math.ValueWithError )

_VVE  . sum      = _ve_sum_
_VVE  . abssum   = _ve_asum_
_VVE  . sumabs   = _ve_asum_
_VVE  . __str__  = lambda s : str( [ i for i in s ] )
_VVE  . __repr__ = lambda s : str( [ i for i in s ] )
_VVE  . __len__  = lambda s : s.size ()

_VVVE = std.vector( _VVE )
_VVVE . __str__  = lambda s : str( [ i for i in s ] )
_VVVE . __repr__ = lambda s : str( [ i for i in s ] )
_VVVE . __len__  = lambda s : s.size ()



_VVE.Vector = _VVVE
cpp.Gaudi.Math.ValueWithError.Vector = _VVE

_C2F            = cpp.Gaudi.Math.Chi2Fit
_C2F . __str__  = lambda s : s.toString ()
_C2F . __repr__ = lambda s : s.toString ()
## chi2-probabilty
def _c2_prob_  ( s ) :
    """
    Chi2 probabiilty

    >>> r = h.hfit ( ... )
    >>> r.Prob()

    """
    dofs = s.points() - s.size()
    return ROOT.TMath.Prob ( s.chi2() , dofs )

_C2F . Prob        = _c2_prob_
_C2F . Probability = _c2_prob_
_C2F . prob        = _c2_prob_
_C2F . probability = _c2_prob_
_C2F . __len__     = lambda s     : s.size  (   )
_C2F . __getitem__ = lambda s , i : s.param ( i )


# =============================================================================
# Operations  
# =============================================================================

def _lav_iadd_ ( self , other ) :
    _typ = type ( self )

    if   isinstance ( other , _typ ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] += other[i]
        return self
    
    elif isinstance ( other , (float,int,long) ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] += other
        return self
    #
    ## print 'NOT-implemented! '
    ## print 'I am SADD', type(other), type(self)
    ## print 'I am len' , len(self)     
    #
    return NotImplemented 

def _lav_isub_ ( self , other ) :
    _typ = type ( self )
    if   isinstance ( other , _typ ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] -= other[i]
        return self
    elif isinstance ( other , (float,int,long) ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] -= other
        return self
    #
    return NotImplemented 

def _lav_imul_ ( self , other ) :
    
    if isinstance ( other , (float,int,long) ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] *= other
        return self
    
def _lav_idiv_ ( self , other ) :
        
    if isinstance ( other , (float,int,long) ) :
        l = len ( self )
        for i in range ( 0 , l ) : self[i] /= other
        return self
    #
    return NotImplemented 

    
def _lav_add_ ( self , other ) :
    """
    Addition of LA-vectors
    
    >>> vct1 = ...
    >>> vct2 = ...
    >>> a1   = vct1 + 2
    >>> a2   = vct1 + vct2
    """
    _typ = type( self )
    tmp  = _typ( self )
    tmp += other
    return tmp

def _lav_sub_ ( self , other ) :
    """
    Subtract LA-vectors
    
    >>> vct1 = ...
    >>> vct2 = ...
    >>> a1   = vct1 - 2
    >>> a2   = vct1 - vct2 
    """
    _typ = type( self )
    tmp  = _typ( self )
    tmp -= other
    return tmp

def _lav_rsub_ ( self , other ) :
    """
    Right subtraction:
    
    >>> vct = ...
    >>> a1  = 2 - vct  
    """
    self *= -1
    self += other
    return self 

def _lav_mul_ ( self , other ) :
    """
    Multiply/scale LA-vector
    
    >>> vct = ...
    >>> a1  = vct * 2 
    >>> a2  = 2   * vct
    >>> a3  = vct * vct  ## ``norm''
    
    """
    _typ = type ( self )
    if   isinstance ( other , _typ ) :
        res = 0.0
        l   = len ( self )
        for i in range( 0 , l ) : res += self[i]*other[i]
        return res
    elif isinstance ( other , (float,int,long) )  :
        tmp  = _typ ( self )
        tmp *= other
        return tmp
    #
    return NotImplemented 

def _lav_div_ ( self , other ) :
    """
    Divide/scale LA-vector
    
    >>> vct = ...
    >>> a   = vct / 2 
    
    """
    _typ = type( self )
    tmp  = _typ( self )
    tmp /= other
    return tmp

def _lav_pow_ ( self , e ) :
    """
    Vector norm
    
    >>> vct   = ...
    >>> norm  = vct ** 2 
    
    """
    if 2 != e : return NotImplemented
    return self*self


## decorate vector 
def deco_vector ( t ) :

    if not hasattr ( t , '__iadd__' ) : t. __iadd__ = _lad_iadd_
    if not hasattr ( t , '__isub__' ) : t. __isub__ = _lad_isub_
    if not hasattr ( t , '__imul__' ) : t. __iadd__ = _lad_idiv_
    if not hasattr ( t , '__idiv__' ) : t. __isub__ = _lad_imul_
    #
    t. __add__  = _lav_add_
    t. __mul__  = _lav_mul_
    t. __sub__  = _lav_sub_
    t. __div__  = _lav_div_
    t. __pow__  = _lav_pow_
    
    t. __radd__ = lambda s,o : s+o
    t. __rmul__ = lambda s,o : s+o
    t. __rsub__ = _lav_rsub_ 

    return t

    
for t in ( cpp.Gaudi.Vector2 ,
           cpp.Gaudi.Vector3 ,
           cpp.Gaudi.Vector4 ,
           cpp.Gaudi.Vector5 ,
           cpp.Gaudi.Vector6 ,
           cpp.Gaudi.Vector8 ) : deco_vector ( t ) 
    #
           

cpp.Gaudi.Vector2             = cpp.Gaudi.Vector(2)
cpp.Gaudi.Vector3             = cpp.Gaudi.Vector(3)
cpp.Gaudi.Vector4             = cpp.Gaudi.Vector(4)
cpp.Gaudi.Vector5             = cpp.Gaudi.Vector(5)
cpp.Gaudi.Vector6             = cpp.Gaudi.Vector(6)
cpp.Gaudi.Vector8             = cpp.Gaudi.Vector(8)

cpp.Gaudi.Math.Vector2        = cpp.Gaudi.Vector2
cpp.Gaudi.Math.Vector3        = cpp.Gaudi.Vector3
cpp.Gaudi.Math.Vector4        = cpp.Gaudi.Vector4
cpp.Gaudi.Math.Vector5        = cpp.Gaudi.Vector5
cpp.Gaudi.Math.Vector8        = cpp.Gaudi.Vector8


## ============================================================================
## some useful decoration:
## ============================================================================

_V4D = cpp.Gaudi.LorentzVector

## 4-vectors 
def _v4_iadd_ ( s , other ) :
    s.SetE    ( s.E  () + other.E  () )
    s.SetPx   ( s.Px () + other.Px () )
    s.SetPy   ( s.Py () + other.Py () )
    s.SetPz   ( s.Pz () + other.Pz () )
    return s

def _v4_isub_ ( s , other ) :
    s.SetE    ( s.E  () - other.E  () )
    s.SetPx   ( s.Px () - other.Px () )
    s.SetPy   ( s.Py () - other.Py () )
    s.SetPz   ( s.Pz () - other.Pz () )
    return s

def _v4_dot_   ( s , other ) :
    res  = s.e  () * other.e  () 
    res -= s.px () * other.px ()
    res -= s.py () * other.py ()
    res -= s.pz () * other.pz ()
    return res 

if not hasattr ( _V4D , '__iadd__' ) : _V4D. __iadd__ = _v4_iadd_ 
if not hasattr ( _V4D , '__isub__' ) : _V4D. __isub__ = _v4_isub_ 
if not hasattr ( _V4D , 'Dot'      ) : _V4D.Dot       = _v4_dot_

def _v4_mul_ ( self , other ) :
    """
    Multiplication/scaling of Lorentz Vectors 
    
    >>> vct = ...
    >>> a   = vct * 2
    
    >>> vct2 = ...
    >>> prod = vct * vct2 ## NB! 
    """
    if isinstance ( other , _V4D ) : return self.Dot ( other )
    #
    tmp   = _V4D ( self )
    tmp  *= other
    return tmp

def _v4_add_ ( self , other ) :
    """
    Addition of Lorentz Vectors 
    
    >>> vct1 = ...
    >>> vct2 = ...
    >>> a    = vct1 + vct2
    """
    tmp   = _V4D ( self )
    tmp  += other
    return tmp

def _v4_sub_ ( self , other ) :
    """
    Subtraction of Lorentz Vectors 
    
    >>> vct1 = ...
    >>> vct2 = ...
    >>> a    = vct1 - vct2
    """
    tmp   = _V4D ( self )
    tmp  -= other
    return tmp

def _v4_div_ ( self , other ) :
    """
    Division/scaling of Lorentz Vectors 
    
    >>> vct = ...
    >>> a   = vct / 2 
    """
    tmp   = _V4D ( self )
    tmp  /= other
    return tmp

_V4D . __mul__  = _v4_mul_
_V4D . __add__  = _v4_add_
_V4D . __sub__  = _v4_sub_
_V4D . __div__  = _v4_div_

_V4D . __radd__ = lambda s,o : s+o 
_V4D . __rmul__ = lambda s,o : s*o 

## 3-vectors 
_P3D = cpp.Gaudi.XYZPoint
_V3D = cpp.Gaudi.XYZVector

## 3-vectors & points

def _v3_iadd_  ( s , other ) :
    s.SetX     ( s.X () + other.X () )
    s.SetY     ( s.Y () + other.Y () )
    s.SetZ     ( s.Z () + other.Z () )
    return s

def _v3_isub_  ( s , other ) :
    s.SetX     ( s.X () - other.X () )
    s.SetY     ( s.Y () - other.Y () )
    s.SetZ     ( s.Z () - other.Z () )
    return s

def _v3_dot_   ( s , other ) :
    res  = s.X ( ) * other.X ( )
    res -= s.Y ( ) * other.Y ( )
    res -= s.Z ( ) * other.Z ( )
    return res 

if not hasattr ( _V3D , '__iadd__' ) : _V3D. __iadd__ = _v3_iadd_
if not hasattr ( _V3D , '__isub__' ) : _V3D. __isub__ = _v3_isub_
if not hasattr ( _V3D , 'Dot'      ) : _V3D.Dot       = _v3_dot_
## _V3D. __iadd__ = _v3_iadd_
## _V3D. __isub__ = _v3_isub_
## _V3D.Dot       = _v3_dot_

if not hasattr ( _P3D , '__iadd__' ) : _P3D. __iadd__ = _v3_iadd_ 
if not hasattr ( _P3D , '__isub__' ) : _P3D. __isub__ = _v3_isub_ 
## _P3D. __iadd__ = _v3_iadd_ 
## _P3D. __isub__ = _v3_isub_ 


def _p3_as_v3_ ( self ) :
    """ Conversion to 3D-vector"""
    return _V3D( self.x() , self.y() , self.z() )
def _v3_as_p3_ ( self ) :
    """ Conversion to 3D-point """    
    return _P3D( self.x() , self.y() , self.z() )

_P3D. asV3 = _p3_as_v3_
_V3D. asP3 = _v3_as_p3_


def _p3_add_ ( self , other ) :
    """
    Addition of 3D-point and 3D-vector
    
    >>> point  = ...
    >>> vector = ...
    >>> result = point + vector 
    """
    # POINT + VECTOR = POINT 
    if isinstance ( other , _V3D ) :
        tmp   = _P3D ( self )
        tmp  += other
        return tmp
    #
    return NotImplemented


def _p3_sub_ ( self , other ) :
    """
    Substraction of 3D-points 
    
    >>> point1 = ...
    >>> point2 = ...
    >>> vector = ...
    >>> result_point  = point1 - vector
    >>> result_vector = point1 - point2  
    """
    # POINT - VECTOR = POINT
    if   isinstance ( other , _V3D ) :
        tmp   = _P3D ( self )
        tmp  -= other
        return tmp
    # POINT - POINT = VECTOR 
    elif isinstance ( other , _P3D ) :
        tmp   = _V3D (  self.x() ,  self.y() ,  self.z() )
        ## tmp  -= other
        tmp  -= _V3D ( other.x() , other.y() , other.z() ) 
        return tmp
    #
    return NotImplemented 

def _p3_mul_ ( self , other ) :
    """
    Scaling of 3D-points 
    
    >>> point  = ...
    >>> result = point1 * 2 
    """
    tmp  = _P3D ( self )
    tmp *= other
    return tmp

def _p3_div_ ( self , other ) :
    """
    Scaling of 3D-points 
    
    >>> point  = ...
    >>> result = point1 / 2 
    """
    tmp  = _P3D ( self )
    tmp /= other
    return tmp
    
def _v3_add_ ( self , other ) :
    """
    Addition  of 3D-vectors
    
    >>> vector1 = ...
    >>> vector2 = ...
    >>> result_vector = vector1 + vector2
    >>> point   =
    >>> result_point  = vector1 + point 
    """
    # VECTOR + VECTOR = VECTOR 
    if   isinstance ( other , _V3D ) :
        tmp   = _V3D ( self )
        tmp  += other
        return tmp
    # VECTOR + POINT  = POINT 
    elif isinstance ( other , _P3D ) : return other + self
    #
    return NotImplemented 

def _v3_sub_ ( self , other ) :
    """
    Subtraction  of 3D-vectors
    
    >>> vector1 = ...
    >>> vector2 = ...
    >>> result_vector = vector1 - vector2
    """
    # VECTOR - VECTOR = VECTOR 
    if   isinstance ( other , _V3D ) :
        tmp   = _V3D ( self )
        tmp  -= other
        return tmp
    #
    return NotImplemented 

def _v3_mul_ ( self , other ) :
    """
    Multiplication  of 3D-vectors
    
    >>> vector1 = ...
    >>> result  = vector1 * 2 
    >>> vector2 = ...
    >>> product = vector1 * vector2
    """
    # VECTOR * VECTOR = NUMBER
    if   isinstance ( other , _V3D ) : return self.Dot ( other )
    # VECTOR * NUMBER = NUMBER 
    elif isinstance ( other , ( float , int , long ) ) :  
        tmp  = _V3D ( self )
        tmp *= other
        return tmp
    #
    return NotImplemented

def _v3_div_ ( self , other ) :
    """
    Scaling of 3D-vectors
    
    >>> vector = ...
    >>> result = vector1 / 2 
    """
    tmp  = _V3D ( self )
    tmp /= other
    return tmp

_P3D . __add__  = _p3_add_
_P3D . __sub__  = _p3_sub_
_P3D . __div__  = _p3_div_
_P3D . __mul__  = _p3_mul_

_V3D . __add__  = _v3_add_
_V3D . __sub__  = _v3_sub_
_V3D . __div__  = _v3_div_
_V3D . __mul__  = _v3_mul_

_P3D . __radd__ = lambda s,o : s+o 
_P3D . __rmul__ = lambda s,o : s*o 
_V3D . __radd__ = lambda s,o : s+o 
_V3D . __rmul__ = lambda s,o : s*o 



def _v4_pow_ ( self , e ) :
    """
    Squared length of the 3D-vector 
    """
    if 2 != e : return NotImplemented
    return self.M2   ()


def _v3_pow_ ( self , e ) :
    """
    Squared length of the 3D-vector 
    """
    if 2 != e : return NotImplemented
    return self.Mag2 ()

_V4D.__pow__ = _v4_pow_
_V3D.__pow__ = _v3_pow_
_P3D.__pow__ = _v3_pow_


## Self-printout of 3D-points and 3D-vectors
def _v3_str_ ( self , fmt = "( %g, %g, %g) ") :
    """
    Self-printout of 3D-points and 3D-vectors

    """
    return fmt % ( self.X() , self.Y( ), self.Z() )

## Self-printout of 4D-vectors
def _v4_str_ ( self , fmt = "[( %g, %g, %g), %g]" ) :
    """
    Self-printout of 4D-vectors

    """
    return fmt % ( self.X() , self.Y( ), self.Z() , self.E() )


if not hasattr ( _P3D , '_new_str_' ) :
    _P3D . _new_str_ = _v3_str_
    _P3D . __str__   = _v3_str_
    _P3D . __repr__  = _v3_str_

if not hasattr ( _V3D , '_new_str_' ) :
    _V3D . _new_str_ = _v3_str_
    _V3D . __str__   = _v3_str_
    _V3D . __repr__  = _v3_str_

if not hasattr ( _V4D , '_new_str_' ) :
    _V4D . _new_str_ = _v4_str_
    _V4D . __str__   = _v4_str_
    _V4D . __repr__  = _v4_str_

# =============================================================================
## Self-printout of line
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _l_str_ ( self ) :
    """
    Self-printout of line: (point, direction)
    
    >>> line = ... 
    >>> print line 
    """
    return "(%s,%s)" % ( self.beginPoint() , self.direction() )

if not hasattr ( cpp.Gaudi.Math.XYZLine , '_new_str_' ) :
    cpp.Gaudi.Math.XYZLine._new_str_ = _l_str_
    cpp.Gaudi.Math.XYZLine.__str__   = _l_str_
    cpp.Gaudi.Math.XYZLine.__repr__  = _l_str_

# ============================================================================
## self-printout of matrices
def _mg_str_ ( self , fmt = ' %+11.4g') :
    """
    Self-printout of matrices
    """
    _rows = self.kRows
    _cols = self.kCols
    _line = ''
    for _irow in range ( 0 , _rows ) :
        _line += ' |'
        for _icol in range ( 0 , _cols ) :
            _line += fmt % self( _irow , _icol )
        _line += ' |'
        if ( _rows - 1 )  != _irow : _line += '\n'
    return _line
#
## self-printout of symmetrical matrices
def _ms_str_ ( self , fmt = ' %+11.4g' , width = 12 ) :
    """
    Self-printout of symetrical matrices
    """
    _rows = self.kRows
    _cols = self.kCols
    _line = ''
    for _irow in range ( 0 , _rows ) :
        _line += ' |'
        for _icol in range ( 0 , _cols  ) :
            if _icol < _irow : _line += width*' '
            else             : _line += fmt % self( _irow , _icol )
        _line += ' |'
        if ( _rows - 1 )  != _irow : _line += '\n'
    return _line

## get the correlation matrix
def _m_corr_ ( self ) :
    """
    Get the correlation matrix

    >>> mtrx = ...
    >>> corr = mtrx.correlations()

    """
    from math import sqrt

    _t = type ( self )
    _c = _t   ()
    _rows = self.kRows
    for i in range ( 0 , _rows ) :
        _dI = sqrt ( self ( i , i ) )
        for j in range ( i + 1 , _rows ) :
            _dJ = sqrt ( self ( j , j ) )
            _c [ i , j ] = self ( i , j ) /  (  _dI * _dJ )
        _c[ i , i ] = 1.0

    return _c


def _m_get_ ( o , i , j ) :

    try :
        return o ( i , j )
    except :
        pass
    
    try :
        return o [ i , j ]
    except :
        pass

    return o [ i ][ j ]
    
    
# =============================================================================
## add some matrix-like object to the matrix
#  @code
#  m = ...
#  o = ...
#  m.add_to  ( o ) 
#  @endcode
def _mg_increment_ ( m , o ) : 
    """ Add some ``matrix-like'' object to the matrix
    >>> m = ...
    >>> o = ...
    >>> m.increment  ( o ) 
    """
    for i in range ( m.kRows ) :
        for j in range ( m.kCols ) :
            m[i,j] = m(i,j) + _m_get_ ( o , i , j )
                    
    return m

# =============================================================================
## add some matrix-like object to the matrix
#  @code
#  m = ...
#  o = ...
#  m.add_to  ( o ) 
#  @endcode
def _ms_increment_ ( m , o ) : 
    """ Add some ``matrix-like'' object to the matrix
    >>> m = ...
    >>> o = ...
    >>> m.increment  ( o ) 
    """
    for i in range ( m.kRows ) :
        for j in range ( i , m.kCols ) :
            m[i,j] = m(i,j) + 0.5 * ( _m_get_ ( o , i , j ) + _m_get_ ( o , j , i ) )  
                    
    return m

# =============================================================================
## construct ``similarity'' with ``vector-like'' object
#  @code
#  m = ...
#  v = ...
#  m.sim ( v )
def  _ms_sim_ ( m , v ) :
    """ construct ``similaroty'' with ``vector-like'' object
    >>> m = ...
    >>> v = ...
    >>> m.sim ( v )
    """
    sim = 0.0
    for i in range(m.kRows) :
        for j in range(m.kCols) :
            sim += v[i]*m(i,j)*v[j] 
    return sim 
            
        
##  decorate the matrix  type 
def deco_matrix ( m  ) :
    if not hasattr ( m , '_new_str_' ) :
        m. _new_str_ = _mg_str_
        m. __repr__  = _mg_str_
        m. __str__   = _mg_str_
    if not hasattr ( m , '_increment_' ) :
        m._increment_   = _mg_increment_
        m.increment     = _mg_increment_
        
    return m

##  decorate the symmetrix matrix  type 
def deco_symmatrix ( m ) :
    
    if not hasattr ( m , 'correlations' ) :
        m.correlations = _m_corr_

    if not hasattr ( m , '_new_str_' ) :
        m. _new_str_ = _ms_str_
        m. __repr__  = _ms_str_
        m. __str__   = _ms_str_

    if not hasattr ( m , '_increment_' ) :
        m._increment_   = _ms_increment_
        m.increment     = _ms_increment_

    if not hasattr ( m , '_sim_' ) :
        m._sim_   = _ms_sim_
        m.sim     = _ms_sim_

    return m


cpp.Gaudi.SymMatrix2x2        = cpp.Gaudi.SymMatrix(2)
cpp.Gaudi.SymMatrix3x3        = cpp.Gaudi.SymMatrix(3)
cpp.Gaudi.SymMatrix4x4        = cpp.Gaudi.SymMatrix(4)
cpp.Gaudi.SymMatrix5x5        = cpp.Gaudi.SymMatrix(5)
cpp.Gaudi.SymMatrix6x6        = cpp.Gaudi.SymMatrix(6)
## LHCb::Particle
cpp.Gaudi.SymMatrix7x7        = cpp.Gaudi.SymMatrix(7)
## Gaudi::Math::ParticleParams
cpp.Gaudi.SymMatrix8x8        = cpp.Gaudi.SymMatrix(8)
## LHCb:TwoProngVertex
cpp.Gaudi.SymMatrix9x9        = cpp.Gaudi.SymMatrix(9)


cpp.Gaudi.Math.SymMatrix2x2   = cpp.Gaudi.SymMatrix2x2
cpp.Gaudi.Math.SymMatrix3x3   = cpp.Gaudi.SymMatrix3x3
cpp.Gaudi.Math.SymMatrix4x4   = cpp.Gaudi.SymMatrix4x4
cpp.Gaudi.Math.SymMatrix5x5   = cpp.Gaudi.SymMatrix5x5
cpp.Gaudi.Math.SymMatrix6x6   = cpp.Gaudi.SymMatrix6x6
cpp.Gaudi.Math.SymMatrix7x7   = cpp.Gaudi.SymMatrix7x7
cpp.Gaudi.Math.SymMatrix8x8   = cpp.Gaudi.SymMatrix8x8
cpp.Gaudi.Math.SymMatrix9x9   = cpp.Gaudi.SymMatrix9x9

#
# specific matrices for 'tracks'
#

cpp.Gaudi.Matrix5x5             = cpp.Gaudi.Matrix(5,5)
cpp.Gaudi.TrackMatrix           = cpp.Gaudi.Matrix5x5
cpp.Gaudi.Math.Matrix5x5        = cpp.Gaudi.Matrix5x5
cpp.Gaudi.Math.TrackMatrix      = cpp.Gaudi.TrackMatrix

cpp.Gaudi.TrackSymMatrix        = cpp.Gaudi.SymMatrix5x5
cpp.Gaudi.Math.TrackSymMatrix   = cpp.Gaudi.TrackSymMatrix

cpp.Gaudi.TrackVector           = cpp.Gaudi.Vector5
cpp.Gaudi.Math.TrackVector      = cpp.Gaudi.TrackVector

#
# matrix from LHCb::Particle
#
cpp.Gaudi.Matrix4x3             = cpp.Gaudi.Matrix(4,3)
cpp.Gaudi.Math.Matrix4x3        = cpp.Gaudi.Matrix4x3



for m in ( cpp.Gaudi.Matrix5x5      ,
           cpp.Gaudi.TrackMatrix    ,
           cpp.Gaudi.Matrix4x3      ) : deco_matrix ( m )

for m in ( cpp.Gaudi.SymMatrix2x2   ,
           cpp.Gaudi.SymMatrix3x3   ,
           cpp.Gaudi.SymMatrix4x4   ,
           cpp.Gaudi.SymMatrix5x5   ,
           cpp.Gaudi.SymMatrix6x6   ,
           cpp.Gaudi.SymMatrix7x7   ,
           cpp.Gaudi.SymMatrix8x8   ,
           cpp.Gaudi.SymMatrix9x9   ,
           cpp.Gaudi.TrackSymMatrix ) : deco_symmatrix ( m ) 

# =============================================================================
## Self-printout of 3D-plane
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _p_str_ ( self ) :
    """
    Self-printout of 3D-plane: (point, normal)
    >>> plane = ...
    >>> print plance 
    """
    return "(%s,%s)" % ( self.ProjectOntoPlane( cpp.Gaudi.XYZPoint()) , self.Normal() )

if not hasattr ( cpp.Gaudi.Plane3D , '_new_str_' ) :
    cpp.Gaudi.Plane3D._new_str_ = _p_str_
    cpp.Gaudi.Plane3D.__str__   = _p_str_
    cpp.Gaudi.Plane3D.__repr__  = _p_str_

# =============================================================================
## self-printout of S-vectors
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-09-12
def _v_str_ ( self , fmt = ' %g' ) :
    """
    Self-printout of SVectors: (...)
    """
    index  = 0
    result = ''
    while index < self.kSize :
        if 0 != index : result += ', '
        result += fmt % self.At( index )
        index  += 1
    return "( " + result + ' )'

for t in  ( cpp.Gaudi.Vector2 ,
            cpp.Gaudi.Vector3 ,
            cpp.Gaudi.Vector4 ,
            cpp.Gaudi.Vector5 ,
            cpp.Gaudi.Vector6 ) :

    ## add len of vectors 
    t . __len__ = lambda s : s.Dim()
    
    if not hasattr ( t , '_new_str_' ) :
        t._new_str_ = _v_str_
        t.__str__   = _v_str_
        t.__repr__  = _v_str_

for t in ( cpp.Gaudi.Math.ValueWithError         ,
           cpp.Gaudi.Math.Point3DWithError       ,
           cpp.Gaudi.Math.Vector3DWithError      ,
           cpp.Gaudi.Math.LorentzVectorWithError ,
           cpp.Gaudi.Math.SVector2WithError      ,
           cpp.Gaudi.Math.SVector3WithError      ,
           cpp.Gaudi.Math.SVector4WithError      ,
           cpp.Gaudi.Math.SVector5WithError      ,
           cpp.Gaudi.Math.SVector6WithError      ,
           cpp.Gaudi.Math.SVector8WithError      ) :
    if not hasattr ( t , '_new_str_' ) :
        t._new_str_ = t.toString
        t.__str__   = t.toString
        t.__repr__  = t.toString


cpp.Gaudi.Math.SVector2WithError  . __len__ = lambda s : 2 
cpp.Gaudi.Math.SVector3WithError  . __len__ = lambda s : 3 
cpp.Gaudi.Math.SVector4WithError  . __len__ = lambda s : 4 
cpp.Gaudi.Math.SVector5WithError  . __len__ = lambda s : 5 
cpp.Gaudi.Math.SVector6WithError  . __len__ = lambda s : 6 
cpp.Gaudi.Math.SVector8WithError  . __len__ = lambda s : 8 

## get the eigenvalues for symmetric matrices :
def _eigen_1_ ( self , sorted = True ) :
    """

    >>> mtrx = ...
    >>> values = mtrx.eigenValues ( sorted = True )

    """
    return cpp.Gaudi.Math.EigenSystems.eigenValues ( self , sorted )


## get the eigevectors for symmetric matrices :
def _eigen_2_ ( self , sorted = True ) :
    """

    >>> mtrx = ...
    >>> values, vectors = mtrx.eigenVectors( sorted = True )

    """
    if   2 == self.kCols :
        _values  = cpp.Gaudi.Vector2  ()
        _vectors = cpp.Gaudi.Vectors2 ()
    elif 3 == self.kCols :
        _values  = cpp.Gaudi.Vector3  ()
        _vectors = cpp.Gaudi.Vectors3 ()
    elif 4 == self.kCols :
        _values  = cpp.Gaudi.Vector4  ()
        _vectors = cpp.Gaudi.Vectors4 ()
    else :
        raise AttributeError, "Not implemented for dimention: %s" % self.kCols

    st = cpp.Gaudi.Math.EigenSystems.eigenVectors ( self , _values , _vectors , sorted )
    if st.isFailure () :
        print 'EigenVectors: Failure from EigenSystems' , st

    return ( _values , _vectors )


_eigen_1_ .__doc__ += '\n' +  cpp.Gaudi.Math.EigenSystems.eigenValues  . __doc__
_eigen_2_ .__doc__ += '\n' +  cpp.Gaudi.Math.EigenSystems.eigenVectors . __doc__

for m in ( cpp.Gaudi.SymMatrix2x2   ,
           cpp.Gaudi.SymMatrix3x3   ,
           cpp.Gaudi.SymMatrix4x4   ) :
    if not hasattr ( m , 'eigenValues'  ) : m.eigenValues  = _eigen_1_
    if not hasattr ( m , 'eigenVectors' ) : m.eigenVectors = _eigen_2_


# =============================================================================
## self-printout of Gaudi::Math::ParticleParams
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2010-05-26
def _pp_str_ ( self ) :
    """
    Self-printout of ParticleParams
    """
    pos = self.position    ()
    mom = self.momentum    ()
    len = self.decayLength ()
    return " ( %s/%s/%s ) " % ( pos, mom , len )

if not hasattr ( cpp.Gaudi.Math.ParticleParams , '_new_str_' ) :
    cpp.Gaudi.Math.ParticleParams._new_str_ = _pp_str_
    cpp.Gaudi.Math.ParticleParams.__str__   = _pp_str_
    cpp.Gaudi.Math.ParticleParams.__repr__  = _pp_str_

# =============================================================================
## various decorators for GeomFun.h
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
if not hasattr ( cpp.Gaudi.Math , 'XYZGeomFun' ) :
    cpp.Gaudi.Math.XYZGeomFun = cpp.Gaudi.Math.GF(
        cpp.Gaudi.XYZPoint  ,
        cpp.Gaudi.XYZLine   ,
        cpp.Gaudi.Plane3D
        )
if not hasattr ( cpp.Gaudi , 'XYZGeomFun' ) :
    cpp.Gaudi.XYZGeomFun = cpp.Gaudi.Math.XYZGeomFun

_GeomFun = cpp.Gaudi.Math.XYZGeomFun

# =============================================================================
## intersection of line and plane
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_line_and_plane_ ( line , plane ) :
    """
    Find the intersection of line and plane

    >>> line  = ...
    >>> plane = ...

    >>> ok, point, mu = line.intersect ( plane )

    The return value is a tuple:
    - the point
    - the parameter along the line
    - the flag (true if intersection exists)

    """
    _point = cpp.Gaudi.XYZPoint(0,0,-1.e+10)
    _mu    = ROOT.Double(-1.e+10)
    _flag  = _GeomFun.intersection ( line   ,
                                     plane  ,
                                     _point ,
                                     _mu    )
    if _flag : _flag = True
    else     : _flag = False
    return (_point,_mu,_flag)

_intersect_line_and_plane_ . __doc__ += '\n' + _GeomFun.intersection . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'intersect' ) :
    cpp.Gaudi.XYZLine.intersect = _intersect_line_and_plane_

# =============================================================================
## intersect two planes
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_two_planes_ ( plane , plane1 ) :
    """
    Find the intersection line for two planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> line, flag = plane.line(plane1)

    Return value is a tuple:

    - the intersection line
    - the flag (true if intersection exists)

    """
    _line = cpp.Gaudi.XYZLine()
    _flag = _GeomFun.intersection ( plane , plane1 , _line )
    if _flag : _flag = True
    else     : _flag = False
    return (_line,_flag)

_intersect_two_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__

if not hasattr ( cpp.Gaudi.Plane3D , 'line' ) :
    cpp.Gaudi.Plane3D.line = _intersect_two_planes_


# =============================================================================
## intersect three planes
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_three_planes_ ( plane , plane1 , plane2 ) :
    """
    Find the intersection point for three planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> plane3 = ...
    >>> point, flag = plane.point(plane1,plane2)

    Return value is a tuple:

    - the intersection point
    - the flag (true if intersection exists)

    """
    _point = cpp.Gaudi.XYZPoint(0,0,-1.e+10)
    _flag = _GeomFun.intersection ( plane , plane1 , plane2 , _point )
    if _flag : _flag = True
    else     : _flag = False
    return (_point,_flag)


_intersect_three_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__

if not hasattr ( cpp.Gaudi.Plane3D , 'point' ) :
    cpp.Gaudi.Plane3D.point = _intersect_three_planes_


# =============================================================================
## intersect the planes
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _intersect_the_planes_ ( plane , plane1 , plane2 = None ) :
    """
    Find the intersection line/point for two or three planes:

    >>> plane  = ...
    >>> plane1 = ...
    >>> line, flag = plane.intersect(plane1)

    >>> plane  = ...
    >>> plane1 = ...
    >>> plane2 = ...
    >>> point, flag = plane.intersect(plane1,plane2)

    Return value is a tuple:

    - the intersection line/point
    - the flag (true if intersection exists)

    """
    if not plane2 : return _intersect_two_planes_ ( plane , plane1 )
    return _intersect_three_planes_ ( plane , plane1 , plane2 )

_intersect_the_planes_ . __doc__ += '\n' + _GeomFun.intersection . __doc__

if not hasattr ( cpp.Gaudi.Plane3D , 'intersect' ) :
    cpp.Gaudi.Plane3D.intersect = _intersect_the_planes_


# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _imp_par_1_ ( line , point ) :
    """
    Calculate the impact parameter of the line and the point

    >>> line  = ...
    >>> point = ...
    >>> ip = line.impactParameter ( point )

    """
    return _GeomFun.impactParameter ( point , line )

_imp_par_1_ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'impactParameter' ) :
    cpp.Gaudi.XYZLine.impactParameter = _imp_par_1_
if not hasattr ( cpp.Gaudi.XYZLine , 'ip'              ) :
    cpp.Gaudi.XYZLine.ip              = _imp_par_1_

# =============================================================================
## calculate the impact parameter of the line & point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _imp_par_2_ ( point , line ) :
    """
    Calculate the impact parameter of the line and the point

    >>> point = ...
    >>> line  = ...
    >>> ip = point.impactParameter ( line )

    """
    return _GeomFun.impactParameter ( point , line )


_imp_par_2_ . __doc__ += '\n' + _GeomFun.impactParameter . __doc__

if not hasattr ( cpp.Gaudi.XYZPoint , 'impactParameter' ) :
    cpp.Gaudi.XYZPoint.impactParameter = _imp_par_2_
if not hasattr ( cpp.Gaudi.XYZPoint , 'ip'              ) :
    cpp.Gaudi.XYZPoint.ip              = _imp_par_2_

# =============================================================================
## distance between two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _distance_between_two_lines_ ( line , line1 ) :
    """
    Find the distance netween two lines :

    >>> line  = ...
    >>> line1 = ...
    >>> dist = line.distance ( line1 )

    """
    return _GeomFun.distance ( line , line1 )

_distance_between_two_lines_ . __doc__ += '\n' + _GeomFun.distance. __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'distance' ) :
    cpp.Gaudi.XYZLine.distance = _distance_between_two_lines_


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_points_ ( line , line1 ) :
    """
    Calculate the two closest points between two lines

    >>> line1 = ...
    >>> line2 = ...
    >>> point1 , point2 , flag = line1.closestPoints ( line2 )

    The return values is a tuple:
    - the point onthe fist line
    - the point on the second line
    - the flag (true is everything OK)

    """
    _point1 = cpp.Gaudi.XYZPoint(0,0,-1.e+10)
    _point2 = cpp.Gaudi.XYZPoint(0,0,-1.e+11)
    _flag   = _GeomFun.closestPoints ( line , line1 , _point1 , _point2 )
    if    _flag : _flag = True
    else        : _flag = False
    return (_point1,_point2,_flag)


_closest_points_ . __doc__ += '\n' + _GeomFun.closestPoints . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'closestPoints' ) :
   cpp.Gaudi.XYZLine.closestPoints = _closest_points_


# =============================================================================
## find the closest points for two lines
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_params_ ( line , line1 ) :
    """
    Calculate the parameters for two closest points between two lines

    >>> line1 = ...
    >>> line2 = ...
    >>> mu1 , mu2 , flag = line1.closestPointParams ( line2 )

    The return values is a tuple:
    - the 'mu-parameter of closest point along the first  line
    - the 'mu-parameter of closest point along the second line
    - the flag (true is everything OK)

    """
    _mu1    = ROOT.Double(-1.e+10)
    _mu2    = ROOT.Double(-1.e+11)
    _flag   = _GeomFun.closestPointParams ( line , line1 , _mu1 , _mu2 )
    if    _flag : _flag = True
    else        : _flag = False
    return (_mu1,_mu2,_flag)


_closest_point_params_ . __doc__ += '\n' + _GeomFun.closestPointParams . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'closestPointParams' ) :
    cpp.Gaudi.XYZLine.closestPointParams = _closest_point_params_

# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_1_ ( line , point ) :
    """
    Find the point on line closest to the given point

    >>> line  = ...
    >>> point = ...
    >>> ClosestPoint  = line.closestPoint ( point )

    """
    return _GeomFun.closestPoint ( point , line )

_closest_point_1_ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'closestPoint' ) :
    cpp.Gaudi.XYZLine.closestPoint = _closest_point_1_

# =============================================================================
## find the point on ilne closest to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_2_ ( point , line ) :
    """
    Find the point on line closest to the given point

    >>> point = ...
    >>> line  = ...
    >>> ClosestPoint  = point.closestPoint ( line )

    """
    return _GeomFun.closestPoint ( point , line )

_closest_point_2_ . __doc__ += '\n' + _GeomFun.closestPoint . __doc__

if not hasattr ( cpp.Gaudi.XYZPoint , 'closestPoint' ) :
    cpp.Gaudi.XYZPoint.closestPoint = _closest_point_2_


# =============================================================================
## find the parameter along the line to the closest point to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def __closest_point_param_1__ ( line , point ) :
    """
    Find the parameter along the line to the closest point

    >>> line  = ...
    >>> point = ...
    >>> mu = line.closestPointParam ( point )

    """
    return _GeomFun.closestPointParam ( point , line )


__closest_point_param_1__ . __doc__ += '\n' + _GeomFun.closestPointParam .__doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'closestPointParam' ) :
    cpp.Gaudi.XYZLine.closestPointParam = __closest_point_param_1__


# =============================================================================
## find the parameter along the line to the closest point to the given point
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _closest_point_param_2_ ( point , line ) :
    """
    Find the parameter along the line to the closest point

    >>> point = ...
    >>> line  = ...
    >>> mu = point.closestPointParam ( line )

    """
    return _GeomFun.closestPointParam ( point , line )

_closest_point_param_2_ . __doc__ += '\n' + _GeomFun.closestPointParam .__doc__

if not hasattr ( cpp.Gaudi.XYZPoint , 'closestPointParam' ) :
    cpp.Gaudi.XYZPoint.closestPointParam = _closest_point_param_2_


# =============================================================================
## check if two lines are parallel
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-10-22
def _parallel_lines_ ( line , line1 ) :
    """
    Check if two lines are parallel:

    >>> line  = ...
    >>> line1 = ...
    >>> par   = line.parallel ( line1 )
    """
    _flag = _GeomFun.parallel ( line , line1 )
    if not _flag : return False
    return True

_parallel_lines_ . __doc__ += '\n' + _GeomFun.parallel . __doc__

if not hasattr ( cpp.Gaudi.XYZLine , 'parallel' ) :
    cpp.Gaudi.XYZLine.parallel = _parallel_lines_

# =============================================================================
## helper function/wrapper for Gaudi::Math:FitMass
#  @see Gaudi::Math::FitMass
#  @see Gaudi::Math::FitMass::fit
#  @see Gaudi::Math::ParticleParams
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @param  particle (INPUT) "particle"
#  @param  mass     (INPUT) the nominal mass
#  @return the tuple (fitter,chi2)
#  @date 2009-10-22
def _fit_mass_ ( particle , mass )  :
    """
    Helper function/wrapper for Gaudi::Math::MassFit

    >>> particle  =  ...   ## get Gaudi::Math::ParticleParams object
    >>> mass = 5.279 * GeV ## get the mass
    >>> result, chi2 = particle.fitMass ( particle , mass )
    >>> print 'result/chi2:', result, chi2

    """
    _fitted = cpp.Gaudi.Math.ParticleParams()
    _chi2   = cpp.Gaudi.Math.FitMass.fit ( particle  ,
                                           mass      ,
                                       _fitted   )
    return (_fitted,_chi2)

cpp.Gaudi.Math.ParticleParams.fitMass = _fit_mass_


# =============================================================================
## decorate some basic vectors
for t in ( 'int'                ,
           ##'long'               ,
           ##'long long'          ,
           ##'unsigned int'       ,
           ##'unsigned long'      ,
           ##'unsigned long long' ,
           ##'float'
           'double'               ) :
    v = std.vector( t )
    v.asList   = lambda s :       [ i for i in s ]   ## convert vector into list
    v.toList   = v.asList
    v.__repr__ = lambda s : str ( [ i for i in s ] ) ## print it !
    v.__str__  = lambda s : str ( [ i for i in s ] ) ## print it !

# =============================================================================
LHCb  = cpp.LHCb
# =============================================================================
cpp.LHCb.Math.lomont_compare_double  = cpp.Gaudi.Math.lomont_compare_double
cpp.LHCb.Math.lomont_compare_float   = cpp.Gaudi.Math.lomont_compare_float 
cpp.Gaudi.Math.round                 = cpp.LHCb.Math.round
cpp.Gaudi.Math.equal_to_int          = cpp.LHCb.Math.equal_to_int
cpp.Gaudi.Math.equal_to_uint         = cpp.LHCb.Math.equal_to_uint
cpp.Gaudi.Math.isint                 = cpp.LHCb.Math.isint
cpp.Gaudi.Math.islong                = cpp.LHCb.Math.islong
cpp.Gaudi.Math.knuth_equal_to_double = cpp.LHCb.Math.knuth_equal_to_double
cpp.Gaudi.Math.round_N               = cpp.LHCb.Math.round_N
cpp.Gaudi.Math.frexp10               = cpp.LHCb.Math.frexp10


if not hasattr ( cpp.Gaudi.Math.Splines , 'DATA'    ) or \
   not hasattr ( cpp.Gaudi.Math.Splines , 'PAIR'    ) or \
   not hasattr ( cpp.Gaudi.Math.Splines , 'DATAERR' ) :

    _pair_ = std.pair('double','double')
    cpp.Gaudi.Math.Splines.DATA    = std.vector ( _pair_ )
    cpp.Gaudi.Math.Splines.PAIR    = std.pair   ( 'double',cpp.Gaudi.Math.ValueWithError )
    cpp.Gaudi.Math.Splines.DATAERR = std.vector ( cpp.Gaudi.Math.Splines.PAIR )

# =============================================================================
## make some simple interpolation for set of data points
#  @see Gaudi::Math::Splines::interpolate
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_interp_ ( self , x ) :
    """
    Simple interpolation for set of data points.
    The vector of pairs is interpreted as vector of (x,y) pairs

    >>> vct_of_pairs = ...

    >>> result = vct_of_pairs.interpolate ( x )

    """
    if hasattr ( x , 'value' ) : x = x.value()
    #
    ## make the interpolation
    #
    return cpp.Gaudi.Math.Splines.interpolate ( self , x )

_v_interp_ . __doc__  += '\n' + cpp.Gaudi.Math.Splines.interpolate. __doc__

cpp.Gaudi.Math.Splines.DATA    .interpolate = _v_interp_
cpp.Gaudi.Math.Splines.DATAERR .interpolate = _v_interp_

# =============================================================================
## build a spline to approximate the data poinst
#  @see Gaudi::Math::Spline
#  @see GaudiMath::Spline
#  @see GaudiMath::SplineBase
#  @see Genfun::GaudiMathImplementation::SplineBase
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_spline_ ( self                                      ,
                 type  = cpp.GaudiMath.Interpolation.Akima ,
                 null  = False                             ,
                 scale = 1                                 ,
                 shift = 0                                 ) :
    """
    Create spline object for the data vector

    >>> vdata  = ...
    >>> spline = vdata.spline ()

    >>> value = spline ( 10 )
    """
    return cpp.Gaudi.Math.Spline ( self , type , null , scale , shift )

# =============================================================================
## build a spline to approximate the data poinst
#  @see Gaudi::Math::SplineErrors
#  @see GaudiMath::Spline
#  @see GaudiMath::SplineBase
#  @see Genfun::GaudiMathImplementation::SplineBase
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2013-03-17
def _v_spline_err_ ( self                                      ,
                     type  = cpp.GaudiMath.Interpolation.Akima ,
                     null  = False                             ,
                     scale = 1                                 ,
                     shift = 0                                 ) :
    """
    Create spline object for the data vector

    >>> vdata  = ...
    >>> spline = vdata.splineErr()

    >>> value = spline ( 10 )
    """
    return cpp.Gaudi.Math.SplineErrors ( self , type , null , scale , shift )

_v_spline_     . __doc__ += '\n' + cpp.Gaudi.Math.Spline       .__init__ .__doc__
_v_spline_err_ . __doc__ += '\n' + cpp.Gaudi.Math.SplineErrors .__init__ .__doc__

cpp.Gaudi.Math.Splines.DATA    .spline     = _v_spline_
cpp.Gaudi.Math.Splines.DATAERR .spline     = _v_spline_
cpp.Gaudi.Math.Splines.DATAERR .splineErr  = _v_spline_err_

# =============================================================================
## get values from the DATAERR
def _v_get_values_ ( self ) :
    """
    Get values from vector

    >>> vdataerr = ...
    >>> vvalues   = vdataerr.getValues()

    """
    return cpp.Gaudi.Math.Spline.getValues ( self )


## get values from the DATAERR
def _v_get_errors_ ( self ) :
    """
    Get errors from vector

    >>> vdataerr = ...
    >>> verrors   = vdataerr.getErrors()

    """
    return cpp.Gaudi.Math.Spline.getErrors ( self )

_v_get_values_ . __doc__  += '\n' + cpp.Gaudi.Math.Splines.getValues. __doc__
_v_get_errors_ . __doc__  += '\n' + cpp.Gaudi.Math.Splines.getErrors. __doc__

cpp.Gaudi.Math.Splines.DATAERR .getValues = _v_get_values_
cpp.Gaudi.Math.Splines.DATAERR .getErrors = _v_get_errors_

cpp.Gaudi.Math.Splines.DATA    . __repr__ = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
cpp.Gaudi.Math.Splines.DATAERR . __repr__ = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
cpp.Gaudi.Math.Splines.DATA    . __str__  = lambda s : str ( [ ( i.first , i.second ) for i in s ] )
cpp.Gaudi.Math.Splines.DATAERR . __str__  = lambda s : str ( [ ( i.first , i.second ) for i in s ] )

if not hasattr ( cpp.Gaudi.Math.Spline       , 'DATA'    ) :
    cpp.Gaudi.Math.Spline.DATA          = Gaudi.Math.Splines.DATA
if not hasattr ( cpp.Gaudi.Math.Spline       , 'DATAERR' ) :
    cpp.Gaudi.Math.Spline.DATAERR       = Gaudi.Math.Splines.DATAERR
if not hasattr ( cpp.Gaudi.Math.SplineErrors , 'DATAERR' ) :
    cpp.Gaudi.Math.SplineErrors.DATAERR = Gaudi.Math.Splines.DATAERR


## self-printout of TMaxtrix 
def _tmg_str_ ( self , fmt = ' %+11.4g') :
    """
    Self-printout of TMatrix
    """
    _rows = self.GetNrows()
    _cols = self.GetNcols()
    _line = ''
    for _irow in range ( 0 , _rows ) :
        _line += ' |'
        for _icol in range ( 0 , _cols ) :
            _line += fmt % self( _irow , _icol )
        _line += ' |'
        if ( _rows - 1 )  != _irow : _line += '\n'
    return _line


ROOT.TMatrix.__repr__  = _tmg_str_
ROOT.TMatrix.__str__   = _tmg_str_

# =============================================================================
VE            = cpp.Gaudi.Math.ValueWithError
SE            = cpp.StatEntity 
WSE           = cpp.Gaudi.Math.WStatEntity 
# =============================================================================
# temporary trick, to be removed 
# =============================================================================

SE .__repr__ = lambda s : 'Stat: '+ s.toString()
SE .__str__  = lambda s : 'Stat: '+ s.toString()

# =============================================================================
# minor decoration for StatEntity 
# ============================================================================= 
if not hasattr ( SE , '_orig_sum'  ) : 
    _orig_sum    = SE.sum
    SE._orig_sum = _orig_sum
    
if not hasattr ( SE , '_orig_mean' ) : 
    _orig_mean    = SE.mean
    SE._orig_mean = _orig_mean
    
SE. sum     = lambda s : VE ( s._orig_sum  () , s.sum2()       )
SE. minmax  = lambda s :    ( s.flagMin()     , s.flagMax()    ) 
SE. mean    = lambda s : VE ( s._orig_mean () , s.meanErr()**2 )

# =============================================================================
# minor decoration for WStatEntity 
# ============================================================================= 
if not hasattr ( WSE , '_orig_sum'  ) : 
    _orig_sum     = WSE.sum
    WSE._orig_sum = _orig_sum

if not hasattr ( WSE , '_orig_mean' ) : 
    _orig_mean_wse = WSE.mean
    WSE._orig_mean = _orig_mean_wse
    
WSE. sum     = lambda s : VE ( s._orig_sum  () , s.sum2()       )
WSE. mean    = lambda s : VE ( s._orig_mean () , s.meanErr()**2 )
WSE. minmax  = lambda s :            s.values  ().minmax() 
WSE.__repr__ = lambda s : 'WStat: '+ s.toString()
WSE.__str__  = lambda s : 'WStat: '+ s.toString()


# =============================================================================
## get the B/S estimate from the formula 
#  \f$ \sigma  = \fras{1}{S}\sqrt{1+\frac{B}{S}}\f$
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-15
def _b2s_ ( s )  :
    """
    Get B/S estimate from the equation:
    
       error(S) = 1/sqrt(S) * sqrt ( 1 + B/S)

    >>> v = ...
    >>> b2s = v.b2s() ## get B/S estimate
    
    """
    #
    c2 = s.cov2  ()
    #
    if s.value() <= 0  or c2 <= 0 : return VE(-1,0) 
    #
    return c2/s - 1 

# =============================================================================
## get the precision with some  error estimation 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-15
def _prec2_ ( s )  :
    """
    Get precision with ``some'' error estimate 

    >>> v = ...
    >>> p = v.prec () 
    
    """
    if not hasattr ( s , 'value' ) :
        return _prec_ ( VE ( s , 0 ) )
    #
    c =       s.error ()
    #
    if     c <  0 or s.value() == 0  : return VE(-1,0)
    elif   c == 0                    : return VE( 0,0)
    #
    return c/abs(s) 


VE . b2s        = _b2s_
VE . prec       = _prec2_
VE . precision  = _prec2_

# =============================================================================
## add something to std::vector 
def _add_to ( vct , arg1 , *args ) :
    ##
    if hasattr ( arg1 , '__iter__' ) :
        for a in arg1 : vct.push_back ( a ) 
    else : vct.push_back ( arg1 ) 
    #
    for a in args : _add_to ( vct , a )
        
# =============================================================================
## construct std::vector<double> from the arguments
def doubles ( arg1 , *args ) :
    """
    Construct the std::vector<double> from the arguments
    
    >>> v1 = doubles ( 1.01 )
    >>> v2 = doubles ( 1.01 , 1.02 , 1.03  )
    >>> v3 = doubles ( [ 1.01 , 1.02 , 1.03 ] )
    
    """
    ## create new vector 
    VT  = std.vector('double')
    vct = VT()
    ## add arguments to the vector 
    _add_to ( vct , arg1 , *args )
    ## 
    return vct

# =============================================================================
## construct std::vector<ints> from the arguments
def ints ( arg1 , *args ) :
    """
    Construct the std::vector<int> from the arguments
    
    >>> v1 = ints ( 1 )
    >>> v2 = ints ( 1 , 1 , 1  )
    >>> v3 = ints ( [ 1 , 2 , 3 ] )
    
    """
    ## create new vector 
    VT  = std.vector('int')
    vct = VT()
    ## add arguments to the vector 
    _add_to ( vct , arg1 , *args )
    ## 
    return vct

# =============================================================================
## construct std::vector<unsigned int> from the arguments
def uints ( arg1 , *args ) :
    """
    Construct the std::vector<unsigned int> from the arguments
    
    >>> v1 = uints ( 1 )
    >>> v2 = uints ( 1 , 1 , 1  )
    >>> v3 = uints ( [ 1 , 2 , 3 ] )
    
    """
    ## create new vector 
    VT  = std.vector('unsigned int')
    vct = VT()
    ## add arguments to the vector 
    _add_to ( vct , arg1 , *args )
    ## 
    return vct

# =============================================================================
## construct std::vector<long> from the arguments
def longs ( arg1 , *args ) :
    """
    Construct the std::vector<long> from the arguments
    
    >>> v1 = longs ( 1 )
    >>> v2 = longs ( 1 , 1 , 1  )
    >>> v3 = longs ( [ 1 , 2 , 3 ] )
    
    """
    ## create new vector 
    VT  = std.vector('long')
    vct = VT()
    ## add arguments to the vector 
    _add_to ( vct , arg1 , *args )
    ## 
    return vct

# =============================================================================
## construct std::vector<unsigned long> from the arguments
def ulongs ( arg1 , *args ) :
    """
    Construct the std::vector<unsigned long> from the arguments
    
    >>> v1 = ulongs ( 1 )
    >>> v2 = ulongs ( 1 , 1 , 1  )
    >>> v3 = ulongs ( [ 1 , 2 , 3 ] )
    
    """
    ## create new vector 
    VT  = std.vector('unsigned long')
    vct = VT()
    ## add arguments to the vector 
    _add_to ( vct , arg1 , *args )
    ## 
    return vct

SPD = std.pair('double','double')
SPD.asTuple  = lambda s : (s.first,s.second)
SPD.__str__  = lambda s : str( (s.first,s.second) )
SPD.__repr__ = SPD.__str__

VE = cpp.Gaudi.Math.ValueWithError

_is_equal_ = cpp.LHCb.Math.Equal_To    ( 'double' )()
_is_zero_  = cpp.LHCb.Math.Zero        ( 'double' )()
_is_le_    = cpp.LHCb.Math.LessOrEqual ( 'double' )()

# =============================================================================
## Comparison of ValueWithError object with other objects
#  @attention it is comparison by value only, errors are ignored 
def _ve_lt_ ( self , other ) :
    """Comparison of ValueWithError object with other objects
    >>> a = VE( ... )
    >>> print a < b 
    Attention: comparison by value only!
    """
    return float(self) < float(other)

# =============================================================================
## Comparison of ValueWithError object with other objects
#  @attention it is comparison by value only, errors are ignored 
def _ve_le_ ( self , other ) :
    """Comparison of ValueWithError object with other objects
    >>> a = VE( ... )
    >>> print a <= b 
    Attention: comparison by value only!
    """
    return _is_le_ ( float(self) , float(other) )

# =============================================================================
## Comparison of ValueWithError object with other objects
#  @attention it is comparison by value only, errors are ignored 
def _ve_gt_ ( self , other ) :
    """Comparison of ValueWithError object with other objects
    >>> a = VE( ... )
    >>> print a > b 
    Attention: comparison by value only!
    """
    return float(self) > float(other)

# =============================================================================
## Comparison of ValueWithError object with other objects
#  @attention it is comparison by value only, errors are ignored 
def _ve_ge_ ( self , other ) :
    """Comparison of ValueWithError object with other objects
    >>> a = VE( ... )
    >>> print a >= b 
    Attention: comparison by value only!
    """
    return _is_le_ ( float(other) , float(self) )
    
VE.__lt__ = _ve_lt_ 
VE.__le__ = _ve_le_ 
VE.__gt__ = _ve_gt_ 
VE.__ge__ = _ve_ge_ 

# =============================================================================
## Equality for ValueWithError objects
def _ve_eq_ ( self , other ) :
    """Equality for ValueWithError objects
    >>> a = VE( ... )
    >>> b = VE( ... )
    >>> print a == b 
    """
    if isinstance ( other , VE ) :
        v1 = self .value()
        v2 = other.value()
        return _is_equal_ ( v1 , v2 ) and _is_equal_ ( self.cov2() , other.cov2() )
    elif _is_zero_ ( self.cov2() )  :
        return _is_equal_ ( float ( self ) ,  float ( other )  ) 
    else :
        raise NotImplementedError,' Equality for %s and  %s is not implemented' % ( self, other) 

# =============================================================================
## inequality for ValueWithError objects
def _ve_ne_ ( self , other ) :
    """Inequality for ValueWithError objects
    >>> a = VE( ... )
    >>> b = VE( ... )
    >>> print a != b 
    """
    try: 
        return not self == other
    except NotImplemented,s :
        raise NotImplementedError,' Inequality for %s and  %s is not implemented' % ( self, other) 

VE . __eq__ = _ve_eq_
VE . __ne__ = _ve_ne_

# =============================================================================
if '__main__' == __name__ :

    ## make printout of the own documentations
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

    _v = [ l for l in dir(Gaudi     ) if 0 != l.find('__') ]
    print ' dir(Gaudi)      : '
    _v.sort()
    for v in _v : print v
    _v = [ l for l in dir(Gaudi.Math) if 0 != l.find('__') ]
    print ' dir(Gaudi.Math) : '
    _v.sort()
    for v in _v : print v


# =============================================================================
# The  END
# =============================================================================
