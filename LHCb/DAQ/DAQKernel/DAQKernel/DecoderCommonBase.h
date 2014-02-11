
//-----------------------------------------------------------------------------
/** @file DecoderCommonBase.h
 *
 *  Header file for DAQ base class : Decoder::CommonBase
 *
 *  @author Rob Lambert    Rob.Lambert@cern.ch
 *  @date   2005-08-27
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERCOMMONBASE_H
#define DAQKERNEL_DECODERCOMMONBASE_H 1

#include "Event/RawEvent.h"

namespace Decoder
{
  //using GaudiUtils::operator<<;

  //-----------------------------------------------------------------------------
  /** @class CommonBase DecoderCommonBase.h DAQKernel/DecoderCommonBase.h
   *
   *  Base class providing common functionality for all Decoder tools and algorithms
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   2014-02-10
   */
  //-----------------------------------------------------------------------------

  template <class PBASE>
  class CommonBase : public PBASE
  {

  public:

    /// Standard algorithm-like constructor
    CommonBase( const std::string& name,
                ISvcLocator* pSvcLocator );

    /// Standard tool-like constructor
    CommonBase( const std::string& type,
                const std::string& name,
                const IInterface* parent );

    /// Standard Converter-like Constructor
    CommonBase( long storage_type,
                const CLID &class_type,
                ISvcLocator *svc = NULL );

    /// Destructor
    virtual ~CommonBase( );

  public:

    /** Initialization of the algorithm after creation
     *
     * @return The status of the initialization
     * @retval StatusCode::SUCCESS Initialization was successful
     * @retval StatusCode::FAILURE Initialization failed
     */
    virtual StatusCode initialize();

    /** Finalization of the algorithm before deletion
     *
     * @return The status of the finalization
     * @retval StatusCode::SUCCESS Finalization was successful
     * @retval StatusCode::FAILURE Finalization failed
     */
    virtual StatusCode finalize();

  protected:

    /** @brief Returns a pointer to the first RawEvent in the search path
     */
    LHCb::RawEvent* findFirstRawEvent( ) const;

    /** @brief Returns a std::vector of raw banks out of a first raw event containing those banks
     */
    const std::vector< LHCb::RawBank * > & findFirstRawBank( const LHCb::RawBank::BankType ) const;

    //get for one location
    LHCb::RawEvent* tryEventAt( const std::string ) const;
    /// Where to look for the raw event
    std::vector<std::string> m_rawEventLocations;

    ///avoid getIfExists name resolution for now
    virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes=true) const
    {
    	throw GaudiException( "Don't call this from the baseclass!", StatusCode::FAILURE );
    	return NULL;
    }

    /// Initialize search paths
    void initSearch();

  private: // data

    /// Where to look first, saves the last place I was able to retrieve successfully
    mutable std::string m_tryFirstRawEventLocation;

    /// whether to use the RootOnTes next time
    mutable bool m_tryRootOnTes;

    std::vector< LHCb::RawBank * > m_emptyRBVec;
  };

}

#endif // DAQKERNEL_DECODERCOMMONBASE_H
