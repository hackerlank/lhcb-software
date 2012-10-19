// Include files:
// GSL
#include "gsl/gsl_math.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Kernel
#include "Kernel/VPChannelID.h"
#include "Kernel/FastClusterContainer.h"
// Event
#include "Event/VPLiteCluster.h"
#include "Event/RawEvent.h"
// VPelDet
#include "VPDet/DeVP.h"
// Local
#include "VPClusterWord.h"
#include "VPRawBankDecoder.h"
#include "VPRawBankToLiteCluster.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPRawBankToLiteCluster
//
// 2009-12-27 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPRawBankToLiteCluster)

//=============================================================================
// Constructor
//=============================================================================
VPRawBankToLiteCluster::VPRawBankToLiteCluster(const std::string& name,
                                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("RawEventLocation", m_rawEventLocation =
                  LHCb::RawEventLocation::Default);
  declareProperty("ClusterLocation", m_clusterLocation = 
                  VPLiteClusterLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
VPRawBankToLiteCluster::~VPRawBankToLiteCluster(){}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPRawBankToLiteCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPRawBankToLiteCluster::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;

  // Make new clusters container
  VPLiteCluster::VPLiteClusters* clusCont =
                      new VPLiteCluster::VPLiteClusters();
  put(clusCont, m_clusterLocation);
  //clusCont->reserve()//could be good to eval that...

  // Retrieve the RawEvent
  RawEvent* rawEvt = getIfExists<RawEvent>(m_rawEventLocation);
  if( NULL == rawEvt ){
    return Warning("Failed to find raw data", StatusCode::SUCCESS,1);
  }

  // Decode RawBanks
  StatusCode sc = decodeRawBanks(rawEvt,clusCont);
  if(sc.isFailure()){
    return Error("Problems in decoding, event skipped", StatusCode::SUCCESS );
  }

  std::sort(clusCont->begin(),clusCont->end(),SiDataFunctor::Less_by_Channel< LHCb::VPLiteCluster >());

  return sc;

}


//=============================================================================
// Decode RawBanks
//=============================================================================
StatusCode VPRawBankToLiteCluster::decodeRawBanks(RawEvent* rawEvt,
                     VPLiteCluster::VPLiteClusters* clusCont) const
{
  const std::vector<RawBank*>& tBanks = rawEvt->banks(LHCb::RawBank::VP);
  if(tBanks.size() == 0) {
    Warning("No VP RawBanks found");
    return StatusCode::SUCCESS;
  }
  // VPLiteCluster::VPLiteClusters* clusCont_tmp;
  
  // Loop over VP RawBanks  
  int nrClu = 0;
  std::vector<RawBank*>::const_iterator iterBank;
  for(iterBank = tBanks.begin(); iterBank != tBanks.end(); ++iterBank) {
    nrClu++;
    // Get sensor number
    unsigned int sensor = (*iterBank)->sourceID();
    // Decoder
    VPRawBankDecoder<VPClusterWord> decoder((*iterBank)->data());
    // Get version of the bank
    unsigned int bankVersion = (*iterBank)->version();
    if(m_isDebug) debug() << "Decoding bank version " << bankVersion << endmsg;
    // Decode lite clusters
    VPRawBankDecoder<VPClusterWord>::pos_iterator iterClu =
                                                            decoder.posBegin();
    for(;iterClu != decoder.posEnd(); ++iterClu) {
      createLiteCluster(sensor,*iterClu,clusCont);
    }
  } 
  return StatusCode::SUCCESS;
}


//=============================================================================
// Create liteCluster
//=============================================================================
void VPRawBankToLiteCluster::createLiteCluster(
                     unsigned int sensor,
                     VPClusterWord aWord,
                     VPLiteCluster::VPLiteClusters* clusCont) const 
{
  LHCb::VPChannelID achan;
  achan.setSensor(sensor);
  achan.setPixel(aWord.pixel());
  std::pair<unsigned int,unsigned int> xyFract;
  xyFract.first  = aWord.xFract();
  xyFract.second = aWord.yFract();
  const VPLiteCluster newCluster(achan,aWord.totValue(),xyFract,
                                      aWord.hasIsLong());    
  clusCont->push_back(newCluster);

  return;
}


//============================================================================
StatusCode VPRawBankToLiteCluster::finalize() {

  return GaudiAlgorithm::finalize();

}
