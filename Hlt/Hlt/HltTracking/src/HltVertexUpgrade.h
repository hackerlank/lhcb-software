// $Id: HltVertexUpgrade.h,v 1.11 2009-12-23 10:50:34 graven Exp $
#ifndef HLTTRACKING_HLTTRACKUPGRADE_H 
#define HLTTRACKING_HLTTRACKUPGRADE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/ITrackUpgrade.h"
#include "HltBase/HltUtils.h"

/** @class HltVertexUpgrade HltVertexUpgrade.h
 *  
 *  functionality:
 *  Options:
 *        RecoName: name of the reconstruction to be done,
 *           valid names: "Velo","VeloTT","Forward","MuonForward"
 *        OrderByPt: true/false order by Pt output tracks in HltDataStore
 *
 *  Note:

 *  @author Jose Angel Hernando Morata
 *  @date   2006-08-28
 */
class HltVertexUpgrade : public HltAlgorithm {
public: 
  /// Standard constructor
  HltVertexUpgrade( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexUpgrade( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  StatusCode setReco();
  
  void saveConfiguration();

  size_t upgrade(LHCb::Track& seed, std::vector<LHCb::Track*>& tracks);
  
private:


  std::string m_recoName;
  bool m_transferExtraInfo;

private:

 double getTrackQuality(const LHCb::Track& tr);

  std::string m_TESOutputVerticesName;

  ITrackUpgrade* m_tool;

  Hlt::VertexCreator _makeVertex;

  std::string m_inputName;

  // one output: vertex
  // one input: vertex
  Hlt::SelectionContainer2<LHCb::RecVertex,LHCb::RecVertex> m_selections;
  
  AIDA::IHistogram1D* m_quality1Histo;
  AIDA::IHistogram1D* m_quality2Histo;
  AIDA::IHistogram1D* m_quality1HistoBest;
  AIDA::IHistogram1D* m_quality2HistoBest;
};
#endif // HLTTRACKING_H
