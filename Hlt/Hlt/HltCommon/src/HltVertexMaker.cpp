// $Id: HltVertexMaker.cpp,v 1.1 2007-06-20 12:17:38 hernando Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltVertexMaker.h"
#include "Event/HltNames.h"
#include "HltBase/HltUtils.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/ParserDescriptor.h"
#include "HltBase/IHltFunctionFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexMaker
//
// 2006-02-21 : Hugo Ruiz
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexMaker );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexMaker::HltVertexMaker( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty( "CheckForOverlaps",       m_checkForOverlaps = false );  
  declareProperty( "PatOutputVerticesName",  
                   m_patOutputVerticesName = "Hlt/Vertex/MakerVertices");

  declareProperty("FilterDescriptor", m_filterDescriptor);
}



//=============================================================================
// Destructor
//=============================================================================
HltVertexMaker::~HltVertexMaker() {
  for (std::vector<Hlt::TrackBiFunction*>::iterator it = m_functions.begin();
       it != m_functions.end(); ++it) delete *it;
  for (std::vector<Hlt::Filter*>::iterator it = m_filters.begin();
       it != m_filters.end(); ++it) delete *it;
};

//=============================================================================
// Initialization
//=============================================================================

StatusCode HltVertexMaker::initialize() {

  m_twoContainers = true;
  if (m_inputTracks2Name == "" )  {
    info() << "only one input container requested " 
           << m_inputTracksName << endreq;
    m_twoContainers = false;
    m_inputTracks2Name = m_inputTracksName;
    m_inputTracks2 = m_inputTracks;
  }

  // NOTE: input tracks2 needs to be defined to book histos
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorithm

  
  // Prepare output vertex container
  m_patOutputVertices = 
    m_patDataStore->createVertexContainer(m_patOutputVerticesName,200);
  
  checkInput(m_inputTracks," input tracks ");
  checkInput(m_inputTracks2," input tracks2 ");
  checkInput(m_outputVertices," output vertices ");

  // create filters
  IHltFunctionFactory* factory = tool<IHltFunctionFactory>("HltFunctionFactory");
  const std::vector<std::string>& hdes = m_filterDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string filtername = "";
    std::string mode = "";
    float x0 = -1.e6;
    float xf =  1.e6;
    if (!ParserDescriptor::parseFilter(*it,filtername,mode,x0,xf)) continue;

    m_filterNames.push_back(filtername);
    
    int id = HltNames::particleInfoID(filtername);
    m_filterIDs.push_back(id);

    Hlt::TrackBiFunction* fun = factory->trackBiFunction(id);
    m_functions.push_back(fun);
    if (!fun) error() << " error crearing function " << filtername 
                      << " " << id << endreq;

    Hlt::Filter* fil = NULL;
    if (mode == "<") fil = new Estd::less<double>(x0);
    else if (mode == ">") fil = new Estd::greater<double>(x0);
    else fil = new Estd::in_range<double>(x0,xf);
    m_filters.push_back(fil);
    if (!fil) error() << " error crearing filter " << filtername 
                      << " " << id << endreq;

    m_tcounters.push_back(0);   

    if (m_histogramUpdatePeriod>0) {
      HltHisto histo = NULL;
      initializeHisto(histo,filtername,0.,100.,100);
      m_histos.push_back(histo);
    }

    debug() << " filter " << filtername << " " << id << " "
            << mode << x0 << "," << xf << endreq;
  }
  release(factory);

  return StatusCode::SUCCESS;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_debug ) debug() << "HltVertexMaker: Execute" << endmsg;
  m_outputVertices->clear();

  if (!m_twoContainers && m_inputTracks->size() <2) {
    debug() << " no enought tracks in container to make vertices " << endreq;
    return sc;
  }

  if (m_debug) {
    printInfo( "tracks [1]", *m_inputTracks);
    printInfo( "tracks [2]", *m_inputTracks2);
  }
  
  for (Hlt::TrackContainer::const_iterator itM = m_inputTracks->begin(); 
       itM != m_inputTracks->end(); itM++) {
    const LHCb::Track& track1 = *(*itM);

    verbose() << " track 0 " << track1.key() << track1.slopes() << endreq;

    // 1st check which mode are we running on...
    Hlt::TrackContainer::const_iterator itHStart;
    if (m_twoContainers) itHStart = m_inputTracks2->begin();
    else itHStart = itM + 1;    

    // And then start the loop itself!
    for (Hlt::TrackContainer::const_iterator itH = itHStart; 
         itH != m_inputTracks2->end(); itH++) {
      const LHCb::Track& track2 = *(*itH);
      verbose() << " track 1 " << track2.key() << track2.slopes() << endreq;
      
      // can not be the same track
      if ((*itH) == (*itM)) continue;
      
      // Check for segment overlaps
      bool accepted = true;
      if (m_checkForOverlaps)
        accepted = !(haveOverlaps(track1,track2));
      
      verbose() << " accepted due to overlaps?" << accepted << endreq;

      m_vals.clear();
      for (size_t i = 0; i < m_functions.size(); ++i) {
        verbose() << " filter " << m_filterNames[i] << endreq;
        Hlt::TrackBiFunction& fun = *(m_functions[i]);
        Hlt::Filter& fil = (*m_filters[i]);
        double val = (fun)(track1,track2);
        accepted = (fil)(val);
        verbose() << " value " << val << " accepted? " << accepted << endreq;
        if (m_monitor) fillHisto(m_histos[i],val,1.);
        if (!accepted) break;
        m_tcounters[i] += 1;
        m_vals.push_back(val);
      }
      
      if (!accepted) continue;


      verbose()<<" pair found [0] "<<track1.key() <<track1.slopes() << endreq;
      verbose()<<" pair found [1] "<<track2.key() <<track2.slopes() << endreq;
      // Write vertex
      //if ( m_outputByVertex ) {
      LHCb::RecVertex* sv = m_patOutputVertices->newEntry();
      _makeVertex(track1,track2,*sv);
      m_outputVertices->push_back(sv);
      for (size_t i = 0; i < m_filterIDs.size(); ++i) {
        verbose()<< " info " << m_filterIDs[i]<<" = "<<m_vals[i] << endreq;
        sv->addInfo(m_filterIDs[i],m_vals[i]);
      }
      
      
    } // loop on tracks2
  } // loop on tracks1
  
  
  int ncan = m_outputVertices->size();
  debug() << " final candidates " << ncan << endreq;
  candidateFound(ncan);
  if (m_selectionSummaryID>0) 
    saveInSummary(*m_outputVertices);
  
  return StatusCode::SUCCESS;  
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltVertexMaker::finalize() {

  for (size_t i = 0; i < m_filterNames.size(); ++i) {
    std::string title =  " event accepted " + m_filterNames[i] + " / input ";
    infoSubsetEvents(m_tcounters[i],m_counterEntries,title);
  }

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorith  
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================

bool HltVertexMaker::haveOverlaps( const LHCb::Track& track1, 
                                    const LHCb::Track& track2) {
  return HltUtils::matchIDs(track1,track2);
}


