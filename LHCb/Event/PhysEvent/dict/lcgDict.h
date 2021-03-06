// $Id: lcgDict.h,v 1.2 2010-05-26 09:31:28 cattanem Exp $
#ifndef PHYSEVENT_LCGDICT_H 
#define PHYSEVENT_LCGDICT_H 1

// Additional classes to be added to automatically generated lcgdict

// begin include files
#include <vector>
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RelatedInfoMap.h"

// end include files

namespace {
  struct _Instantiations {
    // begin instantiations
    std::vector<const LHCb::Particle*>  _i1;
    std::vector<const LHCb::Particle*>::iterator  _i2;
    std::vector<const LHCb::Vertex*>  _i3;
    //
    LHCb::Particle::Selection                       _i4 ;
    Gaudi::NamedRange_<LHCb::Particle::ConstVector> _i5 ;
    Gaudi::Range_<LHCb::Particle::ConstVector>      _i6 ;    

    LHCb::RelatedInfoMap      _i7 ;
    LHCb::RelatedInfoMap::MAP _i8 ;
    
    // end instantiations
  };
}
#endif // PHYSEVENT_LCGDICT_H
