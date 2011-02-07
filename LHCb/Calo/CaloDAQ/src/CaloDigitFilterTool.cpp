// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// local
#include "CaloDigitFilterTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitFilterTool
//
// 2010-12-13 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloDigitFilterTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitFilterTool::CaloDigitFilterTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_maskMap(),
    m_mask( 0 ),
    m_offsets(),
    m_offsetsRMS(),
    m_digits(NULL),
    m_calo(NULL),
    m_caloName("None"),
    m_scalingMethod(0),
    m_scaling("None"),
    m_reset(true),
    m_scale(0){
  declareInterface<ICaloDigitFilterTool>(this);
  declareProperty("MaskMap", m_maskMap);
  declareProperty("UseCondDB" , m_useCondDB = true);
  declareProperty("EcalOffset", m_ecalOffset);
  declareProperty("HcalOffset", m_hcalOffset);
  declareProperty("PrsOffset" , m_prsOffset);
  declareProperty("EcalOffsetRMS", m_ecalOffsetRMS);
  declareProperty("HcalOffsetRMS", m_hcalOffsetRMS);
  declareProperty("PrsOffsetRMS" , m_prsOffsetRMS);
  declareProperty("ScalingMethod" , m_scalingMethod=0); // 0 : SpdMult ; 1 = nPV  (+10 for clusters)
  declareProperty("ScalingBin"    , m_scalingBin=50);
  declareProperty("ScalingMin"    , m_scalingMin=150.);
  m_maskMap["Default"]= CaloCellQuality::OfflineMask;
  declareProperty( "VertexLoc"   , m_vertLoc = "");
  declareProperty( "UsePV3D"     , m_usePV3D = false);

}
//=============================================================================
// Destructor
//=============================================================================
CaloDigitFilterTool::~CaloDigitFilterTool() {} 

//=============================================================================


StatusCode CaloDigitFilterTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by Calo2Dview
  if(m_vertLoc == "")m_vertLoc=(m_usePV3D) ? LHCb::RecVertexLocation::Velo3D :  LHCb::RecVertexLocation::Primary;

  // check
  for(std::map<std::string,int>::iterator i = m_maskMap.begin() ; m_maskMap.end() != i ; ++i){
    if( (i->second & CaloCellQuality::OfflineMask) == 0)
      Warning("OfflineMask flag is disabled for " + i->first + " - Are you sure ?",StatusCode::SUCCESS).ignore();  
  }
  // subscribe to incident Svc
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;


  return StatusCode::SUCCESS;
}


//-----------------------
bool CaloDigitFilterTool::setDet(std::string det){
  m_caloName = LHCb::CaloAlgUtils::CaloNameFromAlg(det); 
  m_calo = getDet<DeCalorimeter>( LHCb::CaloAlgUtils::DeCaloLocation( m_caloName ) );
  if(NULL == m_calo)return false;
  m_mask     = getMask( m_caloName );
  getOffsetMap( m_caloName );
  return true;
  
}

//-----------------------
void CaloDigitFilterTool::getOffsetMap(std::string det){
  m_offsets.clear();
  m_offsetsRMS.clear();
  if( m_useCondDB ){ 
    m_scalingMethod=m_calo->pileUpSubstractionMethod();
    m_scalingMin = m_calo->pileUpSubstractionMin();
    m_scalingBin = m_calo->pileUpSubstractionBin();
    if( m_scalingMethod < 0 )return;
    const CaloVector<CellParam>& cells = m_calo->cellParams();
    for( CaloVector<CellParam>::const_iterator ic = cells.begin();cells.end() != ic ; ++ic){
      LHCb::CaloCellID id = ic->cellID();
      if( !m_calo->valid( id ) || id.isPin() )continue;
      m_offsets[id] = ic->pileUpOffset();  
      m_offsetsRMS[id] = ic->pileUpOffsetRMS();  
    }
   }else{
    if( det == "Ecal" ){
      m_offsets = m_ecalOffset;
      m_offsetsRMS = m_ecalOffsetRMS;
    }
    else if( det == "Hcal" ){
      m_offsets = m_hcalOffset;
      m_offsetsRMS = m_hcalOffsetRMS;
    }
    else if( det == "Prs" ){
      m_offsets = m_prsOffset;
      m_offsetsRMS = m_prsOffsetRMS;
    }    
  }
}

//-----------------------
double CaloDigitFilterTool::getOffset(LHCb::CaloCellID id, int scale){
  std::map<LHCb::CaloCellID,double>::iterator  it = m_offsets.find( id );
  if( it == m_offsets.end() )return 0.;
  double ref =  it->second;
  if( 0 == scale)return 0;
  if( scale <= m_scalingMin)return 0;
  //  overlap probabilty (approximate)
  double ncells = 6016.;
  double rscale = scale/(1.-scale/ncells);
  if( 4*scale/ncells < 1)rscale = 0.5*ncells*(1.-sqrt(1.-4*scale/ncells));
  double offset = 0;
  double step = double(m_scalingBin);
  double bin = double(rscale - m_scalingMin);
  offset = bin/step * ref;
  m_offsetRMS = sqrt(bin/step)*ref;
  return offset ;
}

