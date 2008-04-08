// $Id: L0MuonFromRawTrans.cpp,v 1.9 2008-04-08 11:19:57 jucogan Exp $
// Include files 

#include "boost/format.hpp"

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

// local
#include "L0MuonFromRawTrans.h"

#include "L0MuonKernel/ProcUtilities.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonFromRawTrans
//
// 2008-01-10 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonFromRawTrans );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonFromRawTrans::L0MuonFromRawTrans( const std::string& name,
                                        ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_configfile="L0MuonKernel.xml";
  declareProperty( "ConfigFile" , m_configfile      );
  declareProperty( "DumpErrors"  , m_dumpError=false      );

  
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonFromRawTrans::~L0MuonFromRawTrans() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonFromRawTrans::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  L0Muon::RegisterFactory::selectInstance(1);
  // Instanciate the MuonTrigger registers
  std::string xmlFileName = m_configfile;
  info() <<  "Configuration file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName,false);

  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    m_ctrlRaw[i] =  L0Muon::CtrlRawCnv(i);
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    m_procRaw[i] = L0Muon::ProcRawCnv(i);
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonFromRawTrans::execute() {

  L0Muon::RegisterFactory::selectInstance(1);
  StatusCode sc;

  // Loop over time slots
  for (int bx=-7; bx<8; ++bx){

    setProperty("RootInTes",timeSlot(bx));  
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    sc = decodeRawBanks();
    if ( sc.isFailure() ) return sc; 

    if (m_dumpError) {
      sc = dumpErrors();
      if ( sc.isFailure() ) return sc; 
    }

    sc = writeOnTES(1);
    if ( sc.isFailure() ) return sc; 

    sc = releaseRegisters();
    if ( sc.isFailure() ) return sc;  
  
  } // End of loop over time slots
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonFromRawTrans::finalize() {

  debug() << "==> Finalize" << endmsg;

  if (m_dumpError) {
    std::string errorCounters;

    errorCounters+="\n--- L0Muon decoding error summary\n";
    for (int i= 0; i<2; ++i) {
      switch(i){
      case(0):
        errorCounters+="Side A : ";
        break;
      case(1):
        errorCounters+="Side C : ";
        break;
      }
      errorCounters+=(boost::format("%3d banks decoded : ") % m_ctrlRaw[i].numberOfDecodedBanks()).str();
      if (m_ctrlRaw[i].numberOfDecodedBanks()>0 ) {
        m_ctrlRaw[i].dumpErrorCounters(errorCounters);
      } else errorCounters+="\n";
    }

    for (int i= 0; i<4; ++i) {
      errorCounters+=(boost::format("Q%d : %3d banks decoded : ") % (i+1) % m_procRaw[i].numberOfDecodedBanks()).str();
      if (m_procRaw[i].numberOfDecodedBanks()>0 ) {
        m_procRaw[i].dumpErrorCounters(errorCounters);
      } else errorCounters+="\n";
    }
    errorCounters+="--- End of L0Muon decoding error summary\n";
  
    info() << errorCounters<<endmsg;
  }
  

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================
 
StatusCode L0MuonFromRawTrans::decodeRawBanks(){

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  if (msgLevel( MSG::DEBUG )) debug() << "decodeRawBanks:  ==> got rawEvt " << endmsg;

  m_l0EventNumber=-1;
  m_l0_B_Id=-1;

  //   const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );
  //  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::TT );
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0MuonRaw );
  if (banks.size()!=0) {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int srcID = (*itBnk)->sourceID();
      int bankVersion  = (*itBnk)->version();
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      int size = (*itBnk)->size()/4;
      if (msgLevel( MSG::DEBUG ))
        debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                <<", sourceID is "<< srcID <<", size is "<< size <<endreq;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (srcID==0) {
        m_ctrlRaw[1].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (msgLevel( MSG::VERBOSE )) {
          for (int ii= 0; ii<2; ++ii) {
            if (m_ctrlRaw[1].decodingError(ii)) {
              m_ctrlRaw[1].dump(data);
              break;
            }
          }
        }
      }
      if (srcID==5) {
        m_ctrlRaw[0].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (msgLevel( MSG::VERBOSE )) {
          for (int ii= 0; ii<2; ++ii) {
            if (m_ctrlRaw[0].decodingError(ii)) {
              m_ctrlRaw[0].dump(data);
              break;
            }
          }
        }
      }
       if (srcID==3){
        m_procRaw[0].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (msgLevel( MSG::VERBOSE )) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[0].decodingError(ib)) {
              m_procRaw[0].dump(data);
              break;
            }
          }
        }
      }
       if (srcID==4){
        m_procRaw[1].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (msgLevel( MSG::VERBOSE )) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[1].decodingError(ib)) {
              m_procRaw[1].dump(data);
              break;
            }
          }
        }
      }
      if (srcID==2) {
        m_procRaw[2].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);      
        if (msgLevel( MSG::VERBOSE )) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[2].decodingError(ib)) {
              m_procRaw[2].dump(data);
              break;
            }
          }
        }
      }
       if (srcID==1){
        m_procRaw[3].decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (msgLevel( MSG::VERBOSE )) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[3].decodingError(ib)) {
              m_procRaw[3].dump(data);
              break;
            }
          }
        }
      }
     }
  }
  

  debug() << "decodeRawBanks:  ==> done" << endmsg;
  
  return StatusCode::SUCCESS;

}
StatusCode L0MuonFromRawTrans::dumpErrors()
{

  static int ievt=0;
  ++ievt;

  bool quarter_in_error[4];
  for (int iq=0; iq<4; ++iq) quarter_in_error[iq]=false;
  
  for (int i= 0; i<2; ++i) {// Loop over sides 
    if (!m_ctrlRaw[i].isActiv()) continue;
    for (int ii= 0; ii<2; ++ii) {// Loop over quarters in side
      int iq=i*2+ii;
      if (m_ctrlRaw[i].decodingError(ii) || m_ctrlRaw[i].inError(ii)) {
        quarter_in_error[iq]=true;
        continue;
      }
    } // End of loop over quarters in side
  } // End of loop over sides
  
  for (int iq=0; iq<4; ++iq) { // Loop over quarters
    for (int ib=0; ib<12; ++ib) { // Loop over processing boards
      if (m_procRaw[iq].decodingError(ib) || m_procRaw[iq].inError(ib)) {
        quarter_in_error[iq]=true;
        break;
      }
    } // End of loop over processing boards
  } // End of loop over quarters
  

  bool error=false;
  for (int iq=0; iq<4; ++iq) error|=quarter_in_error[iq];
  if (error){ // If an error occured
    std::cout<<boost::format("****** L0Muon Error report evt %d") % ievt<<std::endl;
    for (int i= 0; i<2; ++i) {// Loop over sides
      for (int ii= 0; ii<2; ++ii) {// Loop over quarters in side
        int iq=i*2+ii;
        if (!quarter_in_error[iq]) continue;
        std::cout<<boost::format("** L0Muon Error report for quarter Q%1d") % (iq+1)<<std::endl;
        m_ctrlRaw[i].dumpErrorHeader(ii,"\t");
        m_ctrlRaw[i].dumpErrorField(ii,"\t");
        m_procRaw[iq].dumpErrorHeader(0,"\t     ");      
        for (int ib= 0; ib<12; ++ib) {
          m_procRaw[iq].dumpErrorField(ib,(boost::format("\tPB%2d ") % (ib)).str());      
        }
      } // End of loop over quarters in side
    } // End of loop over sides
  } // End if an error occured
  
  return StatusCode::SUCCESS;
  
}

