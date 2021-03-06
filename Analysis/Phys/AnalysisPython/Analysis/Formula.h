// $Id$ 
// ============================================================================
#ifndef ANALYSIS_FORMULA_H 
#define ANALYSIS_FORMULA_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TTreeFormula.h"
// ============================================================================
class TCut ; // ROOT 
// ============================================================================
namespace Analysis 
{
  // ==========================================================================
  /** @class Formula Analysis/Formula.h
   * 
   *  Simple extention of class TTreeFormula 
   *  for easier usage in python 
   *
   *  @see TTreeFormula
   *  @author Vanya Belyaev
   *  @date   2013-05-06
   */
  class GAUDI_API Formula : public TTreeFormula 
  {
    // ========================================================================
  public:
    // ========================================================================
    ClassDef(Analysis::Formula, 1) ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from name, expression and the tree 
    Formula ( const std::string& name       , 
              const std::string& expression ,
              TTree*             tree       ) ;
    /// constructor from name, expression and the tree 
    Formula ( const std::string& name       , 
              const TCut&        expression ,
              TTree*             tree       ) ;
    /// virtual destructor 
    virtual ~Formula () ;
    // ========================================================================
  public:
    // ========================================================================
    /// evaluate the formula 
    double evaluate () ;       // evaluate the formula 
    // is formula OK?
    bool   ok       () const { return this->GetNdim() ; } // is formula OK ? 
    // ========================================================================    
  };
  // ==========================================================================
} //                                                  End of namespace Analysis 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // ANALYSIS_CUTFORMULA_H
// ============================================================================
