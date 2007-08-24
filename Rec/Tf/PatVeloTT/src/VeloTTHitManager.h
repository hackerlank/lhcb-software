#ifndef VELOTTTHITMANAGER_H 
#define VELOTTHITMANAGER_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TfKernel/TTStationHitManager.h"

#include "VeloTTHit.h"

namespace Tf {

  /** @class VeloTTHitManager PatTStationHitManager.h
   *
   *  Velo station hit manager for Pat.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class VeloTTHitManager : public TTStationHitManager<VeloTTHit>
  {

  public:

    VeloTTHitManager( const std::string& type,
                      const std::string& name,
                      const IInterface* parent) ;

    virtual ~VeloTTHitManager( );

  };
}

#endif // VELOTTHITMANAGER_H
