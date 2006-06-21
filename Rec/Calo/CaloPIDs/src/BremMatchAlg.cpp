// $Id: BremMatchAlg.cpp,v 1.3 2006-06-21 18:43:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/06/20 18:17:48  odescham
// minor update to please ChargedProtoPAlg
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
//  the firstcommmit for DC06 branch
//
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchAlg.h"
// ============================================================================
/** @class BremMatchAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-16 
 */
// ============================================================================
class BremMatchAlg : public CaloTrackMatchAlg 
{
  // friend factory for instantiation
  friend class AlgFactory<BremMatchAlg> ;
public:
  /// execution of the algorithm
  virtual StatusCode execute () ;  
protected:
  /// standard (protected) constructor 
  BremMatchAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrackMatchAlg ( name , pSvc ) 
  {
    Inputs inputs = Inputs( 1 , LHCb::CaloHypoLocation::Photons     ) ;
    //
    setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )   ) ;
    setProperty ( "Output"    , LHCb::CaloIdLocation::BremMatch     ) ;
    setProperty ( "Tool"      , "CaloBremMatch/BremMatch:PUBLIC"    ) ;
    setProperty ( "Filter"    , LHCb::CaloIdLocation::InBrem        ) ;
    setProperty ( "Threshold" , "10000"                             ) ;
    // track types:
    setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Velo     ,
                    LHCb::Track::Long     ,
                    LHCb::Track::Upstream ) ) ;
  }
  /// virtual and protected destrcutror
  virtual ~BremMatchAlg() {}
private:
  BremMatchAlg() ;
  BremMatchAlg           ( const BremMatchAlg& ) ;
  BremMatchAlg& operator=( const BremMatchAlg& ) ;
};
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( BremMatchAlg );
// ============================================================================
/// Standard execution of the algorithm
// ============================================================================
StatusCode BremMatchAlg::execute () 
{
  //
  Assert ( !m_tracks .empty() , "No Input tracks"   ) ;
  Assert ( !m_calos  .empty() , "No Input Clusters" ) ;
  //
  typedef LHCb::RelationWeighted2D<LHCb::Track,LHCb::CaloHypo,float> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::ITrHypoTable2D)) ;
  
  // create the relation table and register it in TES 
  Table* table = new Table ( 10 * 100  ) ; 
  put ( table , m_output ) ;
  
  // perform the actual jobs 
  return doTheJob<LHCb::CaloHypo,Table>( table ) ;
} ;
// ============================================================================
// The END 
// ============================================================================