//-----------------------
int  CaloDigitFilterTool::getMask(std::string det){
  std::map<std::string,int>::iterator it = m_maskMap.find( det );
  if( it == m_maskMap.end() ){
    it == m_maskMap.find( "Default");
    if( it == m_maskMap.end() )return 0;
    else return it->second;
   }
  return it->second;
}
 
//-----------------------
void CaloDigitFilterTool::setMaskMap(std::map<std::string,int> maskMap){  
  m_maskMap = maskMap; 
}  

//-----------------------
int CaloDigitFilterTool::getScale(){
  if( !m_reset )return m_scale;
  m_reset = false;
  m_scale = 0;
  if( m_scalingMethod == 0 || m_scalingMethod == 10){ // SpdMult
    m_scaling = "SpdMult";
    m_scale = nSpd();
  }else if( m_scalingMethod == 1 || m_scalingMethod == 11 ) { // nPV
    m_scaling = "nPV";
    m_scale= nVertices();
  }
  m_scaling = "None";
  return m_scale;
}

double CaloDigitFilterTool::offset(LHCb::CaloCellID id){
  if( id.caloName() != m_caloName)setDet( id.caloName() );
  int scale = getScale();
  return getOffset( id , scale);
}

double CaloDigitFilterTool::offsetRMS(LHCb::CaloCellID id){
  offset(id);
  return m_offsetRMS;
}



//-----------------------
bool CaloDigitFilterTool::cleanDigits(std::string det, bool substr, bool mask){
  if( !setDet( det ) )return false;
  
  m_digits = NULL;
  std::string container =  LHCb::CaloAlgUtils::CaloDigitLocation( det );
  if( exist<LHCb::CaloDigits>( container ) )m_digits = get<LHCb::CaloDigits>( container );
  if( NULL == m_digits )return false;
  
  //
  int scale = getScale();
  counter("offset scale (" + m_scaling +")") += scale;
  m_nMask = 0;
  m_mOffs = 0.;
  int nOffs = 0;
  for(LHCb::CaloDigits::iterator it = m_digits->begin() ; m_digits->end()!=it ; ++it){
    LHCb::CaloDigit* digit = *it;
    if(NULL != digit)cleanDigit( digit , substr, scale , mask);
    nOffs++;
  }
  counter("Masked " + m_caloName + " digits") += m_nMask;
  double ave = (nOffs !=0) ? m_mOffs/double(nOffs) : 0;
  counter(m_caloName + " average offset in ADC (method = " + Gaudi::Utils::toString(m_scalingMethod)+")" ) -= ave;
  return true;
}

//-----------------------
void CaloDigitFilterTool::cleanDigit(LHCb::CaloDigit* digit, bool substr , int scale , bool mask){
  if(NULL == digit)return;
  LHCb::CaloCellID id = digit->cellID();

  // apply mask
  if( mask && m_mask !=0 && m_calo->hasQuality( id  , (CaloCellQuality::Flag) m_mask) ){
    digit->setE(0.);
    m_nMask++;
  }

  // apply offset to channel (method < 10)
  if(m_scalingMethod < 10 && substr && m_offsets.size() != 0){
    if( m_caloName == "Spd" )return;
    if( scale < 0)scale = getScale();
    double offset = getOffset( id, scale );
    if( 0. != offset ){
      double e = digit->e() - offset;
      if(  e < 0. ) e=0.;
      digit->setE( e );
      m_mOffs += offset/m_calo->cellGain( id ); // offset (in ADC)
    }
  }
}


  //================= Scale accessors

unsigned int CaloDigitFilterTool::nVertices(){
  int nVert = 0;
  if( !m_usePV3D){
    if( exist<LHCb::RecVertices>(m_vertLoc) ){
      LHCb::RecVertices* verts= get<LHCb::RecVertices>(m_vertLoc);
      if( NULL != verts){
        nVert = verts->size();   
        return nVert;
      }
    }
  }
  // try PV3D if explicitely requested or if RecVertices not found
  if(!m_usePV3D)m_vertLoc = LHCb::RecVertexLocation::Velo3D;
  if( exist<LHCb::VertexBases>( m_vertLoc ) ){
    LHCb::VertexBases* verts= get<LHCb::VertexBases>( m_vertLoc );
    if( NULL != verts)nVert = verts->size();
  }
  return nVert;
}
unsigned int CaloDigitFilterTool::nSpd(){
  std::string loc =   LHCb::CaloAlgUtils::CaloDigitLocation( "SPD" );
  if( !exist<LHCb::CaloDigits>(loc) )return 0;
  const LHCb::CaloDigits* digits = get<LHCb::CaloDigits> (loc );
  return (NULL != digits) ? digits->size() : 0;
};




StatusCode CaloDigitFilterTool::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  return GaudiTool::finalize();  // must be called after all other actions  
}
