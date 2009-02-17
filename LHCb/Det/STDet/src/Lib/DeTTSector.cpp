#include "STDet/DeTTSector.h"
#include "STDet/DeTTHalfModule.h"
#include "STDet/DeTTSensor.h"
#include "STDet/DeSTSensor.h"
#include "STDet/STDetFun.h"

#include "Kernel/TTNames.h"
#include "Kernel/STLexicalCaster.h"

/** @file DeTTSector.cpp
*
*  Implementation of class :  DeTTSector
*
*    @author Matthew Needham
*/

using namespace LHCb;

DeTTSector::DeTTSector( const std::string& name ) :
  DeSTSector( name )
{ 
  // constructer
}

DeTTSector::~DeTTSector() {
  // destructer
}

const CLID& DeTTSector::clID () const
{
  return DeTTSector::classID() ;
}

std::string DeTTSector::hybridType() const
{
  return m_hybridType;
}

StatusCode DeTTSector::initialize() {
  
  // initialize method
  MsgStream msg(msgSvc(), name() );

  StatusCode sc = DeSTSector::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq;
    return sc;
  }
  else {

    // get the parent
    m_parent = getParent<DeTTSector>();

    unsigned int tSize;
    m_parent->type() == "KLM" ? tSize = 3 : tSize = 2;

    // sub id
    const unsigned subID = param<int>("subID");

    // sector number needs info from mother
    if (m_parent->position() == "B"){
     setID(m_parent->firstSector()+subID-1);
    }
    else {
      setID(m_parent->firstSector() + tSize - subID);
    }
   
    // row..
    if (m_parent->type() == "KLM"){
      if (m_parent->position() == "B"){
        m_row = id() - m_parent->firstSector() + 1;
      }
      else {
        m_row = id() - m_parent->firstSector() + 4;
      }  
    } 
    else {
      if (m_parent->position() == "B"){
        m_row = id() - m_parent->firstSector() + 1;
      }
      else {
        m_row = id() - m_parent->firstSector() + 3;
      }
    }

    // build the id
    const STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeTT,parentID.station(),parentID.layer(), 
                     parentID.detRegion(),  id(), 0);
    setElementID(chan);

    // get the nickname
    m_nickname = TTNames().UniqueSectorToString(chan); 

    // get the attached sensors
    std::vector<DeTTSensor*> sensors = getChildren<DeTTSector>();
    std::sort(sensors.begin(),sensors.end(),STDetFun::SortByY());
    m_sensors.reserve(sensors.size());
    m_sensors.insert(m_sensors.begin(), sensors.begin(), sensors.end());
    m_thickness = m_sensors.front()->thickness();

    if (sensors.size() == 4)
    {
      m_hybridType = "L";
    }
    else if (sensors.size() == 3)
      m_hybridType = "K";
    else
      m_hybridType = "M";

    m_conditionPathName = TTNames().UniqueLayerToString(chan) + "LayerR"
      + ST::toString(chan.detRegion()) + "Module" + ST::toString(column())
      + position() + "Sector" + hybridType();

    sc = registerConditionsCallbacks();
    if (sc.isFailure()){
      msg << MSG::ERROR << "Failed to registerConditions call backs" << endmsg;
      return sc;
    }
    sc = cacheInfo();
    if (sc.isFailure()){
      msg << MSG::ERROR << "Failed to cache geometry" << endmsg;
      return sc;
    }

  }

  return StatusCode::SUCCESS;
}

unsigned int DeTTSector::prodID() const{
  return m_parent->prodID();
}

std::string DeTTSector::conditionsPathName() const
{
  return m_conditionPathName;
}

