// $Id: RawBankToSTLiteClusterAlg.h,v 1.9 2008-07-04 15:52:21 mneedham Exp $
#ifndef RAWBANKTOSTLITECLUSTERALG_H 
#define RAWBANKTOSTLITECLUSTERALG_H 1

#include "STDecodingBaseAlg.h"
#include "Event/RawBank.h"
#include "Kernel/STDAQDefinitions.h"

#include "Event/STCluster.h"

#include <vector>
#include <string>

/** @class RawBankToSTLiteClusterAlg RawBankToSTLiteClusterAlg.h
 *  
 *  Algorithm to create STClusters from RawEvent object
 * 
 *  @author M. Needham
 *  @date   2004-01-07
 */


#include "Kernel/STClusterWord.h"

class SiADCWord;
class STTell1Board;

namespace LHCb{
 class STChannelID;
 class STLiteCluster;
};

class RawBankToSTLiteClusterAlg : public STDecodingBaseAlg {

public:

  /// Standard constructor
  RawBankToSTLiteClusterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawBankToSTLiteClusterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize(); ///< finalize

private:

  // create Clusters from this type
  StatusCode decodeBanks(LHCb::RawEvent* rawEvt) const;


  std::string m_clusterLocation;  
  
  class Less_by_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() < obj2.channelID() ;
    }
  };

   
  class Equal_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() == obj2.channelID() ;
    }
  };

  
};



#endif //  RAWBANKTOSTLITECLUSTERALG_H 
