// $Id: CaloTool.cpp,v 1.2 2001-12-09 14:12:57 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// ============================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Stat.h"
// CaloKernel
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloTool.h"

// ============================================================================
/** @file CaloTool.cpp
 * 
 *  Implementation file for class CaloTool
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 01/11/2001 
 */
// ============================================================================

static const std::string s_detSvcName( "DetectorDataSvc" );
static const std::string s_evtSvcName( "EventDataSvc"    );

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
CaloTool::CaloTool( const std::string& type   ,
                    const std::string& name   ,
                    const IInterface*  parent )
  : AlgTool     ( type , name , parent )
  , m_chronoSvc ( 0  ) 
  , m_toolSvc   ( 0  )
  , m_svcLoc    ( 0  ) 
  , m_detSvc    ( 0  ) 
  , m_det       ( 0  ) 
  , m_detName   ( "" )
{
  /// check for servce locator 
  if   ( 0 != serviceLocator() ) { m_svcLoc = serviceLocator    () ; }
  else                           { m_svcLoc = Gaudi::svcLocator () ; }
  if( 0 != svcLoc() )            { svcLoc()->addRef()              ; }
  ///
  StringProperty detectorName( "Detector" , m_detName );
  if( 0 != parent ) 
    {    
      const IProperty* parentProperty =
        dynamic_cast<const IProperty*>( parent );
      if( 0 != parentProperty )
        { parentProperty->getProperty( &detectorName ); }
    }
  ///
  m_detName = detectorName.value();
  ///
  declareProperty( "Detector" , m_detName );
};

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::initialize ()
{
  ///
  { /// set own properties 
    StatusCode sc = setProperties() ;
    if( sc.isFailure() ) 
      { return Error("Could not set own properties ",sc);} 
  }
  ///
  { /// reset Chrono & Stat service 
    if( 0 != chronoSvc() ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
    StatusCode sc = 
      svcLoc()->service("ChronoStatSvc" , m_chronoSvc , true );
    if( sc.isFailure() )
      { return Error("Could not locate ChronoStatSvc!", sc ); };
    if( 0 != chronoSvc() ) { chronoSvc()->addRef() ; }
  }
  ///
  { /// reset Tool  service 
    if( 0 != toolSvc()   ) { toolSvc()->release() ; m_toolSvc = 0 ; }
    ///
    StatusCode sc = 
      svcLoc()->service("ToolSvc"       , m_toolSvc   , true );
    if( sc.isFailure() )
      { return Error("Could not locate ToolSvc!", sc ); };
    if( 0 != toolSvc()   ) { toolSvc()->addRef() ; }
  }
  ///
  { /// locate detector data svc 
    if( 0 != detSvc()    ) { detSvc()->release() ; m_detSvc = 0 ; }
    StatusCode sc = 
      svcLoc()->service( s_detSvcName   , m_detSvc    , true );
    if( sc.isFailure() )
      { return Error("Could not locate DetSvc='"+s_detSvcName+"'", sc ); };
    if( 0 != detSvc()   ) { detSvc()->addRef() ; }    
  }
  ///
  { /// reset detector 
    if( 0 != det() ) { m_det = 0 ; }
    if( !detName().empty() && 0 == detSvc() )
      { return Error("Detector name is '"+detName()+"' ,but detSvc()==0!"); }
  }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode    CaloTool::finalize   ()
{
  /// release services 
  if( 0 != toolSvc    () ) { toolSvc  ()->release() ; m_toolSvc   = 0 ; }
  if( 0 != chronoSvc  () ) { chronoSvc()->release() ; m_chronoSvc = 0 ; }
  if( 0 != detSvc     () ) { detSvc   ()->release() ; m_detSvc    = 0 ; }
  ///
  if( 0 != det        () ) { m_det = 0 ; }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** destructor 
 */
// ============================================================================
CaloTool::~CaloTool(){};

// ============================================================================
/** query interafce method  
 *  @param  id  unique interface ID 
 *  @param  pI  placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTool::queryInterface ( const InterfaceID& id ,
                                      void**             pI )
{
  /// check the placeholder 
  if( 0 == pI ) { return StatusCode::FAILURE ; }
  ///
  if      ( id == IAlgTool::   interfaceID() ) 
    { *pI = static_cast<IAlgTool*>   (this); }
  else if ( id == IInterface:: interfaceID() )
    { *pI = static_cast<IInterface*> (this); }
  else 
    { return AlgTool::queryInterface( id , pI ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
};


// ============================================================================
/** Print the error  message and return status code
 * @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloTool::Error     ( const std::string& msg , 
                      const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( msg , st , MSG::ERROR ); 
};

// ============================================================================
/** Print the warning  message and return status code 
 *  @param msg    warning message 
 *  @param st     statsu code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloTool::Warning   ( const std::string& msg , 
                      const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return Print( msg , st , MSG::WARNING ); 
};

// ============================================================================
/** Print the message and return status code 
 *  @param msg    warning message 
 *  @param st     status code 
 *  @param lvl    print level 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloTool::Print     ( const std::string& msg , 
                      const StatusCode & st  ,
                      const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl 
      << type () 
      << " "   << msg ;
  ///
  if( !st.isSuccess() ) { log << " \tStatusCode=" << st ;}
  ///
  log << endreq ; 
  return  st;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,
                                 const GaudiException & exc ,
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg , exc, sc );
  return  sc ;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,  
                                 const std::exception & exc , 
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( msg+"("+exc.what()+")", sc );
  return  sc ;
};

// ============================================================================
/** Create and throw the exception  
 *  @param msg    exception message 
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloTool::Exception ( const std::string    & msg ,  
                                 const MSG::Level     & lvl ,
                                 const StatusCode     & sc  ) const 
{ 
  Error( msg , lvl );
  throw CaloException( msg , sc );
  return  sc ;
};

// ============================================================================
// The End
// ============================================================================
