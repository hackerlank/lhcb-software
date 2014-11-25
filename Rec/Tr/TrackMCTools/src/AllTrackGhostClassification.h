#ifndef _AllTrackGhostClassification_H
#define _AllTrackGhostClassification_H

#include "TrackGhostClassificationBase.h"


/** @class AllTrackGhostClassification AllTrackGhostClassification.h 
 *
 *  Wrapps around the GhostClassification tools and determines based on the track type which one to use
 *
 *  @author Paul Seyfert
 *  @date   18-11-2014
 */



namespace LHCb{
  class Track;
}

class AllTrackGhostClassification: public TrackGhostClassificationBase {

public:


  /// constructer
  AllTrackGhostClassification(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

  /** destructer */
  virtual ~AllTrackGhostClassification();

  StatusCode initialize();

  /**
   *  Check this is a ghost .
   *  @param aTrack to link
   *  @return bool true if a ghost
   */
  virtual bool isGhost(LHCbIDs::const_iterator& start, 
                       LHCbIDs::const_iterator& stop) const;


 private:

  StatusCode specific(LHCbIDs::const_iterator& start, 
                LHCbIDs::const_iterator& stop, LHCb::GhostTrackInfo& tinfo) const;

  //StatusCode specific(const LHCb::Track& aTrack, LHCb::GhostTrackInfo& tinfo) const;

  ITrackGhostClassification* m_downstreamGhostClassification;
  ITrackGhostClassification* m_longGhostClassification;
  ITrackGhostClassification* m_tTrackGhostClassification;
  ITrackGhostClassification* m_upstreamGhostClassification;
  ITrackGhostClassification* m_veloGhostClassification;
  ITrackGhostClassification* m_veloRGhostClassification;


};



#endif
