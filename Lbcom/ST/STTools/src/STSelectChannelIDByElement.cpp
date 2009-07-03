// $Id: STSelectChannelIDByElement.cpp,v 1.2 2009-07-03 13:39:08 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Kernel/STChannelID.h"

#include "STSelectChannelIDByElement.h"
#include "STDet/DeSTDetector.h"

//boost
#include <boost/foreach.hpp>


DECLARE_TOOL_FACTORY( STSelectChannelIDByElement);
 
STSelectChannelIDByElement::STSelectChannelIDByElement( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  declareProperty("elementNames", m_elementNames);
  setForcedInit();
  declareInterface<ISTChannelIDSelector>(this);
}

StatusCode STSelectChannelIDByElement::initialize() {

  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  if (m_elementNames.size() == 0){
    info() << name() << "Empy list given: default to selecting all " << endmsg;
  }

  BOOST_FOREACH(std::string name, m_elementNames) {
    DeSTBaseElement* detElement = tracker()->findTopLevelElement(name);
    if (detElement == 0){
      return Error("Failed to find detector element",StatusCode::FAILURE); 
    }
    else {
      debug() << "Adding " <<  name << " to element list" << endmsg;     
      m_detElements.push_back(detElement);
    }
  } // for each
  
  return StatusCode::SUCCESS;
}

STSelectChannelIDByElement::~STSelectChannelIDByElement()
{
  //destructer
}

bool STSelectChannelIDByElement::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDByElement::operator()( const LHCb::STChannelID& id ) const{

  if (m_elementNames.size() == 0u) return true;

  std::vector<DeSTBaseElement*>::const_iterator iterElem = m_detElements.begin();
  for ( ; iterElem != m_detElements.end() ; ++iterElem){
    if ((*iterElem)->contains(id) == true) break;
  } 
  return (iterElem != m_detElements.end() ? true : false);
}
