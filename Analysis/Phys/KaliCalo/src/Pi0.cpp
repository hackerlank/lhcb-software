// $Id: Pi0.cpp,v 1.1 2009-10-31 16:59:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
// Local 
// ============================================================================
#include "KaliUtils.h"
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  /** @class Pi0  
   *  Simple algorithm for Calorimeter Cailbration using pi0 peak
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  class Pi0 : public LoKi::Algo
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<Kali::Pi0> ;    // friend factory for instantiation 
    // ========================================================================
  public:
    // ========================================================================
    /// the main 'execution' method 
    virtual StatusCode analyse    () ;          //  the main 'execution' method 
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name (INPUT) the algorithm instance name 
     *  @param pSvc (INPUT) the pointer to Service Locator 
     */
    Pi0 ( const std::string& name ,            //    the algorithm instance name 
          ISvcLocator*       pSvc )            // the pointer to Service Locator 
      : LoKi::Algo ( name , pSvc ) 
    {}
    /// virtual & protected destructor 
    virtual ~Pi0() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Pi0 () ;                            //  the default constructor is disabled      
    /// the copy  constructor is disabled 
    Pi0 ( const Pi0& ) ;                //     the copy constructor is disabled 
    /// the assignement operator is disabled 
    Pi0& operator=( const Pi0& ) ;      // the assignement operator is disabled 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namesapce Kali
// ============================================================================
// the only one essential method 
// ============================================================================
StatusCode Kali::Pi0::analyse    ()            // the only one essential method 
{
  using namespace LoKi         ;
  using namespace LoKi::Types  ;
  using namespace LoKi::Cuts   ;
  using namespace Gaudi::Units ;
  
  LHCb::CaloDataFunctor::DigitFromCalo spd ( "Spd" ) ;
  LHCb::CaloDataFunctor::DigitFromCalo prs ( "Prs" ) ;
  
  // get all photons with 
  Range gamma = select ( "g" , ( "gamma" == ID ) && ( PT > 300 * MeV ) ) ;
  
  counter("#gamma") += gamma.size() ;
  
  Tuple tuple = nTuple ( "Pi0-Tuple" ) ;
  
  typedef std::set<const LHCb::CaloDigit*> SET ;
  SET digits ;
  
  AIDA::IHistogram1D* h1 = book ( "mpi0-0"       , 0 , 0.250 , 250 ) ;
  AIDA::IHistogram1D* h2 = book ( "mpi0-1-pt"    , 0 , 0.250 , 250 ) ;
  AIDA::IHistogram1D* h3 = book ( "mpi0-2-spd-1" , 0 , 0.250 , 250 ) ;
  AIDA::IHistogram1D* h4 = book ( "mpi0-3-spd-2" , 0 , 0.250 , 250 ) ;

  typedef std::set<const LHCb::Particle*> Photons ;
  Photons                                 photons ;
  
  for ( Loop pi0 = loop( "g g" , "pi0" ) ; pi0 ; ++pi0 ) 
  {
    
    const double              m12 = pi0->mass ( 1 , 2 ) ;
    const LoKi::LorentzVector p12 = pi0->p    ( 1 , 2 ) ;

    const LHCb::Particle* g1 = pi0(1) ;
    if ( 0 == g1         ) { continue ; }  // CONTINUE    

    const LHCb::Particle* g2 = pi0(2) ;
    if ( 0 == g2         ) { continue ; }  // CONTINUE
    
    if ( 0 != h1 && m12 < 250 * MeV ) { h1 -> fill ( m12 / GeV ) ; }
    
    const bool good  = ( m12         < 250 * MeV && p12.Pt()  > 800 * MeV ) ;
    
    if ( !good   ) { continue ; }   // CONTINUE!!!
    
    if ( 0 != h2 ) { h2 -> fill ( m12 / GeV ) ; }
    
    //double spd1e = energyFrom ( g1 , spd ) / GeV ;
    double spd1e = seedEnergyFrom ( g1 , spd ) / GeV ;
    
    if ( 0 < spd1e ) { continue ; }
    if ( 0 != h3 ) { h3 -> fill ( m12 / GeV ) ; }
    
    double spd2e = seedEnergyFrom ( g2 , spd ) / GeV ;
    if ( 0 < spd2e ) { continue ; }
    
    if ( 0 != h4 ) { h4 -> fill ( m12 / GeV ) ; }
    
    double prs1e = energyFrom ( g1 , prs ) / GeV ;
    double prs2e = energyFrom ( g2 , prs ) / GeV ;
    
    if ( prs1e > prs2e ) 
    {
      std::swap ( g1    , g2    ) ;
      std::swap ( spd1e , spd2e ) ;
      std::swap ( prs1e , prs2e ) ; 
    }
    
    // fill N-tuple
    
    tuple -> column ( "m12"  , m12             / GeV ) ;
    
    tuple -> column ( "p0"   , p12             / GeV ) ;
    tuple -> column ( "g1"   , g1->momentum()  / GeV ) ;
    tuple -> column ( "g2"   , g2->momentum()  / GeV ) ;
    
    tuple -> column ( "pt"   , p12.Pt()        / GeV ) ;
    tuple -> column ( "pt1"  , PT ( g1 )       / GeV ) ;
    tuple -> column ( "pt2"  , PT ( g2 )       / GeV ) ;
    
    tuple -> column ( "prs1" , prs1e ) ;
    tuple -> column ( "prs2" , prs2e ) ;
    
    const LHCb::CaloCellID cel1 = cellID( g1 ) ;
    const LHCb::CaloCellID cel2 = cellID( g2 ) ;
    
    const unsigned short index1 = cel1.index() ;
    const unsigned short index2 = cel2.index() ;
    
    tuple -> column ( "ind1"     , index1 ) ;
    tuple -> column ( "ind2"     , index2 ) ;
    
    const bool ispd1 = 0 < spd1e ;
    const bool ispd2 = 0 < spd2e ;
    tuple -> column ( "spd1" , ispd1 ) ;
    tuple -> column ( "spd2" , ispd2 ) ;

    tuple -> write () ; 
    
    // finaly save good photons: 

    photons.insert  ( g1 ) ;
    photons.insert  ( g2 ) ;    
    //
  }
  
  for ( Photons::const_iterator iphoton = photons.begin() ; 
        photons.end() != iphoton ; ++iphoton ) 
  {
    // keep these photons 
    LHCb::Particle ph (**iphoton) ;
    desktop()->keep( &ph  ) ;
  }
  
  counter ( "#photons" ) += photons .size  () ;
  
  setFilterPassed ( !photons.empty() ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Pi0)
// ============================================================================
// The END 
// ============================================================================
