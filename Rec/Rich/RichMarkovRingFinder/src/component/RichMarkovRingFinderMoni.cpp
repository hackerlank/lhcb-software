// $Id: RichMarkovRingFinderMoni.cpp,v 1.10 2004-10-22 19:15:28 abuckley Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// local
using namespace std;
#include "RichMarkovRingFinderMoni.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichMarkovRingFinderMoni
//
// 2003-12-04 : Chris Jones
// 2004-02-10 : Andy Buckley
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMarkovRingFinderMoni>          s_factory ;
const        IAlgFactory& RichMarkovRingFinderMoniFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMarkovRingFinderMoni::RichMarkovRingFinderMoni( const string& name, ISvcLocator* pSvcLocator)
  : RichRecAlgBase ( name, pSvcLocator )
{
  declareProperty( "HistoPath", m_histPth = "RICH/Markov" );
  declareProperty( "MCHistoPath", m_mcHistPth = "RICH/Markov/MC" );
  declareProperty( "RingHitMatchFractionCutoff", m_RingMatchFractionCutoff = 0.90 );
}


//=============================================================================
// Destructor
//=============================================================================
RichMarkovRingFinderMoni::~RichMarkovRingFinderMoni() {};


//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode RichMarkovRingFinderMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  acquireTool( "RichRecMCTruthTool",  m_richRecMCTruth );
  acquireTool( "RichMCTruthTool",     m_richMCTruth    );
  acquireTool( "RichMCTrackInfoTool", m_mcTrackInfo    );
  acquireTool( "RichCherenkovAngle",  m_ckAngle        );
  acquireTool( "RichRayTracing",      m_raytrace       );
  acquireTool( "MCEffReconstructible", m_mcReconstructible );

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  // Initialise the MC-rec matching agreement tally
  m_numMcVsRecMatchAgreements[Rich::Rich1][false] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich1][true] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich2][false] = 0;
  m_numMcVsRecMatchAgreements[Rich::Rich2][true] = 0;

  return StatusCode::SUCCESS;
}
//=============================================================================



//=============================================================================
//  Finalize
//=============================================================================
StatusCode
RichMarkovRingFinderMoni::finalize()
{
  // Execute base class method
  StatusCode sc = RichRecAlgBase::finalize();

  // Fractional agreement between MC and rec-matching
  map<Rich::DetectorType, float> fractionAgreed;

  // Agreement in Rich1
  fractionAgreed[Rich::Rich1] = m_numMcVsRecMatchAgreements[Rich::Rich1][true] /
    (m_numMcVsRecMatchAgreements[Rich::Rich1][true] + m_numMcVsRecMatchAgreements[Rich::Rich1][false]);
  always() << "Number of MC-rec matching agree/disagreements in RICH1 = "
           << m_numMcVsRecMatchAgreements[Rich::Rich1][true]   << " / "
           << m_numMcVsRecMatchAgreements[Rich::Rich1][false]  << " ("
           << 100 * fractionAgreed[Rich::Rich1] << "% agreed)" << endreq;

  // Agreement in Rich2
  fractionAgreed[Rich::Rich2] = m_numMcVsRecMatchAgreements[Rich::Rich2][true] /
    (m_numMcVsRecMatchAgreements[Rich::Rich2][true] + m_numMcVsRecMatchAgreements[Rich::Rich2][false]);
  always() << "Number of MC-rec matching agree/disagreements in RICH2 = "
           << m_numMcVsRecMatchAgreements[Rich::Rich2][true]   << " / "
           << m_numMcVsRecMatchAgreements[Rich::Rich2][false]  << " ("
           << 100 * fractionAgreed[Rich::Rich1] << "% agreed)" << endreq;

  return sc;
}
//=============================================================================



