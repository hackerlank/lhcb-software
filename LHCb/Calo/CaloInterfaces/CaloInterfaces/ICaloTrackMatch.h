// $Id: ICaloTrackMatch.h,v 1.6 2004-02-17 11:42:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/05/23 09:41:37  ibelyaev
//  add 'state' method to ICaloTrackMatch interface
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKMATCH_H 
#define CALOINTERFACES_ICALOTRACKMATCH_H 1
// Include files
// STD & STL 
#include <functional>
#include <utility>
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloTrackMatch.h"

class CaloPosition  ; // from CaloEvent package 
class TrStoredTrack ; // from TrEvent   package
class TrState       ; // from TrEvent   package
 
/** @class ICaloTrackMatch ICaloTrackMatch.h CaloInterfaces/ICaloTrackMatch.h
 *  
 *  The generic interface for matching of 
 *  calorimeter object with tracking object 
 *
 *  The potential implementation candidates are:
 *
 *    - full matching of CaloParticle/CaloHypo object with Track 
 *      for electron/positron PID. The energy is taken into account 
 *      for matching procedure
 *    - matching of position of cluster with track for photon 
 *      selection/identification. The energy is not taken 
 *      into account in the matching procedure 
 *    - matching of track DIRECTION before magnet with 
 *      CaloCluster/CaloHypo/CaloParticle for searching for 
 *      bremmstrahlung photons. Energy is not taken 
 *      into account.
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Konstantin Beloous Konstantin.Beloous@cern.ch
 *  @date   30/10/2001
 */

class ICaloTrackMatch: 
  public  virtual IAlgTool ,
  public  std::binary_function<const CaloPosition*,const TrStoredTrack*,
  std::pair<StatusCode,double> >
{
 public:
  
  /** useful typedef for result
   *  - it is just a pair of status  and chi2 of the matching  
   */
  typedef std::pair<StatusCode,double>    MatchingPair;
  
  /** interface identification
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  virtual StatusCode match 
  ( const CaloPosition*   caloObj  , 
    const TrStoredTrack*  trObj    ,
    double&               chi2     ) = 0 ;
  
  /** The main matching method (Stl interface) 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @return pair of status code/chi2  for matching procedure 
   */
  virtual MatchingPair    operator() 
    ( const CaloPosition*   caloObj  , 
      const TrStoredTrack*  trObj    ) = 0 ;
  
  /** extract the TrState which was actually used for last matching
   *  @attention TrState is owned by the tool itself 
   *  The better solution could be to return the pointer 
   *  to TrStateP 
   *  @return pointer to the state actually used for last matching
   */
  virtual const TrState* state   () const = 0 ;
  
 protected:
  
  /// destructor
  virtual ~ICaloTrackMatch() ; 
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOTRMATCH_H
// ============================================================================
