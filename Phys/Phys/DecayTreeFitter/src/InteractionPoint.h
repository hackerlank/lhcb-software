#ifndef DECAYTREEFITTER_INTERACTIONPOINT_H
#define DECAYTREEFITTER_INTERACTIONPOINT_H

#include "ParticleBase.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb
{
  class VertexBase ;
}

namespace decaytreefit 
{

  class InteractionPoint : public ParticleBase
  {
  public:
    InteractionPoint(const LHCb::VertexBase& ipvertex, const LHCb::Particle& daughter, bool forceFitAll) ;
    ~InteractionPoint() ;

    virtual int dim() const { return 3 ; } // (x,y,z)
    virtual ErrCode initPar1(FitParams*) ; 
    virtual ErrCode initPar2(FitParams*) ; 
    virtual ErrCode initCov(FitParams*) const ; 

    virtual int type() const { return kInteractionPoint ; }

    virtual double chiSquare(const FitParams* par) const ;
    
    ErrCode projectIPConstraint(const FitParams& fitpar, Projection&) const ;
    virtual ErrCode projectConstraint(Constraint::Type, const FitParams&, Projection&) const ;

    virtual void addToConstraintList(constraintlist& alist, int depth) const ;
    
    virtual int posIndex() const { return index() ; }
    virtual int momIndex() const { return -1; }//daughters().front()->momIndex() ; } // terrible hack
    virtual int tauIndex() const { return -1; }
    virtual bool hasEnergy() const { return false ; }

  private:
    Gaudi::Vector3      m_ipPos ;    // interaction point position
    Gaudi::SymMatrix3x3 m_ipCov ;    // cov matrix
    Gaudi::SymMatrix3x3 m_ipCovInv ; // inverse of cov matrix
    ParticleBase* m_daughter ;
  } ;
  
}


#endif
