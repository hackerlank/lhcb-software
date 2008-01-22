// $Id: HltRecChecker.cpp,v 1.6 2008-01-22 11:04:06 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Linker/LinkedTo.h"
// local
#include "HltRecChecker.h"
#include "HltRecCheckUtils.h"

using namespace LHCb;

//---------------------------------------------------------------------------
// Implementation file for class : HltRecChecker
//
// 2006-05-24 : Hugo Ruiz Perez
//---------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltRecChecker );

HltRecChecker::HltRecChecker( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  
  declareProperty( "LinkName" ,    m_linkName     = "" );
  
}

HltRecChecker::~HltRecChecker() {} 

StatusCode HltRecChecker::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  book1D("Quark",0.,6.,6);
  book1D("Track Bs",0.,10.,10);
  book1D("Track TISTOS",0.,4.,8);

  book1D("Vertex Bs",0.,15.,15);
  book1D("Vertex TISTOS",0.,4.,8);

  // toolSvc()->retrieveTool("MCDecayFinder",m_finder);

  return sc;
}

StatusCode HltRecChecker::execute() {

  //  bool ok = true;
  
  checkQuark();
  
  if (m_inputTracks) checkTracks();

  if (m_inputVertices) checkVertices();

  return StatusCode::SUCCESS;  
}

void HltRecChecker::checkQuark() {
  MCParticles* mcpars = get<MCParticles>(MCParticleLocation::Default);
  int q = MCHlt::iquark(*mcpars);
  std::string title = "Quark";
  fill( histo1D(title), q, 1.);
  debug() << " check quark " << q << endreq;
}

void HltRecChecker::checkTracks() {

  LinkedTo<MCParticle> link(evtSvc(), msgSvc(), m_linkName);
  
  int nbs = 0;
  for (Hlt::TrackContainer::iterator it = m_inputTracks->begin();
       it != m_inputTracks->end(); ++it) {
    const Track& track = *(*it);
    printTrack(&track).ignore();
    MCParticle* par = link.first( track.key() );
    if (!par) continue;
    const MCParticle& mother = MCHlt::ancestor( (*par) );
    int q = MCHlt::iquark(mother);
    if (q == 1) {
      nbs +=1;
      if (m_outputTracks) m_outputTracks->push_back( (Track*) &track);
    }
  }
  
  std::string title = "Track Bs";
  fill( histo1D(title), nbs, 1.);
  
  bool tos = false;
  bool tis = false;
  
  if (nbs >0) tos = true;
  if (m_inputTracks->size() > nbs) tis = true;

  // int tostis = MCHlt::tostis(tos,tis);
  // title = "Track TISTOS";
  // fill( histo1D(title),tostis,1.);

  debug() << " check tracks nbs " << nbs << endreq;
  // debug() << " check tracks tostis " << tostis << endreq;

}

void HltRecChecker::checkVertices() {
  
}

//=========================================================================
//  Print a track
//=========================================================================
StatusCode HltRecChecker::printTrack(const LHCb::Track* T) {
  if (!msgLevel(MSG::DEBUG)) return StatusCode::SUCCESS ;
  if ( NULL==T ){
    Warning("NULL Track");
    return StatusCode::SUCCESS;
  }
  
  if ( T->firstState().qOverP()!=0.){
    Gaudi::SymMatrix6x6 cov6D ; 
    T->posMomCovariance(cov6D);
    debug() << "    Track " << T->key() << " " << T->type() << " " 
            << T->position() << " " << T->momentum() << "\n"
            << cov6D << endmsg ;
  } else { // velo tracks
    Gaudi::XYZPoint p ;
    Gaudi::SymMatrix3x3 cov3D ; 
    T->position(p,cov3D);
    debug() << "    Track " << T->key() << " " << T->type() << " " 
            << p << "\n" << cov3D << endmsg ; 
  }
  
  if (!T->ancestors().empty()) debug() << "Looking at " << T->ancestors().size() << " ancestors" << endmsg ;
  for ( SmartRefVector< LHCb::Track >::const_iterator t = 
          T->ancestors().begin(); t!= T->ancestors().end(); ++t ) {
    const LHCb::Track* at = *t ;
    StatusCode sc = printTrack(at);
    if (!sc) return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode HltRecChecker::finalize() {
  return HltAlgorithm::finalize();
}
