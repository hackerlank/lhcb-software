#ifndef TRACKPROJECTORS_TRACKMASTERPROJECTOR_H 
#define TRACKPROJECTORS_TRACKMASTERPROJECTOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackProjector.h"            // Interface

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

/** @class TrackMasterProjector TrackMasterProjector.h TrackProjectors/TrackMasterProjector.h
 *
 *  A TrackMasterProjector is the master projector that delegates the projection
 *  to the "measurement-type" projector.
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-03-12
 */
class TrackMasterProjector : public GaudiTool,
                             virtual public ITrackProjector {
public:
  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  StatusCode project( const State& state,
                      Measurement& meas );
  
  /// Retrieve the projection matrix H of the (last) projection
  const HepVector& projectionMatrix() const;

  /// Retrieve the chi squared of the (last) projection
  double chi2() const;

  /// Retrieve the residual of the (last) projection
  double residual() const;

  /// Retrieve the error on the residual of the (last) projection
  double errResidual() const;

  /// Standard constructor
  TrackMasterProjector( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent );

  virtual ~TrackMasterProjector( ); ///< Destructor

  StatusCode initialize();

protected:

  StatusCode selectProjector( unsigned int type );

private:

  std::string m_OTProjName;
  std::string m_ITProjName;
  std::string m_VeloRProjName;
  std::string m_VeloPhiProjName;

  std::map<unsigned int,ITrackProjector*> m_projectors;

  unsigned int m_selectedMeasType;

  ITrackProjector*  m_selectedProjector;
};
#endif // TRACKPROJECTORS_TRACKMASTERPROJECTOR_H
