// $Id: TsaSeed.h,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
#ifndef _TSASEED_H_
#define _TSASEED_H_

/** @class TsaSeed
 *
 *
 *  @author M.Needham
 *  @date   07/03/2002
 */

#include "GaudiAlg/GaudiTupleAlg.h"
#include <string>
#include "TsaKernel/stopwatch.h"
#include "SeedHit.h"
#include "SeedPnt.h"
#include "SeedTrack.h"

class ILHCbIDsToMCParticles;
class IOTExpectedHits;
class IOTDataSvc;
class IITDataSvc;

namespace Tsa{
  class Cluster;
}

namespace LHCb{
  class MCParticle;
}

class TsaSeed: public GaudiTupleAlg {

public:
  
  // Constructors and destructor
  TsaSeed(const std::string& name, 
                   ISvcLocator* pSvcLocator); 
  virtual ~TsaSeed();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

  class increasingX  {
  public:
    bool operator() ( SeedHit* first, SeedHit* second ) const;
  };

  class decreasingLikelihood  {
  public:
    bool operator() ( SeedTrack* first, SeedTrack* second ) const;
  };

private:

  void search( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  void searchStereo( int& sector, std::vector<SeedHit*> hits[], std::vector<SeedTrack*>& seeds );
  int fitLine( SeedTrack* seed );
  int fitParabola( SeedTrack* seed, double &cth );
  int refitParabola( SeedTrack* seed, double &cth );
  void likelihood( std::vector<SeedTrack*>& seeds );
  void select( std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[], std::vector<SeedHit*> hit2[] );
  void monitor( int& sector, std::vector<SeedTrack*>& seeds, double& time, 
                std::vector<SeedHit*> hits[], std::vector<SeedHit*> hit2[] );

  std::string m_otDataSvcName;
  std::string m_itDataSvcName;

  double m_likCut;
  IOTDataSvc* m_otDataSvc;
  IITDataSvc* m_itDataSvc;

  LHCb::MCParticle* particle(Tsa::Cluster* aCluster);
                         
  Tsa::stopWatch aWatch;
  ILHCbIDsToMCParticles* m_linkTool;
  IOTExpectedHits* m_expectedHits;

  int m_ev;
};

#endif // _TSASEED
