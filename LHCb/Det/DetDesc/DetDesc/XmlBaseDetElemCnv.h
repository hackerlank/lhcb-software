///  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlBaseDetElemCnv.h,v 1.4 2001-12-11 10:02:21 sponce Exp $

#ifndef DETDESC_XMLDETELEMCNV_H
#define DETDESC_XMLDETELEMCNV_H

/// Include files
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/DetectorElement.h"

#include "GaudiKernel/Converter.h"

/// Forward and external declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


/** @class XmlBaseDetElemCnv XmlBaseDetElemCnv.h DetDesc/XmlBaseDetElemCnv.h
 *
 * Generic XML converter for DetectorElements. It provides support for basic
 * DetectorElements and should be the ancester of every specific DetectorElement
 * converter.
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlBaseDetElemCnv : public XmlGenericCnv {

public:
  
  /**
   * Initializes the converter - Overrides the default method in XmlGenericCnv
   * @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID();
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlBaseDetElemCnv(ISvcLocator* svcs);

  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   * @param clsID the type of object the converter is able to convert
   */
  XmlBaseDetElemCnv(ISvcLocator* svc, const CLID& clsID );

  /**
   * Default destructor
   */
  virtual ~XmlBaseDetElemCnv() {}

  /**
   * Resolves the references of the created transient object.
   */
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);

  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject);

  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Element childElement,
                                DataObject* refpObject,
                                IOpaqueAddress* address);

  /** This fills the current object for specific child.
   * Specific children are children of children <specific>
   * of the <detelem> tag. This is typically used for specializing
   * detector elements and define new properties.
   * This will be called for each specific child of the current object
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DetectorElement* refpObject) = 0;


private:
  
  /// Whether to use the generic converter in case a specific one does not exist
  bool m_doGenericCnv;
  
};

#endif // DETDESC_XMLDETELEMCNV_H

