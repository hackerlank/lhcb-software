// $Id: MCChild.h,v 1.1 2007-06-03 20:41:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_MCCHILD_H 
#define LOKI_MCCHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
namespace LoKi 
{
  namespace Child 
  {
    /** get the number of children for the given MC-particle
     *  @attention Only the particles from "decay" end-vertex are counted 
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  mother pointer to MC-particle
     *  @return number of chilren fomr decay vertex 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    std::size_t nChildren ( const LHCb::MCParticle*   mother ) ;
    /** Trivial accessor to the daughter "decay" particles for the given MC-particle
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index    index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    const LHCb::MCParticle* child 
    ( const LHCb::MCParticle*   mother , 
      const size_t             index  ) ;    
    /** Trivial accessor to the daughter "decay" particles for the given MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    inline 
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const size_t             index1   , 
      const size_t             index2   )
    { return child ( child ( particle , index1 ) , index2 ) ; }
    /** Trivial accessor to the daughter "decay" particles for the given MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */    
    inline 
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const size_t             index1   , 
      const size_t             index2   ,
      const size_t             index3   )
    { return child ( child ( particle , index1 ) , index2 , index3 ) ; }
    /** Trivial accessor to the daughter particles for the given MC-particle.
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *  @attention Only the particles from isDecay" end-vertex are considered
     *
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex::isDecay
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @param  index4   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@phsycis.syr.edu
     *  @date 2007-06-02
     */
    inline 
    const LHCb::MCParticle* child
    ( const LHCb::MCParticle*  particle , 
      const size_t             index1   , 
      const size_t             index2   ,
      const size_t             index3   ,
      const size_t             index4   )
    { return child ( child ( particle , index1 ) , index2 , index3 , index4 ) ; }
    
  } // end of namespace LoKi::Child
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCCHILD_H
// ============================================================================
