// $Id: CaloGetterTool.cpp,v 1.3 2009-05-19 13:48:22 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "CaloGetterTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloGetterTool
//
// 2009-04-17 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloGetterTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloGetterTool::CaloGetterTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloGetterTool>(this);
  declareProperty("GetDigits"   , m_digiUpd = true);
  declareProperty("GetClusters" , m_clusUpd = false);
  declareProperty("GetHypos"    , m_hypoUpd = false);
  declareProperty("GetProviders", m_provUpd = false);
  declareProperty("DigitLocations"   , m_digiLoc );
  declareProperty("ClusterLocations" , m_clusLoc );
  declareProperty("HypoLocations"    , m_hypoLoc );


  // digits
  m_digiLoc.push_back( LHCb::CaloDigitLocation::Ecal );
  m_digiLoc.push_back( LHCb::CaloDigitLocation::Hcal );
  m_digiLoc.push_back( LHCb::CaloDigitLocation::Prs  );
  m_digiLoc.push_back( LHCb::CaloDigitLocation::Spd  );
  if("HLT"==context() || "Hlt" == context()){
    // clusters
    m_clusLoc.push_back( LHCb::CaloClusterLocation::EcalHlt );
    m_clusLoc.push_back( LHCb::CaloClusterLocation::EcalSplitHlt );
    m_clusLoc.push_back( LHCb::CaloClusterLocation::HcalHlt );
    // hypos
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::PhotonsHlt      );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::ElectronsHlt    );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::MergedPi0sHlt   );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::SplitPhotonsHlt );
  }else{
    // clusters
    m_clusLoc.push_back( LHCb::CaloClusterLocation::Ecal );
    m_clusLoc.push_back( LHCb::CaloClusterLocation::EcalSplit );
    m_clusLoc.push_back( LHCb::CaloClusterLocation::Hcal );
    // hypos
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::Photons      );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::Electrons    );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::MergedPi0s   );
    m_hypoLoc.push_back( LHCb::CaloHypoLocation::SplitPhotons );
  }

}
//=============================================================================
// Destructor
//=============================================================================
CaloGetterTool::~CaloGetterTool() {} 

//=============================================================================



StatusCode CaloGetterTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  debug() << "Initialize Calo2Calo tool " << endmsg;
  m_provider["Ecal"] = tool<ICaloDataProvider>( "CaloDataProvider" , "EcalDataProvider" );
  m_provider["Hcal"] = tool<ICaloDataProvider>( "CaloDataProvider" , "HcalDataProvider" );
  m_provider["Prs"]  = tool<ICaloDataProvider>( "CaloDataProvider" , "PrsDataProvider"  );
  m_provider["Spd"]  = tool<ICaloDataProvider>( "CaloDataProvider" , "SpdDataProvider"  );
  return StatusCode::SUCCESS;
}


void CaloGetterTool::update()
{
  // digits
  if ( m_digiUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_digiLoc.begin();m_digiLoc.end() != iloc; ++iloc)
    {
      if ( exist<LHCb::CaloDigits>(*iloc) ) 
      { 
        LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( *iloc );
        m_digits[ *iloc ] =  digits ;
        counter ("#Digits @ " + (*iloc) ) += digits->size() ;
      }
    }
  }
  // clusters
  if( m_clusUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_clusLoc.begin();m_clusLoc.end() != iloc; ++iloc)
    {
      if ( exist<LHCb::CaloClusters>(*iloc)) 
      {
        LHCb::CaloClusters* clusters = get<LHCb::CaloClusters>( *iloc );  
        m_clusters[ *iloc ] = clusters ;
        counter ("#Clusters @ " + (*iloc) ) += clusters->size() ;        
      }
    }
  }
  // hypos
  if( m_hypoUpd )
  {
    for(std::vector<std::string>::iterator iloc = m_hypoLoc.begin();m_hypoLoc.end() != iloc; ++iloc)
    {
      if ( exist<LHCb::CaloHypos>(*iloc) ) 
      {
        LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( *iloc );  
        m_hypos[ *iloc ] = hypos ;
        counter ("#Hypos @ " + (*iloc) ) += hypos->size() ;        
      }      
    }
  }  
  // provider
  if( m_provUpd)
  {
    for(std::map<std::string,ICaloDataProvider*>::iterator ip = m_provider.begin();m_provider.end()!=ip;++ip)
    {
      const std::string& det = ip->first;
      ICaloDataProvider* provider = ip->second;
      m_prov[det] = provider->getBanks();
    }    
  }
}
// ============================================================================
// The END 
// ============================================================================


 
  
