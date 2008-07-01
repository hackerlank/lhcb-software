#include "ConfigStackAccessSvc.h"
#include "boost/assign/list_of.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "GaudiKernel/SvcFactory.h"

namespace bl=boost::lambda;

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigStackAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigStackAccessSvc::ConfigStackAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
{
    declareProperty("ConfigAccessSvcs", s_svcs = boost::assign::list_of("ConfigDBAccessSvc"));
}

//=============================================================================
// Destructor
//=============================================================================
ConfigStackAccessSvc::~ConfigStackAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigStackAccessSvc::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown) {
    if ( IConfigAccessSvc::interfaceID().versionMatch(riid) )   {
        *ppvUnknown = (IConfigAccessSvc*)this;
        addRef();
        return SUCCESS;
    }
    return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigStackAccessSvc::initialize() {
    info() << "Initialize" << endmsg;
    StatusCode status = Service::initialize();
    if (!status.isSuccess()) return status;
    for (std::vector<std::string>::const_iterator i = s_svcs.begin(); i!=s_svcs.end() ; ++i) {
        IConfigAccessSvc *svc(0);
        info() << " requesting service " << *i << endmsg;
        status = service(*i,svc);
        info() << " requested service " << *i << " : " << status << endmsg;
        if (!status.isSuccess()) return status;
        assert(svc!=0);
        m_svcs.push_back(svc);
    }
    return StatusCode::SUCCESS;
}

boost::optional<PropertyConfig> 
ConfigStackAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
    boost::optional<PropertyConfig> x;
    for (std::vector<IConfigAccessSvc*>::iterator i =m_svcs.begin();
                                                  i!=m_svcs.end()&&!x;
                                                  ++i)  {
        x = (*i)->readPropertyConfig(ref);
        debug() << "read of " << ref 
                << " from " << (*i)->name() 
                << ": " << (!x?"failed":"OK") << endmsg;
    }
    return x;
}

PropertyConfig::digest_type
ConfigStackAccessSvc::writePropertyConfig(const PropertyConfig& config) {
    PropertyConfig::digest_type id = m_svcs.front()->writePropertyConfig(config);
    debug() << "write of " << config.name() 
            << " to " << m_svcs.front()->name() 
            << ": " << (id.invalid()?"failed":"OK") << endmsg;
    return id;
}


boost::optional<ConfigTreeNode> 
ConfigStackAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref) {
    boost::optional<ConfigTreeNode> x;
    for (std::vector<IConfigAccessSvc*>::iterator i =m_svcs.begin();
                                                  i!=m_svcs.end()&&!x;
                                                  ++i)  {
         x = (*i)->readConfigTreeNode(ref);
         debug() << "read of " << ref 
                 << " from " << (*i)->name() 
                 << ": " << (!x?"failed":"OK") << endmsg;
    }
    return x;
}


std::vector<ConfigTreeNodeAlias> 
ConfigStackAccessSvc::configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias) {
    std::vector<ConfigTreeNodeAlias> vec;
    for (std::vector<IConfigAccessSvc*>::iterator i =m_svcs.begin();
                                                  i!=m_svcs.end();
                                                  ++i)  {
         std::vector<ConfigTreeNodeAlias> x = (*i)->configTreeNodeAliases(alias);
         // merge, killing overlaps, checking for consistency...
         for (std::vector<ConfigTreeNodeAlias>::const_iterator i = x.begin(); i!= x.end(); ++i) {
            std::vector<ConfigTreeNodeAlias>::const_iterator j = 
                std::find_if( vec.begin(), vec.end(), 
                              bl::bind(&ConfigTreeNodeAlias::alias,bl::_1)==i->alias());
               // check consistency if duplication
            if (j!=vec.end()) {
                if ( *j != *i) {
                     warning() << "inconsistent alias in stack (" << *i
                               << ") -- continuing to use earlier definition: " << *j
                               << endmsg;
                } else { } // consistent... skip

                continue; // don't push this!!!
            }
            vec.push_back(*i);
         }
    }
    return vec;
}

ConfigTreeNode::digest_type
ConfigStackAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {
    ConfigTreeNode::digest_type id = m_svcs.front()->writeConfigTreeNode(config);
    debug() << "write of " << config
            << " to " << m_svcs.front()->name() 
            << ": " << (id.invalid()?"failed":"OK") << endmsg;
    return id;
}

boost::optional<ConfigTreeNode>  
ConfigStackAccessSvc::readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias) {
    boost::optional<ConfigTreeNode> x;
    for (std::vector<IConfigAccessSvc*>::iterator i =m_svcs.begin();
                                                  i!=m_svcs.end()&&!x;
                                                  ++i)  {
         x = (*i)->readConfigTreeNodeAlias(alias);
         debug() << "read of " << alias 
                 << " from " << (*i)->name() 
                 << ": " << (!x?"failed":"OK") << endmsg;
    }
    return x;
}

ConfigTreeNodeAlias::alias_type 
ConfigStackAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias) {
    ConfigTreeNodeAlias::alias_type id = m_svcs.front()->writeConfigTreeNodeAlias(alias);
    debug() << "write of " << alias
            << " to " << m_svcs.front()->name() 
            << ": " << (id.invalid()?"failed":"OK") << endmsg;
    return id;
}

MsgStream& ConfigStackAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}
