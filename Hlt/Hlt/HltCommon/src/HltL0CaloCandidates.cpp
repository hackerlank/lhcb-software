// $Id: HltL0CaloCandidates.cpp,v 1.19 2009-09-30 09:15:14 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <memory>
// local
#include "HltL0CaloCandidates.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0CaloCandidates
// 2011-04-14  Albert Puig Navarro (albert.puig@cern.ch)
// based on the older implementation by Gerhard Raven
// which was based on HltL0CaloPrepare by Jose Angel Hernando Morata
// based on the ideas of the HltIsPhotonTool by M.Witek
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0CaloCandidates )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0CaloCandidates::HltL0CaloCandidates( const std::string& name,
                                          ISvcLocator* pSvcLocator )
    : GaudiHistoAlg( name, pSvcLocator )
{
    declareProperty( "L0DULocation", m_l0Location = L0DUReportLocation::Default );
    declareProperty( "TESOutputPrefix", m_TESprefix = "/Event/Rec/Calo" );
    declareProperty( "EtThresholdLow", m_lowEtThreshold = 500 );
    declareProperty( "EtThresholdHigh", m_highEtThreshold = 2500 );
    declareProperty( "ClusterizationLevel", m_level = 2 );
    declareProperty( "L0CaloCandidateLocation",
                     m_l0loc = L0CaloCandidateLocation::Full );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0CaloCandidates::~HltL0CaloCandidates()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0CaloCandidates::initialize()
{
    StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    // Calo clusterization interface
    m_l02CaloTool =
        tool<IL0Calo2Calo>( "L0Calo2CaloTool", "ClusterLowPhoton", this );
    // Gaudi::Utils::setProperty(m_l02CaloTool, "OutputLevel", 3).ignore() ;

    if ( !m_l02CaloTool ) {
        error() << "Error retrieving L02Calo tool" << endmsg;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltL0CaloCandidates::execute()
{
    // Create containers
    // Loop over candidates
    double etMax = -1.;
    const L0CaloCandidates* candidates = get<L0CaloCandidates>( m_l0loc );

    // counters
    int nc = 0;
    // create cluster containers and put on TES
    LHCb::CaloCluster::Container* cont_e = new LHCb::CaloCluster::Container();
    put( cont_e, m_TESprefix + "/LowEtElectrons" );
    LHCb::CaloCluster::Container* cont_g = new LHCb::CaloCluster::Container();
    put( cont_g, m_TESprefix + "/LowEtPhotons" );
    LHCb::CaloCluster::Container* cont_h = new LHCb::CaloCluster::Container();
    put( cont_h, m_TESprefix + "/HighEtPhotons" );

    // loop over l0calocandidates
    if ( candidates->empty() ) return StatusCode::SUCCESS;

    std::vector<LHCb::CaloCellID> ids;

    // no longer needed (done when producing the candidate on TES)
    // std::stable_sort   ( candidates->begin(),candidates->end(),SortL0CaloByEt());

    for( const L0CaloCandidate * calo: *candidates ) {
        if ( calo->type() != L0DUBase::CaloType::Electron &&
             calo->type() != L0DUBase::CaloType::Photon )  continue;

        nc++;


        // Check the cuts
        bool passHigh = L0CaloCandidateCut(
            (L0DUBase::CaloType::Type)calo->type(), m_highEtThreshold )( calo );
        bool passLow = L0CaloCandidateCut(
            (L0DUBase::CaloType::Type)calo->type(), m_lowEtThreshold )( calo );
        if ( !passLow ) continue; // passLow => passHigh
        // Fine, we got a candidate
        if ( msgLevel( MSG::DEBUG ) )
            debug() << "-> Accepted calo candidate with type = " << calo->type()
                    << " and et = " << calo->et()
                    << " and etcode = " << calo->etCode() << " and id =" << calo->id()
                    << endmsg;
        etMax = std::max( etMax, calo->et() );


        // Clusterize
        std::vector<CaloCluster*> clusters;
        m_l02CaloTool->clusterize( clusters, calo, m_level );
        for ( CaloCluster* c : clusters ) {

            if ( msgLevel( MSG::DEBUG ) )
                debug() << "Cluster " << c->e() << " " << c->seed()
                        << endmsg;
            //
            if ( !checkID( c->seed(), ids ) ) continue;

            // LowEt photon clusters from L0g+L0e
            cont_g->insert( c );

            if ( msgLevel( MSG::DEBUG ) )
                debug() << "Inserted  " << clusters.size()
                        << " lowET photons clusters" << endmsg;

            // highEt photon clusters from L0g+L0e
            if ( passHigh ) {
                cont_h->insert( c );
                if ( msgLevel( MSG::DEBUG ) )
                    debug() << "Inserted  " << clusters.size()
                            << " highET photons clusters" << endmsg;
            }
            // electron clusters from L0e only
            if ( calo->type() == L0DUBase::CaloType::Electron ) {
                cont_e->insert( c );
                if ( msgLevel( MSG::DEBUG ) )
                    debug() << "Inserted  " << clusters.size()
                            << " lowET electrons clusters" << endmsg;
            }
        }
    }

    int ne = cont_e->size();;
    int ng = cont_g->size();
    int nh = cont_h->size();

    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "-> Put " << nh << " clusters in TES -> "
                << m_TESprefix + "/HighEtPhotons from " << nc << " L0CaloCandidates"
                << endmsg;
        debug() << "-> Put " << ng << " clusters in TES -> "
                << m_TESprefix + "/LowEtPhotons from " << nc << " L0CaloCandidates"
                << endmsg;
        debug() << "-> Put " << ne << " clusters in TES -> "
                << m_TESprefix + "/LowEtElectrons from " << nc << " L0CaloCandidates"
                << endmsg;
    }

    counter( "#L0CaloCandidates" ) += nc;
    counter( "#L0Calo->LowEt photons" ) += ng;
    counter( "#L0Calo->LowEt Electrons" ) += ne;
    counter( "#L0Calo->HighEt photons" ) += nh;

    // setFilterPassed(true);
    return StatusCode::SUCCESS;
}

//=============================================================================
bool HltL0CaloCandidates::checkID( LHCb::CaloCellID id,
                                   std::vector<LHCb::CaloCellID>& ids )
{
    bool none = std::none_of( std::begin( ids ), std::end( ids ),
                           [&]( const LHCb::CaloCellID& i ) {
        return  id.area() == i.area() && 
                abs( id.col() - i.col() ) <= 1 &&
                abs( id.row() - i.row() ) <= 1 ;
    } );
    ids.push_back( id );
    return none;
}