//=============================================================================
// Main execution
//=============================================================================
StatusCode
RichMarkovRingFinderMoni::execute()
{
  typedef vector<const MCParticle*> MCParticleVector;
  debug() << "Execute" << endreq;

  // Locate RichRecRings from TES
  SmartDataPtr<RichRecRings> rings( eventSvc(), RichRecRingLocation::MarkovRings );
  if ( !rings ) {
    warning() << "Failed to find RichRecRings at " << RichRecRingLocation::MarkovRings << endreq;
    return StatusCode::SUCCESS;
  } else if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Successfully located " << rings->size() << " RichRecRings at "
            << RichRecRingLocation::MarkovRings << endreq;
  }



  // Just some basic histogramming of pixs and rings in the event
  {
    // Loop over event pixels
    map<Rich::DetectorType, unsigned int> numPixelsInEvent;
    for (RichRecPixels::const_iterator iPix = richPixels()->begin(); 
         iPix != richPixels()->end(); ++iPix) {
      // Is this a RICH1 or RICH2 pixel?
      const Rich::DetectorType whichRich( (*iPix)->detector() );
      numPixelsInEvent[whichRich]++;
    }
    m_NumPixsPerEvent[Rich::Rich1]->fill(numPixelsInEvent[Rich::Rich1]);
    m_NumPixsPerEvent[Rich::Rich2]->fill(numPixelsInEvent[Rich::Rich2]);
    
    // Loop over event rings
    map<Rich::DetectorType, unsigned int> numRingsInEvent;
    for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {
      // Is this a RICH1 or RICH2 ring?
      const Rich::DetectorType whichRich( (*iRing)->rich() );
      numRingsInEvent[whichRich]++;
    }
    m_NumRingsPerEvent[Rich::Rich1]->fill(numPixelsInEvent[Rich::Rich1]);
    m_NumRingsPerEvent[Rich::Rich2]->fill(numPixelsInEvent[Rich::Rich2]);
  }



  // Loop over rings for more in-depth purposes
  info() << endreq;
  for ( RichRecRings::const_iterator iRing = rings->begin(); iRing != rings->end(); ++iRing ) {

    // Is this a RICH1 or RICH2 ring?
    const Rich::DetectorType whichRich( (*iRing)->rich() );
    const string whichRichName(Rich::text(whichRich));
    debug() << "This ring is in " << whichRich << endreq;

    // Retrieve the pixels that constitute the ring and associate to the track MCParticles
    SmartRefVector<RichRecPixel> & pixels = (*iRing)->richRecPixels();


    // Histogram some ring params: radius, number of pixels (and the correlation)
    m_MarkovRingRadius[whichRich]->fill( (*iRing)->radius() );
    m_MarkovRingNumPixs[whichRich]->fill( (*iRing)->richRecPixels().size() );
    m_MarkovRingRadiusVsNumPixs[whichRich]->fill( (*iRing)->radius(), (*iRing)->richRecPixels().size() );


    // Loop over this ring's pixels
    map<const MCParticle*, size_t> seenMCParts;
    for ( SmartRefVector<RichRecPixel>::iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix ) {
      if ( !(*iPix) ) { Error("Null RichRecPixel in RichRecRing"); continue; }

      // Get the MC particles associated to this pixel
      MCParticleVector thispixelMCParts;
      m_richRecMCTruth->mcParticle( *iPix, thispixelMCParts );

      // Update list of seen MC particles
      for (MCParticleVector::const_iterator iMCP = thispixelMCParts.begin(); iMCP != thispixelMCParts.end(); ++iMCP) {
        seenMCParts[*iMCP]++;
      }
    } // loop over ring pixels


    if (0 == seenMCParts.size()) {
      debug() << "Found a ring with no associated MC track" << endreq;
    } else {

      // Extract useful numbers from the ring's matched MCParticle list
      pair<const MCParticle*, size_t> mostMatchedPair( *max_element(seenMCParts.begin(), seenMCParts.end()) );
      const MCParticle* mostMatchedMCP( mostMatchedPair.first );
      const size_t mostMatchedMCPNoMatches( mostMatchedPair.second );
      const size_t totalNoMatches( for_each(seenMCParts.begin(), seenMCParts.end(), CountMap<const MCParticle*>()) );
      const double bestMatchFraction( static_cast<double>(mostMatchedMCPNoMatches) / static_cast<double>(totalNoMatches) );


      // Let's invent a measure of ring "quality"
      // *** maybe a probability-weighted sum over pixs in rec mode?
      const double ringquality( mostMatchedMCPNoMatches ); // normalising would be nice, but hey: can't have everything


      // Tell the world all about it...
      debug() << "No pixels = " << pixels.size()
              << " :: Total no assoc tracks = " << totalNoMatches
              << " :: No unique tracks = " << seenMCParts.size()
              << endreq;
      debug() << "Most matched MC particle for this ring is " << mostMatchedMCP
             << " with " << mostMatchedMCPNoMatches << "/"
             << totalNoMatches << " matches" << endreq;


      // Histogram the best match fraction and absolute number
      m_MarkovRingBestMCMatchNumber[whichRich]->fill(mostMatchedMCPNoMatches);
      m_MarkovRingBestMCMatchFraction[whichRich]->fill(bestMatchFraction);


      // Decide whether this ring is to be considered "sufficiently uniquely matched" to the
      // best MCParticle candidate to be considered a successful Markov ring
      if (bestMatchFraction > m_RingMatchFractionCutoff) {
        debug() << "Found ring with a single MC track causing >"
               << 100 * m_RingMatchFractionCutoff << "% of hits" << endreq;


        // MC type info
        // Options are Rich::Electron, Rich::Muon, Rich::Pion, Rich::Kaon, Rich::Proton or Rich::Unknown
        // numerically 0               1           2           3           4               -1
        const MCParticle* mcpart = mostMatchedMCP;
        const Rich::ParticleIDType mcType = m_richMCTruth->mcParticleType(mcpart);
        info() << "MC type: " << mcType << endreq;


        // Get the MCParticle associated to the "best" reconstructed RichRecSegment for this ring
        const MCParticle* segMCPart( m_richRecMCTruth->mcParticle((*iRing)->richRecSegment()) );


        // Does this ring have a corresponding reconstructed track segment?
        // If not then the global algorithm will probably miss it
        string tempRecOrNot("rec");
        // The projected segment is the best match within the specified ring radius fraction
        // (the matching has already been done in the RichMarkovRingFinderAlg)
        if ( (*iRing)->richRecSegment() ) {
          info() << "Found a ring with a fairly unique MC track and a rec track" << endreq;
        } else {
          info() << "This ring has a good MCParticle match but isn't reconstructed!" << endreq;
          tempRecOrNot = "notRec";
        }
        const string recOrNot(tempRecOrNot); // I want this to be const, thank you very much!


        // Histogram the MC particle type
        const string histosetID(whichRichName + recOrNot);
        m_RingTrackMCType[histosetID]->fill(mcType);


        // Do the rec segment and the MC particle correspond to each other? (when neither is null)
        if ( segMCPart && mcpart != segMCPart ) {
          info() << "Non-null geometry-matched (" << segMCPart 
                 << ") and pixel-matched (" << mcpart 
                 << ") MCParticles disagree for Markov ring " << *iRing << endreq;
          m_numMcVsRecMatchAgreements[whichRich][true]++;
        }



        // *** Histogram ring purity (number of true photons for that ring's MCParticle) 
        // *** and efficiency (fraction of successfully matched rings)


        // Get two ring points separated by ~90 degrees and the centre point to construct
        // a normal vector to the ring. Note that the angular ordering and regular separation of points
        // is assumed, but not guaranteed.
        const HepPoint3D ringCentre( (*iRing)->centrePointGlobal() );
        const vector<HepPoint3D>& ringPoints( (*iRing)->ringPoints() );
        const HepPoint3D point1( *( ringPoints.begin() ) );
        const HepPoint3D point2( ringPoints[ static_cast<size_t>(floor(ringPoints.size()/4.0)) ] );
        const HepVector3D vector1( point1 - ringCentre );
        const HepVector3D vector2( point2 - ringCentre );
        HepVector3D ringNormal( vector1.cross(vector2) );
        ringNormal.setMag(1.0);
        if (ringNormal.z() > 0) { ringNormal *= -1; } // slightly hacky way of making sure it's the right normal
        debug() << "Normal vector = " << ringNormal << " : mag = " << ringNormal.mag() << endreq;


        // *** What's the true angle of incidence of this ring's photons on the detector plane? Principle axes?
        // Run over pixels and associated photons -> MC photons for this ring and get the true incidence angle
        for (SmartRefVector<RichRecPixel>::const_iterator iPix = pixels.begin(); iPix != pixels.end(); ++iPix) {
          for (vector<RichRecPhoton*>::const_iterator iRecphoton = (*iPix)->richRecPhotons().begin(); 
               iRecphoton != (*iPix)->richRecPhotons().end(); ++iRecphoton) {
            const MCRichOpticalPhoton* mcphoton( m_richRecMCTruth->trueOpticalPhoton(*iRecphoton) );
            if (mcphoton) {
              const HepVector3D truePhotonDirection( mcphoton->pdIncidencePoint() - mcphoton->flatMirrorReflectPoint() );
              // What to do with these direction vectors? Diagonalising? Just scalar to normal for now:
              const double angleToNormal( truePhotonDirection.angle(ringNormal) );
              debug() << "MC photon PD pt = " << mcphoton->pdIncidencePoint() 
                     << " and reflect pt = " << mcphoton->flatMirrorReflectPoint() << endreq;
              debug() << "MC photon angle to normal = " << angleToNormal << endreq;
            } else {
              debug() << "No real MC photon" << endreq;
            }
          }
        }


        // Trace back this ring's normal though the Rich optics
        HepPoint3D endPoint;
        HepVector3D endDir;
        m_raytrace->traceBackFromDetector( ringCentre, ringNormal, endPoint, endDir );
        info() << "Back-traced tracking point on sph mirror = ("
               << endPoint.x() << ", "
               << endPoint.y() << ", "
               << endPoint.z() << ") in direction ("
               << endDir.x() << ", "
               << endDir.y() << ", "
               << endDir.z() << ")" << endreq;


        if ( mcpart == segMCPart && segMCPart ) {
          // Look at Markov pulls on distributions like Cerenkov angle and 
          // centre posn. for pix&track-matched MCMC rings

          // Get Cerenkov angle for MC-true mass hypothesis (from REC track, so not pure truth)
          const double thetaExp( m_ckAngle->avgCherenkovTheta( (*iRing)->richRecSegment(), mcType ) );
          const double thetaMarkov( (*iRing)->radius() );
          info() << "Expected theta = " << thetaExp
                 << " vs. Markov theta = " << thetaMarkov 
                 << " (shift = " << thetaMarkov - thetaExp <<  ")"
                 << endreq;
          m_CkPull[histosetID]->fill(thetaMarkov - thetaExp);


          // Compare ring centre positions
          //const HepPoint3D centreLocal( (*iRing)->centre().x()/scale, (*iRing)->centre().y()/scale, 0 );
          const HepPoint3D segPoint( (*iRing)->richRecSegment()->pdPanelHitPointLocal() );
          //const double localCentrePointSep( segPoint.distance(centreLocal) );
        }


        // *** Get ring purity and efficiency numbers.



        //SmartDataPtr<MCParticles> mcEvt(eventSvc(), MCParticleLocation::Default);
        //sc = toolSvc()->retrieveTool( "DebugTool", m_pDebugTool, this );
        //m_pDebugTool->printEventAsTree(mcEvt);


        // Origin vertices
        if (mcpart) {
          // Examine the particle origin vertex type and position
          const MCVertex::MCVertexType originvertextype( mcpart->originVertex()->type() );
          string originvertextypename("Other");
          if (MCVertex::ppCollision == originvertextype) {
            originvertextypename = "Primary";
          } else if (MCVertex::Decay == originvertextype) {
            originvertextypename = "Decay";
          } else if (MCVertex::Hadronic == originvertextype ||
                     MCVertex::Pair == originvertextype ||
                     MCVertex::Brem == originvertextype ||
                     MCVertex::LHCHalo == originvertextype) {
            originvertextypename = "Interaction";
          } else if (MCVertex::Unknown == originvertextype) {
            originvertextypename = "Unknown";
          }
          info() << "Origin vertex (MC) is " << originvertextypename << endreq;


          // Get the particle origin vertex position
          const HepPoint3D& startPoint = mcpart->originVertex()->position();
          info() << "Origin vtx position = ("
                 << startPoint.x()/m << ", "
                 << startPoint.y()/m << ", "
                 << startPoint.z()/m << ") m" << endreq;


          // Categorize which subdetector the origin falls within by z coord
          // Note that these numbers are NOT precise!
          MCOriginLocation originvertexlocation(Other);
          if (fabs(startPoint.z()/m) < 1.0) { originvertexlocation = InVelo; }
          else if (startPoint.z() > 0.0) {
            if (startPoint.z()/m < 1.9) { originvertexlocation = InRich1; }
            else if (startPoint.z()/m < 3.5) { originvertexlocation = InTT; }
            else if (startPoint.z()/m < 7.0) { originvertexlocation = InMagnet; }
            else if (startPoint.z()/m < 10.0) { originvertexlocation = InT123; }
            else if (startPoint.z()/m < 12.0) { originvertexlocation = InRich2; }
          }
          // Get azimuthal radius
          const double rho(sqrt( startPoint.x() * startPoint.x() + startPoint.y() * startPoint.y() ));


          // *** Need to track MC particle types from the PV and work out why
          // *** they're still leaving rings in Rich2 but aren't tracked. MCTrackInfo, MCEffReconstructible
          debug() << "It segvs after here" << endreq;
          const IMCEffReconstructible::RecblCategory reconstructCategory( m_mcReconstructible->reconstructible(mcpart) );
          debug() << "And before or after here?" << endreq;
          m_RingTrackRecCategory[histosetID]->fill( reconstructCategory );
          debug() << "Should be safe now" << endreq;

          // Fill origin vertex position histos
          m_RingTrackOriginZ[histosetID]->fill( startPoint.z()/m );
          m_RingTrackOriginRZ[histosetID]->fill( startPoint.z()/m, rho/m );
          m_RingTrackOrigin[histosetID]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );


          // Origin position (by subdetector) -specific histos
          if (InVelo == originvertexlocation) {
            m_RingTrackOrigin1Zoom[histosetID]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginZ1[histosetID]->fill( startPoint.z()/m );
            m_RingTrackOriginXY1[histosetID]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ1[histosetID]->fill( startPoint.z()/m, rho/m );
            m_RingTrackOriginRZ1Zoom[histosetID]->fill( startPoint.z()/m, rho/m );
            m_RingTrackOriginInVeloVertexType[histosetID]->fill( mcpart->originVertex()->type() );
            if (Rich::Electron == mcType) {
              m_RingTrackOriginInVeloElectronVertexType[histosetID]->fill( mcpart->originVertex()->type() );
            }
            m_RingTrackOriginInVeloRecCategory[histosetID]->fill( reconstructCategory );
          } else if (InTT == originvertexlocation) {
            m_RingTrackOriginXY2[histosetID]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ2[histosetID]->fill( startPoint.z()/m, rho/m );
          } else if (InT123 == originvertexlocation ) {
            m_RingTrackOriginXY3[histosetID]->fill( startPoint.x()/m, startPoint.y()/m );
            m_RingTrackOriginRZ3[histosetID]->fill( startPoint.z()/m, rho/m );
          }


          // Print out properties of the MC tracks origins clustered in the VELO
          if (InVelo == originvertexlocation) { // in VELO
            map<string, double> partProps;
            partProps["momentum"] = mcpart->momentum().vect().mag();
            partProps["momentum_t"] = mcpart->pt();
            partProps["pseudorapidity"] = mcpart->pseudoRapidity();
            // Just me playing with algorithms
            //for_each(partProps.begin(), partProps.end(), printMap);
          }

        }


        // -------------


        // Decay vertices
        if (mcpart) {
          const SmartRefVector<MCVertex> endVertices = mcpart->endVertices();
          for (SmartRefVector<MCVertex>::const_iterator vtx = endVertices.begin(); vtx != endVertices.end(); ++vtx) {
            const HepPoint3D & endPoint = (*vtx)->position();
            debug() << "Start vtx position = ("
                    << endPoint.x()/m << "m, "
                    << endPoint.y()/m << "m, "
                    << endPoint.z()/m << "m)" << endreq;

            // Get azimuthal distance
            double rho = sqrt( endPoint.x() * endPoint.x() + endPoint.y() * endPoint.y() );

            // Fill decay vertex position histos
            m_RingTrackDecayZ[histosetID]->fill( endPoint.z()/m );
            m_RingTrackDecayRZ[histosetID]->fill( endPoint.z()/m, rho/m );
            m_RingTrackDecay[histosetID]->fill( endPoint.z()/m, endPoint.x()/m, endPoint.y()/m );

            if (  MCVertex::Decay == (*vtx)->type() ) {
              m_RingTrackEndDecayZ[histosetID]->fill( endPoint.z()/m );
            } else {
              m_RingTrackEndNotDecayZ[histosetID]->fill( endPoint.z()/m );
            }

            /*
              if (fabs(startPoint.z()/m) < 1) { // in VELO
              m_RingTrackOrigin1Zoom[recOrNot]->fill( startPoint.z()/m, startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginZ1[recOrNot]->fill( startPoint.z()/m );
              m_RingTrackOriginXY1[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ1[recOrNot]->fill( startPoint.z()/m, rho/m );
              m_RingTrackOriginRZ1Zoom[recOrNot]->fill( startPoint.z()/m, rho/m );
              } else if (startPoint.z()/m > 1.9  && startPoint.z()/m < 3.5 ) { // in TT
              m_RingTrackOriginXY2[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ2[recOrNot]->fill( startPoint.z()/m, rho/m );
              } else if (startPoint.z()/m > 7  && startPoint.z()/m < 10 ) { // in T1-T3
              m_RingTrackOriginXY3[recOrNot]->fill( startPoint.x()/m, startPoint.y()/m );
              m_RingTrackOriginRZ3[recOrNot]->fill( startPoint.z()/m, rho/m );
              }
            */

          }
        }

      } // ring passes best MCParticle match criterion?
    }

  } // end ring loop


  return StatusCode::SUCCESS;
}



