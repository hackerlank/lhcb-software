// $Id: ANNSvc.h,v 1.9 2008-06-25 14:12:02 graven Exp $
#ifndef ANNSVC_H  
#define ANNSVC_H 1

// Include files
#include "Kernel/IANNSvc.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include <string>

namespace ANNSvcUtilities {
  template <typename KEY, typename VALUE> class bimap_t;
}
/** @class ANNSvc ANNSvc.h
 *  
 *  functionality:
 *     maps between 'Assigned Names and Numbers'
 *         and also 'Assigned Numbers and Names'
 *
 *  @author Gerhard Raven
 *  @date   2008-01-02
 */


class ANNSvc : public Service, 
               virtual public IANNSvc {
public:
  ANNSvc( const std::string& name, ISvcLocator* pSvcLocator,
          const std::vector<major_key_type>& majors );
  ~ANNSvc();
  
  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  StatusCode finalize();

  boost::optional<minor_value_type>  value(const major_key_type& major, const std::string& minor) const;
  boost::optional<minor_value_type>  value(const major_key_type& major, int minor) const;

  bool                           hasMajor(const major_key_type& major) const;

  std::vector<minor_key_type>    keys(const major_key_type& major) const;
  std::vector<minor_value_type>  items(const major_key_type& major) const;
  std::vector<major_key_type>    majors() const;

protected:
  // Call handleUndefined if an unknown key/value is requested...
  //  default implementation just returns an unitialized boost::optional
  virtual boost::optional<minor_value_type> handleUndefined(const major_key_type& major, int minor) const;
  virtual boost::optional<minor_value_type> handleUndefined(const major_key_type& major, const std::string& minor) const;

  MsgStream& log() const { return m_log; }


private:
  
  typedef ANNSvcUtilities::bimap_t<minor_key_type, minor_mapped_type> bimap_type;
  typedef GaudiUtils::VectorMap< major_key_type, bimap_type* > maps_type;
  maps_type   m_maps;

  mutable MsgStream m_log;
};
#endif // ANNSVC_H 1
