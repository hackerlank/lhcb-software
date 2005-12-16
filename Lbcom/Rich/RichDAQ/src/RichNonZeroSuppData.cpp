
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppData.cpp
 *
 *  Implementation file for RICH DAQ helper class : RichNonZeroSuppData
 *
 *  $Id: RichNonZeroSuppData.cpp,v 1.6 2005-12-16 15:11:34 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

// local
#include "RichNonZeroSuppData.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

// =================================================================================================

namespace RichNonZeroSuppDataV1 {

  void RichNonZeroSuppData::fillRichSmartIDs( RichSmartID::Vector & ids,
                                              const IRichDetNumberingTool * hpdTool ) const
  {
    // header word object
    const Header head ( header() );

    // Get HPD software ID for this Level 0 ID
    const RichSmartID sID = hpdTool->richSmartID( head.l0ID() );

    // Fill with active channels
    for ( RichDAQ::ShortType iRow = 0; iRow < dataSize(); ++iRow )
    {
      for ( RichDAQ::ShortType iCol = 0; iCol < dataSize(); ++iCol )
      {
        if ( isPixelActive(iRow,iCol) )
        {
          ids.push_back( RichSmartID( sID.rich(),
                                      sID.panel(),
                                      sID.hpdNumInCol(),
                                      sID.hpdCol(),
                                      iRow, iCol ) );
        }
      }
    }

  }

  void RichNonZeroSuppData::fillMsgStream( MsgStream & os ) const
  {

    os << "======================================================================================================"
       << endreq;

    // Header
    os << Header( header() ) << endreq;

    // Raw print out of data block
    os << endreq;
    dumpAllBits( os );
    os << endreq;

    os << "======================================================================================================"
       << endreq;

  }

} // end V1 namespace
