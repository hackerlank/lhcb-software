// $Id: OTMeasurement.cpp,v 1.2 2005-05-13 13:09:08 erodrigu Exp $
// Include files

// local
#include "Event/OTMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTMeasurement::OTMeasurement( OTTime& otTime,
                              DeOTDetector& geom,
                              int ambiguity,
                              double tu ) {

  m_tu        = tu;
  m_time      = &otTime;        // pointer to hit
  m_ambiguity = ambiguity;      // drift ambiguity

  // set the LHCbID
  setLhcbID ( LHCbID( m_time -> channel() ) );

  // some constants...
  double driftVelocity = geom.driftDelay(); // ns/mm
  // double wireVelocity  = geom.propagationDelay();      // ns/mm

  m_measure      = m_time->calibratedTime() * m_ambiguity;
  m_errMeasure   = geom.resolution() * driftVelocity;

  OTChannelID OTChan = m_time->channel();
  DeOTModule* module = geom.module( OTChan );
  HepPoint3D wirePos = module->centerOfStraw( OTChan.straw() );

  // double stereoAngle      = module->stereoAngle();
  // double mwirePos =  wirePos.x() * cos(stereoAngle) +
  //   wirePos.y() * sin(stereoAngle);
  m_z                = wirePos.z();
//  m_wireLength       = module->wireLength();

}
//=============================================================================
