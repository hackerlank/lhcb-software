//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/XmlBaseDetElemCnv.cpp,v 1.5 2001-06-20 07:48:42 sponce Exp $

// include files
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"
#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlCnvException.h"
#include "DetDesc/XmlBaseDetElemCnv.h"
#include "DetDesc/IXmlSvc.h"


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, XmlBaseDetElemCnv::classID()) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlBaseDetElemCnv::XmlBaseDetElemCnv (ISvcLocator* svc,
                                              const CLID& clsID ) :
  XmlGenericCnv (svc, clsID) {
  m_doGenericCnv = false;
}


// -----------------------------------------------------------------------
// Initialize the converter
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::initialize() {
  StatusCode sc = XmlGenericCnv::initialize();
  if (sc.isSuccess()) {
    MsgStream log (msgSvc(), "XmlDetElemCnv");
    log << MSG::VERBOSE << "Initializing" << endreq;
    /// Get the XML conversion service interface and
    try {
      m_doGenericCnv = m_xmlSvc->allowGenericCnv();
      log << MSG::INFO << "Generic conversion status: "
          << (unsigned int)m_doGenericCnv << endreq;
    } catch( ... )  {
      sc = StatusCode::FAILURE;
      log << MSG::ERROR << "Getting XmlCnvSvc failed" << endreq;
    }
  }
  return sc;
}


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_createObj (DOM_Element element,
                                               DataObject*& refpObject) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  
  // creates an object for the node found
  log << MSG::DEBUG << "Normal generic detector element conversion" << endreq;
  std::string elementName = dom2Std (element.getAttribute("name"));
  refpObject = new DetectorElement (elementName);
  
  // returns
  return StatusCode::SUCCESS;
} // end i_createObj


