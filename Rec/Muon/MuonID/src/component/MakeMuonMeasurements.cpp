// Include files

// local
#include "MakeMuonMeasurements.h"
#include "SmartMuonMeasProvider.h"
#include "Event/MuonCoord.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MakeMuonMeasurements
//
// 2008-07-16 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MakeMuonMeasurements )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MakeMuonMeasurements::MakeMuonMeasurements( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  // Use or not uncrossed logical channels in the non-pad detector areas
  declareProperty("UseUncrossed",m_use_uncrossed);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode MakeMuonMeasurements::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider",this);
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");

  if (msgLevel(MSG::DEBUG) ) debug()<<"m_use_uncrossed="<<m_use_uncrossed<<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MakeMuonMeasurements::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  SmartMuonMeasProvider* muprov = new SmartMuonMeasProvider();
  put(muprov,"Rec/Muon/SmartMuonMeasProvider");

  if ( msgLevel(MSG::DEBUG) ) debug()<<"cleared m_muonProvider"<<endmsg;

  for (const auto& c : *get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords) ){

    const LHCb::MuonTileID tile = c->key();

    if ( msgLevel(MSG::DEBUG) ) debug() << " MuonCoord " << tile << endmsg;

    if ((m_mudet->mapInRegion(tile.station(),tile.region() ) != 1)
        &&  (c->uncrossed()) && (!m_use_uncrossed)) {

        if (msgLevel(MSG::DEBUG) ) debug()<<"skipping hit!"<<endmsg;
        continue;
    }

    LHCb::LHCbID id = LHCb::LHCbID(tile);

    int istation = id.muonID().station();
    if (istation == 0) continue;

    std::unique_ptr<LHCb::Measurement> xmeas{  m_measProvider->measurement(id,0) };
    std::unique_ptr<LHCb::Measurement> ymeas{  m_measProvider->measurement(id,1) };

    muprov->atStation(istation).emplace_back( std::move(xmeas), std::move(ymeas) );

  }

  if ( msgLevel(MSG::DEBUG) )
    for (int i = 0; i <5; i++) {
      debug() << " measurements at station " << i <<" is "
              << muprov->atStation(i).size() << endmsg;
    }

  return StatusCode::SUCCESS;
}

//=============================================================================
