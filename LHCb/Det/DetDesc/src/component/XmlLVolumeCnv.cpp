// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlLVolumeCnv.cpp,v 1.12 2001-06-18 14:32:44 sponce Exp $

// Include files
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"

#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/xtoa.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/IXmlSvc.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/Surface.h"
#include "DetDesc/XmlCnvException.h"

#include <cstdlib>
#include <map>

#include "XmlLVolumeCnv.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static CnvFactory<XmlLVolumeCnv> s_factoryLVolume;
const ICnvFactory& XmlLVolumeCnvFactory = s_factoryLVolume;


// -----------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
XmlLVolumeCnv::XmlLVolumeCnv (ISvcLocator* svc) :
  XmlGenericCnv (svc, CLID_LVolume) {}


// -----------------------------------------------------------------------
// Tests whether the tag denotes a boolean solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isBooleanSolid (std::string tag) {
  if ("union" == tag ||
      "subtraction" == tag ||
      "intersection" == tag) {
    return true;
  }
  return false;
} // end isBooleanSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a simple solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isSimpleSolid (std::string tag) {
  if ("box" == tag ||
      "trd" == tag ||
      "trap" == tag ||
      "cons" == tag ||
      "tubs" == tag ||
      "sphere" == tag) {
    return true;
  }
  return false;
} // end isSimpleSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a solid
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isSolid (std::string tag) {
  if (isSimpleSolid(tag) ||
      isBooleanSolid(tag)) {
    return true;
  }
  return false;
} // end isSolid


// -----------------------------------------------------------------------
// Tests whether the tag denotes a transformation
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isTransformation (std::string tag) {
  if ("transformation" == tag ||
      "posXYZ" == tag ||
      "posRPhiZ" == tag ||
      "posRThPhi" == tag ||
      "rotXYZ" == tag ||
      "rotAxis" == tag) {
    return true;
  }
  return false;
} // end isTransformation


// -----------------------------------------------------------------------
// Tests whether the tag denotes a parametrized physical volume
// -----------------------------------------------------------------------
bool XmlLVolumeCnv::isParamphysvol (std::string tag) {
  if ("paramphysvol" == tag ||
      "paramphysvol2D" == tag ||
      "paramphysvol3D" == tag) {
    return true;
  }
  return false;
} // end isTransformation

      
// -----------------------------------------------------------------------
// returns a string locating the element
// -----------------------------------------------------------------------
std::string XmlLVolumeCnv::locateElement (DOM_Element element) {
  // tries to find a parent element with a name
  DOM_Node parentNode = element;
  DOM_Element parentElement;
  std::string parentName;
  bool hasName = false;
  while (!hasName) {
    parentNode = parentNode.getParentNode();
    if (parentNode == 0) {
      break;
    }
    if (parentNode.getNodeType() == DOM_Node::ELEMENT_NODE) {
      parentElement = (DOM_Element&) parentNode;
      parentName = dom2Std (parentElement.getAttribute("name"));
      if (parentName != "") {
        hasName = true;
      }
    }
  }
  // if one was found, builds the result from it
  std::string result("");
  if (hasName) {
    char buffer[32];
    parentNode = element;
    DOM_Node grandParent = element.getParentNode();
    while (parentNode != parentElement) {
      DOM_NodeList list = grandParent.getChildNodes();
      unsigned int i = 0;
      for (i = 0; i < list.getLength(); i++) {
        if (parentNode == list.item(i)) {
          break;
        }
      }

      result = "child number ";
      result += _itoa(i+1, buffer, 10);
      result += " (";
      result += dom2Std (parentNode.getNodeName());
      result += ") of ";
      parentNode = grandParent;
      grandParent = parentNode.getParentNode();
    }
    result += dom2Std (parentElement.getNodeName());
    result += " ";
    result += parentName;
  } else {
    // else just give the name of the parent
    parentNode = element.getParentNode();
    if (parentNode != 0) {
      result += "tag ";
      result += dom2Std (parentNode.getNodeName());
    } else {
      result += "top node";
    }
  }
  return result;
} // end locateElement


