
// from Gaudi
#include <GaudiKernel/ToolFactory.h>

// LHCb
#include "Event/MCParticle.h"

// MicroDST
#include <MicroDST/Functors.hpp>
// local
#include "MCParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCParticleCloner
//
// 2007-11-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCParticleCloner::MCParticleCloner( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent ),
    m_vertexCloner ( NULL )
{
  declareProperty("ICloneMCVertex", m_vertexClonerName = "MCVertexCloner" );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================

StatusCode MCParticleCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_vertexCloner = ( m_vertexClonerName == "NONE" ? NULL :
                     this->tool<ICloneMCVertex>( m_vertexClonerName,
                                                 this->parent() ) );

  return sc;
}

//=============================================================================

LHCb::MCParticle* MCParticleCloner::clone( const LHCb::MCParticle* mcp )
{
  if ( !mcp ) return NULL;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "clone() called for " << *mcp << endmsg;

  // Clone the MCParticle
  LHCb::MCParticle * clone = cloneKeyedContainerItem<BasicMCPCloner>(mcp);
  
  // Original origin vertex
  const LHCb::MCVertex * originVertex = mcp->originVertex();

  // Should we clone the origin vertex ?
  if ( cloneOriginVertex(originVertex) )
  {
   
    // Has it already been cloned
    LHCb::MCVertex* originVertexClone = getStoredClone<LHCb::MCVertex>(originVertex);
    if ( !originVertexClone )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cloning origin vertex " << *originVertex << endmsg;

      originVertexClone =
        cloneKeyedContainerItem<BasicVtxCloner>(originVertex);
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Cloned vertex " << *originVertexClone << endmsg;
      originVertexClone->clearProducts();

      // Clone the origin vertex mother
      const LHCb::MCParticle* mother = mcp->mother();
      LHCb::MCParticle* motherClone = ( mother ? (*this)(mother) : NULL );
      if ( motherClone && msgLevel(MSG::DEBUG) )
        debug() << "Cloned mother " << *motherClone << endmsg;
      originVertexClone->setMother(motherClone);
    }

    // Add the cloned origin vertex to the cloned MCP
    clone->setOriginVertex( originVertexClone );

    // Add the cloned MCP to the cloned origin vertex, if not already there
    bool found = false;
    for ( const auto& mcP : originVertexClone->products() )
    {
      if ( mcP.target() == clone ) { found = true; break; }
    }
    if ( !found ) { originVertexClone->addToProducts(clone); }

  }
  else
  {
    clone->setOriginVertex(NULL);
  }

  // Clone the end vertices
  clone->clearEndVertices();
  cloneDecayVertices( mcp->endVertices(), clone );

  return clone;
}

//=============================================================================

void
MCParticleCloner::cloneDecayVertices( const SmartRefVector<LHCb::MCVertex>& endVertices,
                                      LHCb::MCParticle* clonedParticle )
{
  for ( const SmartRef<LHCb::MCVertex>& endVtx : endVertices )
  {
    if ( endVtx->isDecay() && !(endVtx->products().empty()) )
    {
      if ( m_vertexCloner )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Cloning Decay Vertex " << *endVtx
                    << " with " << endVtx->products().size()
                    << " products!" << endmsg;
        clonedParticle->addToEndVertices( (*m_vertexCloner)(endVtx) );
      }
      else
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Copying decay vertex SmartRefs" << endmsg;
        clonedParticle->addToEndVertices(endVtx);
      }
    }
  }
}

//=============================================================================

LHCb::MCParticle* MCParticleCloner::operator() ( const LHCb::MCParticle* mcp )
{
  if ( !mcp ) return NULL;
  LHCb::MCParticle * clone = getStoredClone<LHCb::MCParticle>(mcp);
  return ( clone ? clone : this->clone(mcp) );
}

//=============================================================================
// Destructor
//=============================================================================
MCParticleCloner::~MCParticleCloner() { }

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCParticleCloner )

//=============================================================================
