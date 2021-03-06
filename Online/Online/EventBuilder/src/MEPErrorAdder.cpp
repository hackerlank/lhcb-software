//  ============================================================
//
//  ErrorAddingSvc.cpp
//  ------------------------------------------------------------
//
//  Package   : EventBuilder
//
//  Author    : David Svantesson
//
//  ===========================================================
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "RTL/Lock.h"
#include "EventBuilder/MEPErrorAdder.h"

#include "EventBuilder/MEPRxSys.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPErrorAdder)

// Publish DIM counters. DNS from environment
#define PUBCNT(name, desc) do {m_ ## name = 0; m_monSvc->declareInfo(#name,"I", & m_ ## name, sizeof(int64_t), desc, this);} while(0);
#define PUBARRAYCNT(name, desc) do {m_monSvc->declareInfo(#name, "I", & m_ ## name [0], m_ ## name.size()*sizeof(int64_t), desc, this);} while(0);
#define UNPUBCNT(name) do {m_monSvc->undeclareInfo(#name, this);} while(0);

// Template for resetting counters
template <typename T> static void resetCounter(T& cnt) { cnt = 0; }
template <typename T> static void resetCounters(T& cnt,size_t len) {
  cnt.resize(len,0);
  std::for_each(cnt.begin(),cnt.end(),resetCounter<typename T::value_type>);
}
// Resetting 2D arrays
template <typename T> static void reset2DCounters(T& cnt, size_t len1, size_t len2) {
  cnt.resize(len1);
  for (unsigned int i=0; i< len1; i++) {
    resetCounters(cnt[i],len2);
  }
}

using namespace LHCb;

MEPErrorAdder::MEPErrorAdder(const std::string& nam, ISvcLocator* svc) 
: Service(nam, svc), m_svcState(NOT_READY), m_incidentSvc(0), m_log(msgSvc(),nam)
{

  
  declareProperty("listenerDnsNode",    m_listenerDnsNode = "localhost.localdomain"); 		//Also where we get RunInfo (ECSXX)
  declareProperty("updateFrequency",    m_updateFrequency =5);					//How often the statistics should be updated
  
  declareProperty("sumPartition",	m_sumPartition=false);			 	  	//Is this a sum over nodes in subfarm, or subfarms in partition?
  
  //Options for sum over partition 
  declareProperty("partitionName",      m_partitionName = "");	 
  //declareProperty("runInfoDnsNode",	m_runInfoDnsNode = "");					// DIM_DNS_NODE for RunInfo, if other than listenerDnsNode  

  //Options for sum over subfarm
  declareProperty("nrSubNodes",         m_nrSubNodes =60);			         	//Number of nodes per subfarm
  
  declareProperty("nSrc",               m_nSrc=1);						//Numer of TELL1 sources this partition have
  
  m_allNames = NULL;
  m_allNamesSize = 0;
  m_zero = 0;
  m_zero32 = 0;
  m_zerof = 0; 
}

MEPErrorAdder::~MEPErrorAdder() {

}

/** Initalization of algorithm
 *
 *  Configure and setup of counters and subscriptions
 */
StatusCode
MEPErrorAdder::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) {
    return StatusCode::FAILURE; 
  }
  if (!service("IncidentSvc", m_incidentSvc).isSuccess()) {
    m_log << MSG::ERROR << "Failed to access incident service." << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    m_incidentSvc->addListener(this, "DAQ_CANCEL");
    m_incidentSvc->addListener(this, "DAQ_ENABLE");
  }

  char temp[100];

  // HOST set by job file
  if (m_listenerDnsNode=="localhost.localdomain" && getenv("HOST")!=NULL) m_listenerDnsNode= getenv("HOST");
  // Partition name set by job file
  if (m_sumPartition && m_partitionName=="" && getenv("SET_PARTITION")!=NULL) {
	m_partitionName = getenv("SET_PARTITION");
  }
 
  // listen to DNS node
  DimClient::setDnsNode(m_listenerDnsNode.c_str());
  m_log << MSG::INFO << "Listening to DNS Node: " << m_listenerDnsNode << endmsg; 
 
  if (m_sumPartition) {
  	// We want to subscribe to when the subfarm setup for this partition changes
	sprintf(temp,"RunInfo/%s/HLTsubFarms",m_partitionName.c_str());
  	m_subsSubFarms = new DimUpdatedInfo(temp,(char *)"",this);
	
	m_log << MSG::INFO << "Summing up statistics for partition: " << m_partitionName << endmsg;

	// Get subfarms first time
	sprintf(temp,"RunInfo/%s/HLTsubFarms",m_partitionName.c_str());
	DimInfo t_subFarms(temp,(char *)"");  
	while (t_subFarms.getSize()<1) { MEPRxSys::microsleep(10000); } // Must wait until data received to avoid segfault

	readRunInfo(&t_subFarms);
  }

  //Setup all services we want to publish, through the Monitoring Svc
  m_log << MSG::DEBUG << "Setup Monitoring Svc" << endmsg;
  if (service("MonitorSvc", m_monSvc).isSuccess()) {
          if (setupCounters()) {
                  m_log << MSG::ERROR << "Failed to setup counters";
                  return StatusCode::FAILURE;
          }
  }
  else {
    m_log << MSG::ERROR << "Failed to access monitor service.";
    return StatusCode::FAILURE;
  }

  // Setup all subscriptions
  setupSubs();

 
  m_log << MSG::DEBUG << "Initialization done" << endmsg;
  m_svcState = READY;
  
  return StatusCode::SUCCESS;
}

