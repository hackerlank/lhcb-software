
//-----------------------------------------------------------------------------
/** @file RichCommonConstructors.cpp
 *
 *  Implementation of templated constructors for RiochCommonBase
 *
 *  CVS Log :-
 *  $Id: RichCommonConstructors.cpp,v 1.2 2005-10-17 09:08:58 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-09-18
 */
//-----------------------------------------------------------------------------

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichCommonBase.h"

//=============================================================================
// Implementation of RichCommonBase for GaudiAlgorithm
//=============================================================================
template <>
RichCommonBase<GaudiAlgorithm>::RichCommonBase( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator             ),
    m_toolReg      ( 0                             ),
    m_jos          ( 0                             ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
RichCommonBase<GaudiAlgorithm>::RichCommonBase( const std::string& /* type */,
                                                const std::string& /* name */,
                                                const IInterface* /* parent */ )
  : GaudiAlgorithm ( "ERROR", 0 )
{
  throw GaudiException( "Invalid RichCommonBase<GaudiAlgorithm> constructor",
                        "RichCommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of RichCommonBase for GaudiTupleAlg
//=============================================================================
template <>
RichCommonBase<GaudiTupleAlg>::RichCommonBase( const std::string& name,
                                               ISvcLocator* pSvcLocator )
  : GaudiTupleAlg  ( name, pSvcLocator             ),
    m_toolReg      ( 0                             ),
    m_jos          ( 0                             ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
RichCommonBase<GaudiTupleAlg>::RichCommonBase( const std::string& /* type */,
                                               const std::string& /* name */,
                                               const IInterface* /* parent */ )
  : GaudiTupleAlg ( "ERROR", 0 )
{
  throw GaudiException( "Invalid RichCommonBase<GaudiTupleAlg> constructor",
                        "RichCommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of RichCommonBase for GaudiTool
//=============================================================================
template <>
RichCommonBase<GaudiTool>::RichCommonBase( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : GaudiTool   ( type, name, parent            ),
    m_toolReg   ( 0                             ),
    m_jos       ( 0                             ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
RichCommonBase<GaudiTool>::RichCommonBase( const std::string& /* name */,
                                           ISvcLocator* /* pSvcLocator */ )
  : GaudiTool ( "ERROR", "ERROR", 0 )
{
  throw GaudiException( "Invalid RichCommonBase<GaudiTool> constructor",
                        "RichCommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of RichCommonBase for GaudiTupleTool
//=============================================================================
template <>
RichCommonBase<GaudiTupleTool>::RichCommonBase( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTupleTool ( type, name, parent            ),
    m_toolReg      ( 0                             ),
    m_jos          ( 0                             ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
RichCommonBase<GaudiTupleTool>::RichCommonBase( const std::string& /* name */,
                                                ISvcLocator* /* pSvcLocator */ )
  : GaudiTupleTool ( "ERROR", "ERROR", 0 )
{
  throw GaudiException( "Invalid RichCommonBase<GaudiTool> constructor",
                        "RichCommonBase", StatusCode::FAILURE );
}
//=============================================================================
