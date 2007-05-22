// $Id: RZMuonMatch.cpp,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "RZMuonMatch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RZMuonMatch
//
// 2007-02-21 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RZMuonMatch );
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RZMuonMatch::RZMuonMatch( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RZMuonMatch::~RZMuonMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RZMuonMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  
  m_matchToolPointer=tool<IMatchTVeloTracks>( "MatchTVeloTracks" );

 info()<<"exit "<<endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RZMuonMatch::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  HltAlgorithm::beginExecute();
  debug()<<" pat "<<m_patInputTracks->size()<<" hlt "<<m_inputTracks->size()
        <<endreq;
  int tt=0;
  
  for ( std::vector<Track*>::const_iterator itT = m_patInputTracks->begin();
        m_patInputTracks->end() != itT; itT++ ) {
    debug()<<" new pat track "<<endreq;
    
    Track* pTr2d = (*itT);
    pTr2d->setFlag(Track::PIDSelected,false);
    pTr2d->setFlag(Track::L0Candidate,false);
    if( pTr2d->checkFlag( Track::Backward ) ) continue; // skip backward tracks
    debug()<<" new pat track "<<tt<<endreq;
    tt++;
    

    for ( std::vector<Track*>::const_iterator itMuon = m_inputTracks->begin();
          m_inputTracks->end() != itMuon; itMuon++ ) {  
      
      debug()<<" enter loop new muon "<<m_inputTracks->size()<<endreq;
      Track* muon=(*itMuon);
      //  std::vector<Track* >::const_iterator pMuon;
      StatusCode sc=m_matchToolPointer->match2dVelo(*pTr2d,*muon);
      
      

      //    StatusCode sc=tryMatch(pTr2d);
    
      if(StatusCode::SUCCESS == sc) {
        debug()<<" success "<<endreq;
        
        m_outputTracks->push_back(pTr2d);
              
        pTr2d->setFlag(Track::PIDSelected,true);
//trasmit if L0 or muon seg
        if(muon->checkFlag(Track::L0Candidate)){
          
        pTr2d->setFlag(Track::L0Candidate,true);
}else{
        pTr2d->setFlag(Track::L0Candidate,false);
}
        debug()<<" selected track "<<endreq;
        
        if(m_debug){
          debug() << " Selected track" << endreq;
        }
        
        setFilterPassed(true);
        break;
      
      }
    }
  }  
  
  HltAlgorithm::endExecute();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RZMuonMatch::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