/** read from RunInfo a list of all subfarms
 */
int
MEPErrorAdder::readRunInfo(DimInfo* dim) {

	m_log << MSG::DEBUG << "Acquiring RunInfo information about subfarms etc. in partition" << endmsg;  
 	//if (m_runInfoDnsNode.size()>0) DimClient::setDnsNode(m_runInfoDnsNode.c_str());
	//else DimClient::setDnsNode(m_listenerDnsNode.c_str());

	// Get the list of subfarms
	const char* subFarmsChar = dim->getString(); 
	int subFarmsSize = dim->getSize();	

  	m_subFarms.clear();
	if (subFarmsChar[0] == 0) {
		// No subfarms
		m_log << MSG::WARNING << "Partition " << m_partitionName << " have no subfarms" << endmsg;
		m_nrSubFarms = 0;
		m_nrServices = 0;
		return 1;
        }

	// Read all subfarms into vector
	int start = 0;
	std::string printSubfarms = "";
	int i=0;
	while (subFarmsSize>start) {    
		m_subFarms.push_back(&subFarmsChar[start]);
    		start += (m_subFarms[i].size() + 1);
		printSubfarms += m_subFarms[i] + ", ";
		i++;
  	}
	m_log << MSG::INFO << "Subfarms in partition " << m_partitionName << ": " << printSubfarms << endmsg;
	m_nrSubFarms = m_subFarms.size();
	m_nrServices = m_nrSubFarms;

	return 0;
}

/** Finalize algorithm
 */
StatusCode
MEPErrorAdder::finalize() {

  removeSubs();

  delete m_subsSubFarms;

  if (m_monSvc) {
    m_monSvc->undeclareAll(this);
    m_monSvc->release();
    m_monSvc = 0;
  }
 
  delete[] m_allNames;
  
  m_svcState = NOT_READY;  
  return Service::finalize();
} 

/** Remove all DIM subscriptions
 *
 *  Used at finalize or
 *  for partition mode, if subfarms have changed
 */
int
MEPErrorAdder::removeSubs() {

  int size = (int) m_subsBadLenPkt.size();
  for (int i=0; i<size; i++) {
    delete m_subsBadLenPkt[i];
    delete m_subsBadPckFktPkt[i];
    delete m_subsMisPkt[i];
    delete m_subsTruncPkt[i];
    delete m_subsMultipleEvt[i];
    delete m_subsRxOct[i];
    delete m_subsRxPkt[i];
    delete m_subsReadOct[i];
    delete m_subsRxEvt[i];
    delete m_subsRxMEP[i];
    delete m_subsTotRxOct[i];
    delete m_subsTotRxPkt[i];
    delete m_subsBytesNotInPartition[i];
    delete m_subsPacketsNotInPartition[i];
    delete m_subsTotReadOct[i];
    delete m_subsTotRxEvt[i];
    delete m_subsTotRxMEP[i];
    delete m_subsIncEvt[i];
    delete m_subsTotBadMEP[i];
    delete m_subsTotMEPReq[i];
    delete m_subsTotMEPReqPkt[i];
    delete m_subsNumMEPRecvTimeouts[i];
    delete m_subsNotReqPkt[i];
    delete m_subsTotWrongPartID[i];
    delete m_subsSentEvt[i];
    delete m_subsSentOct[i];
    delete m_subsSentEvtErr[i];
    delete m_subsNetworkMonitor[i];
    delete m_subsNifRxOct[i];
    delete m_subsNifRxPkt[i];
    delete m_subsNifRxDropped[i];
    delete m_subsNifRxError[i];
    delete m_subsNifRxFrameError[i];

    delete m_subsSrcName[i];
  }	 

  m_subsBadLenPkt.clear();
  m_subsBadPckFktPkt.clear();
  m_subsMisPkt.clear();
  m_subsTruncPkt.clear();
  m_subsMultipleEvt.clear();
  m_subsRxOct.clear();
  m_subsRxPkt.clear();
  m_subsReadOct.clear();
  m_subsRxEvt.clear();
  m_subsTotRxOct.clear();
  m_subsTotRxPkt.clear();
  m_subsBytesNotInPartition.clear();
  m_subsPacketsNotInPartition.clear();
  m_subsTotReadOct.clear();
  m_subsTotRxEvt.clear();
  m_subsTotRxMEP.clear();
  m_subsIncEvt.clear();
  m_subsTotBadMEP.clear();
  m_subsTotMEPReq.clear();
  m_subsTotMEPReqPkt.clear();
  m_subsNumMEPRecvTimeouts.clear();
  m_subsNotReqPkt.clear();
  m_subsTotWrongPartID.clear();
  m_subsSentEvt.clear();
  m_subsSentOct.clear();
  m_subsSentEvtErr.clear();
  m_subsNetworkMonitor.clear();
  m_subsNifRxOct.clear();
  m_subsNifRxPkt.clear();
  m_subsNifRxDropped.clear();
  m_subsNifRxError.clear();
  m_subsNifRxFrameError.clear();

  m_subsSrcName.clear();

  return 0;
}