// -----------------------------------------------------------------------
// Create an object corresponding to a DOM element
// -----------------------------------------------------------------------
StatusCode XmlLVolumeCnv::internalCreateObj (DOM_Element element,
                                             DataObject*& refpObject) {
  // gets the attributes
  std::string materialName = dom2Std (element.getAttribute ("material"));
  std::string magFieldName = dom2Std (element.getAttribute ("magfield"));
  std::string sensDetName = dom2Std (element.getAttribute ("sensdet"));
  std::string volName = dom2Std (element.getAttribute ("name"));

  // computes the actual material name
  if (materialName.empty() || materialName[0] != '/') {
    materialName.insert(0,"/dd/Materials/");
  }

  // creates an empty LVolume
  LVolume* dataObj = new LVolume(volName, 
                                 0, 
                                 materialName,
                                 sensDetName,
                                 magFieldName);
  refpObject = dataObj;
  
  // processes the children. The dtd says we should find
  // ((%solid;, %transformation;?)?, (physvol | paramphysvol | surf)*)
  DOM_NodeList childNodes = element.getChildNodes();
  // get the first one, it should be a solid, a physical volume, a
  // parametrized physical volume, a surface or nothing
  unsigned int i = 0;
  while (i < childNodes.getLength() && 
         childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
    i += 1;
  }

  // if no child, return
  if (i >= childNodes.getLength()) {
    return StatusCode::SUCCESS;
  }

  // if there is a child, gets it as an element and gets its name
  DOM_Node childNode = childNodes.item(i);
  DOM_Element childElement = (DOM_Element &) childNode;
  std::string tagName = dom2Std (childElement.getNodeName());

  // try to see if it is a solid and deal with it and a possible
  // transformation if yes
  if (isSolid (tagName)) {
    // deal with the solid itself
    ISolid* solid = dealWithSolid (childElement);
    // finds the next child
    i += 1;
    while (i < childNodes.getLength() && 
           childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
      i += 1;
    }
    // if there is more children, try to see if it is a transformation
    if (i < childNodes.getLength()) {
      childNode = childNodes.item(i);
      childElement = (DOM_Element &) childNode;
      tagName = dom2Std (childElement.getNodeName());
      if (isTransformation(tagName)) {
        // deal with the transformation itself
        HepTransform3D* transformation = dealWithTransformation (element, &i);
        // modifies the solid in consequence
        // TO BE IMPLEMENTED --- TODO
        // frees the memory
        delete (transformation);
        transformation = 0;
        // finds the next child
        while (i < childNodes.getLength() && 
               childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
          i += 1;
        }
      }
    }
    // stores the solid inside the logical volume
    if (0 != solid) {
      dataObj->setSolid(solid);
    }
  }
  
  // Whatever we found up to now, the next child to process is at index i.
  // The dtd says that we will find (physvol | paramphysvol | surf)* now
  // So let's deal with it.
  while (i < childNodes.getLength()) {
    childNode = childNodes.item(i);
    childElement = (DOM_Element &) childNode;
    tagName = dom2Std (childElement.getNodeName());
    // dispatches according to the tagname
    if ("physvol" == tagName) {
      // deals with a physical volume, adds it to the logical volume
      // and frees the memory
      PVolumeItem* volume = dealWithPhysvol(childElement);
      if (0 == volume->transformation) {
        dataObj->createPVolume (volume->physvolName,
                                volume->logvolName,
                                HepTransform3D::Identity);
      } else {
        dataObj->createPVolume (volume->physvolName,
                                volume->logvolName,
                                *(volume->transformation));
      }
      if (volume->transformation != 0) {
        delete (volume->transformation);
        volume->transformation = 0;
      }
      delete (volume);
      volume = 0;
    } else if (isParamphysvol (tagName)) {
      // deals with a parametrized physical volume, adds all physical
      // volumes created to the logical volume and frees the memory      
      PVolumes* volumes = dealWithParamphysvol(childElement);
      for (PVolumes::iterator it = volumes->begin();
           volumes->end() != it;
           ++it) {
        // builds the actual name of the volume
        const int buffer_size = 256;
        char buffer [buffer_size] = { 0 , 0 };
        std::ostrstream ost(buffer, buffer_size);
        ost << (*it)->physvolName << "_[" << (*it)->tag << "]";
        const unsigned int len = strlen(ost.str()); 
        char *resstr = new char[len+1]; 
        strncpy(resstr,ost.str(),len);
        resstr[len] = 0; 
        std::string pvname (resstr); 
        delete [] resstr;
        if (0 == (*it)->transformation) {
          dataObj->createPVolume (pvname,
                                  (*it)->logvolName,
                                  HepTransform3D::Identity);
        } else {
          dataObj->createPVolume (pvname,
                                  (*it)->logvolName,
                                  (*it)->transformation->inverse());
        }
      }
      while (volumes->size() > 0) {
        PVolumeItem* item = volumes->back();
        volumes->pop_back();
        if (item->transformation != 0) {
          delete (item->transformation);
          item->transformation = 0;
        }
        delete (item);
        item = 0;
      }
      delete (volumes);
      volumes = 0;
    } else if ("surf" == tagName) {
      // deals with a surface and adds it to the logical volume
      std::string address = dealWithSurf(childElement);
      long linkID = dataObj->addLink (address, 0);
      SmartRef<Surface> reference (dataObj, linkID);
      dataObj->surfaces().push_back(reference); 
    } else {
      // Something goes wrong, does it?
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING
          << "This tag makes no sense to LVolume " << volName
          << " : " << tagName
          << ". It will be ignored" << endreq;
      return StatusCode::FAILURE;
    }

    //XmlLVolumeCnv:: gets the next child
    i += 1;
    while (i < childNodes.getLength() && 
           childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
      i += 1;
    }
  }
  
  return StatusCode::SUCCESS;
} // end internalCreateObj


// -----------------------------------------------------------------------
// Deal with Physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumeItem*
XmlLVolumeCnv::dealWithPhysvol (DOM_Element element) {
  // gets attributes
  std::string nameAttribute = dom2Std (element.getAttribute ("name"));
  std::string logvolAttribute = dom2Std (element.getAttribute ("logvol"));

  // gets the children
  DOM_NodeList childNodes = element.getChildNodes();

  // deal with a possible transformation inside element
  HepTransform3D* transformation = 0;
  // while there are children, and no transformation found, try
  // to find one
  unsigned int i = 0;
  while ((transformation == 0 ||
          *transformation == HepTransform3D::Identity)
         && i < childNodes.getLength()) {    
    while (i < childNodes.getLength() && 
           childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
      i += 1;
    }
    // is there is one, deal with the transformation
    if (i < childNodes.getLength()) {
      transformation = dealWithTransformation (element, &i);
    }
  }
  // if there are still children after the transformation,
  // raise an error message
  while (i < childNodes.getLength() && 
         childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
    i += 1;
  }
  if (i < childNodes.getLength()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING
        << "There are too many children in physical volume "
        << nameAttribute << ". The exceeding ones will be ignored."
        << endreq;
  }

  // builds physvol and returns
  PVolumeItem* result = new PVolumeItem;
  result->physvolName = nameAttribute;
  result->tag = 0;
  result->logvolName = logvolAttribute;
  result->transformation = transformation;
  return result;
} // end dealWithPhysVol


// -----------------------------------------------------------------------
// Deal with parametrized physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::dealWithParamphysvol (DOM_Element element) {
  // gets the element's name
  std::string tagName = dom2Std (element.getNodeName());
  // dispatches, based on the name
  if ("paramphysvol" == tagName) {
    return dealWithParamphysvol (element, 1);
  } else if ("paramphysvol2D" == tagName) {
    return dealWithParamphysvol (element, 2);
  } else if ("paramphysvol3D" == tagName) {
    return dealWithParamphysvol (element, 3);
  } else {
    // unknown tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : the tag " << tagName
        << " does no denote a parametrized physical volume. "
        << "It will be ignored" << endreq;
  }
  return 0;
} // end dealWithParamphysvol


