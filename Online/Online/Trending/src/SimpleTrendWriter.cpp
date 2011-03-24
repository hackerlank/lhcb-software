// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "SimpleTrendWriter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimpleTrendWriter
//
// 2011-03-24 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SimpleTrendWriter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimpleTrendWriter::SimpleTrendWriter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_trend( 0 )
  , m_fileName( "" )
  , m_fileIsOpen( false )
  , m_tagChanged( false )
  , m_tagVersion( 0 )
{
  declareInterface<ISimpleTrendWriter>(this);
}
//=============================================================================
// Destructor
//=============================================================================
SimpleTrendWriter::~SimpleTrendWriter() {} 

//=========================================================================
//  Initialize: Retieve the tool
//=========================================================================
StatusCode SimpleTrendWriter::initialize( ) {
  StatusCode sc = GaudiTool::initialize();
  if ( !sc ) return sc;
  m_trend = tool<ITrendingTool>( "TrendingTool" );
  if ( 0 == m_trend ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Set partition and name. Open file and load latest tags
//=========================================================================
void SimpleTrendWriter::setPartitionAndName( std::string& partition, std::string& name ) {
  if ( m_fileIsOpen ) close();
  m_fileName = partition + "_" + name;
  bool status = m_trend->openRead( m_fileName );
  if ( status ) {
    m_trend->tags( m_tags );  // get the tags;
    m_tagVersion = m_trend->tagVersion();
    m_values.resize( m_tags.size(), 0. );
    m_tagChanged = false;
    //== get the most recent values. Delicate!!!
    int start = (int)::time(0) - 1000;
    int nTry = 0;
    while ( !m_trend->select( start ) ) {
      start -= 24*3600;  // previous day
      if ( ++nTry > 50 ) break;
    }
    if ( nTry < 50  ) {
      while( m_trend->nextEvent( start, m_values ) ) { }
    }
    m_trend->closeFile();
  }  
}
//=========================================================================
//  Start to collect data for a new event
//=========================================================================
void SimpleTrendWriter::startEvent( ) {
  m_tagChanged = false;
}

//=========================================================================
//  Add a new value for an existing tag.
//=========================================================================
void SimpleTrendWriter::addEntry( std::string tag, double value ) {
  for ( unsigned int i = 0; m_tags.size() > i; ++i ) {
    if ( tag == m_tags[i] ) {
      m_values[i] = value;
      return;
    }
  }
  m_tags.push_back( tag );
  m_values.push_back( value );
  m_tagChanged = true;    
}
//=========================================================================
//  Write teh event in the file. Update the tags if needed.
//=========================================================================
void SimpleTrendWriter::saveEvent ( ) {
  if ( m_tagChanged ) {
    if ( m_fileIsOpen ) close();
    m_tagVersion += 1;
  }
  
  if ( !m_fileIsOpen ) {
    m_fileIsOpen = m_trend->openWrite( m_fileName, m_tags, m_tagVersion );
    if ( !m_fileIsOpen ) {
      error() << "Can't open file " << m_fileName << " for writing" << endmsg;
      return;
    }
  } 
  m_trend->write( m_values );
}

//=========================================================================
//  Close the file and setthe appropriate flag
//=========================================================================
void SimpleTrendWriter::close ( ) {
  if ( m_fileIsOpen ) m_trend->closeFile();
  m_fileIsOpen = false;
}
//=============================================================================
