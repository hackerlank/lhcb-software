// $Id: IMuonPad2MCTool.h,v 1.3 2010-03-02 09:49:16 asatta Exp $
#ifndef IMUONPAD2MCTOOL_H 
#define IMUONPAD2MCTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/MuonTileID.h"
#include "GaudiKernel/IAlgTool.h"

namespace LHCb
{
  class MCParticle;
}


static const InterfaceID IID_IMuonPad2MCTool ( "IMuonPad2MCTool", 1, 0 );

/** @class IMuonPad2MCTool IMuonPad2MCTool.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-12-06
 */
class IMuonPad2MCTool  : public virtual IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMuonPad2MCTool; }


  virtual LHCb::MCParticle* Pad2MC(LHCb::MuonTileID value)=0;


  
  virtual bool isXTalk(LHCb::MuonTileID value, LHCb::MCParticle*& pp)=0;
//  virtual LHCb::MCParticle* PadXtalk2MC(LHCb::MuonTileID value, 
//std::vector<LHCb::MuonTileID> list)=0;
  virtual LHCb::MCParticle* PadNoXtalk2MC(LHCb::MuonTileID value)=0;
protected:

private:

};
#endif // IMUONPAD2MCTOOL_H
