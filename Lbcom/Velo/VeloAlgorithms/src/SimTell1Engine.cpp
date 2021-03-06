// $Id: SimTell1Engine.cpp,v 1.2 2009-12-18 08:12:33 szumlat Exp $
// Include files

// local
#include "SimTell1Engine.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimTell1Engine
//
// 2009-08-01 : Tomasz Szumlak
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimTell1Engine::SimTell1Engine(  ):
  m_processEnable ( 0 ),
  m_dataSize ( sizeof(VeloTELL1::Data)/sizeof(int) ),
  m_linkPedestalSize ( sizeof(VeloTELL1::LinkPedestal)/sizeof(VeloTELL1::u_int16_t) ),
  m_linkMaskSize ( sizeof(VeloTELL1::LinkMask)/sizeof(VeloTELL1::u_int8_t) ),
  m_inData ( 0 ),
  m_outData ( m_dataSize, 0 )
{ }
//=============================================================================
// Destructor
//=============================================================================
SimTell1Engine::~SimTell1Engine() {}
//
void SimTell1Engine::setInData(const VeloTELL1::EngineVec& inVec) { 
    m_inData=&inVec;
}
//--