StatusCode L0MuonFromRawTrans::writeOnTES(int procVersion, std::string extension){

  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;

  std::string location;
  location = LHCb::L0MuonCandidateLocation::Default + extension;
  LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
  put(pl0mcands , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i].isActiv()) continue;
    cands = m_ctrlRaw[i].muonCandidates();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: side "<<i<< endreq;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found"<< endreq;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pl0mcands->insert(l0mcand);
    }
  }

  location = LHCb::L0MuonCandidateLocation::BCSU + extension;
  LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  put(pbcsucands , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int i= 0; i<4; ++i) {
    if (!m_procRaw[i].isActiv()) continue;
    cands = m_procRaw[i].muonCandidatesBCSU();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: side "<<i<< endreq;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endreq;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      pbcsucands->insert(l0mcand);
    }
  }

  location = LHCb::L0MuonCandidateLocation::PU  + extension;
  LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
  put(ppucands , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int i= 0; i<4; ++i) {      
    if (!m_procRaw[i].isActiv()) continue;
    cands = m_procRaw[i].muonCandidatesPU();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: quarter "<<i<< endreq;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endreq;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endreq;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,procVersion);
      ppucands->insert(l0mcand);
    }
  }


  location = LHCb::L0MuonDataLocation::Default + extension;
  LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
  put(pdata , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int i= 0; i<4; ++i) {
    if (!m_procRaw[i].isActiv()) continue;
//     if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: quarter "<<i<< endreq;
    std::vector<LHCb::MuonTileID>  pus = m_procRaw[i].pus();
    for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
      //debug() << "writeOnTES: pu = "<<itpu->toString()<< endreq;
      std::vector<LHCb::MuonTileID> ols = m_procRaw[i].ols(*itpu);
      //debug() << "writeOnTES: ols length = "<<ols.size()<< endreq;
      std::vector<LHCb::MuonTileID> neighs = m_procRaw[i].neighs(*itpu);
      //debug() << "writeOnTES: neighs length = "<<neighs.size()<< endreq;
      LHCb::L0MuonData *l0muondata = new LHCb::L0MuonData(*itpu, ols, neighs);
      pdata->insert(l0muondata);
    }
  }

  location = LHCb::L0MuonInfoLocation::Default + extension;
  LHCb::L0MuonInfo* pl0minfo = new LHCb::L0MuonInfo();
  pl0minfo->setL0EventNumber(m_l0EventNumber);
  pl0minfo->setL0_B_Id(m_l0_B_Id);
  put(pl0minfo, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }

  location = LHCb::L0MuonCtrlErrorLocation::Default + extension;
  LHCb::L0MuonCtrlErrors* pl0mctrlerrors = new LHCb::L0MuonCtrlErrors();
  put(pl0mctrlerrors, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i].isActiv()) continue;
    for (int ii=0; ii<2; ++ii){
      int quarter = i*2+ii;
      LHCb::L0MuonCtrlError * l0mctrlerror = new LHCb::L0MuonCtrlError(quarter);
      // Fill errors
      if (m_ctrlRaw[i].decodingError(ii)>0 ) l0mctrlerror->setHeader(4);
      else if (m_ctrlRaw[i].inError(ii)>0 ) l0mctrlerror->setHeader(2);
      else l0mctrlerror->setHeader(1);
      pl0mctrlerrors->insert(l0mctrlerror);
    }
  }

  location = LHCb::L0MuonProcErrorLocation::Default + extension;
  LHCb::L0MuonProcErrors* pl0mprocerrors = new LHCb::L0MuonProcErrors();
  put(pl0mprocerrors, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endreq;
//     debug() << "writeOnTES at "<< location << endreq;
//   }
  for (int iq= 0; iq<4; ++iq) {
    if (!m_procRaw[iq].isActiv()) continue;
    for (int ib=0; ib<12; ++ib){
      LHCb::L0MuonProcError * l0mprocerror = new LHCb::L0MuonProcError(m_procRaw[iq].mid_BCSU(ib),ib);
      // Fill errors
      if (m_procRaw[iq].decodingError(ib)>0 ) l0mprocerror->setHeader(4);
      else if (m_procRaw[iq].inError(ib)>0 ) l0mprocerror->setHeader(2);
      else l0mprocerror->setHeader(1);
      pl0mprocerrors->insert(l0mprocerror);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode L0MuonFromRawTrans::releaseRegisters(){

  for (int i= 0; i<2; ++i) m_ctrlRaw[i].release();
  for (int i= 0; i<4; ++i) m_procRaw[i].release();

  return StatusCode::SUCCESS;
}

LHCb::L0MuonCandidate* L0MuonFromRawTrans::l0muoncandidate(L0Muon::PMuonCandidate cand,int procVersion)
{
  bool debug = false;
//   if (msgLevel( MSG::VERBOSE )) debug=true;
    
  std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(cand->quarter(),cand->board(),cand->pu(),
                                                        cand->colM3(),cand->rowM3(), cand->offM2(),cand->offM1(),
                                                        procVersion,debug);
  std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,debug);
//   debug()<<"l0muoncandidate Seed"<<pads[2].toString()<<" M2: "<<pads[1].toString()<<" M1: "<<pads[0].toString()<< endreq;
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

