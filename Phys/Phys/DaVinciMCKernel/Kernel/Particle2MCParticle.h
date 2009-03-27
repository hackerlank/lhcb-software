// $Id: Particle2MCParticle.h,v 1.5 2009-03-27 18:00:23 jpalac Exp $
#ifndef KERNEL_PARTICLE2MCPARTICLE_H 
#define KERNEL_PARTICLE2MCPARTICLE_H 1

// Include files
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Relations/Relation.h"
#include "Relations/Relation1D.h"
#include "Kernel/MCAssociation.h"

/** @namespace Particle2MCParticle Kernel/Particle2MCParticle.h
 *  
 *
 *  Namespace containing types corresponding to the Particle -> MCParticle 
 *  uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2009-01-20
 */
namespace Particle2MCParticle {

  typedef LHCb::Relation1D<LHCb::Particle, 
                           LHCb::MCParticle  >            Table      ;

  typedef Relations::Relation<LHCb::Particle, 
                              LHCb::MCParticle >          LightTable ;
  typedef Table::Range                                    Range      ;
  typedef Table::IBase::TypeTraits::Entry                 Relation   ;
  typedef Table::IBase::TypeTraits::Entries               Relations  ;
  typedef Table::To                                       To         ;
  typedef Table::From                                     From       ;

  typedef std::vector<MCAssociation>                      ToVector;


  struct SumWeights : public std::binary_function<MCAssociation, MCAssociation, double>
  {
    double operator() (const double& x,
                       const MCAssociation& assoc) const
    {
      return x + assoc.weight();
    }
  };
    
    struct SortByWeight : public std::binary_function<const MCAssociation, const MCAssociation, bool>
    {
      bool operator() (const MCAssociation& assoc1,
                       const MCAssociation& assoc2) const
      {
        return assoc1.weight() < assoc2.weight();
      }
    
    };

  struct NotInRange : public std::unary_function<MCAssociation, bool>
  {

    NotInRange(const LHCb::MCParticle::ConstVector* range)
      :
      m_toRange(range) 
    {
    }
  
    bool operator() (const MCAssociation& assoc) const
    {
      
      for (LHCb::MCParticle::ConstVector::const_iterator itW = m_toRange->begin();
           itW != m_toRange->end();
           ++itW) {
        if ( (*itW) == assoc.to() ) return false;
      }
      return  true;
    }

  private:

    NotInRange() {}
  
    const LHCb::MCParticle::ConstVector* m_toRange;
    
  };

}
#endif // KERNEL_PARTICLE2MCPARTICLE_H
