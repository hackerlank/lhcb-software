// $Id: IConfigAccessSvc.h,v 1.2 2008-02-06 14:52:00 graven Exp $
#ifndef ICONFIGACCESSSVC_H 
#define ICONFIGACCESSSVC_H 1

// Include files
#include <string>
#include "GaudiKernel/INamedInterface.h"
#include "boost/optional.hpp"
#include "HltBase/PropertyConfig.h"


/** @class IConfigAccessSvc IConfigAccessSvc.h
 *  
 *  functionality:
 *         Interface for obtaining an
 *         individual configuration description
 *         given its ID
 *
 *         The ID corresponds to the MD5 digest of the contents
 *         of the configration description
 *
 *  @author Gerhard Raven
 *  @date   2007-12-12
 */


class IConfigAccessSvc : virtual public INamedInterface  {
public:
  /// Return the interface ID
  static const InterfaceID& interfaceID();
  virtual ~IConfigAccessSvc();
  virtual boost::optional<PropertyConfig> read(const PropertyConfig::digest_type& ref) = 0;
  virtual PropertyConfig::digest_type    write(const PropertyConfig& config) = 0;
};
#endif // ICONFIGACCESSSVC_H
