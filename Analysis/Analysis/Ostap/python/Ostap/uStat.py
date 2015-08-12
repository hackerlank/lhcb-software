#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ============================================================================
# $Id$
# ============================================================================
# @file uStat.py
#
# Helper module to get ``U-statistics'' useful for ``Goodnes-Of-Fit'' tests
#
# This is a simple translation of
#   the original C++ lines written by Greig Cowan into python
#
# Usage is fairly trivial:
#
#  @code
# 
#   >>> pdf  = ...               ## pdf
#   >>> data = ...               ## dataset
#   >>> pdf.fitTo( data , ... )  ## fit it!
#
#   >>> import Ostap.uStat as uStat
#
#   >>> r,histo = uStat.uPlot ( pdf , data ) 
#   >>> print r                  ## print fit results
#   >>> histo.Draw()             ## plot the results  
#
#  @endcode
#
# @author Vanya Belyaev Ivan.Belyaev@cern.ch
# @date 2011-09-21
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
#
# ============================================================================
""" ``U-statistics'' useful for ``Goodness-Of-Fit'' tests

This is a simple translation of
  the original C++ lines written by Greig Cowan into Python

Usage is fairly trivial:

   >>> pdf  = ...               ## pdf
   >>> data = ...               ## dataset
   >>> pdf.fitTo( data , ... )  ## fit it!

   >>> import Ostap.uStat as uStat

   >>> r,histo = uStat.uPlot ( pdf , data ) 
   >>> print r                  ## print fit results
   >>> histo.Draw()             ## plot the results  
   
"""
# ============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@cern.ch"
__date__    = "2010-09-21"
__version__ = "$Revision$"
# ============================================================================
__all__     = (
    "uPlot" ,  ## make  plot of U-statistics 
    "uDist" ,  ## calculate  U-statistics 
    "uCalc" ,  ## calclulate the distance between two data points 
    )
# ============================================================================
import ROOT, math
import Ostap.PyRoUts 
# =============================================================================
# logging 
# =============================================================================
from   AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.uStat' )
else                       : logger = getLogger ( __name__      )
# =============================================================================
## calculate the distance between two data points 
#  @author Vanya Belyaev Ivan.Belyaev@cern.ch
#  @date 2011-09-21
def uDist ( x , y ) :
    """
    Calculate the distance between two data points 
    """
    
    ix = x.createIterator()
    iy = y.createIterator()
    
    dist = 0.0
    
    xv = ix.Next()
    yv = iy.Next()
    while xv and yv :

        if not hasattr ( xv  , 'getVal' ) : break 
        if not hasattr ( yv  , 'getVal' ) : break
        
        d     = xv.getVal()-yv.getVal()
        dist += d*d
        xv = ix.Next()
        yv = iy.Next()

    del ix
    del iy

    return math.sqrt( dist )

# =============================================================================
##  calculate U-statistics
#   @param pdf    (input) PDF
#   @param args   (input) arguments/variables
#   @param data   (input) dataset 
#   @param histo  (input) the histogram to be filled 
#   @author Vanya Belyaev Ivan.Belyaev@cern.ch
#   @see Analysis::UStat
#   @see Analysis::UStat::calculate
#   @date 2011-09-21
def uCalc ( pdf            ,
            args           , 
            data           ,
            histo          ,
            silent = False )  :
    """
    Calculate U-statistics 
    """
    import sys
    
    from Ostap.PyRoUts import cpp 
    tStat = ROOT.Double(-1)
    sc    = cpp.Analysis.UStat.calculate ( pdf   ,
                                           data  ,
                                           histo ,
                                           tStat ,
                                           args  )
    return histo, tStat 
    
    numEntries = data.numEntries ()
    dim        = args.getSize    ()
    data_clone = data.Clone      ()

    from Ostap.progress_bar import progress_bar 
    for i in progress_bar ( xrange ( numEntries ) ) : 

        event_x = data_clone.get ( i ) 
        event_i = event_x.selectCommon ( args )
        
        ## fill args and evaluate PDF 
        for a in args : a.setVal ( event_i.getRealValue ( a.GetName () ) )        
        pdfValue = pdf.getVal ( args )
        
        small_v = 1.e+100 
        small_j = 0
        for j in xrange ( 0 , numEntries ) :
            
            if j == i  : continue

            event_y = data.get( j )
            event_j = event_y.selectCommon ( args ) 

            dist = uDist ( event_i , event_j )
            
            if 0 == j or dist < small_v :
                small_v = dist
                small_j = j 
                
        value = 0 
        if   1 == dim :
            value  = small_v
            value *= numEntries * pdfValue 
        elif 2 == dim :
            value  = small_v**2
            value *= numEntries * pdfValue
            value *= math.pi 
        else :
            print ' Not-implemented (yet)', dim
            continue 

        value = math.exp ( -1 * value )

        histo.Fill ( value ) 

    del data_clone 
    return histo

# =============================================================================
##  make the plot of U-statistics
#
#   @code
#
#    >>> pdf  = ...               ## pdf
#    >>> data = ...               ## dataset
#    >>> pdf.fitTo( data , ... )  ## fit it!
#    >>> vars = ...               ## get variables
#    
#    >>> import AnalysisPython.uStat as uStat
#    
#    >>> r,histo = uStat.uPlot ( pdf , data ) 
#    >>> print r                  ## print fit results
#    >>> histo.Draw()             ## plot the results
#
#   @endcode
#
#   @param pdf    (input) PDF
#   @param args   (input) arguments/variables 
#   @param data   (input) dataset 
#   @param bins   (input) bumbef of bins in histogram 
#   @param silent (input) keep the silence 
def uPlot ( pdf            ,
            data           ,
            bins   = None  ,
            args   = None  ,
            silent = False ) :
    """
    Make the plot of U-statistics 
    
    >>> pdf  = ...               ## pdf
    >>> data = ...               ## dataset
    >>> pdf.fitTo( data , ... )  ## fit it!
    
    >>> import AnalysisPython.uStat as uStat
    
    >>> r,histo = uStat.uPlot ( pdf , data ) 
    >>> print r                  ## print fit results
    >>> histo.Draw()             ## plot the results  

    """

    if not bins or bins <= 0 :
        nEntries = float(data.numEntries())
        bins = 10 
        for nbins in ( 50  ,
                       40  ,
                       25  ,
                       20  ,
                       16  ,
                       10  ,
                       8   ,
                       5   ) :
            if nEntries/nbins < 100 : continue  
            bins = nbins
            break
        print '#bins %s' % bins 
        
    hID = AnalysisPython.PyRoUts.hID

    histo = ROOT.TH1F ( hID () ,'U-statistics', bins , 0 , 1 )
    histo.Sumw2      (   )
    histo.SetMinimum ( 0 )

    if not args : args = pdf.getObservables ( data )
    
    h,tStat = uCalc ( pdf       ,
                      args      ,
                      data      ,
                      histo     ,
                      silent    )    
    
    res  = histo.Fit         ( 'pol0' , 'SLQ0+' )
    func = histo.GetFunction ( 'pol0' )
    if func :
        func.SetLineWidth ( 3 )
        func.SetLineColor ( 2 )
        func.ResetBit     ( 1 << 9 )
        
    return res , histo, float(tStat)

# ===========================================================================

if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 

# ===========================================================================
# The END 
# ===========================================================================
