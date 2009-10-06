#include "Gaucho/DimServiceMonObject.h"
#include "GaudiKernel/StatusCode.h"
#include "Gaucho/Misc.h"
#include <iostream>
#include <string>
#include <sstream>


DimServiceMonObject::DimServiceMonObject(const std::string& svcName, MonObject *monObject):DimService(svcName.c_str(),"C",0,0), m_monObject(monObject), m_msgSvc(0)
{
  setDataFromMonObject();
  m_name = "DimServiceMonObject";
}

DimServiceMonObject::~DimServiceMonObject() {
}

void DimServiceMonObject::updateService(bool endOfRun) {
  m_monObject->setEndOfRun(endOfRun);
  setDataFromMonObject();
}

void DimServiceMonObject::setDataFromMonObject() {
  std::stringstream os;
  boost::archive::binary_oarchive oa(os);
  try {
    m_monObject->save(oa, m_monObject->version());
    this->DimService::setData((void*)os.str().data(),os.str().length());
    this->DimService::updateService();
  }
  catch (...) {
    MsgStream msg(msgSvc(), name());
    msg << MSG::WARNING << "Unable to setdata from DimServiceMonObject. " << endreq;
    try {
      Misc::printSerializedString(os.str().data(),os.str().size());
    }
    catch (const std::exception &ex){
      msg << MSG::WARNING << "std::exception: " << ex.what() << endreq;
    }
    catch (...){
      msg << MSG::WARNING << "unrecognized exception. "<< endreq;
    }
  }
}

