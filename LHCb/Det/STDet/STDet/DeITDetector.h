// $Id: DeITDetector.h,v 1.1 2005-12-19 15:18:24 mneedham Exp $
#ifndef _DeITDetector_H_
#define _DeITDetector_H_

#include "STDet/DeSTDetector.h"

static const CLID& CLID_DeITDetector = 9201;

/** @class DeITDetector DeITDetector.h "STDet/DeITDetector.h"
 *
 *  IT Detector Element class
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeSTSector;

class DeITDetector : public DeSTDetector  {

public:

  /** Constructor */
  DeITDetector ( const std::string& name = "" ) ;

 
  /** Destructor */
  virtual ~DeITDetector(); 
 
  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeITDetector;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const ;

  virtual StatusCode initialize();
  
  /** 
  *  short cut to pick up the wafer corresponding to x,y,z
  * @param point in global frame  
  * @return sector 
  */
  virtual DeSTSector* findSector(const Gaudi::XYZPoint& aPoint); 

  /** 
  *  short cut to pick up the wafer corresponding to a channel
  * @param channel 
  * @return sector 
  */
  virtual DeSTSector* findSector(const LHCb::STChannelID aChannel); 

private:

  /** make flat list of lowest descendents  and also layers*/
  void flatten();

};

#endif // _DeITDetector_H








