
#include "STDet/DeITLayer.h"
#include "STDet/DeITLadder.h"

/** @file DeITLadder.cpp
*
*  Implementation of class : DeITLadder
*
*    @author Matthew Needham
*/

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;
using namespace LHCb;

DeITLadder::DeITLadder( const std::string& name ) :
  DeSTBaseElement( name ),
  m_child(0)
{ 
  // constructer
}

DeITLadder::~DeITLadder() {

}

const CLID& DeITLadder::clID () const
{
  return DeITLadder::classID() ;
}

StatusCode DeITLadder::initialize() {

  // initialize
  MsgStream msg(msgSvc(), name() );
  
  StatusCode sc = DeSTBaseElement::initialize();
  if (sc.isFailure() ){
    msg << MSG::ERROR << "Failed to initialize detector element" << endreq; 
  }
  else {
    // get parent
    m_id = param<int>("sectorID");
    m_parent = getParent<DeITLadder>(); 
    STChannelID parentID = m_parent->elementID();
    STChannelID chan(STChannelID::typeIT,
                     parentID.station(),parentID.layer(),
                     m_id, 0, 0);
    setElementID(chan);

    std::vector<child_type*> tVector = getChildren<DeITLadder>();
    if (tVector.size() !=0) {
      m_child = tVector.front();
    }
    else {
      msg << MSG::ERROR << "No child sector found - failed to init" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return sc;
}

std::ostream& DeITLadder::printOut( std::ostream& os ) const{

  // stream to cout
  os << " Box : "  << name() 
     << " id " << id() 
     << std::endl;

  return os;
}

MsgStream& DeITLadder::printOut( MsgStream& os ) const{

  // stream to Msg service
  os << " Box : "  << name() 
     << " id " << id() 
     << std::endl;

  return os;
}





