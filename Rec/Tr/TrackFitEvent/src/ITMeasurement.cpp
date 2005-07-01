// $Id: ITMeasurement.cpp,v 1.4 2005-07-01 15:45:24 erodrigu Exp $
// Include files 

#include "STDet/STDetectionLayer.h"

// local
#include "Event/ITMeasurement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ITMeasurement
//
// 2005-04-07 : Jose Hernando, Eduardo Rodrigues
// Author: Rutger van der Eijk
// Created: 07-04-1999
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ITMeasurement::ITMeasurement( ITCluster& itCluster,
                              DeSTDetector& geom ) {

  m_mtype = Measurement::ST;
  
  m_cluster = &itCluster; //pointer to ITCluster

  ITChannelID ITChan = m_cluster->channelID();

  // set the LHCbID
  setLhcbID ( LHCbID( ITChan ) );

  const STDetectionLayer* ITLay = geom.layer( ITChan );
  m_measure      = ITLay->U( ITChan ) + m_cluster->distToStripCenter() ;
  m_errMeasure    = m_cluster->distToStripError();
  m_z            = ITLay->centerZ( ITChan );
//  m_stereoAngle  = ITLay->stereoAngle();

//  std::cout << "- stereo angle = " << ITLay->stereoAngle() << std::endl;

}

//=============================================================================
