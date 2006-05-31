// $Id: TrackSimpleExtraSelector.cpp,v 1.4 2006-05-31 12:50:57 erodrigu Exp $

#include "TrackSimpleExtraSelector.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

DECLARE_TOOL_FACTORY( TrackSimpleExtraSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackSimpleExtraSelector::TrackSimpleExtraSelector(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent):
  GaudiTool(type, name, parent)
{
  declareInterface<ITrackExtraSelector>( this );

  declareProperty( "ExtrapolatorName",
                   m_extraName = "TrackParabolicExtrapolator" );
}

//=============================================================================
// Destructor
//=============================================================================
TrackSimpleExtraSelector::~TrackSimpleExtraSelector() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackSimpleExtraSelector::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_extrapolator= tool<ITrackExtrapolator>( m_extraName, m_extraName, this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
ITrackExtrapolator* TrackSimpleExtraSelector::select( const double,
                                                      const double) const
{
  return m_extrapolator;
}

//=============================================================================
