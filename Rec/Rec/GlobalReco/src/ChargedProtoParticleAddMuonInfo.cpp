
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddMuonInfo.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleAddMuonInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddMuonInfo.cpp,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoParticleAddMuonInfo.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoParticleAddMuonInfo );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleAddMuonInfo::
ChargedProtoParticleAddMuonInfo( const std::string& name,
                                 ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

  // context specific locations
  if      ( context() == "Offline" )
  {
    m_muonPath   = LHCb::MuonPIDLocation::Offline;
    m_protoPath  = LHCb::ProtoParticleLocation::Charged;
  }
  else if ( context() == "HLT" || context() == "Hlt" )
  {
    m_muonPath   = LHCb::MuonPIDLocation::Hlt;
    m_protoPath  = LHCb::ProtoParticleLocation::HltCharged;
  }

  // Muon data
  declareProperty( "InputMuonPIDLocation", m_muonPath );

  // ProtoParticles
  declareProperty( "ProtoParticleLocation", m_protoPath );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleAddMuonInfo::~ChargedProtoParticleAddMuonInfo() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoParticleAddMuonInfo::execute()
{
  // Load the MuonPIDs
  const bool muonSc = getMuonData();
  if ( !muonSc )
  {
    return Warning( "No MuonPIDs -> ProtoParticles will not be changed.", StatusCode::SUCCESS );
  }

  // ProtoParticle container
  if ( !exist<LHCb::ProtoParticles>(m_protoPath) )
  {
    return Warning( "No existing ProtoParticle container at " +  m_protoPath + " thus do nothing.",
                    StatusCode::SUCCESS );
  }
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>(m_protoPath);

  // Loop over proto particles
  for ( LHCb::ProtoParticles::iterator iProto = protos->begin();
        iProto != protos->end(); ++iProto )
  {
    // replace the muon information
    updateMuon(*iProto);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Replace MUON info to the protoparticle
//=============================================================================
void ChargedProtoParticleAddMuonInfo::updateMuon( LHCb::ProtoParticle * proto ) const
{
  // Erase current MuonPID information
  proto->removeMuonInfo();

  // Does this track have a MUON PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( proto->track() );
  if ( m_muonMap.end() == iM ) return;

  // MuonPID object is found
  const LHCb::MuonPID * muonPID = (*iM).second;

  // MuonPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found MuonPID data : MuLL=" <<  muonPID->MuonLLMu()
              << " BkLL=" <<  muonPID->MuonLLBg()
              << " nSharedHits=" << muonPID->nShared()
              << " isMuonLoose=" << muonPID->IsMuonLoose()
              << " isMuon=" << muonPID->IsMuon()
              << endmsg;
  }

  // store acceptance flag for those in acceptance (lack of flag signifies
  // track was outside acceptance)
  if ( muonPID->InAcceptance() )
  {
    proto->addInfo( LHCb::ProtoParticle::InAccMuon, true );
  }

  // check Loose IsMuon flag - Reject non-muons
  if ( !muonPID->IsMuonLoose() ) return;

  // reference to MuonPID object
  proto->setMuonPID( muonPID );

  // Store the PID info
  proto->addInfo( LHCb::ProtoParticle::MuonMuLL,      muonPID->MuonLLMu() );
  proto->addInfo( LHCb::ProtoParticle::MuonBkgLL,     muonPID->MuonLLBg() );
  proto->addInfo( LHCb::ProtoParticle::MuonNShared,   muonPID->nShared()  );

  // Store History
  proto->addInfo( LHCb::ProtoParticle::MuonPIDStatus, muonPID->Status()   );

}

//=============================================================================
// Loads the Muon data
//=============================================================================
bool ChargedProtoParticleAddMuonInfo::getMuonData()
{
  // empty the map
  m_muonMap.clear();

  // Do we have any MuonPID results
  if ( !exist<LHCb::MuonPIDs>(m_muonPath) )
  {
    Warning( "No MuonPIDs at '"+m_muonPath+"'", StatusCode::SUCCESS );
    return false;
  }

  // yes, so load them
  const LHCb::MuonPIDs * muonpids = get<LHCb::MuonPIDs>( m_muonPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << muonpids->size()
            << " MuonPIDs from " << m_muonPath << endmsg;
  }

  // refresh the reverse mapping
  for ( LHCb::MuonPIDs::const_iterator iM = muonpids->begin();
        iM != muonpids->end(); ++iM )
  {
    m_muonMap[ (*iM)->idTrack() ] = *iM;
  }

  return true;
}