// -----------------------------------------------------------------------
// Deal with parametrized physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::dealWithParamphysvol (DOM_Element element, unsigned int nD) {
  // nD should be positive
  if (0 == nD) {
    return 0;
  }
  
  // gets attributes
  std::vector<std::string> numberAttributes(nD);
  if (1 == nD) {
    numberAttributes[0] = dom2Std (element.getAttribute ("number"));
  } else {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      // builds the actual name of the attribute
      const int buffer_size = 16;
      char buffer [buffer_size] = { 0 , 0 };
      std::ostrstream ost(buffer, buffer_size);
      ost << "number" << i + 1;
      const unsigned int len = strlen(ost.str());
      char *resstr = new char[len+1];
      strncpy(resstr,ost.str(),len);
      resstr[len] = 0;
      DOMString attrName = DOMString::transcode (resstr); 
      delete [] resstr;
      numberAttributes[i] = dom2Std (element.getAttribute (attrName));
    }
  }

  // computes the values
  std::vector<int> numbers(nD);
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      numbers[i] = (int) xmlSvc()->eval(numberAttributes[i], false);
    }
  }

  // some checks : numbers should be positive
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      if (numbers[i] < 0) {
        throw XmlCnvException
          (" paramphysvolume : number of copies must be positive ! ",
           CORRUPTED_DATA);
        return 0;
      }
    }
  }
  
  // builds the result
  PVolumes* result = 0;

  // deals with the children of the tag. The dtd says it should be
  // (physvol | paramphysvol), %transformation;*nD
  DOM_NodeList childNodes = element.getChildNodes();
  // finds the first child that is an element
  unsigned int i = 0;
  while (i < childNodes.getLength() && 
         childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
    i += 1;
  }

  // is there is one, it should be a physvol or a paramphysvol
  if (i < childNodes.getLength()) {
    DOM_Node firstChildNode = childNodes.item(i);
    DOM_Element firstChildElement = (DOM_Element &) firstChildNode;
    // get the name of the first child
    std::string tagName = dom2Std (firstChildElement.getNodeName());
    if ("physvol" == tagName) {
      // gets the physical volume and add it to the result as the first
      // volume of the vector
      PVolumeItem* volume = dealWithPhysvol (firstChildElement);
      result = new PVolumes (1, volume);
    } else if (isParamphysvol (tagName)) {
      // gets the vector of physical volumes and use it as first result
      // that will be upgraded later for the current parametrisation
      result = dealWithParamphysvol (firstChildElement);
    } else {
      // unknown tag -> display an error, return 0
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING << "In " << locateElement (element)
          << " : the tag " << tagName
          << " is not a valid tag as first child of <paramphysvol";
      if (0 != nD) {
        log << nD << "D";
      }
      log << ">. It should be either physvol or paramphysvol(nD). "
          << "As a consequence, "
          << "this parametrised physical volume will be ignored." << endreq;    
      return 0;
    }

    // now looks subsequent child, they will define the transformations
    std::vector<HepTransform3D*> transformations(nD);

    // while there are still children, and not enough transformations found, try
    // to find more transformations
    unsigned int j = i + 1;
    unsigned int nbOfTransfoFound = 0;
    while (nbOfTransfoFound < nD
           && j < childNodes.getLength()) {
      while (j < childNodes.getLength() && 
             childNodes.item(j).getNodeType() != DOM_Node::ELEMENT_NODE) {
        j += 1;
      }
      // is there is one child, deal with the transformation
      if (j < childNodes.getLength()) {
        transformations[nbOfTransfoFound] =
          dealWithTransformation (element, &j);
        nbOfTransfoFound++;
      }
    }
  
    // if there are still children after the transformation,
    // raise an error message
    while (j < childNodes.getLength() && 
           childNodes.item(j).getNodeType() != DOM_Node::ELEMENT_NODE) {
      j += 1;
    }
    if (j < childNodes.getLength()) {
      MsgStream log(msgSvc(), "XmlLVolumeCnv" );
      log << MSG::WARNING << "In " << locateElement (element)
          << " : there are too many children in this parametrized physical "
          << "volume. The exceeding ones will be ignored."
          << endreq;
    }
    
    // last but not least build the final result from the current one
    // by expansion of the vector using transformation and number
    result = expandParamPhysVol (result, numbers, transformations);

    // frees the memory allocated for the transformations
    {
      unsigned int i = 0;
      for (i = 0; i < nD; i++) {
        if (transformations[i] != 0) {
          delete (transformations[i]);
          transformations[i] = 0;
        }
      }
    }

  } else {
    // no element child -> display an error, return 0
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this <paramphysvol> cannot be empty, it should contain "
        << "either physvol or paramphysvol(nD). As a consequence, this "
        << "parametrised physical volume will be ignored." << endreq;    
    return 0;
  }

  // returns
  return result;
} // end dealWithParamphysvol


// -----------------------------------------------------------------------
// Expand parametrised physical volume
// -----------------------------------------------------------------------
XmlLVolumeCnv::PVolumes*
XmlLVolumeCnv::expandParamPhysVol
(PVolumes* volumes,
 std::vector<int> numbers,
 std::vector<HepTransform3D*> transformations) {
  // gets the number of dimensions, check the arguments are ok
  unsigned int nD = numbers.size();
  if (transformations.size() != nD) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::FATAL
        << "In expandParamPhysVol, the dimensions of the arguments are "
        << "not compatible : I got " << nD << " numbers and "
        << transformations.size() << " transformations. "
        << "The parametrized physical volume will be ignored." << endreq;
    return 0;
  }

  // get the number of volumes in volumes
  unsigned int volNumber = volumes->size();
  // computes the number of replication of a single volume
  unsigned int nV = 1;
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      nV *= numbers[i];
    }
  }
    
  // get the translations and rotations to apply
  std::vector<Hep3Vector> stepTranslations(nD);
  std::vector<HepRotation> stepRotations(nD);
  {
    unsigned int i = 0;
    for (i = 0; i < nD; i++) {
      stepTranslations[i] = transformations[i]->getTranslation();
      stepRotations[i] = transformations[i]->getRotation();
    }
  }
  
  // creates the result
  PVolumes* result = new PVolumes();
  // creates the set of translations and rotations that the volumes will use
  std::vector<Hep3Vector> translations(nV);
  std::vector<HepRotation> rotations(nV);
  {
    unsigned int numberOfItems = 1;
    unsigned int dimension, item;
    int i;
    for (dimension = 0; dimension < nD; dimension++) {
      for (item = 0; item < numberOfItems; item++) {
        for (i = 1; i < numbers[dimension]; i++) {
          translations[numberOfItems*i+item] =
            stepTranslations[dimension] +
            translations[numberOfItems*(i-1)+item];
          rotations[numberOfItems*i+item] =
            stepRotations[dimension] *
            rotations[numberOfItems*(i-1)+item];
        }
      }
      numberOfItems *= numbers[dimension];
    }
  }
  
  // for each volume
  for (PVolumes::iterator it = volumes->begin();
       volumes->end() != it;
       ++it) {
    // add the volume to the result
    result->push_back (*it);
    // replicates the volume
    unsigned int step;
    for (step = 1; step < nV; ++step) {
      // first give it a tag
      unsigned int tag = volNumber * step + (*it)->tag;
      // add a new physical volume to the result
      PVolumeItem* newPvi = new PVolumeItem;
      newPvi->physvolName = (*it)->physvolName;
      newPvi->tag = tag;
      newPvi->logvolName = (*it)->logvolName;
      if (0 == (*it)->transformation) {
        HepTransform3D *transformation =
          new HepTranslate3D (translations[step]);
        *transformation = *transformation * HepRotate3D (rotations[step]);
        newPvi->transformation = transformation;
      } else {
        HepTransform3D *transformation =
          new HepTranslate3D (translations[step] +
                              (*it)->transformation->getTranslation());
        *transformation = *transformation * HepRotate3D
          (rotations[step] * (*it)->transformation->getRotation());
        newPvi->transformation = transformation;
      }
      result->push_back (newPvi);
    }
  }
  delete (volumes);
  volumes = 0;
  return result;
} // end expandParamphysvol


