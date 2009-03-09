// $Id: CaloHypo2Calo.cpp,v 1.2 2009-03-09 13:33:22 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
// from LHCb
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
// local
#include "CaloHypo2Calo.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CaloHypo2Calo
//
// 2008-09-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloHypo2Calo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloHypo2Calo::CaloHypo2Calo( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : Calo2Calo ( type, name , parent )
{
  declareInterface<ICaloHypo2Calo>(this);


  declareProperty("Seed"         , m_seed   = true );
  declareProperty("PhotonLine"   , m_line   = true);
  declareProperty("AddNeighbors" , m_neighb = true );
  declareProperty("xTolerance"   , m_x      =  5.*Gaudi::Units::mm);
  declareProperty("yTolerance"   , m_y      =  5.*Gaudi::Units::mm);
  declareProperty("StatusMask"   , m_status = 0x0  );
  declareProperty("WholeCluster" , m_whole  = false);

}
//=============================================================================
// Destructor
//=============================================================================
CaloHypo2Calo::~CaloHypo2Calo() {} 

//=============================================================================
StatusCode CaloHypo2Calo::initialize(){
  StatusCode sc = Calo2Calo::initialize();
  debug() << "Initialize CaloHypo2Calo tool " << endreq;
  m_lineID = LHCb::CaloCellID(); // initialize
  m_point  = Gaudi::XYZPoint();
  return sc;
}


std::vector<LHCb::CaloCellID> CaloHypo2Calo::cellIDs(LHCb::CaloHypo fromHypo, std::string toCalo){
  LHCb::CaloHypo::Clusters clusters = fromHypo.clusters();

  // get the smallest cluster (i.e. for merged photons)
  LHCb::CaloCluster* cluster = 0;
  unsigned int minSize = 999999;
  for(LHCb::CaloHypo::Clusters::iterator icl = clusters.begin();icl!=clusters.end();++icl){
    if( (*(*icl)).entries().size() < minSize)cluster = *icl;
  }

  if(cluster == 0){
    Error("No valid cluster!").ignore() ;
    return m_cells;
  }

  if(m_whole){
    return Calo2Calo::cellIDs( *cluster, toCalo);
  }


  m_point  = Gaudi::XYZPoint();
  m_lineID = LHCb::CaloCellID();
  if(m_line && fromHypo.position() != NULL){
    const Gaudi::XYZPoint  ref( fromHypo.position()->x(), fromHypo.position()->y(),fromHypo.position()->z() );
    const Gaudi::XYZVector vec = (ref-Gaudi::XYZPoint(0,0,0));
    const Gaudi::Plane3D   plane = m_det[toCalo]->plane(CaloPlane::ShowerMax);
    Gaudi::Math::XYZLine   line(ref, vec);
    double mu;
    Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,m_point,mu);
    m_lineID = m_det[toCalo]->Cell( m_point );
    if ( msgLevel( MSG::DEBUG) ) debug() << "Matching cell " << m_lineID << endreq;
  }


  return cellIDs( *cluster, toCalo);
}



