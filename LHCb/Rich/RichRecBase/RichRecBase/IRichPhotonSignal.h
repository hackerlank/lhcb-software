// $Id: IRichPhotonSignal.h,v 1.1 2003-06-30 15:11:54 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHPHOTONSIGNAL_H
#define RICHRECTOOLS_IRICHPHOTONSIGNAL_H 1

// Event
class RichRecPhoton;

/** @class IRichPhotonSignal IRichPhotonSignal.h
 *
 *  Interface to the tool to calculate for a given photon the predicted contribution
 *  to its associated RichRecPixel, under a certain mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichPhotonSignal( "IRichPhotonSignal" , 1 , 0 );

class IRichPhotonSignal : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichPhotonSignal; }

  /// Predicted pixel signal for a given reconstructed photon under a given mass hypothesis
  virtual double predictedPixelSignal( RichRecPhoton * photon,
                                       const Rich::ParticleIDType id ) = 0;

};

#endif // RICHRECTOOLS_IRICHPHOTONSIGNAL_H