// -----------------------------------------------------------------------
// Deal with Surface
// -----------------------------------------------------------------------
std::string XmlLVolumeCnv::dealWithSurf (DOM_Element element) {
  return dom2Std (element.getAttribute ("address"));
} // end dealWithSurf


// -----------------------------------------------------------------------
// Deal with solid
// -----------------------------------------------------------------------
ISolid* XmlLVolumeCnv::dealWithSolid (DOM_Element element) {
  // gets the element's name
  std::string tagName = dom2Std (element.getNodeName());
  // dispatches, based on the name
  if (isSimpleSolid (tagName)) {
    return dealWithSimpleSolid (element);
  } else if (isBooleanSolid(tagName)) {
    return dealWithBoolean (element);
  } else {
    // unknown tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this should be a Solid but is a : " << tagName
        << ". It will be ignored" << endreq;
  }
  return 0;
} // end dealWithSolid


// -----------------------------------------------------------------------
// Deal with boolean
// -----------------------------------------------------------------------
SolidBoolean* XmlLVolumeCnv::dealWithBoolean (DOM_Element element) {
  // gets the element's name
  std::string nameAttribute = dom2Std (element.getAttribute ("name"));
  std::string tagName = dom2Std (element.getNodeName());

  // Checks it's a boolean solid
  if (!isBooleanSolid (tagName)) {
    // unknow tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this sould be a boolean solid but is a : " << tagName
        << ". It will be ignored" << endreq;
    return 0;
  }
  
  // builds the list of the children
  PlacedSolidList* solids = dealWithBooleanChildren (element);

  // checks that there are at least two solids
  if (solids->size() < 2) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this boolean solid contains less than 2 solids. "
        << "It will be ignored" << endreq;
    if (solids->size() > 0) {
      PlacedSolid solid = solids->back();
      solids->pop_back();
      delete (solid.solid);
      solid.solid = 0;
      if (solid.transformation != 0) {
        delete (solid.transformation);
        solid.transformation = 0;
      }
    }
    delete (solids);
    solids = 0;
    return 0;
  }

  // creates the object
  SolidBoolean* result = 0;
  PlacedSolid placedSolid = solids->front();
  solids->pop_front();
  if ("union" == tagName) {
    SolidUnion* unionResult = new SolidUnion (nameAttribute, placedSolid.solid);
    result = unionResult;
    // TO BE IMPLEMENTED -- TODO
    // what about placedSolid.transformation ?
    delete (placedSolid.transformation);
    placedSolid.transformation = 0;
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      unionResult->unite (placedSolid.solid, placedSolid.transformation);
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  } else if ("subtraction" == tagName) {
    SolidSubtraction* subtractionResult =
      new SolidSubtraction (nameAttribute, placedSolid.solid);
    result = subtractionResult;
    // TO BE IMPLEMENTED -- TODO
    // what about placedSolid.transformation ?
    delete (placedSolid.transformation);
    placedSolid.transformation = 0;
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      subtractionResult->subtract
        (placedSolid.solid, placedSolid.transformation);
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  } else if ("intersection" == tagName) {
    SolidIntersection* intersectionResult =
      new SolidIntersection (nameAttribute, placedSolid.solid);
    result = intersectionResult;
    // TO BE IMPLEMENTED -- TODO
    // what about placedSolid.transformation ?
    delete (placedSolid.transformation);
    placedSolid.transformation = 0;
    // add every child to the boolean solid
    while (!solids->empty()) {
      placedSolid = solids->front();
      solids->pop_front();
      intersectionResult->intersect
        (placedSolid.solid, placedSolid.transformation);
      if (placedSolid.transformation != 0) {
        delete (placedSolid.transformation);
        placedSolid.transformation = 0;
      }
    }
  }

  // frees some memory
  delete solids;
  solids = 0;
  
  // returns
  return result;
} // end dealWithBoolean


// -----------------------------------------------------------------------
// Deal with boolean children
// -----------------------------------------------------------------------
XmlLVolumeCnv::PlacedSolidList*
XmlLVolumeCnv::dealWithBooleanChildren (DOM_Element element) {
  // builds an empty list
  PlacedSolidList* result = new PlacedSolidList();
  
  // the dtd says ((%solid; %transformation;?), (%solid; %transformation;?)+)
  // but we do here (%solid; %transformation;?)*
  DOM_NodeList childNodes = element.getChildNodes();
  unsigned int i = 0;
  while (i < childNodes.getLength()) {
    if (childNodes.item(i).getNodeType() == DOM_Node::ELEMENT_NODE) {
      // gets the current child
      DOM_Node childNode = childNodes.item(i);
      DOM_Element childElement = (DOM_Element &) childNode;
      // gets its name
      std::string tagName = dom2Std (childElement.getNodeName());
      // checks it's a solid
      if (isSolid (tagName)) {
        // get the C++ object from it
        ISolid* solid = dealWithSolid(childElement);
        // see if there is a transformation afterwards
        HepTransform3D* transformation = 0;
        i += 1;
        while (i < childNodes.getLength() && 
               childNodes.item(i).getNodeType() != DOM_Node::ELEMENT_NODE) {
          i += 1;
        }
        if (i < childNodes.getLength()) {
          childNode = childNodes.item(i);
          childElement = (DOM_Element &) childNode;
          tagName = dom2Std (childElement.getNodeName());
          if (isTransformation (tagName)) {
            transformation = dealWithTransformation (element, &i);
          }
        }
        PlacedSolid newPs = { solid, transformation };
        result->push_back (newPs);
      } else {
        // we should have a solid here
        MsgStream log(msgSvc(), "XmlLVolumeCnv" );
        log << MSG::WARNING << "In " << locateElement (childElement)
            << " : this " << tagName
            << " makes no sense to a boolean solid. "
            << "A solid is needed here. It will be ignored" << endreq;
        i += 1;
      }
    } else {
      i += 1;
    }
  }
  // returns
  return result;
} // end dealWithBooleanChildren


