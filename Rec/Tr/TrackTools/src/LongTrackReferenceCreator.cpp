// $Id: LongTrackReferenceCreator.cpp,v 1.15 2007-10-05 16:51:54 wouter Exp $

// from GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Measurement.h"
#include "Event/OTMeasurement.h"
#include "Event/StateTraj.h"
#include "Event/StateParameters.h"

// track tools
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"

// local
#include "LongTrackReferenceCreator.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( LongTrackReferenceCreator );

//=============================================================================
// 
//=============================================================================
LongTrackReferenceCreator::LongTrackReferenceCreator(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent):
  GaudiTool(type, name, parent)
{ 
  // constructer
  declareInterface<ITrackManipulator>(this);
  declareProperty( "SetLRAmbiguities", m_setLRAmbiguities = false  );
};

//=============================================================================
// 
//=============================================================================
LongTrackReferenceCreator::~LongTrackReferenceCreator(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode LongTrackReferenceCreator::initialize()
{

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // extrapolator
  m_extrapolator = tool<ITrackExtrapolator>("TrackFastParabolicExtrapolator");

  // Retrieve the magnetic field and the poca tool
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );
  m_poca = tool<ITrajPoca>( "TrajPoca" );

 
  return StatusCode::SUCCESS;
};

//=============================================================================
// 
//=============================================================================
StatusCode LongTrackReferenceCreator::execute(LHCb::Track& aTrack) const{

  LHCb::State vState;
  LHCb::State tState;
  // get the starting states in the Velo and T
  if ( aTrack.hasStateAt(LHCb::State::EndVelo) ) {
    vState = aTrack.stateAt(LHCb::State::EndVelo);
  }
  else {
    vState = aTrack.closestState( StateParameters::ZBegRich1 );
  }
  if ( fabs( vState.z() - StateParameters::ZBegRich1 ) > 1.5*Gaudi::Units::meter )
    return Warning( "No Velo State retrieved!", StatusCode::FAILURE );

  if ( aTrack.hasStateAt(LHCb::State::AtT) ) {
    tState = aTrack.stateAt(LHCb::State::AtT);
  }
  else {
    tState = aTrack.closestState( StateParameters::ZAtT );
    if( tState.z() < StateParameters::ZBegT || tState.z() > StateParameters::ZBegRich2 ) 
      return Warning( "No T State retrieved!", StatusCode::FAILURE );
  }
    
  // reset velo Q/p to T one
  vState.setQOverP(tState.qOverP());

  typedef std::vector<LHCb::Measurement*> MeasContainer;
  const MeasContainer& aCont = aTrack.measurements();

  if (aCont.size() == 0){
    return Warning("Tool called for track without measurements",
                   StatusCode::FAILURE);
  }

  MeasContainer::const_iterator iterM = aCont.begin();
  for ( ; iterM != aCont.end(); ++iterM) {
    
    if ( (*iterM)->type() == Measurement::IT  ||
         (*iterM)->type() == Measurement::OT ) {
      addReference(*iterM,tState);
    }
    else {
      addReference(*iterM,vState);
    }
    
  } //iterM

  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
void LongTrackReferenceCreator::addReference( LHCb::Measurement* meas, 
                                              LHCb::State& aState ) const
{
  // Get the measurement trajectory representing the centre of gravity
  StatusCode sc = m_extrapolator->propagate(aState,meas->z());
  if( sc.isFailure() ) {
    Warning("Extrapolation failed ", StatusCode::FAILURE, 1);
  }
  meas->setRefVector(aState.stateVector());

  // Add the L/R ambiguity
  if ( m_setLRAmbiguities && meas->type() == Measurement::OT ) {
    XYZVector distance;
    XYZVector bfield;
    m_pIMF -> fieldVector( aState.position(), bfield );
    StateTraj stateTraj = StateTraj( aState, bfield );
    double s1 = 0.0;
    double s2 = (meas->trajectory()).arclength( stateTraj.position(s1) );
    sc = m_poca->minimize(stateTraj, s1, meas->trajectory(),
                          s2, distance, 20*Gaudi::Units::mm);
    if( sc.isFailure() ) {
      warning() << "TrajPoca minimize failed in addReference." << endreq;
    }
    int ambiguity = ( distance.x() > 0.0 ) ? 1 : -1 ;

    OTMeasurement* otmeas = dynamic_cast<OTMeasurement*>(meas);
    otmeas->setAmbiguity( ambiguity );
  }

}

//=============================================================================
