// $Id: GaussTrackActionFinal.cpp,v 1.3 2004-02-23 08:19:16 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/22 16:51:54  ibelyaev
//  few minor fixes
//
// Revision 1.1  2004/02/20 19:35:27  ibelyaev
//  major update
// 
// ============================================================================
// Include files 
// ============================================================================
// CLHEP
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4TrackingManager.hh"
#include "G4VProcess.hh"
#include "G4ProcessType.hh"
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/GiGaMACROs.h"
// ============================================================================
// GaussTools 
// ============================================================================
#include "GaussTools/GaussTrajectory.h"
#include "GaussTools/GaussTrackInformation.h"
// ============================================================================
// local
// ============================================================================
#include "GaussTrackActionFinal.h"
// ============================================================================

/** @file 
 *  Implementation file for class : GaussTrackActionFinal
 *
 *  @date 2004-02-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionFinal );
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionFinal::GaussTrackActionFinal
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaussTrackActionBase ( type , name , parent ) 
  //
  , m_storeMarked   ( false ) //  
  , m_storeWithHits ( true  ) //  
  //
{
  declareProperty ( "StoreMarked"   , m_storeMarked    ) ;
  declareProperty ( "StoreWithHits" , m_storeWithHits  ) ;
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
GaussTrackActionFinal::~GaussTrackActionFinal() {}
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionFinal::PreUserTrackingAction  
( const G4Track* /* track */ ) 
{
  // get the trajectory 
  GaussTrajectory*       tr   = trajectory() ;  
  // check the validity 
  if( 0 == tr   ) 
  { Error ( "Pre...: GaussTrajectory*       points to NULL" ) ; }
  // get the trajectory 
  GaussTrackInformation* info = trackInfo() () ;  
  // check the validity 
  if( 0 == into ) 
  { Error ( "Pre...: GaussTrackInformation* points to NULL" ) ; }

};
// ============================================================================

// ============================================================================
/** perform action 
 *  @see G4VUserTrackingAction
 *  @param pointer to new track opbject 
 */
// ============================================================================
void GaussTrackActionFinal::PostUserTrackingAction  
( const G4Track* track ) 
{
  if( 0 == track ) { Error("Post..: G4Track* points to NULL") ; }
  
  // get the trajectory 
  GaussTrajectory*       tr   = trajectory() ;  
  if( 0 == tr ) { Error ( "Post...: GaussTrajectory* points to NULL " ) ; }
  
  // get the information 
  GaussTrackInformation* info = trackInfo() ;
  
  bool store = false ;
  if      ( trackMgr      () ->  GetStoreTrajectory() ) { store = true ; }
  else if ( storeMarked   () &&  info->toBeStored  () ) { store = true ; }
  else if ( storeWithHits () && !info->hits().empty() ) { store = true ; }
  
  // update global flug 
  if ( store ) { trackMgr() -> SetStoreTrajectory( true ) ; }
  
  // update track info 
  if ( trackMgr() ->GetStoreTrajectory() ) { info->setToBeStored( true ) ; }
  
  // 
  if ( store ) { return  ; }                                     // RETURN 
  
  // update track links 
  info -> updateHitsTrackID( track->GetParentID() ) ;
  
  //  if track is not to be stored,
  //  propagate it's parent ID (stored) to its secondaries
  G4TrackVector* children = trackMgr()->GimmeSecondaries() ;
  if ( 0 == children )  { return ; }                             // RETURN 
  for( unsigned int index = 0 ; index < children->size() ; ++index )
  {
    G4Track* child = (*children)[index] ;
    if( 0 == child ) { continue ; }                             // CONTINUE 
    //
    if ( child -> GetParentID() != track -> GetTrackID() ) 
    { Error ( "Could not reconstruct properly the parent!" ) ; } 
    //
    child -> SetParentID( track->GetParentID() );
    //
  }
  
//   // delete the trajectory by hand 
//   if ( ! trackMgr()->GetStoreTrajectory() )
//   {
//     G4VTrajectory* traj = trackMgr()->GimmeTrajectory();    
//     if( 0 != traj ) { delete traj ; } 
//     trackMgr() -> SetStoreTrajectory ( false ) ;
//     trackMgr() -> SetTrajectory      ( 0     ) ;
//   }
  
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
