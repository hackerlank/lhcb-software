
//-----------------------------------------------------------------------------
/** @file RichSmartIDTool.cpp
 *
 * Implementation file for class : RichSmartIDTool
 *
 * CVS Log :-
 * $Id: RichSmartIDTool.cpp,v 1.7 2005-01-07 13:24:31 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/10/27 14:41:03  jonrob
 * Various updates
 *
 * Revision 1.5  2004/10/12 09:58:26  papanest
 * speed up globaltoPDpanel
 *
 * Revision 1.4  2004/07/26 18:03:05  jonrob
 * Various improvements to the doxygen comments
 *
 * @author Antonis Papanestis
 * @date 2003-10-28
 */
//-----------------------------------------------------------------------------

// local
#include "RichSmartIDTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichSmartIDTool>          s_factory ;
const        IToolFactory& RichSmartIDToolFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichSmartIDTool::RichSmartIDTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase( type, name , parent )
{
  declareInterface<IRichSmartIDTool>(this);
}

//=============================================================================
///Destructor
RichSmartIDTool::~RichSmartIDTool(){}
//=============================================================================


//=============================================================================
StatusCode RichSmartIDTool::initialize()
{

  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  const std::string pdPanelName[2][2] = { { DeRichHPDPanelLocation::Rich1Panel0,
                                            DeRichHPDPanelLocation::Rich1Panel1 },
                                          { DeRichHPDPanelLocation::Rich2Panel0,
                                            DeRichHPDPanelLocation::Rich2Panel1 } };
  //loop over riches and photo detector panels
  unsigned int rich, panel;
  for( rich=0; rich<m_photoDetPanels.size(); ++rich ) {
    for( panel=0; panel<m_photoDetPanels[rich].size(); ++panel ) {
      m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      debug() << "Stored photodetector panel "
              << m_photoDetPanels[rich][panel]->name() << endreq;
      m_localOffset[rich][panel] = m_photoDetPanels[rich][panel]->localOffset();
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode RichSmartIDTool::finalize  ()
{
  return RichToolBase::finalize();
}

//=============================================================================
// Returns the position of a SmartID in global coordinates
HepPoint3D RichSmartIDTool::globalPosition ( const RichSmartID& inSmartID ) const
{
  return (m_photoDetPanels[inSmartID.rich()][inSmartID.panel()]->detectionPoint(inSmartID));
}


//=============================================================================
// Returns the SmartID for a given global position
StatusCode RichSmartIDTool::smartID ( const HepPoint3D& globalPoint,
                                      RichSmartID& smartid) const
{
  if (globalPoint.z() < 8000.0)
    // Rich1
    if (globalPoint.y() > 0.0) {
      // top side
      smartid.setRich(Rich::Rich1);
      smartid.setPanel(Rich::top);
      return ( m_photoDetPanels[Rich::Rich1][Rich::top]->smartID(globalPoint, smartid) );
    }
    else {
      // bottom side
      smartid.setRich(Rich::Rich1);
      smartid.setPanel(Rich::bottom);
      return ( m_photoDetPanels[Rich::Rich1][Rich::bottom]->smartID(globalPoint, smartid) );
    }
  else
    // Rich2
    if (globalPoint.x() > 0.0) {
      // left side
      smartid.setRich(Rich::Rich2);
      smartid.setPanel(Rich::left);
      return ( m_photoDetPanels[Rich::Rich2][Rich::left]->smartID(globalPoint, smartid) );
    }
    else {
      // right side
      smartid.setRich(Rich::Rich2);
      smartid.setPanel(Rich::right);
      return ( m_photoDetPanels[Rich::Rich2][Rich::right]->smartID(globalPoint, smartid) );
    }
}

//=============================================================================
// Returns the SmartID for a given global position
// z coord is not valid
//=============================================================================
HepPoint3D RichSmartIDTool::globalToPDPanel ( const HepPoint3D& globalPoint ) const
{
  if (globalPoint.z() < 8000.0)
    // Rich1
    if (globalPoint.y() > 0.0) {
      // top side

      HepPoint3D tempPoint( m_photoDetPanels[Rich::Rich1][Rich::top]->
                            //globalToPDPanel(globalPoint) );
                            geometry()->toLocal( globalPoint ) );
      tempPoint.setY( tempPoint.y() + m_localOffset[Rich::Rich1][Rich::top] );
      tempPoint.setZ( 0.0 );
      return tempPoint;
    }
    else {
      // bottom side
      HepPoint3D tempPoint( m_photoDetPanels[Rich::Rich1][Rich::bottom]->
                            //globalToPDPanel(globalPoint) );
                            geometry()->toLocal( globalPoint ) );
      tempPoint.setY(tempPoint.y() - m_localOffset[Rich::Rich1][Rich::bottom]);
      tempPoint.setZ( 0.0 );
      return tempPoint;
    }
  else
    // Rich2
    if (globalPoint.x() > 0.0) {
      // left side
      HepPoint3D tempPoint( m_photoDetPanels[Rich::Rich2][Rich::left]->
                            //globalToPDPanel(globalPoint) );
                            geometry()->toLocal( globalPoint ) );
      tempPoint.setX( tempPoint.x() + m_localOffset[Rich::Rich2][Rich::left] );
      tempPoint.setZ( 0.0 );
      return tempPoint;

    }
    else {
      // right side
      HepPoint3D tempPoint( m_photoDetPanels[Rich::Rich2][Rich::right]->
                            //globalToPDPanel(globalPoint) );
                            geometry()->toLocal( globalPoint ) );
      tempPoint.setX(tempPoint.x() - m_localOffset[Rich::Rich2][Rich::right]);
      tempPoint.setZ( 0.0 );
      return tempPoint;
    }

}

//=============================================================================
// Returns a list with all valid smartIDs
const RichSmartID::Collection & RichSmartIDTool::readoutChannelList( ) const
{

  // Static vector -> filled only once
  static RichSmartID::Collection readoutChannels;

  if ( readoutChannels.empty() ) {

    unsigned int marker(0), i(0);

    m_photoDetPanels[Rich::Rich1][Rich::top]->readoutChannelList(readoutChannels);
    for ( i = 0; i < readoutChannels.size(); ++i ) {
      readoutChannels[i].setRich(Rich::Rich1);
      readoutChannels[i].setPanel(Rich::top);
      //++marker;
    }
    marker = readoutChannels.size();
    m_photoDetPanels[Rich::Rich1][Rich::bottom]->readoutChannelList(readoutChannels);
    for (i = marker; i < readoutChannels.size(); ++i ) {
      readoutChannels[i].setRich(Rich::Rich1);
      readoutChannels[i].setPanel(Rich::bottom);
      //++marker;
    }
    marker = readoutChannels.size();
    const unsigned int nRich1 = readoutChannels.size();
    m_photoDetPanels[Rich::Rich2][Rich::left]->readoutChannelList(readoutChannels);
    for (i = marker; i < readoutChannels.size(); ++i ) {
      readoutChannels[i].setRich(Rich::Rich2);
      readoutChannels[i].setPanel(Rich::left);
      //++marker;
    }
    marker = readoutChannels.size();
    m_photoDetPanels[Rich::Rich2][Rich::right]->readoutChannelList(readoutChannels);
    for (i = marker; i < readoutChannels.size(); ++i ) {
      readoutChannels[i].setRich(Rich::Rich2);
      readoutChannels[i].setPanel(Rich::right);
      //++marker;
    }
    const unsigned int nRich2 = readoutChannels.size() - nRich1;

    info() << "Created active HPD channel list : # channels RICH(1/2) = " 
           << nRich1 << " / " << nRich2 << endreq;

  }

  return readoutChannels;
}

HepPoint3D RichSmartIDTool::globalPosition ( const HepPoint3D& localPoint,
                                             const Rich::DetectorType rich,
                                             const Rich::Side side ) const
{
  return (m_photoDetPanels[rich][side]->globalPosition(localPoint,
                                                       side));
}
