// $Id: NeutralPPsFromCPsAlg.cpp,v 1.1 2002-11-20 20:00:24 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// Event 
#include "Event/CaloParticle.h"
// Event 
#include "Event/ProtoParticle.h"
#include "Event/TrStoredTrack.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
// local
#include "NeutralPPsFromCPsAlg.h"

// ============================================================================
/** @file 
 *  Implementation file for class NeutralPPsFromCPsAlg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-20
 */
// ============================================================================

// ============================================================================
/** @var  NeutralPPsFromCPsAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<NeutralPPsFromCPsAlg>         s_factory ;
const        IAlgFactory&NeutralPPsFromCPsAlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm instance name 
 *  @param pSvc service locator 
 */
// ============================================================================
NeutralPPsFromCPsAlg::NeutralPPsFromCPsAlg( const std::string& name ,
                                            ISvcLocator*       pSvc )
  : CaloAlgorithm ( name , pSvc ) 
  , m_hyposLong   ()
  , m_hypos       ()
  ///
  , m_matchType   ("AssociatorWeighted<CaloCluster,TrStoredTrack,float>")
  , m_matchName   ("PhotonMatch")
  , m_match       ( 0           )
  ///
  , m_spdprsType  ("CaloSingleGammaTool")
  , m_spdprsName  ("SpdPrsID"   )
  , m_spdprs      ( 0           )
  ///
  , m_caloTrMatch_bad   (  1.e+6 )
  , m_caloDepositID_bad ( -1.e+6 )
  , m_showerShape_bad   ( -1.e+6 )
  , m_clusterMass_bad   ( -1.e+6 )
  ///
  , m_calo              ( DeCalorimeterLocation:: Ecal )
{
  m_hyposLong.push_back( (long) CaloHypotheses::Photon    ) ;
  m_hyposLong.push_back( (long) CaloHypotheses::Pi0Merged ) ;
  // declare the properties 
  declareProperty( "Hypos"        , m_hyposLong  ) ;
  declareProperty( "MatchingType" , m_matchType  ) ;
  declareProperty( "MatchingName" , m_matchName  ) ;
  declareProperty( "SpdPrsIDType" , m_spdprsType ) ;
  declareProperty( "SpdPrsIDName" , m_spdprsName ) ;
  
  // set the appropriate default for input data
  setOutputData( CaloParticleLocation::  Default  ) ;
  // set the appropriate default for output data
  setOutputData( ProtoParticleLocation:: Neutrals ) ;
  // set appropriate defaut for detector data 
  setDetData    ( DeCalorimeterLocation:: Ecal    ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
NeutralPPsFromCPsAlg::~NeutralPPsFromCPsAlg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization method 
 *  @see CaloAlgorithm
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPsFromCPsAlg::initialize() 
{
  MsgStream  msg( msgSvc(), name() );
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() )
    { return Error( "CaloAlgorithm base class  is not initialized" , sc ) ; }
  
  for( std::vector<long>::const_iterator value = m_hyposLong.begin() ; 
       m_hyposLong.end() != value ; ++value ) 
    {
      if( *value < (long) CaloHypotheses::Undefined || 
          *value > (long) CaloHypotheses::Other      ) 
        { return Error("Unknown calorimeter hypothesis!"); }
      CaloHypotheses::Hypothesis hypo =  (CaloHypotheses::Hypothesis) *value ;
      m_hypos.push_back( hypo );
      msg << MSG::DEBUG << " Hyposesis accepted: " ;
      CaloHypoPrint( msg , hypo ) ;
      msg << endreq ;
    };
  
  // locate match 
  m_match  = tool( m_matchType  , m_matchName  , m_match  ) ;
  if( 0 == m_match  ) { return StatusCode::FAILURE ; }
  
  // locate spdprs
  m_spdprs = tool( m_spdprsType , m_spdprsName , m_spdprs ) ;
  if( 0 == m_spdprs ) { return StatusCode::FAILURE ; }

  // redefine the  detector 
  m_calo.setCalo( detData() );
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization method 
 *  @see CaloAlgorithm
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPsFromCPsAlg::finalize() 
{
  // release used tools 
  if( 0 != m_spdprs ) { m_spdprs -> release ()  ; m_spdprs = 0 ; }
  if( 0 != m_match  ) { m_match  -> release ()  ; m_match  = 0 ; }
  // finalize the base class 
  return CaloAlgorithm::finalize() ;
};
// ============================================================================

// ============================================================================
/** standard execution  method 
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPsFromCPsAlg::execute() 
{
  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  
  /// avoid long names 
  typedef const CaloParticles       CPs   ;
  typedef const CaloParticle::Hypos Hypos ;
  
  /// create and register the output container 
  ProtoParticles* pprtcls = new ProtoParticles();
  StatusCode sc = put( pprtcls , outputData() ) ;
  if( sc.isFailure() ) { return StatusCode::FAILURE ; }           // RETURN 
  
  /// get input container of CaloParticles
  const CPs* cprtcls = get( eventSvc() , inputData() , cprtcls );
  if( 0 == cprtcls   ) { return StatusCode::FAILURE ; }           // RETURN 
  
  /// get the relation table 
  const Table* table = m_match->direct();
  if( 0 == table     ) { return Error("Table* points to NULL!");} // RETURN 
  
  /// loop over all caloparticles and create the protoparticles  
  for( CaloParticles::const_iterator icp = cprtcls->begin() ;
       cprtcls->end() != icp  ; ++icp )
    {
      const CaloParticle* cp = *icp ;
      // skip nulls 
      if( 0 == cp             ) { continue ; }                  // CONTINUE
      // set some "initial values" 
      ProtoParticle* pp             =  0                   ;
      double         caloTrMatch_   =  m_caloTrMatch_bad   ;
      double         caloDepositID_ =  m_caloDepositID_bad ;
      double         showerShape_   =  m_showerShape_bad   ;
      double         clusterMass_   =  m_clusterMass_bad   ;
      
      // get all hypos, and loop over all "allowed" hypos  
      const Hypos& hypos = cp->hypos();
      for( Hypos::const_iterator ihypo = hypos.begin() ; 
           hypos.end() != ihypo ; ++ihypo )
        {
          const CaloHypo* hypo = *ihypo ;
          // skip invalid hypos 
          if( 0 == hypo     ) { continue ; }                    // CONTINUE 
          // valid hypothesis ? 
          if( m_hypos.end() == 
              std::find( m_hypos.begin    () , 
                         m_hypos.end      () , 
                         hypo->hypothesis () ) ) { continue ; } // CONTINUE 
          //
          // add it into the "current" ProtoParticle!
          if( 0 == pp ) { pp = new ProtoParticle() ; }
          pp->addToCalo( hypo );
          
          { // process with Chi2Match estimators
            const double chi2           = caloTrMatch( hypo , table );
            if( chi2 < caloTrMatch_   ) { caloTrMatch_ = chi2 ; }
          } // end of Chi2Match estimators
          
          { // process with spdPrsID estimator 
            const double dep            = caloDepositID( hypo ) ;
            if( caloDepositID_ < dep  ) { caloDepositID_ = dep ; }
          } // end of spdPrsID processing 
          
          { // process with ShowerShape estimator 
            const double shape          = showerShape( hypo );
            if(  showerShape_ < shape ) { showerShape_ = shape ; }            
          } // end of ShowerShape  processing 
          
          { // process with ClusterMass estimator 
            const double mass           = clusterMass( hypo );
            if( clusterMass_  < mass  ) { clusterMass_ = mass ; }
          } // end of ClusterMass processing
          
        } // end of loop over all hypos for given CaloParticle
      
      // ProtoParticle was not created ? 
      if( 0 == pp ) { continue ; }                        // CONTINUE 
      // insert new protoparticle into container 
      pprtcls->insert( pp ) ;
      // fill protoparticle 
      pp-> pIDDetectors().
        push_back( std::make_pair( ProtoParticle::CaloTrMatch   , 
                                   caloTrMatch_                 ) );
      pp-> pIDDetectors().
        push_back( std::make_pair( ProtoParticle::CaloDepositID , 
                                   caloDepositID_               ) );
      pp-> pIDDetectors().
        push_back( std::make_pair( ProtoParticle::ShowerShape   , 
                                   showerShape_                 ) );
      pp-> pIDDetectors().
        push_back( std::make_pair( ProtoParticle::ClusterMass   , 
                                   clusterMass_                 ) );
    }
  
  msg << MSG::DEBUG 
      << " Total number of created neutral ProtoParticles is " 
      << pprtcls -> size() << endreq ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::CaloTrMatch etimator
 *  @see ProtoParticle
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg::caloTrMatch
( const CaloHypo*                    hypo  , 
  const NeutralPPsFromCPsAlg::Table* table )  const 
{
  // reset the value 
  double chi2 = m_caloTrMatch_bad ;
  // check arguments
  if     ( 0 == hypo ) 
    {
      Error("caloTrMatch(): CaloHypo* points to NULL!");
      return chi2;                                              // RETURN !!
    }
  else if( 0 == table ) 
    {
      Error("caloTrMatch(): Table* points to NULL!");
      return chi2;                                              // RETURN !!
    }
  // select the first Ecal cluster 
  const CaloHypo::Clusters& clusters = hypo->clusters();
  CaloHypo::Clusters::const_iterator cluster =
    std::find_if( clusters.begin() , clusters.end() , m_calo );
  // no Ecal clusters is found , probably OK 
  if( clusters.end() == cluster ) { return chi2 ; }            // RETURN !!
  
  // get to all related tracks
  const Range range = table->relations( *cluster ) ;
  if( range.empty()            )  { return chi2 ; }            // RETURN !!
  
  // get minimal value 
  const double match = range.front().weight();
  
  return match ;
};
// ============================================================================

// ============================================================================
  /** helpful technical method to evaluate 
   *  the ProtoParticle::ClusterMass etimator
   *  @see ProtoParticle
   *  @param hypo  pointer to the hypo 
   *  @return the value of the estimator 
   */
// ============================================================================
double NeutralPPsFromCPsAlg::clusterMass ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double mass = m_clusterMass_bad ;
  // check arguments
  if     ( 0 == hypo ) 
    {
      Error("clusterMass(): CaloHypo* points to NULL!");
      return mass;                                              // RETURN !!
    }
  
  if( CaloHypotheses::Pi0Merged == hypo->hypothesis() ) 
    {
      const CaloMomentum* momentum = hypo->momentum ();
      if( 0 == momentum ) 
        { Error("CaloMomentum* points to NULL for 'Pi0Merged'"); }
      else 
        { mass = momentum->momentum().m(); }
    }  
  else { mass = 0 ; }
  
  return mass ;
};
// ============================================================================
  

// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::ShowerShape etimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double NeutralPPsFromCPsAlg::showerShape ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double shape = m_showerShape_bad ;
  // check arguments
  if     ( 0 == hypo ) 
    {
      Error("showerShape(): CaloHypo* points to NULL!");
      return shape;                                              // RETURN !!
    }
  
  // get the position 
  const CaloPosition* position = hypo->position();
  if( 0 == position ) 
    {
      Error("showerShape(): CaloPosition* points to NULL!");
      return shape;                                              // RETURN !!
    }
  
  // get the "shape"
  shape = position->spread()( 1 , 1 )+ position->spread()( 2 , 2 ) ;
  
  return shape ;
};
// ============================================================================


// ============================================================================
/** helpful technical method to evaluate 
 *  the ProtoParticle::CaloDepositID estimator
 *  @see ProtoParticle
 *  @param hypo  pointer to the hypo 
 *  @return the value of the estimator 
 */
// ============================================================================
double  NeutralPPsFromCPsAlg::caloDepositID ( const CaloHypo*  hypo  )  const 
{
  // reset the value 
  double dep = m_caloDepositID_bad ;
  // check arguments
  if     ( 0 == hypo ) 
    {
      Error("caloDepositID(): CaloHypo* points to NULL!");
      return dep;                                              // RETURN !!
    }
  
  dep = m_spdprs->likelyhood( hypo ) ;
  
  return dep ;
};
// ============================================================================
  

// ============================================================================
// The END 
// ============================================================================

