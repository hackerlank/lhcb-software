#ifndef L0MUONKERNEL_CTRLRAWCNV_H
#define L0MUONKERNEL_CTRLRAWCNV_H 

/** @class CtrlRawCnv CtrlRawCnv.h  L0MuonKernel/CtrlRawCnv.h
 
  Class for constructing the raw data generated by the TELL1 linked to the processing boards in transparent mode. 

  @author
  @date

*/
#include "L0MuonKernel/CandRegisterHandler.h"
#include "L0MuonKernel/MuonCandidate.h"
#include <string>
#include <map>
#include <vector>

namespace L0Muon {
  
  class CtrlRawCnv  {

  public:

    /// Default Constructor
    CtrlRawCnv();

    /// Default Constructor
    CtrlRawCnv(int side);

    /// Destructor
    ~CtrlRawCnv();
    
    void release();

    std::vector<PMuonCandidate> muonCandidates();
    std::vector<PMuonCandidate> muonCandidatesBCSU();

    void decodeBank(std::vector<unsigned int> raw, int version);
    std::vector<unsigned int> rawBank(int version, int ievt);

    void dump(int version, int ievt, std::string tab);
    void dump(int version, int ievt);

  private:
    
    int m_side;
    // Input candidate registers
    CandRegisterHandler m_candRegHandler[2];
    CandRegisterHandler m_candRegHandlerBCSU[2][12];

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_CTRLRAWCNV_H