/** Help-function for receiving array counters
 *
 *  return false if not the correct service, otherwise get data and return true
 *
 *  If the received data size does not correspond to the number of sources, 
 *  resetPublishedServices is called, which will republish all services with the 
 *  correct size
 *
 */
bool
MEPErrorAdder::ReceiveArrayService(DimInfo * curr, DimInfo * subs,  std::vector<int64_t> &rArray, std::vector<int64_t> &sArray) {

  // Return false if not the correct service
  if (curr != subs) return false;

  // If new size of data, we have to resize all published counters.
  int arraySize = curr->getSize()/sizeof(int64_t);
  if (arraySize != m_nSrc) { 
	resetPublishedServices(arraySize);
	m_log << MSG::INFO << " Received from " << subs->getName() << endmsg;
  }

  int64_t * data = (int64_t*) curr->getData();
  int64_t diff;

  // Receive all data
  for (int j=0; j<arraySize; j++) {
    diff = data[j] - rArray[j];	// Value to add/subt
    rArray[j] = data[j];	// Save value
    sArray[j] += diff;  	// Change value
  }

  return true;
}

/** Help-function for receiving single counters
 *
 *  return false if not the correct service, otherwise get data and return true
 */
bool
MEPErrorAdder::ReceiveSingleService(DimInfo * curr, DimInfo * subs, int64_t &rValue, int64_t &sValue) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int64_t data = *((int64_t*) curr->getData());
  int64_t diff = data - rValue;	//Value to add/subtract
  
  rValue = data;	// saved value
  sValue += diff;	// Change value

  return true;

}

/** Help-function for receiving single counters, 32bit! 
 * (DiskWR)
 *
 *  return false if not the correct service, otherwise get data and return true
 */
bool
MEPErrorAdder::ReceiveSingleService_32(DimInfo * curr, DimInfo * subs, int64_t &rValue, int64_t &sValue) {

  // Return false if not the correct service
  if (curr != subs) return false;

  int32_t data = *((int32_t*) curr->getData());
  int64_t diff = data - rValue;	//Value to add/subtract
  
  rValue = data;	// saved value
  sValue += diff;	// Change value

  return true;

}

/** DimInfoHandler function
 */
