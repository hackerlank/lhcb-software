// $Id: IRichExpectedTrackSignal.h,v 1.2 2003-10-13 16:10:53 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
#define RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H 1

// Event model
class RichRecSegment;
class RichRecTrack;
class RichPID;

// RichKernel
#include "RichKernel/RichParticleIDType.h"
#include "RichKernel/RichRadiatorType.h"

/** @class IRichExpectedTrackSignal IRichExpectedTrackSignal.h
 *
 *  Interface for tool to calculate the expected signals for given 
 *  RichRecSegments and RichRecTracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichExpectedTrackSignal( "IRichExpectedTrackSignal", 1, 0 );

class IRichExpectedTrackSignal : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichExpectedTrackSignal;}

  /// Expected number of signal photons for given track and hypothesis
  virtual double nSignalPhotons ( RichRecTrack * track,
                                  const Rich::ParticleIDType id ) = 0;

  /// Expected number of signal photons for given segment and hypothesis
  virtual double nSignalPhotons ( RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal photons for given track and hypothesis
  virtual double nObservableSignalPhotons ( RichRecTrack * track,
                                            const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal photons for given segment and hypothesis
  virtual double nObservableSignalPhotons ( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) = 0;

  /// Expected number of scattered photons for given track and hypothesis
  virtual double nScatteredPhotons ( RichRecTrack * track,
                                     const Rich::ParticleIDType id ) = 0;

  /// Expected number of scattered photons for given segment and hypothesis
  virtual double nScatteredPhotons ( RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable scattered photons for given track and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecTrack * track,
                                               const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable scattered photons for given segment and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal+scattered photons for given track and hypothesis
  virtual double nTotalObservablePhotons ( RichRecTrack * track,
                                           const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  virtual double nTotalObservablePhotons ( RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) = 0;

  /// Expected emitted photons for given track and hypothesis
  virtual double nEmittedPhotons ( RichRecTrack * track,
                                   const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis
  virtual double nEmittedPhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecTrack * track,
                                      const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) = 0;


  /// The average energy of signal Cherenkov photons
  virtual double avgSignalPhotEnergy( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) = 0;

  /// The average energy of emitted Cherenkov photons
  virtual double avgEmitPhotEnergy( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) = 0;

  /// Is it possible for this track to give Rich information
  virtual bool hasRichInfo( RichRecTrack * track ) = 0;

  /// Is it geometrically possible for this segment to give Rich information
  virtual bool hasRichInfo( RichRecSegment * segment ) = 0;


  /// Is this track above threshold for a given particle type in any radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type ) = 0;
  
  /// Is this track above threshold for a given particle type in a given radiator
  virtual bool aboveThreshold( RichRecTrack * track,
                               const Rich::ParticleIDType type,
                               const Rich::RadiatorType radiator ) = 0;
  
  /// Is this segment above threshold for a given particle hypothesis
  virtual bool aboveThreshold( RichRecSegment * segment,
                               const Rich::ParticleIDType type ) = 0;
  
  /// Set the threshold information in a RichPID object for given segment
  virtual void setThresholdInfo( RichRecSegment * segment,
                                 RichPID * pid ) = 0;

  /// Set the threshold information in a RichPID object for given track
  virtual void setThresholdInfo( RichRecTrack * track,
                                 RichPID * pid ) = 0;

  /// Is this track active in given radiator for given particle id
  virtual bool activeInRadiator( RichRecTrack * track,
                                 const Rich::RadiatorType rad,
                                 const Rich::ParticleIDType id =
                                 Rich::Electron ) = 0;

};

#endif // RICHRECTOOLS_IRICHEXPECTEDTRACKSIGNAL_H