// -----------------------------------------------------------------------
// Fill an object with a new child element
// -----------------------------------------------------------------------
StatusCode XmlBaseDetElemCnv::i_fillObj (DOM_Element childElement,
                                             DataObject* refpObject) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );

  // gets the object
  DetectorElement* dataObj = dynamic_cast<DetectorElement*> (refpObject);
  // gets the element's name
  std::string tagName = dom2Std (childElement.getNodeName());
  // dispatches, based on the name
  if ("detelemref" == tagName) {
    // first get the CLID and checks a converter exists for this type
    // even it it does not exist, we could continue using a generic
    // converter if checkConverterExistence returns true
    char *clidAttribute = childElement.getAttribute("classID").transcode();
    const CLID clsID = (CLID) atol(clidAttribute);
    delete [] clidAttribute;
    bool doGeneric = checkConverterExistence(clsID);
    log << MSG::DEBUG << "checkConverterExistence returned " << doGeneric
        << endreq;
    CLID usedCLID;
    if (doGeneric) {
      usedCLID = CLID_DetectorElement;
    } else {
      usedCLID = clsID;
    }
    
    // gets the reference value and the position of the '#' in it
    std::string referenceValue = dom2Std (childElement.getAttribute("href"));
    unsigned int poundPosition = referenceValue.find_last_of('#');
    // gets the directory where the xmlFile is located
    unsigned int dPos  = m_objRcpt->dbName().find_last_of('/');
    std::string locDir = m_objRcpt->dbName().substr( 0, dPos + 1 );
    // builds the location of the file
    std::string location = referenceValue.substr(0, poundPosition);
    if( location.empty() ) {
      // This means that "href" has the form "#objectID" and referenced
      // object resides in the same file we are currently parsing
      location = m_objRcpt->dbName();
    } else {
      location = locDir + location;
    }

    // creation of the XmlAddress for this child
    XmlAddress* xmlAddress = new XmlAddress (usedCLID,
                                             location,
                                             m_objRcpt->objectName());

    // builds an entryName
    std::string entryName = referenceValue.substr(poundPosition + 1);
    entryName = /*m_objRcpt->objectName() +*/ "/" + entryName;

    log << MSG::DEBUG << "New XmlAddress created : CLID = " << usedCLID
        << ", location = " << location << ", container name = "
        << m_objRcpt->objectName() << ", object name = " << entryName
        << endreq;

    // stores the new entry
    xmlAddress->setObjectName (entryName);
    
    // And register it to current data object we're converting now
    RegistryEntry* currentEntry = 
      dynamic_cast<RegistryEntry*> (m_objRcpt->directory());
    currentEntry->add (entryName, xmlAddress);

  } else if ("version" == tagName || "author" == tagName) {
    // currently ignored
  } else if ("geometryinfo" == tagName) {
    // Everything is in the attributes
    std::string logVolName = dom2Std (childElement.getAttribute ("lvname"));
    std::string support = dom2Std (childElement.getAttribute ("support"));
    std::string replicaPath = dom2Std (childElement.getAttribute ("rpath"));
    std::string namePath = dom2Std (childElement.getAttribute ("npath"));
    log << MSG::DEBUG << "GI volume : " << logVolName  << endreq;
    log << MSG::DEBUG << "GI support: " << support     << endreq;
    log << MSG::DEBUG << "GI rpath  : " << replicaPath << endreq;
    log << MSG::DEBUG << "GI npath  : " << namePath    << endreq;

    // creates a geometryInfo child
    if (logVolName.empty()) {
      dataObj->createGeometryInfo();
    } else if (support.empty()) {
      dataObj->createGeometryInfo (logVolName);
    } else if (!namePath.empty()) {
      dataObj->createGeometryInfo (logVolName, support, namePath);
    } else if (!replicaPath.empty()) {
      ILVolume::ReplicaPath repPath;            
      // Replica path has the format "1/3/7/2"
      const char *rp = replicaPath.c_str();
      char buf[512];
      char *replica = buf;
      bool wasColon = false;
      unsigned int i = 0;
      do {
        wasColon = false;
        if (*rp == '/') {
          wasColon = true;
        } else if (isdigit(*rp)) {
          *replica = *rp;
          replica++;
          i++;
        }
        if (true == wasColon || *(rp + 1) == '\0') {
          if (i > 0) {
            *replica = '\0';
            i = (unsigned int)atol (buf);
            repPath.push_back (i);
            log << MSG::DEBUG << "Found replica number "
                << repPath.back() << endreq;
            replica = buf;
            i = 0;
          }
        }
        rp++;
      } while (*rp != 0);
      dataObj->createGeometryInfo (logVolName,support,repPath);
    } else {
      log << MSG::ERROR << "File " << m_objRcpt->dbName() << ": "
          << tagName
          << " Missing \"rpath\" or \"npath\" element, "
          << "please correct XML data\n"
          << " Either remove support element or provide proper rpath or npath"
          << endreq;
      StatusCode st( CORRUPTED_DATA );
      throw XmlCnvException( " Corrupted XML data", st );            
    }
  } else if (tagName == "specific") {
    // this is the place where the user will put new elements he wants
    // to add to the default detector element
    // So we just go through the children of this element and call
    // i_fillSpecificObj on them
    DOM_NodeList specificChildren = childElement.getChildNodes();
    unsigned int i;
    for (i = 0; i < specificChildren.getLength(); i++) {
      if (specificChildren.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
        // gets the current child
        DOM_Node childNode = specificChildren.item(i);
        StatusCode sc = i_fillSpecificObj ((DOM_Element &) childNode,
                                           dataObj);
        if (sc.isFailure()) {
          std::string childNodeName =
            dom2Std (((DOM_Element &) childNode).getNodeName());
          log << MSG::WARNING << "parsing of specific child "
              << childNodeName << " raised errors."
              << endreq;
        }
      }
    }
  } else if ("userParameter" == tagName || "userParameterVector" == tagName) {
    // get the attributes
    std::string type = dom2Std (childElement.getAttribute ("type"));
    std::string name = dom2Std (childElement.getAttribute ("name"));
    std::string comment = dom2Std (childElement.getAttribute ("comment"));
    
    // gets the value
    std::string value;
    DOM_NodeList nodeChildren = childElement.getChildNodes();
    unsigned int i;
    for (i = 0; i < nodeChildren.getLength(); i++) {
      if (nodeChildren.item(i).getNodeType() == DOM_Node::TEXT_NODE) {
        std::string newVal = dom2Std (nodeChildren.item(i).getNodeValue());
        unsigned int begin = 0;
        while (begin < newVal.length() && isspace(newVal[begin])) {
          begin++;
        }
        if (begin < newVal.length()) {
          unsigned int end = newVal.length() - 1;
          while (isspace(newVal[end])) {
            end--;
          }
          value += " ";
          value += newVal.substr(begin, end - begin + 1);
        }
      }
    }

    if ("userParameter" == tagName) {
      // adds the new parameter to the detectorElement
      log << MSG::DEBUG << "Adding user parameter " << name << " with value "
          << value << ", type " << type << " and comment \"" << comment
          << "\"" << endreq;
      dataObj->addUserParameter (name,
                                 type,
                                 comment,
                                 value,
                                 xmlSvc()->eval(value, false));
    } else if ("userParameterVector" == tagName) {
      // parses the value
      std::vector<std::string> vect;
      const char *textValue = value.c_str();
      std::string val;
      std::istrstream cstr (textValue, value.length());
      //delete (textValue);
      while (cstr >> val) {
        vect.push_back (val);
      }
      
      // evaluates the value
      std::vector<double> d_vect;
      for (std::vector<std::string>::iterator it = vect.begin();
           vect.end() != it;
           ++it) {
        d_vect.push_back (xmlSvc()->eval(*it, false));
      }
      
      // adds the new parameterVector to the detectorElement
      log << MSG::DEBUG << "Adding user parameter vector " << name
          << " with values ";
      for (std::vector<std::string>::iterator it = vect.begin();
           vect.end() != it;
           ++it) {
        log << *it << " ";
      }
      log << ", type " << type << " and comment \""
          << comment << "\"" << endreq;
      dataObj->addUserParameterVector (name, type, comment, vect, d_vect);
    }
    
  } else {
    // Something goes wrong, does it?
    log << MSG::WARNING << "This tag makes no sense to element : "
        << tagName << endreq;
  }

  // returns
  return StatusCode::SUCCESS;
} // end i_fillObj


