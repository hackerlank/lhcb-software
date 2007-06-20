// $Id: HltTrackFilter.cpp,v 1.1 2007-06-20 12:17:38 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "HltBase/ParserDescriptor.h"
#include "Event/HltNames.h"

// local
#include "HltTrackFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackFilter
//
// 2007-03-22 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackFilter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFilter::HltTrackFilter( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("FilterDescriptor", m_filterDescriptor);
  declareProperty("ComputeInfo", m_computeInfo = true);
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackFilter::~HltTrackFilter() {
  for (std::vector<Hlt::TrackFunction*>::iterator it = m_functions.begin();
       it != m_functions.end(); ++it)
    delete *it;
  for (std::vector<Hlt::TrackFilter*>::iterator it = m_filters.begin();
       it != m_filters.end(); ++it)
    delete *it;
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackFilter::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IHltFunctionFactory* factory = tool<IHltFunctionFactory>("HltFunctionFactory");

  if (m_inputTracks2) factory->setTracks(*m_inputTracks2);
  if (m_primaryVertices) factory->setVertices(*m_primaryVertices);

  const std::vector<std::string>& hdes = m_filterDescriptor.value();
  for (std::vector<std::string>::const_iterator it = hdes.begin();
       it != hdes.end(); it++){
    std::string filtername = "";
    std::string mode = "";
    float x0 = -1.e6;
    float xf =  1.e6;
    if (!ParserDescriptor::parseFilter(*it,filtername,mode,x0,xf)) continue;

    int id = HltNames::particleInfoID(filtername);

    if (m_computeInfo) {
      Hlt::TrackFunction* fun = factory->trackFunction(id);
      if (!fun) error() << " error crearing function " << filtername 
                        << " " << id << endreq;
      m_functions.push_back(fun);
    }
    Hlt::TrackFilter* fil = factory->trackFilter(id,mode,x0,xf);    
    if (!fil) error() << " error crearing filter " << filtername 
                      << " " << id << endreq;

    m_filterNames.push_back(filtername);
    m_filterIDs.push_back(id);
    m_filterModes.push_back(mode);
    m_filters.push_back(fil);
    m_tcounters.push_back(0);   

    if (m_histogramUpdatePeriod>0) {
      HltHisto histo = NULL;
      initializeHisto(histo,filtername,0.,100.,100);
      m_histos.push_back(histo);
      
      HltHisto histo1 = NULL;
      initializeHisto(histo1,filtername+"Best",0.,100.,100);
      m_histos1.push_back(histo1);  
    }

    debug() << " filter " << filtername << " " << id << " "
            << mode << x0 << "," << xf << endreq;
  }

  release(factory);

  checkInput(m_inputTracks," input tracks");
  checkInput(m_outputTracks," output tracks");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackFilter::execute() {

  debug() << "==> Execute" << endmsg;

  int ncan = 0;
  for (size_t i = 0; i < m_filters.size(); ++i) {
    
    // prepare next loop tracks
    int key = m_filterIDs[i];
    m_otracks.clear();
    if (i==0) copy(*m_inputTracks,m_otracks);
    else copy(*m_outputTracks,m_otracks);

    // compute information
    if (m_computeInfo) {
      Hlt::TrackFunction* fun = m_functions[i];
      Hlt::map(m_otracks,*fun,key);
    }

    // debug and monitor
    if (m_debug || m_monitor) {
      std::vector<double> vals = infos(m_otracks,key,1e6);
      std::sort(vals.begin(),vals.end());
      double val = 0.;
      if (m_filterModes[i] == "<") val = vals[0];
      else val = vals.back();      
      if (m_debug) {
        std::string name = m_filterNames[i]+" : ";
        print(vals.begin(),vals.end(),name);
        debug() << " best " << val << endreq;
      }
      if (m_monitor) {
        fillHisto(m_histos[i],vals,1.);
        fillHisto(m_histos1[i],val,1.);                  
      }      
    }

    // filter
    m_outputTracks->clear();
    Hlt::TrackFilter* fil = m_filters[i];
    debug() << " applying filter " << fil << endreq;
    Hlt::select(m_otracks,*fil,*m_outputTracks);
    ncan = m_outputTracks->size();
    if (m_debug) {
      debug() << " entering tracks " << m_otracks.size() << endreq;
      debug() << " fitered tracks " << ncan << endreq;
    }

    // continue or break
    if (ncan <=0) break;
    m_tcounters[i] +=1;
  }


  ncan = m_outputTracks->size();
  candidateFound(ncan);
  if (m_selectionSummaryID>0) 
    saveInSummary(*m_outputTracks);

  debug() << " final candidates " << ncan << endreq;
  
  if (ncan>0 && m_debug) printInfo("filtered tracks ", *m_outputTracks);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTrackFilter::finalize() {

  debug() << "==> Finalize" << endmsg;
  
  for (size_t i = 0; i < m_filterNames.size(); ++i) {
    std::string title =  " event accepted " + m_filterNames[i] + " / input ";
    infoSubsetEvents(m_tcounters[i],m_counterEntries,title);
  }
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