void
MEPErrorAdder::infoHandler() {
  DimInfo * curr = getInfo();
 
  m_log << MSG::VERBOSE << "Receiving data from " << curr->getName() << endmsg;

  // If subfarms have changed, update and re-setup subscriptions
  if (curr==m_subsSubFarms) {
    changeSubFarms(m_subsSubFarms);
    return;
  }

  //To know what service, we must iterate over all subscriptions...
  for (int i=0;i<m_nrServices; i++) {
    // These array counters can change size, if the number of TELL1 sources changes
    if (ReceiveArrayService(curr,m_subsBadLenPkt[i], m_rBadLenPkt[i], m_badLenPkt)) break;
    if (ReceiveArrayService(curr,m_subsBadPckFktPkt[i], m_rBadPckFktPkt[i], m_badPckFktPkt)) break;
    if (ReceiveArrayService(curr,m_subsMisPkt[i], m_rMisPkt[i], m_misPkt)) break;
    if (ReceiveArrayService(curr,m_subsTruncPkt[i], m_rTruncPkt[i], m_truncPkt)) break;
    if (ReceiveArrayService(curr,m_subsMultipleEvt[i], m_rMultipleEvt[i], m_multipleEvt)) break;
    if (ReceiveArrayService(curr,m_subsRxOct[i], m_rRxOct[i], m_rxOct)) break;
    if (ReceiveArrayService(curr,m_subsRxPkt[i], m_rRxPkt[i], m_rxPkt)) break;
   if (ReceiveArrayService(curr,m_subsReadOct[i], m_rReadOct[i], m_readOct)) break;
    if (ReceiveArrayService(curr,m_subsRxEvt[i], m_rRxEvt[i], m_rxEvt)) break;
    if (ReceiveArrayService(curr,m_subsRxMEP[i], m_rRxMEP[i], m_rxMEP)) break;
   
    // These will not change
    if (ReceiveSingleService(curr,m_subsTotRxOct[i], m_rTotRxOct[i], m_totRxOct)) break;    
    if (ReceiveSingleService(curr,m_subsTotRxPkt[i], m_rTotRxPkt[i], m_totRxPkt)) break;    
    if (ReceiveSingleService(curr,m_subsBytesNotInPartition[i], m_rBytesNotInPartition[i], m_bytesNotInPartition)) break;
    if (ReceiveSingleService(curr,m_subsPacketsNotInPartition[i], m_rPacketsNotInPartition[i], m_packetsNotInPartition)) break;
    if (ReceiveSingleService(curr,m_subsTotReadOct[i], m_rTotReadOct[i], m_totReadOct)) break;   
    if (ReceiveSingleService(curr,m_subsTotRxEvt[i], m_rTotRxEvt[i], m_totRxEvt)) break;
    if (ReceiveSingleService(curr,m_subsTotRxMEP[i], m_rTotRxMEP[i], m_totRxMEP)) break;
    if (ReceiveSingleService(curr,m_subsIncEvt[i], m_rIncEvt[i], m_incEvt)) break;    
    if (ReceiveSingleService(curr,m_subsTotBadMEP[i], m_rTotBadMEP[i], m_totBadMEP)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReq[i], m_rTotMEPReq[i], m_totMEPReq)) break;    
    if (ReceiveSingleService(curr,m_subsTotMEPReqPkt[i], m_rTotMEPReqPkt[i], m_totMEPReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsNumMEPRecvTimeouts[i], m_rNumMEPRecvTimeouts[i], m_numMEPRecvTimeouts)) break;    
    if (ReceiveSingleService(curr,m_subsNotReqPkt[i], m_rNotReqPkt[i], m_notReqPkt)) break;    
    if (ReceiveSingleService(curr,m_subsTotWrongPartID[i], m_rTotWrongPartID[i], m_totWrongPartID)) break;    
 
    if (m_sumPartition) { 
        if (ReceiveSingleService(curr,m_subsSentEvt[i], m_rSentEvt[i], m_sentEvt)) break;
        if (ReceiveSingleService(curr,m_subsSentOct[i], m_rSentOct[i], m_sentOct)) break;
        if (ReceiveSingleService(curr,m_subsSentEvtErr[i], m_rSentEvtErr[i], m_sentEvtErr)) break;
    } else {
        if (ReceiveSingleService_32(curr,m_subsSentEvt[i], m_rSentEvt[i], m_sentEvt)) break;
        if (ReceiveSingleService_32(curr,m_subsSentOct[i], m_rSentOct[i], m_sentOct)) break;
        if (ReceiveSingleService_32(curr,m_subsSentEvtErr[i], m_rSentEvtErr[i], m_sentEvtErr)) break;         
    }

    // Network Manager subscriptions
    if (m_sumPartition) {
    	if (ReceiveSingleService(curr,m_subsNifRxOct[i], m_rNifRxOct[i], m_nifRxOct)) break;
    	if (ReceiveSingleService(curr,m_subsNifRxPkt[i], m_rNifRxPkt[i], m_nifRxPkt)) break;
    	if (ReceiveSingleService(curr,m_subsNifRxDropped[i], m_rNifRxDropped[i], m_nifRxDropped)) break;
    	if (ReceiveSingleService(curr,m_subsNifRxError[i], m_rNifRxError[i], m_nifRxError)) break;
    	if (ReceiveSingleService(curr,m_subsNifRxFrameError[i], m_rNifRxFrameError[i], m_nifRxFrameError)) break;
    } else {
	if (curr == m_subsNetworkMonitor[i]) {
	    if (curr->getSize()/sizeof(int64_t)<16) break;

  	    int64_t * data = ((int64_t*) curr->getData());
            // Interesting values in array; rx bytes: 0, rx packets/frames: 1, Rx Errorss: 2, Rx Dropped: 3, Rx Frame Error: 5
	    m_nifRxOct += (data[0] - m_rNifRxOct[i]);
	    m_nifRxPkt += (data[1] - m_rNifRxPkt[i]);
	    m_nifRxError += (data[2] - m_rNifRxError[i]);
	    m_nifRxDropped += (data[3] - m_rNifRxDropped[i]);
	    m_nifRxFrameError += (data[5] - m_rNifRxFrameError[i]);

	    m_rNifRxOct[i] = data[0];
	    m_rNifRxPkt[i] = data[1];
	    m_rNifRxError[i] = data[2];
	    m_rNifRxDropped[i] = data[3];
	    m_rNifRxFrameError[i] = data[5];
	
	    break;
	}
    }

    if (curr == m_subsSrcName[i]) {
	//Receiving source names
	
	int size = curr->getSize();
        // Now only look if size have changed, need to go through whole list?
        if (size!=m_allNamesSize) {
		//Source names seem to have changed, remove...
		if (m_allNamesSize!=0) m_monSvc->undeclareInfo("srcName", this);
		delete[] m_allNames;
		m_allNames = NULL;		

		if (size>0) {
			// Publish new source-names
			m_allNamesSize=size;
			if (!(m_allNames = new char[size+1])) {
				m_log << MSG::ERROR << "Failed to store source names" << endmsg;
				return;
			}
   		
			::memcpy(m_allNames, (const char *) curr->getData(), size);
		
			m_monSvc->declareInfo("srcName", "C", m_allNames, size, "Source IP names", this);
		}
	}
    }

  }

}

