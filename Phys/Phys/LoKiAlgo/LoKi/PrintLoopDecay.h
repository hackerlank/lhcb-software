// $Id$
// ============================================================================
#ifndef LOKI_PRINTLOOPDECAY_H 
#define LOKI_PRINTLOOPDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ===========================================================================
#include <string>
#include <limits>
#include <iostream>
// ===========================================================================
// LoKi
// ===========================================================================
#include "LoKi/PhysTypes.h"
// ===========================================================================
// forward declarations 
// ===========================================================================
class MsgStream ;
// ===========================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-26 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  class Loop ;
  // ==========================================================================
  namespace PrintLoop 
  {
    // ========================================================================
    /// the maximal recursion level 
    const int s_maxLevel4 = std::numeric_limits<int>::max() ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ; 
     * 
     *  printDecay( B , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to particle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition
     *  @param level    recursion level 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    std::ostream& printDecay 
    ( const LoKi::Loop&            particle            , 
      std::ostream&                stream              , 
      const LoKi::Types::Cuts&     cut                 ,
      const int                    level = s_maxLevel4 ,
      const std::string&           blank = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ;
     * 
     *  printDecay ( B , std::cout ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to particle to be printed 
     *  @param stream   stream to be used 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    std::ostream& 
    printDecay 
    ( const LoKi::Loop&            particle  , 
      std::ostream&                stream    ) ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ;
     * 
     *  printDecay ( B , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to Particle to be printed 
     *  @param stream   stream to be used 
     *  @param cut      condition 
     *  @param level    recursion level 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    MsgStream& 
    printDecay 
    ( const LoKi::Loop&            particle            , 
      MsgStream&                   stream              , 
      const LoKi::Types::Cuts&     cut                 ,
      const int                    level = s_maxLevel4 ,
      const std::string&           blank = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ;
     * 
     *  printDecay( B , always() ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to Particle to be printed 
     *  @param stream   stream to be used 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    MsgStream& 
    printDecay 
    ( const LoKi::Loop&     particle  , 
      MsgStream&            stream    ) ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ;
     * 
     *  info() << printDecay( p ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to Particle to be printed 
     *  @param cut      condition 
     *  @param level    recursion level 
     *  @param blank    to be printed instead of cutted particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    std::string 
    printDecay 
    ( const LoKi::Loop&            particle            , 
      const LoKi::Types::Cuts&     cut                 ,
      const int                    level = s_maxLevel4 ,
      const std::string&           blank = "<cut>"     ) ;
    // ========================================================================
    /** Simple function to print decay in more or less "readable" format 
     *
     *  @code
     *
     *  Loop B = ... ;
     * 
     *  info() << printDecay( B ) ;
     *
     *  @endcode 
     *
     *  @param particle pointer to Particle to be printed 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    GAUDI_API 
    std::string 
    printDecay 
    ( const LoKi::Loop&            particle ) ;
    // ========================================================================
  } //                                         end of namespace LoKi::PrintLoop 
  // ==========================================================================
  namespace Print
  {
    // ========================================================================
    using namespace LoKi::PrintLoop ;
    // ========================================================================
  } //                                             end of namespace LoKi::Print 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PRINTLOOPDECAY_H
// ============================================================================
