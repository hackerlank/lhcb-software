// $Id: CaloDigitsFromRaw.h,v 1.2 2005-11-10 16:43:22 ocallot Exp $
#ifndef CALOEVENT_CALODIGITSFROMRAW_H 
#define CALOEVENT_CALODIGITSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// from CaloEvent
#include "Event/CaloDigit.h"

#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"

#include "GaudiAlg/ISequencerTimerTool.h"
// from DAQEvent
#include "Event/RawEvent.h"

/** @class CaloDigitsFromRaw CaloDigitsFromRaw.h component/CaloDigitsFromRaw.h
 *  Create the CaloDIgits containers from the Raw buffer
 *
 *  @author Olivier Callot
 *  @date   2003-11-18
 */
class CaloDigitsFromRaw : public GaudiAlgorithm {
public:
  /// Standard constructor
  CaloDigitsFromRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloDigitsFromRaw( ); ///< Destructor

  virtual StatusCode initialize ();    ///< Algorithm initialization
  virtual StatusCode execute    ();    ///< Algorithm execution

  class IncreasingByCellID {
  public:
    inline bool operator ()
      ( const CaloDigit* dig1 , 
        const CaloDigit* dig2 ) const {
      return 
      ( 0 == dig1 ) ? true  :
      ( 0 == dig2 ) ? false : 
      dig1->cellID().raw() < dig2->cellID().raw() ;
    }
  };

protected:
  
  void convertSpd( std::string containerName, double energyScale );
  
  void convertCaloEnergies( std::string containerName );

private: 
  
private:
  std::string m_extension ; ///< Added to the default container name, for tests
  int         m_calo      ;

  ICaloTriggerBitsFromRaw* m_spdTool;
  ICaloEnergyFromRaw*  m_energyTool;
};
#endif // CALOEVENT_CALODIGITSFROMRAW_H