StatusCode MEPErrorAdder::run() {

  while (m_svcState != RUNNING) {
    switch(m_svcState) {
    case STOPPED:
    case NOT_READY:
      m_log << MSG::DEBUG << "Exiting from receive loop" << endmsg;
      //continue;
      return StatusCode::SUCCESS;
    case READY:
      MEPRxSys::microsleep(100000); // 100 ms
      break;
    default: continue;
    }
  }
  bool run = true;
  while (run) {
    if (m_svcState != RUNNING) {
      //Do something here?      
      return StatusCode::SUCCESS;
    }
    MEPRxSys::microsleep(100000); // 100 ms
  }
  return 1;
}

// IInterface implementation: Query interface
StatusCode
MEPErrorAdder::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( IRunable::interfaceID().versionMatch(riid) )   {
    *ppvInterface = (IRunable*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  // Interface is not directly availible: try a base class
  return Service::queryInterface(riid, ppvInterface);
}

void  MEPErrorAdder::handle(const Incident& inc)    {
  m_log << MSG::INFO << "Got incident:" << inc.source() << " of type " << inc.type() << endmsg;
  if (inc.type() == "DAQ_CANCEL")  {
    m_svcState = STOPPED;
  }
  else if (inc.type() == "DAQ_ENABLE")  {
    m_svcState = RUNNING;
  }
}

/** Setup published counters
 *
 *  reset array counters and publish array and single counters
 */
int
MEPErrorAdder::setupCounters() {  
  resetAllCounters();
  publishArrayCounters();
  publishSingleCounters();

  return 0;
}

/**Reset size and values to zero for all array counters
 *
 * Single counters set to zero when published...
 */
int
MEPErrorAdder::resetAllCounters() {
  resetCounters(m_rxOct, m_nSrc);
  resetCounters(m_rxPkt, m_nSrc);
  resetCounters(m_readOct, m_nSrc);
  resetCounters(m_rxEvt, m_nSrc);
  resetCounters(m_rxMEP, m_nSrc);
  resetCounters(m_badPckFktPkt, m_nSrc);
  resetCounters(m_badLenPkt, m_nSrc);
  resetCounters(m_misPkt, m_nSrc);
  resetCounters(m_truncPkt, m_nSrc);
  resetCounters(m_multipleEvt, m_nSrc); 

  return 0;
}

int
MEPErrorAdder::resetSingleCounters() {
  m_totRxOct = 0;
  m_totRxPkt = 0;
  m_bytesNotInPartition = 0;
  m_packetsNotInPartition = 0;
  m_totReadOct = 0;
  m_totRxEvt = 0;
  m_totRxMEP = 0;
  m_incEvt = 0;
  m_totBadMEP = 0;
  m_totMEPReq = 0;
  m_totMEPReqPkt = 0;
  m_numMEPRecvTimeouts = 0;
  m_notReqPkt = 0;
  m_totWrongPartID = 0;

  m_sentEvt = 0;
  m_sentOct = 0;
  m_sentEvtErr = 0;

  m_nifRxOct = 0;
  m_nifRxPkt = 0;
  m_nifRxDropped = 0;
  m_nifRxError = 0;
  m_nifRxFrameError = 0;

  return 0;
}

/** Reset all 2D arrays, they keep track of received values
 */
int
MEPErrorAdder::resetRem2DCounters() {
  reset2DCounters(m_rBadLenPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rBadPckFktPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMisPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rTruncPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rMultipleEvt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxOct,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxPkt,m_nrServices,m_nSrc);
  reset2DCounters(m_rReadOct,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxEvt,m_nrServices,m_nSrc);
  reset2DCounters(m_rRxMEP,m_nrServices,m_nSrc);

  return 0;
}

/** Reset all arrays for remembering received single counters
 */
int
MEPErrorAdder::resetRemSingleCounters() {
  resetCounters(m_rTotRxOct,m_nrServices);
  resetCounters(m_rTotRxPkt,m_nrServices);
  resetCounters(m_rBytesNotInPartition,m_nrServices);
  resetCounters(m_rPacketsNotInPartition,m_nrServices);
  resetCounters(m_rTotReadOct,m_nrServices);
  resetCounters(m_rTotRxEvt,m_nrServices);
  resetCounters(m_rTotRxMEP,m_nrServices);
  resetCounters(m_rIncEvt,m_nrServices);
  resetCounters(m_rTotBadMEP,m_nrServices);
  resetCounters(m_rTotMEPReq,m_nrServices);
  resetCounters(m_rTotMEPReqPkt,m_nrServices);
  resetCounters(m_rNumMEPRecvTimeouts,m_nrServices);
  resetCounters(m_rNotReqPkt,m_nrServices);
  resetCounters(m_rTotWrongPartID,m_nrServices);

  resetCounters(m_rSentEvt,m_nrServices); 
  resetCounters(m_rSentOct,m_nrServices);
  resetCounters(m_rSentEvtErr,m_nrServices);
 
  resetCounters(m_rNifRxOct,m_nrServices);
  resetCounters(m_rNifRxPkt,m_nrServices);
  resetCounters(m_rNifRxDropped,m_nrServices);
  resetCounters(m_rNifRxError,m_nrServices);
  resetCounters(m_rNifRxFrameError,m_nrServices);

  return 0;
}

/** Publishes all single counters
 */
int
MEPErrorAdder::publishSingleCounters() {
  PUBCNT(totRxOct,           "Total received bytes");
  PUBCNT(totRxPkt,           "Total received packets");
  PUBCNT(bytesNotInPartition,	     "Bytes received not belonging to partition");
  PUBCNT(packetsNotInPartition,	     "Packets received not belonging to partition");
  PUBCNT(totReadOct,	     "Total received and read bytes");
  PUBCNT(totRxEvt,	     "Total received events");
  PUBCNT(totRxMEP,           "Total received MEPs");
  PUBCNT(incEvt,             "Incomplete events"); 
  PUBCNT(totBadMEP,          "Total bad MEPs");
  PUBCNT(totMEPReq,          "Total requested MEPs");
  PUBCNT(totMEPReqPkt,       "Total Sent MEP-request packets");
  PUBCNT(numMEPRecvTimeouts, "MEP-receive Timeouts");
  PUBCNT(notReqPkt,          "Total unsolicited packets");
  PUBCNT(totWrongPartID,     "Packets with wrong partition-ID");
  
  PUBCNT(sentEvt,	     "Events sent from HLT (MDF)");
  PUBCNT(sentOct,            "Bytes sent from HLT");
  PUBCNT(sentEvtErr,	     "Number of event send errors from HLT");

  PUBCNT(nifRxOct,	     "NIF received bytes");
  PUBCNT(nifRxPkt,	     "NIF received frames");
  PUBCNT(nifRxDropped,	     "NIF rx dropped frames");
  PUBCNT(nifRxError,         "NIF receive errors");
  PUBCNT(nifRxFrameError,    "NIF receive frame errors");

  return 0;
}

/** Publishes all array counters
 */
int
MEPErrorAdder::publishArrayCounters() {
  PUBARRAYCNT(badLenPkt,     "MEPs with mismatched length");
  PUBARRAYCNT(misPkt,        "Missing MEPs");
  PUBARRAYCNT(badPckFktPkt,  "MEPs with wrong packing (MEP) factor");
  PUBARRAYCNT(truncPkt,      "Truncated (empty) MEPs");
  PUBARRAYCNT(multipleEvt,   "Duplicate Events");
  PUBARRAYCNT(rxOct,         "Received bytes");
  PUBARRAYCNT(rxPkt,         "Received packets");
  PUBARRAYCNT(readOct,	     "Received and read bytes");
  PUBARRAYCNT(rxEvt,         "Received events");
  PUBARRAYCNT(rxMEP,         "Received MEPs"); 

  return 0;
}

/** Unpublishes all array counters
 */
int
MEPErrorAdder::unpublishArrayCounters() {
  UNPUBCNT(badLenPkt);
  UNPUBCNT(misPkt);
  UNPUBCNT(badPckFktPkt);
  UNPUBCNT(truncPkt);
  UNPUBCNT(multipleEvt);
  UNPUBCNT(rxOct);
  UNPUBCNT(rxPkt);
  UNPUBCNT(readOct);
  UNPUBCNT(rxEvt);
  UNPUBCNT(rxMEP);

  return 0;
}

/** This setup all subscriptions
 */
int
MEPErrorAdder::setupSubs() {

  char temp[100];

  // add all services we want to listen to..
  if (m_sumPartition) m_nrServices = m_nrSubFarms;
  else m_nrServices = m_nrSubNodes;     
  
  m_log << MSG::DEBUG << "Listening to " << m_nrServices << " services" << endmsg;

  // need to resize
  m_subsBadLenPkt.resize(m_nrServices,NULL);
  m_subsBadPckFktPkt.resize(m_nrServices,NULL);
  m_subsMisPkt.resize(m_nrServices,NULL);
  m_subsTruncPkt.resize(m_nrServices,NULL);
  m_subsMultipleEvt.resize(m_nrServices,NULL);
  m_subsRxOct.resize(m_nrServices,NULL);
  m_subsRxPkt.resize(m_nrServices,NULL);
  m_subsReadOct.resize(m_nrServices,NULL);
  m_subsRxEvt.resize(m_nrServices,NULL);
  m_subsRxMEP.resize(m_nrServices,NULL);

  m_subsTotRxOct.resize(m_nrServices,NULL);
  m_subsTotRxPkt.resize(m_nrServices,NULL);
  m_subsBytesNotInPartition.resize(m_nrServices,NULL);
  m_subsPacketsNotInPartition.resize(m_nrServices,NULL);
  m_subsTotReadOct.resize(m_nrServices,NULL);
  m_subsTotRxEvt.resize(m_nrServices,NULL);
  m_subsTotRxMEP.resize(m_nrServices,NULL);
  m_subsIncEvt.resize(m_nrServices,NULL);
  m_subsTotBadMEP.resize(m_nrServices,NULL);
  m_subsTotMEPReq.resize(m_nrServices,NULL);
  m_subsTotMEPReqPkt.resize(m_nrServices,NULL);
  m_subsNumMEPRecvTimeouts.resize(m_nrServices,NULL);
  m_subsNotReqPkt.resize(m_nrServices,NULL);
  m_subsTotWrongPartID.resize(m_nrServices,NULL);
 
  m_subsSentEvt.resize(m_nrServices,NULL);
  m_subsSentOct.resize(m_nrServices,NULL);
  m_subsSentEvtErr.resize(m_nrServices,NULL);

  m_subsNetworkMonitor.resize(m_nrServices,NULL);
  m_subsNifRxOct.resize(m_nrServices,NULL);
  m_subsNifRxPkt.resize(m_nrServices,NULL);
  m_subsNifRxDropped.resize(m_nrServices,NULL);
  m_subsNifRxError.resize(m_nrServices,NULL);
  m_subsNifRxFrameError.resize(m_nrServices,NULL);

  m_subsSrcName.resize(m_nrServices,NULL);

  // Reset arrays for remembering received values
  resetRem2DCounters();
  resetRemSingleCounters();

  if (m_sumPartition) { 
    //Partition sum

    for (int i=0;i<m_nrSubFarms;i++) {
  	//all subfarms
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/badLenPkt",m_subFarms[i].c_str());
	m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/badPckFktPkt",m_subFarms[i].c_str());
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s_MEPRxSTAT_1/Runable/misPkt",m_subFarms[i].c_str());
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/truncPkt",m_subFarms[i].c_str());
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/multipleEvt",m_subFarms[i].c_str());
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxOct",m_subFarms[i].c_str());
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxPkt",m_subFarms[i].c_str());
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/readOct",m_subFarms[i].c_str());
    	m_subsReadOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxEvt",m_subFarms[i].c_str());
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/rxMEP",m_subFarms[i].c_str());
    	m_subsRxMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
		
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxOct",m_subFarms[i].c_str());
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxPkt",m_subFarms[i].c_str());
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totReadOct",m_subFarms[i].c_str());
	m_subsTotReadOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/bytesNotInPartition",m_subFarms[i].c_str());
	m_subsBytesNotInPartition[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/packetsNotInPartition",m_subFarms[i].c_str());
	m_subsPacketsNotInPartition[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxEvt",m_subFarms[i].c_str());
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totRxMEP",m_subFarms[i].c_str());
	m_subsTotRxMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
      	sprintf(temp,"%s_MEPRxSTAT_1/Runable/incEvt",m_subFarms[i].c_str());
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totBadMEP",m_subFarms[i].c_str());
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totMEPReq",m_subFarms[i].c_str());
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totMEPReqPkt",m_subFarms[i].c_str());
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/numMEPRecvTimeouts",m_subFarms[i].c_str());
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/notReqPkt",m_subFarms[i].c_str());
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/totWrongPartID",m_subFarms[i].c_str());
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    
	//MDF, from HLT
    	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentEvt",m_subFarms[i].c_str());
	m_subsSentEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentOct",m_subFarms[i].c_str());
	m_subsSentOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s_MEPRxSTAT_1/Runable/sentEvtErr",m_subFarms[i].c_str());
	m_subsSentEvtErr[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	sprintf(temp,"%s_MEPRxSTAT_1/Runable/nifRxOct",m_subFarms[i].c_str());
	m_subsNifRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/nifRxPkt",m_subFarms[i].c_str());
	m_subsNifRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/nifRxDropped",m_subFarms[i].c_str());
	m_subsNifRxDropped[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/nifRxError",m_subFarms[i].c_str());
	m_subsNifRxError[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s_MEPRxSTAT_1/Runable/nifRxFrameError",m_subFarms[i].c_str()); 	
	m_subsNifRxFrameError[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	sprintf(temp,"%s_MEPRxSTAT_1/Runable/srcName",m_subFarms[i].c_str());
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,(char *)"",this);
    }
  } else {
    //Subfarm sum
    for (int i=0;i<m_nrSubNodes;i++) {
   	//all nodes in this subfarm 
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badLenPkt",m_listenerDnsNode.c_str(),i+1);
        m_subsBadLenPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/badPckFktPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsBadPckFktPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
   	sprintf(temp,"%s%.2i_MEPRx_1/Runable/misPkt",m_listenerDnsNode.c_str(),i+1);
   	m_subsMisPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/truncPkt",m_listenerDnsNode.c_str(),i+1);
    	m_subsTruncPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/multipleEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsMultipleEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxOct",m_listenerDnsNode.c_str(),i+1);
    	m_subsReadOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s%.2i_MEPRxIPCnt/Bytes",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRxIPCnt/Packets",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxEvt",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/rxMEP",m_listenerDnsNode.c_str(),i+1);
    	m_subsRxMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	sprintf(temp,"%s%.2i_MEPRxIPCnt/TotalBytes",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRxIPCnt/TotalPackets",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxOct",m_listenerDnsNode.c_str(),i+1);
	m_subsTotReadOct[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRxIPCnt/BytesNotInPartition",m_listenerDnsNode.c_str(),i+1);
	m_subsBytesNotInPartition[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRxIPCnt/PacketsNotInPartition",m_listenerDnsNode.c_str(),i+1);
	m_subsPacketsNotInPartition[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxEvt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totRxMEP",m_listenerDnsNode.c_str(),i+1);
	m_subsTotRxMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
     	sprintf(temp,"%s%.2i_MEPRx_1/Runable/incEvt",m_listenerDnsNode.c_str(),i+1);
	m_subsIncEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totBadMEP",m_listenerDnsNode.c_str(),i+1);
	m_subsTotBadMEP[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReq",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReq[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totMEPReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsTotMEPReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/numMEPRecvTimeouts",m_listenerDnsNode.c_str(),i+1);
	m_subsNumMEPRecvTimeouts[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/notReqPkt",m_listenerDnsNode.c_str(),i+1);
	m_subsNotReqPkt[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);
    	sprintf(temp,"%s%.2i_MEPRx_1/Runable/totWrongPartID",m_listenerDnsNode.c_str(),i+1);
	m_subsTotWrongPartID[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	//MDF, from HLT
    	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/BytesOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentOct[i] = new DimInfo(temp,m_updateFrequency,m_zero32,this);
     	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/EventsOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentEvt[i] = new DimInfo(temp,m_updateFrequency,m_zero32,this);
     	sprintf(temp,"%s%.2i_DiskWR_1/SND_0/ErrorsOut",m_listenerDnsNode.c_str(),i+1);
	m_subsSentEvtErr[i] = new DimInfo(temp,m_updateFrequency,m_zero32,this);

	// To subscribe to the nifSrv DIM service, we need to know the device name (net_0X) corresponding to eth1
	char * servcont;	// get service contents
	std::string ipn;	// ip
	int dnr;		// device nr
	for (dnr=0;dnr<10;dnr++) {
		sprintf(temp,"/FMC/%s%.2i/net/ifs/details/total/net_%.2i/data",m_listenerDnsNode.c_str(),i+1,dnr);
		DimCurrentInfo getdevice(temp,(char *)"");
		int subsize = getdevice.getSize();
		if (subsize<130) continue;
		// find ip number
		servcont = getdevice.getString()+128;
		int tm = 128;
		for (int j=0;j<3&&tm<subsize;j++,tm+=strlen(servcont)+1,servcont+=strlen(servcont)+1);
		if (tm>=subsize) continue;
		ipn = servcont;
		// check ip number
		if (ipn.substr(0,3)=="192") break;
	}
	sprintf(temp,"/FMC/%s%.2i/net/ifs/details/total/net_%.2i/data",m_listenerDnsNode.c_str(),i+1,dnr);
	m_subsNetworkMonitor[i] = new DimInfo(temp,m_updateFrequency,m_zero,this);

	sprintf(temp,"%s%.2i_MEPRx_1/Runable/srcName",m_listenerDnsNode.c_str(),i+1);
	m_subsSrcName[i] = new DimInfo(temp,m_updateFrequency,(char *)"",this);
 
    }
  }

  return 0;
}


/** When received arrays change size...
 * 
 * 1. Unpublish counters
 * 2. Reset all counters that are published or rememberd 2D counters
 * 3. Publish them again
 */
int
MEPErrorAdder::resetPublishedServices(int nSrc) {
   
  m_log << MSG::INFO << "Services have changed size (nr Src), previous: "<<m_nSrc<< ", new: "<<nSrc<<endmsg;
  
  m_nSrc = nSrc;  

  unpublishArrayCounters();
  
  resetAllCounters();
  resetRem2DCounters();
  
  publishArrayCounters();

  return 0;

}

/** Called when RunInfo setup changes, i.e. subfarms in partition changes
 *
 *  Will remove all subscriptions, update the list of subfarms, and setup 
 *  the subscriptions for the new subfarm list.
 *  Published counters also reset
 *
 */
int
MEPErrorAdder::changeSubFarms(DimInfo* dim) {

  m_log << MSG::INFO << "Partition subFarm setup changed" << endmsg;

  //Remove subscriptions
  removeSubs();
 
  m_log << MSG::DEBUG << "Subscriptions removed" << endmsg;

  // Read RunInfo again to update subfarm list
  readRunInfo(dim);
  
  // Reset all counters
  resetAllCounters();
  resetSingleCounters();
  resetRemSingleCounters();
  resetRem2DCounters();

  // Setup subscriptions again from all new subfarms
  m_log << MSG::DEBUG << "Setting up subscriptions again..." << endmsg;
  setupSubs();

  m_log << MSG::INFO << "Subscriptions have been re-initialized." << endmsg;  

  return 0;
	
}

