// $Id: ICaloParticleTool.h,v 1.3 2004-02-17 11:42:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/02 11:08:12  ibelyaev
//  new interfaces
//
// Revision 1.1.1.2  2001/11/02 16:53:14  ibelyaev
// New Package: the first release
//
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOPARTICLETOOL_H 
#define CALOINTERFACES_ICALOPARTICLETOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local
#include "CaloInterfaces/IIDICaloParticleTool.h"

class    CaloParticle                ;  ///< from CaloEvent 

/** @class ICaloParticleTool ICaloParticleTool.h 
 *                                 CaloInterfaces/ICaloParticleTool.h
 *  
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloParticle objects, the potential candidates are: 
 *
 *    - any global scoring algorithms
 *    - PID 
 *    - dispatching 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/10/2001
 */

class ICaloParticleTool: 
  public virtual IAlgTool ,
  public std::unary_function<CaloParticle*,StatusCode>
{
  
 public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  @param  particle pointer to CaloParticle object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloParticle* particle ) const = 0 ;

  /** The main processing method (functor interface)
   *  @param  particle pointer to CaloParticle object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloParticle* particle ) const = 0 ;

 protected:
  
  /// destructor, virtual 
  virtual ~ICaloParticleTool() ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
