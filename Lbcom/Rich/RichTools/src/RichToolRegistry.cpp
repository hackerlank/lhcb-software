// $Id: RichToolRegistry.cpp,v 1.4 2004-07-15 15:51:47 jonrob Exp $

// Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/Kernel.h"

// local
#include "RichToolRegistry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichToolRegistry
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichToolRegistry>          s_factory ;
const        IToolFactory& RichToolRegistryFactory = s_factory ;

// Standard constructor
RichToolRegistry::RichToolRegistry( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool( type, name, parent ) 
{
  // declare interface
  declareInterface<IRichToolRegistry>(this);

  // job option for mapping between nickname and class name
  declareProperty( "Tools", m_names );
}

StatusCode RichToolRegistry::initialize() 
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "Initialize" << endreq;

  // setup tool registry
  for ( ToolList::iterator it = m_names.begin();
        it != m_names.end(); ++it ) {
    const int slash = (*it).find_first_of( "/" );
    const std::string name = ( slash>0 ? (*it).substr(slash+1) : *it );
    const std::string type = ( slash>0 ? (*it).substr(0,slash) : *it );
    addEntry(name,type);
  }
  m_names.clear();

  return StatusCode::SUCCESS;
}

StatusCode RichToolRegistry::finalize() 
{
  debug() << "Finalize" << endreq;
  // Finalise base class
  return GaudiTool::finalize();
}

const std::string & RichToolRegistry::toolType( const std::string & nickname ) const
{
  if ( "" == nickname ) Exception("Received request to retrieve a tool with no name !");
  if ( "" == m_myTools[nickname] ) addEntry( nickname, nickname );
  return m_myTools[nickname];
}
