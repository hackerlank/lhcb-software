// $Id: HltTracking.cpp,v 1.15 2007-11-14 14:00:10 hernando Exp $
// Include files 

// from Gaudi
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// local
#include "Event/HltNames.h"
#include "HltTracking.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTracking
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTracking );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTracking::HltTracking( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("MeasureTime"   , m_measureTime   = false );
  declareProperty("RecoName", m_recoName = "empty");

  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  declareProperty("OrderByPt", m_orderByPt = false);

  m_configs["Velo"] = RecoConfiguration("Velo","RZVelo",
                                        "Tf::PatVeloSpaceTracking",
                                        LHCb::TrackLocation::HltRZVelo,
                                        LHCb::TrackLocation::HltVelo);
  
  m_configs["VeloTT"] = RecoConfiguration("VeloTT","Velo","PatVeloTT",
                                          LHCb::TrackLocation::HltVelo,
                                          LHCb::TrackLocation::HltVeloTT);
    
  m_configs["Forward"] = RecoConfiguration("Forward","Velo",
                                           "PatForward",
                                           LHCb::TrackLocation::HltVelo,
                                           LHCb::TrackLocation::HltForward);

  m_configs["MuonForward"] = RecoConfiguration("MuonForward","VeloMuon",
                                               "PatForward",
                                               "Hlt/Track/MuonVelo",
                                               "Hlt/Track/MuonForward");
  
}
//=============================================================================
// Destructor
//=============================================================================
HltTracking::~HltTracking() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTracking::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_doRest = (m_inputTracksName == "empty");

  // deal with timing
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_timeFlag = m_timer->addTimer( "Marck Tracks" );
    m_timePat  = m_timer->addTimer( "Pat Reco" );   
    m_timeLoad  = m_timer->addTimer( "Load Tracks" );
    m_timer->decreaseIndent();
  }

  // create the internal reconstruction algorithms and set properties
  //-------------------------------------------------------------------
  if (m_configs.find(m_recoName) == m_configs.end())
    return Error( " No configuration for reconstruction named "+m_recoName );

  sc = iniRecoAlgorithm();
  if ( sc.isFailure() ) return sc;  // error printed already by iniRecoAlgorithm
  return StatusCode::SUCCESS;

};

