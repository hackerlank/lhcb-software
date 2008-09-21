// $Id: L0MuonMuonComp.h,v 1.1 2008-09-21 21:45:17 jucogan Exp $
#ifndef COMPONENT_L0MUONMUONCOMP_H 
#define COMPONENT_L0MUONMUONCOMP_H 1

// Include files

//#include "Kernel/IEventTimeDecoder.h"
#include "MuonDAQ/IMuonRawBuffer.h"

#include "L0MuonMonitorBase.h"
//#include "L0MuonPadsHistos.h"
#include "L0MuonChannelsHistos.h"
#include "L0MuonOptLinksHistos.h"

/** @class L0MuonMuonComp L0MuonMuonComp.h component/L0MuonMuonComp.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-08
 */
  
class L0MuonMuonComp : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonMuonComp( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMuonComp( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode compareTiles(std::vector<std::pair<LHCb::MuonTileID,double > > & muontiles);
  
protected:

private:

  StatusCode getMuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID, std::pair<int,int> > > & muontiles);

  void diffL0MuonAndMuon(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles,
                         std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > & muontiles,
                         std::vector<std::pair<LHCb::MuonTileID,int > > & diff) ;
  
  void tilesMismatchSummary(MsgStream & msg) const;

  std::map<LHCb::MuonTileID,int> m_tiles_mismatch;

  bool m_muonZS; 

  //  IEventTimeDecoder* m_odin;

  L0MuonChannelsHistos* m_channelHist_l0muon;
  L0MuonChannelsHistos* m_channelHist_muon;
  L0MuonChannelsHistos* m_channelHist_l0muononly;
  L0MuonChannelsHistos* m_channelHist_muononly;
  L0MuonChannelsHistos* m_channelHist_mismatch;
  L0MuonOptLinksHistos* m_optlinkHist_mismatch;
  L0MuonOptLinksHistos* m_optlinkHist_error;  
  
  IMuonRawBuffer* m_muonBuffer;

  
};
#endif // COMPONENT_L0MUONMUONCOMP_H
