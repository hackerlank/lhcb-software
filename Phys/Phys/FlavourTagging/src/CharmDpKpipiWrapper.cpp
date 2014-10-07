// Include files 

// local
#include "CharmDpKpipiWrapper.h"
#include "TMVAClassification/mva_charmtagger_reco14/pur_Dp2kpipi.C"

namespace MyDpKpipiSpace {
#ifndef SKIP_TMVA_COMPILE
#include "TMVAClassification/mva_charmtagger_reco14/train_Dp_Kpipi_BDT.class.C"
#endif
}


//-----------------------------------------------------------------------------
// Implementation file for class : CharmDpKpipiWrapper
//
// 2014-02-18 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

CharmDpKpipiWrapper::CharmDpKpipiWrapper(std::vector<std::string> & names) {
#ifndef SKIP_TMVA_COMPILE
  mcreader = new MyDpKpipiSpace::ReadBDT(names);
  purtable = new MyDpKpipiSpace::PurityTable();
#endif
}

CharmDpKpipiWrapper::~CharmDpKpipiWrapper() {
#ifndef SKIP_TMVA_COMPILE
  delete mcreader;
  delete purtable;
#endif
}

double CharmDpKpipiWrapper::GetMvaValue(std::vector<double> const & values) {
#ifndef SKIP_TMVA_COMPILE
  double bdtOut = mcreader->GetMvaValue(values);
  double purity = purtable->GetPurityAtBDT(bdtOut);
  return purity;
#endif
  return 0.0;
  
}

//=============================================================================
