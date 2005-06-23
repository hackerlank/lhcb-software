
/** @file RichMCTrackInfoTool.h
 *
 *  Header file for tool : RichMCTrackInfoTool
 *
 *  CVS Log :-
 *  $Id: RichMCTrackInfoTool.h,v 1.7 2005-06-23 15:08:50 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
#define RICHMCTOOLS_RICHMCTRACKINFOTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Event model
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

// Interfaces
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichMCTrackInfoTool RichMCTrackInfoTool.h
 *
 *  Tool providing tracking extrapolation information from
 *  RICH Monte Carlo data objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichMCTrackInfoTool : public RichToolBase,
                            virtual public IRichMCTrackInfoTool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMCTrackInfoTool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );


  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Takes the direction information from a MCRichSegment and ray traces it through the
  // appropriate RICH optical system and computes the intersect points with the HPD
  // panel in LHCb global coordinates
  const bool panelIntersectGlobal ( const MCRichSegment * segment,
                                    HepPoint3D & hitPoint ) const;

  // Takes the direction information from a MCRichSegment and ray traces it through the
  // appropriate RICH optical system and computes the intersect points with the HPD
  // panel in local HPD panel coordinates
  const bool panelIntersectLocal ( const MCRichSegment * segment, 
                                   HepPoint3D & hitPoint ) const;

private: // private data

  const IRichRayTracing * m_rayTrace;     ///< Pointer to ray tracing tool
  const IRichSmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

};

#endif // RICHMCTOOLS_RICHMCTRACKINFOTOOL_H
