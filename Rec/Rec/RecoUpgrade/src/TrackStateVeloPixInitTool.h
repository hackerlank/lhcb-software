#include <map>
#include <boost/foreach.hpp>
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// from TrackInterfaces
#include "TrackInterfaces/IPatSeedFit.h"
#include "TrackInterfaces/ITrackStateInit.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/IPatVeloTTFit.h"

// from LHCb
#include "LHCbMath/LHCbMath.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/State.h"
#include "TrackKernel/TrackTraj.h"

// gsl
#include "gsl/gsl_math.h"


/** @class TrackStateVeloPixInitTool TrackStateVeloPixInitTool.h 
 *
 * Tool to initialize the track starting from LHCbIDs
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-02
 */

class DeSTDetector ;

static const InterfaceID IID_TrackStateVeloPixInitTool ( "TrackStateVeloPixInitTool", 1, 0 );

class TrackStateVeloPixInitTool : public GaudiTool, virtual public ITrackStateInit
{
public:
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_TrackStateVeloPixInitTool; }

  TrackStateVeloPixInitTool(const std::string& type, const std::string& name,
		     const IInterface* parent);
  virtual ~TrackStateVeloPixInitTool();
  StatusCode initialize();
  virtual StatusCode fit(LHCb::Track& track, bool clearStates = true) const;
  virtual StatusCode initializeRefStates(LHCb::Track& track,
					 LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;

protected:
  StatusCode createTStationStates(LHCb::Track& track) const;
  StatusCode createTTState(LHCb::Track& track) const;
  StatusCode createVeloStates(LHCb::Track& track ) const ;
  StatusCode createVeloTTStates(LHCb::Track& track ) const ;
  
private:
  IPatSeedFit* m_seedFit ;
  ITrackFitter* m_veloFitter ;  
  IPatVeloTTFit* m_veloTTFit;

  ITrackExtrapolator* m_extrapolator;
  const DeSTDetector* m_ttdetector ;
  bool m_useVeloPix;
  
} ;

template<class T>
inline bool LessThanFirst(const T& lhs, const T& rhs)
{
  return lhs.first < rhs.first ;
}