StatusCode
RichMarkovRingFinderMoni::bookHistograms() {

  string title, id;
  const int nbins1D(100), nbins2D(50), nbins3D(10);
  double rmax(0);
  const Rich::DetectorType richType[2] = {Rich::Rich1, Rich::Rich2};



  for (int richNo = 0; richNo < 2; ++richNo) {

    // Histos where rec/nonrec doesn't matter (or make sense)
    const string mainindex( Rich::text(richType[richNo]) );
    const string mainsubtitle( Rich::text(richType[richNo]) ); // 
    const string mainhistopath( m_histPth + "/" + Rich::text(richType[richNo]) );
    m_NumRingsPerEvent[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "NumRingsPerEvent", "Number of Markov rings per event in " + mainsubtitle, 51, -1, 101);
    m_NumPixsPerEvent[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "NumPixsPerEvent", "Number of pixels per event in " + mainsubtitle, 51, -5, 505);

    m_MarkovRingRadius[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "MarkovRingRadius", "Ring radius in " + mainsubtitle, 50, 0, 1*m);
    m_MarkovRingNumPixs[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "MarkovRingNumPixs", "Number of pixs per ring in " + mainsubtitle, 51, -0.5, 50.5);
    m_MarkovRingRadiusVsNumPixs[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "MarkovRingRadiusVsNumPixs", "Markov ring radius vs num pixs in "  + mainsubtitle,
                       30, 0, 1*m, 51, -0.5, 50.5);

    m_MarkovRingBestMCMatchNumber[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "MarkovRingBestMCMatchNumber", "Num MC matches for the best MCParticle in " + mainsubtitle, 51, -0.5, 50.5);
    m_MarkovRingBestMCMatchFraction[richType[richNo]] = 
      histoSvc()->book(mainhistopath, "MarkovRingBestMCMatchFraction", "Frac MC matches for the best MCParticle " + mainsubtitle, 50, 0, 1);

    for (int recNo = 0; recNo < 2; ++recNo) {
      // Histos where rec/nonrec matters
      const string recOrNot[2] = {"rec", "notRec"};
      const string trackingTitle[2] = {"tracking-matched MCMC rings", "MCMC rings not found by tracking"};
      const string index( mainindex + recOrNot[recNo] );
      const string subtitle( mainsubtitle + " " + trackingTitle[recNo] );
      const string histopath( mainhistopath + "/" + recOrNot[recNo] );

      // MC type IDs
      title = "MC particle type of tracks giving " + subtitle;
      id = "RingTrackMCType";
      m_RingTrackMCType[index] = histoSvc()->book(histopath, id, title, 6, -1.5, 4.5);

      // Cerenkov angle and center-point pulls dbns
      title = "Cerenkov angle pull dbn of " + subtitle;
      id = "CerenkovAnglePull";
      m_CkPull[index] = histoSvc()->book(histopath, id, title, 50, -0.01, 0.01);

      // *** also look at the 2D correlation of pull magnitude and ring "quality"

      // Track reconstruction category
      title = "Reconstruction type for tracks of " + subtitle + " with origins in the VELO";
      id = "RingTrackOriginInVeloRecCategory";
      m_RingTrackOriginInVeloRecCategory[index] = histoSvc()->book(histopath, id, title, 5, -0.5, 4.5);

      title = "Reconstruction type for tracks of " + subtitle;
      id = "RingTrackRecCategory";
      m_RingTrackRecCategory[index] = histoSvc()->book(histopath, id, title, 5, -0.5, 4.5);

      // Origin vertex types
      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO";
      id = "RingTrackOriginInVeloVtxType";
      m_RingTrackOriginInVeloVertexType[index] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);

      title = "Origin vertex type for origin vertices of " + subtitle + " in the VELO (electrons)";
      id = "RingTrackOriginInVeloElectronVtxType";
      m_RingTrackOriginInVeloElectronVertexType[index] = histoSvc()->book(histopath, id, title, 15, -0.5, 14.5);


      // Vertex positions
      // 1D plots
      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ";
      m_RingTrackOriginZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z origins of " + subtitle;
      id = "RingTrackOriginZ1";
      m_RingTrackOriginZ1[index] = histoSvc()->book(histopath, id, title, nbins1D, -0.5, 0.5);

      title = "Z end points of " + subtitle;
      id = "RingTrackDecayZ";
      m_RingTrackDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z end (decay) points of " + subtitle;
      id = "RingTrackEndDecayZ";
      m_RingTrackEndDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z end (not decay) points of " + subtitle;
      id = "RingTrackNotEndDecayZ";
      m_RingTrackEndNotDecayZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 15);

      title = "Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloZ";
      m_RingTrackDecaysWithVeloOriginZ[index] = histoSvc()->book(histopath, id, title, nbins1D, -1, 20);


      // ----------------

      // 2D plots
      title = "X-Y origins of VELO " + subtitle;
      id = "RingTrackOriginXY1";
      rmax = 0.01;
      m_RingTrackOriginXY1[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "X-Y origins of TT " + subtitle;
      id = "RingTrackOriginXY2";
      rmax = 0.35;
      m_RingTrackOriginXY2[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "X-Y origins of T1-T3 " + subtitle;
      id = "RingTrackOriginXY3";
      rmax = 1.5;
      m_RingTrackOriginXY3[index] = histoSvc()->book(histopath, id, title, nbins2D, -rmax, rmax, nbins2D, -rmax, rmax);

      title = "rho-Z origins of " + subtitle;
      id = "RingTrackOriginRZ";
      rmax = 1.5;
      m_RingTrackOriginRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);

      title = "rho-Z decay points of " + subtitle;
      id = "RingTrackDecayRZ";
      rmax = 1.8;
      m_RingTrackDecayRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 12, nbins2D, 0, rmax);

      title = "rho-Z decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVeloRZ";
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOriginRZ[index] = histoSvc()->book(histopath, id, title, nbins2D, -1, 20, nbins2D, 0, rmax);

      title = "rho-Z origins of VELO " + subtitle;
      id = "RingTrackOriginRZ1";
      rmax = 0.02;
      m_RingTrackOriginRZ1[index] = histoSvc()->book(histopath, id, title, nbins2D, -0.5, 1, nbins2D, 0, rmax);

      title = "rho-Z origins of zoomed VELO " + subtitle;
      id = "RingTrackOriginRZ1Zoom";
      rmax = 0.01;
      m_RingTrackOriginRZ1Zoom[index] = histoSvc()->book(histopath, id, title, nbins2D, -0.2, 0.2, nbins2D, 0, rmax);

      title = "rho-Z origins of TT " + subtitle;
      id = "RingTrackOriginRZ2";
      rmax = 0.35;
      m_RingTrackOriginRZ2[index] = histoSvc()->book(histopath, id, title, nbins2D, 1.9, 3.5, nbins2D, 0, rmax);

      title = "rho-Z origins of T1-T3 " + subtitle;
      id = "RingTrackOriginRZ3";
      rmax = 1.5;
      m_RingTrackOriginRZ3[index] = histoSvc()->book(histopath, id, title, nbins2D, 7, 10, nbins2D, 0, rmax);


      // 3D plots
      title = "Track origins of " + subtitle;
      id = "RingTrackOrigin";
      rmax = 1.8;
      m_RingTrackOrigin[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track origins of zoomed VELO " + subtitle;
      id = "RingTrackOrigin1Zoom";
      rmax = 0.01;
      m_RingTrackOrigin1Zoom[index] = histoSvc()->book(histopath, id, title, nbins3D, -0.2, 0.2, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track decay points of " + subtitle;
      id = "RingTrackDecay";
      rmax = 1.8;
      m_RingTrackDecay[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 12, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

      title = "Track decay points of " + subtitle + " with VELO origins";
      id = "RingTrackDecaysFromVelo";
      rmax = 3.0;
      m_RingTrackDecaysWithVeloOrigin[index] = histoSvc()->book(histopath, id, title, nbins3D, -1, 20, nbins3D, -rmax, rmax, nbins3D, -rmax, rmax);

    }
  }

  return StatusCode::SUCCESS;
}
