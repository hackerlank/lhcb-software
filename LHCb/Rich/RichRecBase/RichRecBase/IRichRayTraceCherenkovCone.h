// $Id: IRichRayTraceCherenkovCone.h,v 1.1 2004-05-31 21:34:03 jonrob Exp $
#ifndef RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
#define RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H 1

// Event model
#include "Event/RichRecRing.h"
class RichRecSegment;

/** @class IRichRayTraceCherenkovCone IRichRayTraceCherenkovCone.h
 *
 *  Interface for tool to ray trace cherenkov photons to the detectors
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRayTraceCherenkovCone( "IRichRayTraceCherenkovCone", 1, 0 );

class IRichRayTraceCherenkovCone : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichRayTraceCherenkovCone; }

  /// Ray trace the Cherenkov cone for the given segment and mass hypothesis to the detector plane
  virtual StatusCode rayTrace ( RichRecSegment * segment,
                                const Rich::ParticleIDType id,
                                std::vector<HepPoint3D> & points ) const = 0;

  /// Ray trace the Cherenkov cone for the given segment and cherenkov angle to the detector plane
  virtual StatusCode rayTrace ( RichRecSegment * segment,
                                const double ckTheta,
                                std::vector<HepPoint3D> & points ) const = 0;

  /// Ray trace the Cherenkov cone for the given ring to the detector plane
  virtual std::vector<HepPoint3D> & rayTrace ( RichRecRing * ring ) const = 0;

};

#endif // RICHRECBASE_IRICHRAYTRACECHERENKOVCONE_H