// -----------------------------------------------------------------------
// Check the existence of a converter
// -----------------------------------------------------------------------
bool XmlBaseDetElemCnv::checkConverterExistence (const CLID& clsID) {
  MsgStream log(msgSvc(), "XmlDetElemCnv" );
  // Checking the other incoming guys according to our DTD!
  bool cnvExists;
  ICnvManager* cnvMgr;
  
  StatusCode stcod = serviceLocator()->queryInterface (IID_ICnvManager,
                                                       (void **) &cnvMgr);
  if (stcod.isFailure()) {
    stcod.setCode( CANT_QUERY_INTERFACE );  
    throw XmlCnvException ("Query to ICnvManager interface failed", stcod);
  }
  
  // Check whether the converter for this storage type and class ID is available
  cnvExists = cnvMgr->existsConverter (XML_StorageType, clsID);
  cnvMgr->release();
  if (!cnvExists) {
    if (!m_doGenericCnv) {
      MsgStream log(msgSvc(), "XmlCatalogCnv");
      log << MSG::ERROR
          << "File " << m_objRcpt->dbName()
          << " class ID "
          << clsID << ", proper converter not found" << endreq;
      stcod.setCode (INVALID_CLASS_ID);  
      throw XmlCnvException ("Unknown class ID", stcod);
    }
    return true;
  }
  return false;
}


// -----------------------------------------------------------------------
// Accessor to the classID
// -----------------------------------------------------------------------
const CLID& XmlBaseDetElemCnv::classID() {
  return CLID_DetectorElement;
}

