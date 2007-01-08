// $Id: CaloElectron.cpp,v 1.4 2007-01-08 09:44:16 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// LHCb
//#include "CaloUtils/CaloMomentum.h"
#include "Kernel/Plane3DTypes.h"
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// local
#include "CaloElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloElectron
//
// 2006-11-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloElectron );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectron::CaloElectron( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_status         ( false)
    , m_particle       ( NULL )
    , m_proto          ( NULL )
    , m_track          ( NULL )
    , m_electron       ( NULL )
    , m_bremstrahlung  ( NULL )
    , m_calopos        ( NULL )
    , m_calo (){
  declareInterface<ICaloElectron>(this);
  declareProperty ( "ExtrapolatorType" , m_extrapolatorType = "TrackMasterExtrapolator" ) ;
  declareProperty ( "Tolerance"        , m_tolerance = 0.01 ) ;
  declareProperty ( "zOffset"          , m_zOffset   = 0. ) ; // Should be 0.0 if ShowerMax plane is correctly defined in condDB
  declareProperty ( "Detector"         , m_det   = DeCalorimeterLocation::Ecal );
}
//=============================================================================
// Destructor
//=============================================================================
CaloElectron::~CaloElectron() {} 

//=============================================================================
StatusCode CaloElectron::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  m_calo = getDet<DeCalorimeter>( m_det ) ;
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  return StatusCode::SUCCESS;  
}

//=============================================================================
bool CaloElectron::setParticle(const LHCb::Particle* particle){
  m_status = setting(particle);
  debug() << " CaloElectron setting " << m_status << endreq;
  return m_status;
}


//=============================================================================
const LHCb::CaloHypo* CaloElectron::electron(){
  return m_electron;
}

//=============================================================================
const LHCb::CaloMomentum CaloElectron::bremstrahlung(){
  if(!m_status || NULL == m_bremstrahlung)return LHCb::CaloMomentum();
  const LHCb::CaloMomentum bremPhoton(   m_bremstrahlung ,
                                   m_particle->referencePoint(), 
                                   m_particle->posCovMatrix() );
  return bremPhoton;
}

//=============================================================================
double CaloElectron::e(){
  if( !m_status )return 0.;
  return m_electron->e() ;
}

//=============================================================================
double CaloElectron::eOverP(){
  if( !m_status )return 0.;
  return m_electron->e()/m_track->p();
}

//=============================================================================
LHCb::State CaloElectron::caloState(CaloPlane::Plane plane , double deltaShower ){
  LHCb::State state; // empty state
  if( !m_status ) return state;

  // get caloPlane
  ROOT::Math::Plane3D refPlane = m_calo->plane( plane );
  // propagate state to refPlane
  LHCb::State calostate(m_track->closestState( refPlane ));
  LHCb::ParticleID pid(m_particle->particleID());
  StatusCode sc = m_extrapolator->propagate ( calostate, refPlane , m_tolerance , pid);
  if(sc.isFailure())return state;  

  if( 0. == deltaShower)return calostate; 
  
  Gaudi::XYZVector dir (calostate.tx(), calostate.ty(), 1.);
  Gaudi::XYZPoint  point = calostate.position() + deltaShower * dir/dir.R();
  // extrapolate to the new point
  sc = m_extrapolator->propagate ( calostate, point.z() , pid);
  if(sc.isFailure())return state;
  return calostate;

}

//=============================================================================
LHCb::State CaloElectron::closestState(std::string toWhat ){

  LHCb::State state; // empty state
  if( !m_status ) return state;

  // get state on Front of Ecal
  LHCb::State calostate = caloState(CaloPlane::Front);
  if(calostate.z() == 0 ) return state;

  // get hypo position
  double x = 0. ;
  double y = 0. ;

  if( "hypo" == toWhat ){
    x = m_calopos->parameters()(LHCb::CaloPosition::X);
    y = m_calopos->parameters()(LHCb::CaloPosition::Y);
  }else if("cluster" == toWhat ){
    x = m_calopos->center()(LHCb::CaloPosition::X);
    y = m_calopos->center()(LHCb::CaloPosition::Y);
  }else{
    return state;
  }
  
  // get frontPlane
  ROOT::Math::Plane3D frontPlane = m_calo->plane( CaloPlane::Front );

  // Define calo line (from transversal barycenter) and track line in Ecal
  typedef Gaudi::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line;
  Gaudi::XYZVector normal = frontPlane.Normal();
  double zEcal = ( -normal.X()*x -normal.Y()*y - frontPlane.HesseDistance() )/normal.Z(); // tilt
  Gaudi::XYZPoint point( x , y , zEcal );
  Line cLine( point ,frontPlane.Normal() );
  Line tLine( calostate.position() , calostate.slopes() );

  // Find points of closest distance between calo Line and track Line
  Gaudi::XYZPoint cP,tP;
  Gaudi::Math::closestPoints<Line,Gaudi::XYZPoint>(cLine,tLine,cP,tP);

  // propagate the state the new Z of closest distance 
  LHCb::ParticleID pid(m_particle->particleID());
  StatusCode sc = m_extrapolator->propagate ( calostate, tP.Z() , pid);
  if(sc.isFailure())return state;
  return calostate;
}


//=============================================================================
double CaloElectron::showerZ(CaloPlane::Plane refPlane , std::string toWhat ){
  LHCb::State theState = closestState( toWhat );
  LHCb::State refState = caloState( refPlane );
  //  if( theState.z() == 0.|| refState.z() == 0.)return 0.;
  return m_zOffset + theState.z() - refState.z();
}

//=============================================================================
double CaloElectron::showerDepth(CaloPlane::Plane refPlane , std::string toWhat ){
  LHCb::State theState = closestState( toWhat );
  LHCb::State refState = caloState( refPlane );
  //if( theState.z() == 0. || refState.z() == 0.)return 0.;
  Gaudi::XYZVector depth = theState.position()-refState.position();
  return depth.R();
}



//=============================================================================
bool CaloElectron::setting(const LHCb::Particle* particle){

  m_particle = NULL;
  m_proto = NULL;
  m_track = NULL;
  m_electron = NULL;
  m_bremstrahlung = NULL;
  m_calopos = NULL;

  if( NULL == particle)return false;

  m_particle = particle;
  m_proto   = m_particle -> proto();
  if( NULL == m_proto)return false;  
  m_track   = m_proto->track();
  if( NULL == m_track) return false;
  SmartRefVector<LHCb::CaloHypo> hypos = m_proto->calo();
  if(0 == hypos.size())return false;

  for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo =  hypos.begin(); ihypo != hypos.end() ; ++ihypo){
    const LHCb::CaloHypo* hypo =  *ihypo;
    if(NULL == hypo)continue;
    if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ) m_electron      = (LHCb::CaloHypo*)  hypo;
    if( LHCb::CaloHypo::Photon ==   hypo->hypothesis() )  m_bremstrahlung = (LHCb::CaloHypo*) hypo;
  }
  if( NULL == m_electron )return false; // Electron hypo is mandatory - brem. not
  m_calopos = m_electron->position();
  if( NULL == m_calopos) return false;
  return true;  
}

