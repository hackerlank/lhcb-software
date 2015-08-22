
// local
#include "TMVAImpFactory.h"

// So the TMVA networks pick up the std:: functions
using namespace std;

namespace ANNGlobalPID
{

  /** @namespace MC12TuneV3 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of MC12TuneV3 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-26
   */
  namespace MC12TuneV3
  {
    // Long tracks
#include "tmva/MC12TuneV3/GlobalPID_Electron_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Long_TMVA.class.C"
    // Downstream
#include "tmva/MC12TuneV3/GlobalPID_Electron_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Downstream_TMVA.class.C"
    // Upstream
#include "tmva/MC12TuneV3/GlobalPID_Electron_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Upstream_TMVA.class.C"
  }

  void TMVAImpFactory::addMC12TuneV3( )
  {
    const std::string tune = "MC12TuneV3";
    // Long
    {
      const std::string tkType = "Long";
      add<MC12TuneV3::ReadElectron_Long_TMVA>       ( tune, "electron", tkType );
      add<MC12TuneV3::ReadMuon_Long_TMVA>           ( tune, "muon",     tkType );
      add<MC12TuneV3::ReadPion_Long_TMVA>           ( tune, "pion",     tkType );
      add<MC12TuneV3::ReadKaon_Long_TMVA>           ( tune, "kaon",     tkType );
      add<MC12TuneV3::ReadProton_Long_TMVA>         ( tune, "proton",   tkType );
      add<MC12TuneV3::ReadGhost_Long_TMVA>          ( tune, "ghost",    tkType );
    }
    // Downstream
    {
      const std::string tkType = "Downstream";
      add<MC12TuneV3::ReadElectron_Downstream_TMVA> ( tune, "electron", tkType );
      add<MC12TuneV3::ReadMuon_Downstream_TMVA>     ( tune, "muon",     tkType );
      add<MC12TuneV3::ReadPion_Downstream_TMVA>     ( tune, "pion",     tkType );
      add<MC12TuneV3::ReadKaon_Downstream_TMVA>     ( tune, "kaon",     tkType );
      add<MC12TuneV3::ReadProton_Downstream_TMVA>   ( tune, "proton",   tkType );
      add<MC12TuneV3::ReadGhost_Downstream_TMVA>    ( tune, "ghost",    tkType );
    }
    // Upstream
    {
      const std::string tkType = "Upstream";
      add<MC12TuneV3::ReadElectron_Upstream_TMVA>   ( tune, "electron", tkType );
      add<MC12TuneV3::ReadMuon_Upstream_TMVA>       ( tune, "muon",     tkType );
      add<MC12TuneV3::ReadPion_Upstream_TMVA>       ( tune, "pion",     tkType );
      add<MC12TuneV3::ReadKaon_Upstream_TMVA>       ( tune, "kaon",     tkType );
      add<MC12TuneV3::ReadProton_Upstream_TMVA>     ( tune, "proton",   tkType );
      add<MC12TuneV3::ReadGhost_Upstream_TMVA>      ( tune, "ghost",    tkType );
    }
  }

}
