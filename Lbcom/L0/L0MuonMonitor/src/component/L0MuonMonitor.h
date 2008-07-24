// $Id: L0MuonMonitor.h,v 1.5 2008-07-24 09:36:53 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITOR_H 
#define COMPONENT_L0MUONMONITOR_H 1

// Include files

//#include "Kernel/IEventTimeDecoder.h"
#include "MuonDAQ/IMuonRawBuffer.h"

#include "L0MuonMonitorBase.h"
//#include "L0MuonPadsHistos.h"
#include "L0MuonChannelsHistos.h"

/** @class L0MuonMonitor L0MuonMonitor.h component/L0MuonMonitor.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-08
 */
class L0MuonMonitor : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode compareTiles(std::vector<std::pair<LHCb::MuonTileID,double > > & muontiles);
  
protected:

private:

  bool m_muonZS; 

  //  IEventTimeDecoder* m_odin;

  L0MuonChannelsHistos* m_channelHist_l0muon;
  L0MuonChannelsHistos* m_channelHist_muon;
  
  IMuonRawBuffer* m_muonBuffer;

  
};
#endif // COMPONENT_L0MUONMONITOR_H