StatusCode HltTracking::iniRecoAlgorithm() 
{
 
  std::string algoType = m_configs[m_recoName].type;
  std::string algoName = "Pat"+name();  

  std::string recoName     = m_configs[m_recoName].reco+"Key";
  std::string prevrecoName = m_configs[m_recoName].prevreco+"Key";
 
  m_recoKey     = HltNames::particleInfoID( recoName );
  m_prevrecoKey = HltNames::particleInfoID( prevrecoName );
  m_recoHasPt   = HltNames::particleInfoID( "VeloTTKey" );

  m_TESInputTracksName = m_configs[m_recoName].inputTracksName;
  m_TESOutputTracksName = m_configs[m_recoName].outputTracksName;
  debug() << " TESInputTracksName " << m_TESInputTracksName << endreq;
  debug() << " TESOutputTracksName " << m_TESOutputTracksName << endreq;

  IAlgManager* appMgr;
  StatusCode sc = service( "ApplicationMgr", appMgr );
  if ( !sc.isSuccess() )  return Error( "Unable to locate the ApplicationMgr" );
  IJobOptionsSvc* optSvc = svc<IJobOptionsSvc>( "JobOptionsSvc" );

  StringProperty propertySelectorName = 
    StringProperty("TrackSelectorName","HltTrackSelector");
  optSvc->addPropertyToCatalogue( algoName, propertySelectorName);

  StringProperty propertyInputTracksName = 
    StringProperty("InputTracksName",m_configs[m_recoName].inputTracksName);
  optSvc->addPropertyToCatalogue( algoName, propertyInputTracksName);

  StringProperty propertyOutputTracksName = 
    StringProperty("OutputTracksName",m_configs[m_recoName].outputTracksName);
  optSvc->addPropertyToCatalogue( algoName, propertyOutputTracksName);

  
  // debug properties of internal reconstruction algorithm
  const std::vector< const Property*>* props = optSvc->getProperties(algoName);
  for (std::vector<const Property*>::const_iterator itp = props->begin();
       itp != props->end(); ++itp)
    debug() << " added property " << (*itp)->name() 
            << " to algorithm named " << algoName << endreq;
  release(optSvc);
  

  sc = appMgr->createAlgorithm(algoType, algoName, m_algo);
  if (!sc.isSuccess())
    return Error( "Unable to create algorithm named " + algoName + 
                  " of type " + algoType );
  
  sc = m_algo->sysInitialize();
  if (!sc.isSuccess())
    return Error( "Unable to initialize algorithm named " + algoName + 
                  " of type " + algoType );
  appMgr->release();
  
  debug() << " created reco algorithm named " << algoName << " of type "
          << algoType << endreq;

  debug() << "          reco Key " << recoName << " = " << m_recoKey << endreq;
  debug() << " previous reco Key " << prevrecoName 
          << " = " << m_prevrecoKey << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTracking::execute() {
  
  m_TESInputTracks  = getOrCreate<Tracks,Tracks>(m_TESInputTracksName);
  m_TESOutputTracks = getOrCreate<Tracks,Tracks>(m_TESOutputTracksName);
  
  flag();
  if (m_measureTime) m_timer->start(m_timePat);
  StatusCode sc = m_algo->execute();
  if (m_measureTime) m_timer->stop(m_timePat);
  load();

  if (m_outputTracks) candidateFound( m_outputTracks->size() );

  if (m_debug && m_outputTracks) 
    printInfo(" output tracks ",*m_outputTracks);
  
  return sc;
  
}



void HltTracking::flag() {
  if (m_measureTime) m_timer->start(m_timeFlag);

  for (Tracks::iterator it = m_TESInputTracks->begin();
       it != m_TESInputTracks->end(); ++it) {
    (*it)->setFlag(Track::IPSelected,false);
    verbose() << "Flagged track " << (*it)->key() << " " 
              << " " << (*it)->momentum() << ", Cov:\n" 
              << (*it)->firstState().covariance() << endmsg ;
  }
  
  if (m_inputTracks) markTracks(*m_inputTracks);
  else markTracks(*m_TESInputTracks);

  if (m_measureTime) m_timer->stop(m_timeFlag);
  
  debug() << " pat input      tracks " << m_TESInputTracks->size() << endreq;
  if (m_inputTracks)
    debug() << "     input      tracks " << m_inputTracks->size() << endreq;
}


void HltTracking::loadFrom(const Track& mon) {
  int keymon = mon.key();
  verbose() << " mon " << mon.key() << mon.slopes() << endreq;
  for (Tracks::iterator it = m_TESOutputTracks->begin();
       it != m_TESOutputTracks->end(); ++it) {
    if ((*it)->nStates() <=0) continue; /// @todo protection to buggy tracks
    Track& son = **it;
    bool ok = false;
    int ikeymon = (int) son.info(m_prevrecoKey,-1);
    // if (ikeymon == keymon) ok = true;
    // else if (ikeymon == -1) ok = isAncestor(son,mon);
    ok = isAncestor(son,mon);
    verbose() << " son track " << son.key() << " is ancestor? " 
            << ikeymon << " " << keymon << " " 
            << ok << son.slopes() << endreq;
    if (ok) {
      if (m_transferExtraInfo) son.setExtraInfo(mon.extraInfo());
      son.addInfo(m_prevrecoKey, keymon);
      if (m_outputTracks) m_outputTracks->push_back( (Track*) &son );
      verbose() << "Added to output track " << son.key() << " " 
                << son.momentum() << ", Cov:\n" 
                << son.firstState().covariance() << endmsg ;
    }
  }
}



void HltTracking::load() {
  if (m_measureTime) m_timer->start(m_timeLoad);

  if (m_inputTracks) loadTracks(*m_inputTracks);
  else loadTracks(*m_TESInputTracks);
  
  if ((m_orderByPt) && (m_outputTracks) && (m_recoKey >= m_recoHasPt)) 
    std::sort(m_outputTracks->begin(),m_outputTracks->end(),_sortByPt);

  if (m_measureTime) m_timer->stop(m_timeLoad);
  if (m_debug) {
    debug() << " pat output tracks " << m_TESOutputTracks->size() << endreq;
    if (m_outputTracks)
      debug() << "     output tracks " << m_outputTracks->size() << endreq;
  }
}


bool HltTracking::isAncestor(const Track& son, const Track& mother) {
  const SmartRefVector<Track>& tracks = son.ancestors();
  for (SmartRefVector<Track>::const_iterator it = tracks.begin(); 
       it != tracks.end(); ++it) {
    const Track& imon = **it;
    verbose() << " is ancestor? " << (Track*) (&imon) << 
      (Track*) (&mother) << endreq;
    if (  (Track*) (&imon) == (Track*) (&mother) ) return true;
  }
  verbose() << " no ancestor! " << endreq;
  return false;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTracking::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_algo->finalize();
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

void HltTracking::saveConfiguration() {

  HltAlgorithm::saveConfiguration();

  std::string type = "HltTracking";
  confregister("Type",type);
  confregister("RecoName",m_recoName);
}
