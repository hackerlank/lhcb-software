// $Id: LumiCountTracks.h,v 1.1 2008-07-31 21:34:47 panmanj Exp $
#ifndef LUMICOUNTTRACKS_H 
#define LUMICOUNTTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiAlg/GaudiTool.h"
// from LHCB
#include "Event/Track.h"
#include "Event/HltLumiSummary.h"


/** @class LumiCountTracks LumiCountTracks.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-07-21
 */
class LumiCountTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiCountTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiCountTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  std::string m_InputSelectionName;
  const LHCb::Tracks* m_InputContainer;
  std::string m_CounterName;
  int m_Counter;

  std::string m_OutputContainerName;
  LHCb::HltLumiSummarys* m_HltLumiSummarys;

private:

};
#endif // LUMICOUNTTRACKS_H
