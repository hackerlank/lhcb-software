// $Id: TrajSTProjector.h,v 1.2 2006-02-27 19:56:04 jvantilb Exp $
#ifndef TRAJSTPROJECTOR_H 
#define TRAJSTPROJECTOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackProjector.h"

class IMagneticFieldSvc;
class ITrajPoca;

using namespace LHCb;

/** @class TrajSTProjector TrajSTProjector.h TrajSTProjector.h
 *  
 *  Projects into the ST measurement space
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-02-21
 */

class TrajSTProjector : public TrackProjector {

public: 

  /// Project a state onto a measurement.
  /// It returns the chi squared of the projection
  virtual StatusCode project( const State& state,
                              Measurement& meas );

  /// initialize
  virtual StatusCode initialize();

  /// Standard constructor
  TrajSTProjector( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent );
  /// Destructor
  virtual ~TrajSTProjector();

private:

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrajPoca*         m_poca; ///< Pointer to the ITrajPoca interface

};
#endif // TRAJSTPROJECTOR_H