// -----------------------------------------------------------------------
// Deal with simple solid
// -----------------------------------------------------------------------
ISolid* XmlLVolumeCnv::dealWithSimpleSolid (DOM_Element element) {
  // gets the element's name
  std::string tagName = dom2Std (element.getNodeName());

  // dispatches, based on the name
  if ("box" == tagName) {
    return dealWithBox (element);
  } else if ("trd" == tagName) {
    return dealWithTrd (element);
  } else if ("trap" == tagName) {
    return dealWithTrap (element);
  } else if ("cons" == tagName) {
    return dealWithCons (element);
  } else if ("tubs" == tagName) {
    return dealWithTubs (element);
  } else if ("sphere" == tagName) {
    return dealWithSphere (element);
  } else {
    // unknow tag
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : this should be a SimpleSolid but is a : " << tagName
        << ". It will be ignored" << endreq;
  }
  return 0;
} // end dealWithSimpleSolid
 

// -----------------------------------------------------------------------
// Deal with box
// -----------------------------------------------------------------------
SolidBox* XmlLVolumeCnv::dealWithBox (DOM_Element element) {
  // gets attributes
  std::string sizeXAttribute = dom2Std (element.getAttribute ("sizeX"));
  std::string sizeYAttribute = dom2Std (element.getAttribute ("sizeY"));
  std::string sizeZAttribute = dom2Std (element.getAttribute ("sizeZ"));
  std::string solidName = dom2Std (element.getAttribute ("name"));

  // computes the values
  double sizeX = 0.0;
  double sizeY = 0.0;
  double sizeZ = 0.0;
  if (!sizeXAttribute.empty()) {
    sizeX = xmlSvc()->eval(sizeXAttribute);
  }
  if (!sizeYAttribute.empty()) {
    sizeY = xmlSvc()->eval(sizeYAttribute);
  }
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }

  // builds solid
  SolidBox* result = new SolidBox
    (solidName, sizeX / 2.0, sizeY / 2.0, sizeZ / 2.0);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : a box should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithBox


// -----------------------------------------------------------------------
// Deal with trd
// -----------------------------------------------------------------------
SolidTrd* XmlLVolumeCnv::dealWithTrd (DOM_Element element) {
  // gets attributes
  std::string sizeX1Attribute = 
    dom2Std (element.getAttribute ("sizeX1"));
  std::string sizeX2Attribute =
    dom2Std (element.getAttribute ("sizeX2"));
  std::string sizeY1Attribute =
    dom2Std (element.getAttribute ("sizeY1"));
  std::string sizeY2Attribute =
    dom2Std (element.getAttribute ("sizeY2"));
  std::string sizeZAttribute =
    dom2Std (element.getAttribute ("sizeZ"));
  std::string trdName = dom2Std (element.getAttribute ("name"));

  // computes the values
  double sizeX1 = 0.0;
  double sizeX2 = 0.0;
  double sizeY1 = 0.0;
  double sizeY2 = 0.0;
  double sizeZ = 0.0;
  if (!sizeX1Attribute.empty()) {
    sizeX1 = xmlSvc()->eval(sizeX1Attribute);
  }
  if (!sizeX2Attribute.empty()) {
    sizeX2 = xmlSvc()->eval(sizeX2Attribute);
  }
  if (!sizeY1Attribute.empty()) {
    sizeY1 = xmlSvc()->eval(sizeY1Attribute);
  }
  if (!sizeY2Attribute.empty()) {
    sizeY2 = xmlSvc()->eval(sizeY2Attribute);
  }
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }

  // builds solid
  SolidTrd* result = new SolidTrd
    (trdName, sizeZ / 2.0, sizeX1 / 2.0, sizeY1 / 2.0, sizeX2 / 2.0,
     sizeY2 / 2.0);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << " : trd should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithTrd


// -----------------------------------------------------------------------
// Deal with trap
// -----------------------------------------------------------------------
SolidTrap* XmlLVolumeCnv::dealWithTrap (DOM_Element element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element.getAttribute ("sizeZ"));
  std::string thetaAttribute =
    dom2Std (element.getAttribute ("theta"));
  std::string phiAttribute =
    dom2Std (element.getAttribute ("phi"));
  std::string sizeY1Attribute =
    dom2Std (element.getAttribute ("sizeY1"));
  std::string sizeX1Attribute = 
    dom2Std (element.getAttribute ("sizeX1"));
  std::string sizeX2Attribute =
    dom2Std (element.getAttribute ("sizeX2"));
  std::string alp1Attribute =
    dom2Std (element.getAttribute ("alp1"));
  std::string sizeY2Attribute =
    dom2Std (element.getAttribute ("sizeY2"));
  std::string sizeX3Attribute = 
    dom2Std (element.getAttribute ("sizeX3"));
  std::string sizeX4Attribute =
    dom2Std (element.getAttribute ("sizeX4"));
  std::string alp2Attribute =
    dom2Std (element.getAttribute ("alp2"));
  std::string trapName = dom2Std (element.getAttribute ("name"));

  // computes the values
  double sizeZ = 0.0;
  double theta = 0.0;
  double phi = 0.0;
  double sizeY1 = 0.0;
  double sizeX1 = 0.0;
  double sizeX2 = 0.0;
  double alp1 = 0.0;
  double sizeY2 = 0.0;
  double sizeX3 = 0.0;
  double sizeX4 = 0.0;
  double alp2 = 0.0;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!thetaAttribute.empty()) {
    theta = xmlSvc()->eval(thetaAttribute);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute);
  }
  if (!sizeY1Attribute.empty()) {
    sizeY1 = xmlSvc()->eval(sizeY1Attribute);
  }
  if (!sizeX1Attribute.empty()) {
    sizeX1 = xmlSvc()->eval(sizeX1Attribute);
  }
  if (!sizeX2Attribute.empty()) {
    sizeX2 = xmlSvc()->eval(sizeX2Attribute);
  }
  if (!alp1Attribute.empty()) {
    alp1 = xmlSvc()->eval(alp1Attribute);
  }
  if (!sizeY2Attribute.empty()) {
    sizeY2 = xmlSvc()->eval(sizeY2Attribute);
  }
  if (!sizeX3Attribute.empty()) {
    sizeX3 = xmlSvc()->eval(sizeX3Attribute);
  }
  if (!sizeX4Attribute.empty()) {
    sizeX4 = xmlSvc()->eval(sizeX4Attribute);
  }
  if (!alp2Attribute.empty()) {
    alp2 = xmlSvc()->eval(alp2Attribute);
  }

  // builds solid
  SolidTrap* result = new SolidTrap
    (trapName, sizeZ / 2.0, theta, phi, sizeY1 / 2.0, sizeX1 / 2.0,
     sizeX2 / 2.0, alp1, sizeY2 / 2.0, sizeX3 / 2.0, sizeX4 / 2.0,
     alp2);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A trap should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithTrap


