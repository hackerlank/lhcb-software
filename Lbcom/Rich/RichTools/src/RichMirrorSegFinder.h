// $Id: RichMirrorSegFinder.h,v 1.3 2004-07-15 15:44:40 jonrob Exp $
#ifndef RICHTOOLS_RICHMIRRORSEGFINDER_H
#define RICHTOOLS_RICHMIRRORSEGFINDER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichMirrorSegFinder.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// RichKernel
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichFlatMirror.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichMirrorSegFinder RichMirrorSegFinder.h
 *
 *  Tool to find the appropriate mirror segment for a given reflection point
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
class RichMirrorSegFinder : public RichToolBase,
                            virtual public IRichMirrorSegFinder {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMirrorSegFinder( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~RichMirrorSegFinder( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // Locates the spherical mirror Segment given a reflection point,
  // RICH identifier and panel
  DeRichSphMirror* findSphMirror( const Rich::DetectorType rich,
                                  const Rich::Side side,
                                  const HepPoint3D& reflPoint ) const;

  // Locates the flat mirror Segment given a reflection point,
  // RICH identifier and panel
  DeRichFlatMirror* findFlatMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const HepPoint3D& reflPoint ) const;

private:

  enum mirrorType {sph = 0, flat };

  // mirror collections
  DeRichSphMirror* m_sphMirrors[2][2][30];
  DeRichFlatMirror* m_flatMirrors[2][2][30];

  unsigned int m_lastMirror[2][2][2];

};
#endif // RICHTOOLS_RICHMIRRORSEGFINDER_H
