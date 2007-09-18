// $Id: PrepareVeloFullRawBuffer.h,v 1.1 2007-09-18 00:04:14 szumlat Exp $
#ifndef PREPAREVELOFULLRAWBUFFER_H 
#define PREPAREVELOFULLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// STL
#include <vector>

// data model
#include "Kernel/VeloChannelID.h"
#include "Kernel/VeloDecodeCore.h"
#include "Kernel/VeloDecodeConf.h"
#include "VeloEvent/VeloFullBank.h"
#include "VeloEvent/VeloErrorBank.h"

/** @class PrepareVeloFullRawBuffer PrepareVeloFullRawBuffer.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-22
 *
 *  Algorithm dedicated to decoding the new 1MHz non-zero suppresed
 *  RawBank which is sent out by TELL1 pre-processing board.
 *  The input for the algorithm is binary file and the output 
 *  is container with VeloDigits objects.
 *  The container is the input for the TELL1
 *  algorithms (pedestal subtraction and following, FIR filter nad
 *  linear CM subtraction)
 */

namespace LHCb
{
  class RawEvent;
  class RawBank;
}

using namespace VeloTELL1;
using namespace LHCb;

class PrepareVeloFullRawBuffer : public GaudiTupleAlg {
public: 

  // define structure of the data sections, look at the code of 
  // writeErrorBanks() method and EDMS document with desciption 
  // of the Velo Error raw bank
  enum parameters{
    ERROR_HEADER_1=1,
    ERROR_HEADER_2=2,
    ERROR_SECOND_SECTION=3,
    ADCHeadersShift=4,
    ERROR_FIRST_SECTION=5,
    ERROR_EVT_INFO=8,
    ODIN_DATA_WORDS=9,
    ERROR_THIRD_SECTION=10,
    ALinkShift=36
  };
  enum bitMasks{
    ADCMask=0x000003ff
  };  
  enum bitShifts{
    ADCShift=10
  };  
  //
  /// Standard constructor
  PrepareVeloFullRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloFullRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  // fetch the RawEvent
  StatusCode getRawEvent();
  // extract vector of the RawBanks of the type VeloFull (non-zero supp.)
  StatusCode getRawBanks();
  // get the number of the read-out sensors == number of TELL1s
  void setNumberOfSensors(int sensors);
  int numberOfSensors();
  // separate the data stream 
  void createOrderedSections(int noOfBank);
  // write-out the Gaudi model of the RawBank (with digit objects,
  // header infos and Beetle headers)
  StatusCode writeVeloFull();
  // return number of TELL1 source (sensor) ID
  int sourceID(int noOfBank);
  // run after decoding each event
  void resetMemory();
  // use only as emergency checker!
  void dumpADCs(const dataVec& inADCs);
  // return pointer to the data bank 
  unsigned int* data(int noOfSensor);
  // return pointer to the pedestal bank
  unsigned int* pedestals(int noOfSensor);
  // return pointer to the error bank
  unsigned int* errors(int noOfSensor);
  // check for errors and write out the error banks
  void writeErrorBanks(int noOfBank);
  // check if PPFPGA segment of the given error bank is empty or not
  // return vector with PPFPGA numbers for which error was sent
  dataVec countErrorBanks(int noOfSensor);
  // find object function
  bool findObject(const dataVec& inCont, const unsigned int obj) const;

protected:

  void setADCBankFlag();
  void setPedBankFlag();
  void setErrorBankFlag();
  void unsetADCBankFlag();
  void unsetPedBankFlag();
  void unsetErrorBankFlag();
  bool adcBankFlag();
  bool pedBankFlag();
  bool errorBankFlag();

private:

  LHCb::RawEvent* m_rawEvent;          /// pointer to RawEvent container
  std::string m_rawEventLoc;           /// location of RawEvent container
  rawVec m_data;                       /// pointers to non-zero suppressed data body
  rawVec m_ped;                        /// pointers to pedestal bank body
  rawVec m_error;                      /// vector of pointers to error bank body
  int m_numberOfSensors;               /// number of read-out sensors
  std::vector<int> m_sensors;          /// vector to keep sensors number
  VeloFullBanks* m_veloADCData;        /// container to store ordered data
  VeloFullBanks* m_veloPedestals;      /// container to store ordered pedestals
  VeloErrorBanks* m_errorBank;             /// container to store error banks
  std::string m_veloADCDataContainer;  /// TES locations of decoded data
  std::string m_veloPedestalsContainer;
  std::string m_errorBankContainer;
  bool m_adcBankPresent;               /// flag to check if data is sent out
  bool m_pedBankPresent;               /// flag to check if ped is sent out
  bool m_errorBankPresent;             /// flag to check if error is present
  
};
#endif // DECODEVELOFULLRAWBUFFER_H