// -----------------------------------------------------------------------
// Deal with cons
// -----------------------------------------------------------------------
SolidCons* XmlLVolumeCnv::dealWithCons (DOM_Element element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element.getAttribute ("sizeZ"));
  std::string outerRadiusPZAttribute =
    dom2Std (element.getAttribute ("outerRadiusPZ"));
  std::string outerRadiusMZAttribute =
    dom2Std (element.getAttribute ("outerRadiusMZ"));
  std::string innerRadiusPZAttribute =
    dom2Std (element.getAttribute ("innerRadiusPZ"));
  std::string innerRadiusMZAttribute =
    dom2Std (element.getAttribute ("innerRadiusMZ"));
  std::string startPhiAngleAttribute =
    dom2Std (element.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (element.getAttribute ("deltaPhiAngle"));
  std::string consName = dom2Std (element.getAttribute ("name"));

  // some checks : both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" cons : only deltaPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } 
    if (deltaPhiAngleAttribute.empty()) {
      throw XmlCnvException (" cons : only startPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } 
  }

  // computes the values
  double sizeZ = 0.0;
  double outerRadiusPZ = 0.0;
  double outerRadiusMZ = 0.0;
  double innerRadiusPZ = 0.0;
  double innerRadiusMZ = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * degree;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!outerRadiusPZAttribute.empty()) {
    outerRadiusPZ = xmlSvc()->eval(outerRadiusPZAttribute);
  }
  if (!outerRadiusMZAttribute.empty()) {
    outerRadiusMZ = xmlSvc()->eval(outerRadiusMZAttribute);
  }
  if (!innerRadiusPZAttribute.empty()) {
    innerRadiusPZ = xmlSvc()->eval(innerRadiusPZAttribute);
  }
  if (!innerRadiusMZAttribute.empty()) {
    innerRadiusMZ = xmlSvc()->eval(innerRadiusMZAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute);
  }

  // builds solid
  SolidCons* result = new SolidCons
    (consName, sizeZ / 2.0, outerRadiusMZ, outerRadiusPZ, innerRadiusMZ,
     innerRadiusPZ, startPhiAngle, deltaPhiAngle);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A cons should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithCons


// -----------------------------------------------------------------------
// Deal with tubs
// -----------------------------------------------------------------------
SolidTubs* XmlLVolumeCnv::dealWithTubs (DOM_Element element) {
  // gets attributes
  std::string sizeZAttribute =
    dom2Std (element.getAttribute ("sizeZ"));
  std::string outerRadiusAttribute =
    dom2Std (element.getAttribute ("outerRadius"));
  std::string innerRadiusAttribute =
    dom2Std (element.getAttribute ("innerRadius"));
  std::string startPhiAngleAttribute =
    dom2Std (element.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (element.getAttribute ("deltaPhiAngle"));
  std::string tubsName = dom2Std (element.getAttribute ("name"));

  // some checks : both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" tubs : only deltaPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } 
    if (deltaPhiAngleAttribute.empty()) {
      throw XmlCnvException (" tubs : only startPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } 
  }

  // computes the values
  double sizeZ = 0.0;
  double outerRadius = 0.0;
  double innerRadius = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * degree;
  if (!sizeZAttribute.empty()) {
    sizeZ = xmlSvc()->eval(sizeZAttribute);
  }
  if (!outerRadiusAttribute.empty()) {
    outerRadius = xmlSvc()->eval(outerRadiusAttribute);
  }
  if (!innerRadiusAttribute.empty()) {
    innerRadius = xmlSvc()->eval(innerRadiusAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute);
  }

  // builds solid
  SolidTubs* result = new SolidTubs
    (tubsName, sizeZ / 2.0, outerRadius, innerRadius,
     startPhiAngle, deltaPhiAngle);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A tubs should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithTubs


// -----------------------------------------------------------------------
// Deal with sphere
// -----------------------------------------------------------------------
SolidSphere* XmlLVolumeCnv::dealWithSphere (DOM_Element element) {
  // gets attributes
  std::string outerRadiusAttribute =
    dom2Std (element.getAttribute ("outerRadius"));
  std::string innerRadiusAttribute =
    dom2Std (element.getAttribute ("innerRadius"));
  std::string startPhiAngleAttribute =
    dom2Std (element.getAttribute ("startPhiAngle"));
  std::string deltaPhiAngleAttribute =
    dom2Std (element.getAttribute ("deltaPhiAngle"));
  std::string startThetaAngleAttribute =
    dom2Std (element.getAttribute ("startThetaAngle"));
  std::string deltaThetaAngleAttribute =
    dom2Std (element.getAttribute ("deltaThetaAngle"));
  std::string sphereName = dom2Std (element.getAttribute ("name"));

  // Some checks : both angles could be omitted simultaneously ! 
  if ((startPhiAngleAttribute.empty()) != (deltaPhiAngleAttribute.empty())) {
    if (startPhiAngleAttribute.empty()) {
      throw XmlCnvException (" sphere :: only deltaPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } else {
      throw XmlCnvException (" sphere :: only startPhiAngle is provided! ",
                             CORRUPTED_DATA);
    } 
  }
  if (startThetaAngleAttribute.empty() != deltaThetaAngleAttribute.empty()) {
    if (startThetaAngleAttribute.empty()) {
      throw XmlCnvException (" sphere :: only deltaThetaAngle is provided! ",
                             CORRUPTED_DATA);
    } else {
      throw XmlCnvException (" sphere :: only startThetaAngle is provided! ",
                             CORRUPTED_DATA);
    } 
  }

  // computes the values
  double outerRadius = 0.0;
  double innerRadius = 0.0;
  double startPhiAngle = 0.0;
  double deltaPhiAngle = 360.0 * degree;
  double startThetaAngle = 0.0;
  double deltaThetaAngle = 180.0 * degree;  
  if (!outerRadiusAttribute.empty()) {
    outerRadius = xmlSvc()->eval(outerRadiusAttribute);
  }
  if (!innerRadiusAttribute.empty()) {
    innerRadius = xmlSvc()->eval(innerRadiusAttribute);
  }
  if (!startPhiAngleAttribute.empty()) {
    startPhiAngle = xmlSvc()->eval(startPhiAngleAttribute);
  }
  if (!deltaPhiAngleAttribute.empty()) {
    deltaPhiAngle = xmlSvc()->eval(deltaPhiAngleAttribute);
  }
  if (!startThetaAngleAttribute.empty()) {
    startThetaAngle = xmlSvc()->eval(startThetaAngleAttribute);
  }
  if (!deltaThetaAngleAttribute.empty()) {
    deltaThetaAngle = xmlSvc()->eval(deltaThetaAngleAttribute);
  }

  // builds the solid
  SolidSphere* result = new SolidSphere
    (sphereName, outerRadius, innerRadius, startPhiAngle,
     deltaPhiAngle, startThetaAngle, deltaThetaAngle);

  // checks there are no children
  if (element.hasChildNodes()) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A sphere should not have any child. They will be ignored" << endreq;
  }

  // returns
  return result;
} // end dealWithSphere


// -----------------------------------------------------------------------
// Deal with transformation
// -----------------------------------------------------------------------
HepTransform3D* XmlLVolumeCnv::dealWithTransformation(DOM_Element element,
                                                      unsigned int* index) {
  // gets children of element
  DOM_NodeList childNodes = element.getChildNodes();

  // gets the first node of the transformation
  DOM_Node childNode = childNodes.item (*index);
  DOM_Element childElement = (DOM_Element&) childNode;

  // put index on the next node
  *index += 1;
  while (*index < childNodes.getLength() && 
         childNodes.item(*index).getNodeType() != DOM_Node::ELEMENT_NODE) {
    *index += 1;
  }

  // the result
  HepTransform3D* result = 0;

  // gets the tag name for childElement
  std::string tagName = dom2Std (childElement.getNodeName());
  // gets the transformation corresponding to the first child
  if ("posXYZ" == tagName) {
    result = dealWithPosXYZ(childElement);
  } else if ("posRPhiZ" == tagName) {
    // catches an exception in case the r attribute is negative
    try {
      result = dealWithPosRPhiZ(childElement);
    } catch (XmlCnvException e) {
      result = new HepTransform3D();
    }
  } else if ("posRThPhi" == tagName) {
    // catches an exception in case the r attribute is negative
    try {
      result = dealWithPosRThPhi(childElement);
    } catch (XmlCnvException e) {
      result = new HepTransform3D();
    }
  } else if ("transformation" == tagName) {
    return dealWithTransformation (childElement);
  } else {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (childElement)
        << " : this should be a rotation or a translation but is a : "
        << tagName
        << ". It will be ignored." << endreq;
    return new HepTransform3D();
  }

  // We are here because we found a translation. Try now to see if
  // it is followed by a rotation
  if (*index < childNodes.getLength()) {
    childNode = childNodes.item (*index);
    childElement = (DOM_Element&) childNode;
    bool foundRotation = false;
    
    // gets the tag name for the new childElement
    tagName = dom2Std (childElement.getNodeName());
    if ("rotXYZ" == tagName) {
      HepTransform3D* rotation = dealWithRotXYZ(childElement);
      *result = *rotation * *result;
      delete (rotation);
      rotation = 0;
      foundRotation = true;
    } else if ("rotAxis" == tagName) {
      // catches an exception in case the theta or phi value is out of range
      try {
        HepTransform3D* rotation = dealWithRotAxis(childElement);
        *result = *rotation * *result;
        delete (rotation);
        rotation = 0;
      } catch (XmlCnvException e) { 
      }
      foundRotation = true;
    }

    // if needed, put index on the next node
    if (foundRotation) {
      *index += 1;
      while (*index < childNodes.getLength() && 
             childNodes.item(*index).getNodeType() != DOM_Node::ELEMENT_NODE) {
        *index += 1;
      }
    }
  }
  
  // returns
  return result;
} // end dealWithTransformation


// -----------------------------------------------------------------------
// Deal with transformation
// -----------------------------------------------------------------------
HepTransform3D* XmlLVolumeCnv::dealWithTransformation(DOM_Element element) {
  // gets the children
  DOM_NodeList childNodes = element.getChildNodes();

  // position of the current child processed
  unsigned int i = 0;

  // list of the transformation already found
  std::deque<HepTransform3D*> tList;

  // scans the children and builds the transformations
  while (i < childNodes.getLength()) {
    HepTransform3D* transformation = dealWithTransformation (element, &i);
    if (transformation != 0) {
      tList.push_back (transformation);
    }
  }

  // checks there are at least 2 transformations
  if (tList.size() < 2) {
    MsgStream log(msgSvc(), "XmlLVolumeCnv" );
    log << MSG::WARNING << "In " << locateElement (element)
        << "A transformation should have at least two children." << endreq;
  }

  // computes the result
  HepTransform3D* result = 0;
  if (!tList.empty()) {
    result = tList.front();
    tList.pop_front();
    while (!tList.empty()) {
      HepTransform3D* transformation = tList.front();
      tList.pop_front();
      *result = *transformation * *result;
      delete transformation;
      transformation = 0;
    }
  } else {
    result = new HepTransform3D();
  }
  
  // returns
  return result;
} // end dealWithTransformation


// -----------------------------------------------------------------------
// Deal with posXYZ
// -----------------------------------------------------------------------
HepTranslate3D* XmlLVolumeCnv::dealWithPosXYZ (DOM_Element element) {
  // gets attributes
  std::string xAttribute = dom2Std (element.getAttribute ("x"));
  std::string yAttribute = dom2Std (element.getAttribute ("y"));
  std::string zAttribute = dom2Std (element.getAttribute ("z"));

  // computes the values
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  if (!xAttribute.empty()) {
    x = xmlSvc()->eval(xAttribute);
  }
  if (!yAttribute.empty()) {
    y = xmlSvc()->eval(yAttribute);
  }
  if (!zAttribute.empty()) {
    z = xmlSvc()->eval(zAttribute);
  }

  // builds the translation.
  return new HepTranslate3D(-x, -y, -z);
} // end dealWithPosXYZ


// -----------------------------------------------------------------------
// Deal with posRPhiZ
// -----------------------------------------------------------------------
HepTranslate3D* XmlLVolumeCnv::dealWithPosRPhiZ (DOM_Element element) {
  // gets attributes
  std::string rAttribute = dom2Std (element.getAttribute ("r"));
  std::string phiAttribute = dom2Std (element.getAttribute ("phi"));
  std::string zAttribute = dom2Std (element.getAttribute ("z"));

  // computes the values
  double r = 0.0;
  double phi = 0.0;
  double z = 0.0;
  if (!rAttribute.empty()) {
    r = xmlSvc()->eval(rAttribute);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute);
  }
  if (!zAttribute.empty()) {
    z = xmlSvc()->eval(zAttribute);
  }
  // checks the validity of the value of r
  if (0.0 > r) {
    throw XmlCnvException
      (" RPhiZTranslation : r must be non-negative value !", CORRUPTED_DATA);
  }
  
  // builds the translation.
  Hep3Vector vect (1,1,1);
  vect.setPerp (r);
  vect.setPhi (phi);
  vect.setZ (z);
  return new HepTranslate3D (-1 * vect);
} // end dealWithPosRPhiZ


// -----------------------------------------------------------------------
// Deal with posRThPhi
// -----------------------------------------------------------------------
HepTranslate3D* XmlLVolumeCnv::dealWithPosRThPhi (DOM_Element element) {
  // gets attributes
  std::string rAttribute = dom2Std (element.getAttribute ("r"));
  std::string thetaAttribute = dom2Std (element.getAttribute ("theta"));
  std::string phiAttribute = dom2Std (element.getAttribute ("phi"));

  // computes the values
  double r = 0.0;
  double theta = 0.0;
  double phi = 0.0;
  if (!rAttribute.empty()) {
    r = xmlSvc()->eval(rAttribute);
  }
  if (!thetaAttribute.empty()) {
    theta = xmlSvc()->eval(thetaAttribute);
  }
  if (!phiAttribute.empty()) {
    phi = xmlSvc()->eval(phiAttribute);
  }
  // checks the validity of the value of r
  if (0.0 > r) {
    throw XmlCnvException
      (" RThPhiTranslation : r must be non-negative value !", CORRUPTED_DATA);
  }
  
  // builds the translation.
  Hep3Vector vect (1,1,1);
  vect.setMag (r);
  vect.setTheta (theta);
  vect.setPhi (phi);
  return new HepTranslate3D (-1 * vect);
} // end dealWithPosRThPhi


// -----------------------------------------------------------------------
// Deal with rotXYZ
// -----------------------------------------------------------------------
HepTransform3D* XmlLVolumeCnv::dealWithRotXYZ (DOM_Element element) {
  // get attributes
  std::string rotXAttribute = dom2Std (element.getAttribute ("rotX"));
  std::string rotYAttribute = dom2Std (element.getAttribute ("rotY"));
  std::string rotZAttribute = dom2Std (element.getAttribute ("rotZ"));

  // computes the values
  double rx = 0.0;
  double ry = 0.0;
  double rz = 0.0;
  if (!rotXAttribute.empty()) {
    rx = xmlSvc()->eval(rotXAttribute);
  }
  if (!rotYAttribute.empty()) {
    ry = xmlSvc()->eval(rotYAttribute);
  }
  if (!rotZAttribute.empty()) {
    rz = xmlSvc()->eval(rotZAttribute);
  }

  // computes the rotation
  HepTransform3D* result = new HepRotateX3D(rx);
  *result = *result * HepRotateY3D(ry);
  *result = *result * HepRotateZ3D(rz);
  *result = result->inverse();
  return result;
} // end dealWithRotXYZ


// -----------------------------------------------------------------------
// Deal with rotAxis
// -----------------------------------------------------------------------
HepTransform3D* XmlLVolumeCnv::dealWithRotAxis (DOM_Element element) {
  // get attributes
  std::string axThetaAttribute =
    dom2Std (element.getAttribute ("axTheta"));
  std::string axPhiAttribute =
    dom2Std (element.getAttribute ("axPhi"));
  std::string angleAttribute =
    dom2Std (element.getAttribute ("angle"));

  // computes the values
  double axTheta = 0.0;
  double axPhi = 0.0;
  double angle = 0.0;
  if (!axThetaAttribute.empty()) {
    axTheta = xmlSvc()->eval (axThetaAttribute);
  }
  if (!axPhiAttribute.empty()) {
    axPhi = xmlSvc()->eval (axPhiAttribute);
  }
  if (!angleAttribute.empty()) {
    angle = xmlSvc()->eval (angleAttribute);
  }
  // checks the validity of the theta angle
  if (axTheta < 0 || axTheta > 180 * degree) {
    throw XmlCnvException
      (" AxisRotation : axTheta must be inside 0*degree"
       " and 180*degree ! ", CORRUPTED_DATA);
  }
  // checks the validity of the phi angle
  if (axPhi < 0 || axPhi > 360 * degree) { 
    throw XmlCnvException 
      (" AxisRotation : axPhi  must be inside 0*degree"
       " and 360*degree ! ", CORRUPTED_DATA);
  }
  // Construction of vector with input of theta and phi
  HepTransform3D* result = new HepRotate3D
    (angle, Hep3Vector (sin(axTheta)*cos(axPhi),
                        sin(axTheta)*sin(axPhi),
                        cos(axTheta)));
  *result = result->inverse();
  return result;
} // end dealWithRotAxis

