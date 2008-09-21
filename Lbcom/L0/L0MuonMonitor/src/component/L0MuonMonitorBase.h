// $Id: L0MuonMonitorBase.h,v 1.5 2008-09-21 21:53:42 jucogan Exp $
#ifndef COMPONENT_L0MUONMONITORBASE_H 
#define COMPONENT_L0MUONMONITORBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Kernel/MuonTileID.h"

#include "MuonKernel/MuonSystemLayout.h"
#include "L0MuonKernel/MonUtilities.h"

/** @class L0MuonMonitorBase L0MuonMonitorBase.h component/L0MuonMonitorBase.h
 *  
 *
 *  @author 
 *  @date   2008-04-08
 */
class L0MuonMonitorBase : public GaudiHistoAlg {
public: 
  /// Standard constructor
  L0MuonMonitorBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMonitorBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void olsErrorSummary(MsgStream & msg) const;
  
  StatusCode getL0MuonTiles(std::vector<LHCb::MuonTileID> & l0muontiles);
  StatusCode getL0MuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles);

  StatusCode getOlsInError(std::vector<LHCb::MuonTileID> & ols);

  StatusCode storeOlsInError();
  
  std::map<std::pair<LHCb::MuonTileID,int> ,bool> m_err_ols;
  std::map<LHCb::MuonTileID,int> m_err_ols_stored;
  
  void setLayouts(); // Set the layouts in use
  MuonSystemLayout  m_opt_link_layout;

  bool excludedBx(); //Return true if the current Bx is in the list of exclusion  
  bool exclusiveBx(); //Return true if the current Bx is in the list of exclusive  
  bool selectedTrigger();

  bool m_shortnames; // Use shortname for histograms created by the monitoring tools  

  std::vector<int> m_time_slots;
  std::vector<int> m_stations;
  std::vector<int> m_quarters;
  std::vector<int> m_regions;
  std::vector<unsigned int> m_excludedBxs;
  std::vector<unsigned int> m_exclusiveBxs;
  //   std::string m_triggerType;
  int m_triggerType;
  

  inline bool quarterInUse(int qua) 
  {
    std::vector<int>::iterator i;
    for (i=m_quarters.begin(); i<m_quarters.end(); ++i){
      if ((*i)==qua) return true;
    }
    return false;
  }
  inline bool regionInUse(int reg) 
  {
    std::vector<int>::iterator i;
    for (i=m_regions.begin(); i<m_regions.end(); ++i){
      if ((*i)==reg) return true;
    }
    return false;
  }
  inline bool stationInUse(int sta) 
  {
    std::vector<int>::iterator i;
    for (i=m_stations.begin(); i<m_stations.end(); ++i){
      if ((*i)==sta) return true;
    }
    return false;
  }

};
#endif // COMPONENT_L0MUONMONITORBASE_H