std::vector<LHCb::CaloCellID> CaloHypo2Calo::cellIDs(LHCb::CaloCluster fromCluster, std::string toCalo){
  debug() << " toCalo " << toCalo << endreq;
  reset();
  LHCb::CaloCellID seedID = fromCluster.seed();
  if ( msgLevel( MSG::DEBUG) ) debug() << "-----  cluster energy " <<  fromCluster.e()<< " " << seedID << endreq;
  std::string fromCalo = CaloCellCode::CaloNameFromNum( seedID.calo() );
  m_neighbour.setDet (m_det[fromCalo] );

  //matching cluster
  for(LHCb::CaloCluster::Entries::const_iterator ent = fromCluster.entries().begin();
      ent != fromCluster.entries().end(); ++ent){
    LHCb::CaloClusterEntry entry = *ent;
    LHCb::CaloCellID cellID = entry.digit()->cellID();
    if( !(m_seed      &&  (LHCb::CaloDigitStatus::SeedCell & entry.status())    != 0 ) &&
        !(m_seed && m_neighb &&   m_neighbour(seedID , cellID)                       ) &&
        !( (m_status & entry.status()) != 0                                          ) &&
        !(m_whole) )continue;
    SmartRef<LHCb::CaloDigit> digit = (*ent).digit();
    Calo2Calo::cellIDs( (*digit).cellID() , toCalo, false );
    if ( msgLevel( MSG::DEBUG) )
      debug() << toCalo << ":  digit is selected in front of the cluster : " 
              << cellID << "/" << seedID << " " << m_digits.size() << endreq;
  }




  // photon line
  if(m_line ){
    if( m_lineID == LHCb::CaloCellID() ){
      const Gaudi::XYZPoint  ref( fromCluster.position().x(), fromCluster.position().y(),fromCluster.position().z() );
      const Gaudi::XYZVector vec = (ref-Gaudi::XYZPoint(0,0,0));
      const Gaudi::Plane3D   plane = m_det[toCalo]->plane(CaloPlane::ShowerMax);
      Gaudi::Math::XYZLine   line(ref, vec);
      double mu;
      Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,m_point,mu);
      m_lineID = m_det[toCalo]->Cell( m_point );
    }
    if( !(m_lineID == LHCb::CaloCellID()) ){
      Calo2Calo::cellIDs( m_lineID , toCalo, false ); // projecting the cell onto itself !
      if ( msgLevel( MSG::DEBUG) )debug() << toCalo << " : digit is selected in the photon line : " 
                                          << m_lineID << "/" << seedID << " " << m_digits.size() << endreq;
      if(m_neighb){
        std::vector<LHCb::CaloCellID>  neighbors = m_det[toCalo]->neighborCells( m_lineID );
        for( std::vector<LHCb::CaloCellID>::iterator n = neighbors.begin();n!=neighbors.end();n++){
          double halfCell = m_det[toCalo]->cellSize( *n )*0.5;
          const Gaudi::XYZPoint cellCenter = m_det[toCalo] -> cellCenter ( *n ) ;
          if( fabs(m_point.X() - cellCenter.X()) < (halfCell+m_x) &&
              fabs(m_point.Y() - cellCenter.Y()) < (halfCell+m_y) ){
            Calo2Calo::cellIDs( *n , toCalo, false ); // projecting the cell onto itself !
            if ( msgLevel( MSG::DEBUG) )debug() << toCalo << " : digit is selected in the photon line neighborhood : " 
                                               << *n << "/" << seedID  << " " << m_digits.size() << endreq;
          } 
        }
      }
    }
  }

  m_lineID = LHCb::CaloCellID();//reset
  return m_cells;
}



std::vector<LHCb::CaloDigit*> CaloHypo2Calo::digits(LHCb::CaloHypo fromHypo, std::string toCalo){  
  cellIDs( fromHypo, toCalo);
  return m_digits;
}  

double CaloHypo2Calo::energy(LHCb::CaloHypo fromHypo, std::string toCalo){
  cellIDs(fromHypo, toCalo);
  return m_energy;
}
std::vector<LHCb::CaloDigit*> CaloHypo2Calo::digits(LHCb::CaloCluster fromCluster, std::string toCalo){  
  cellIDs( fromCluster, toCalo);
  return m_digits;
}  

double CaloHypo2Calo::energy(LHCb::CaloCluster fromCluster, std::string toCalo){
  cellIDs(fromCluster, toCalo);
  return m_energy;
}

int CaloHypo2Calo::multiplicity(LHCb::CaloCluster fromCluster, std::string toCalo){
  cellIDs(fromCluster, toCalo);
  return m_count;
}
int CaloHypo2Calo::multiplicity(LHCb::CaloHypo fromHypo, std::string toCalo){
  cellIDs(fromHypo, toCalo);
  return m_count;
}

