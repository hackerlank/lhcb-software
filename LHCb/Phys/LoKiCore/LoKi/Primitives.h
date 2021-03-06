// $Id$ 
// ============================================================================
#ifndef LOKI_PRIMITIVES_H 
#define LOKI_PRIMITIVES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <algorithm>
// ============================================================================
// LHCb 
// ============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/Bit.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/BiFunctions.h"
#include "LoKi/Reference.h"
#include "LoKi/Field.h"
#include "LoKi/valid.h"
#include "LoKi/same.h"
#include "LoKi/apply.h"
#include "LoKi/Operations.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/integer_traits.hpp"
// ============================================================================
// Original Primitives first
// ============================================================================
#define _GEN_LOKI_PRIMITIVES 1
#define typedef_void_TYPE
#define argument_a_unless_void argument a
#define typename_v typename
#define a_unless_void a
#define class_TYPE_unless_void class TYPE,
#endif
// ============================================================================
#ifdef _GEN_LOKI_PRIMITIVES
// ============================================================================
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class TwoFunctors 
   *  helper class to keep two functors of the same type
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01 
   */ 
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class TwoFunctors<void,TYPE2>
#else
  template <class TYPE,class TYPE2>
  class TwoFunctors
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the actual type of underlying functor 
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ;
    // ========================================================================
  public:
    // ========================================================================
    TwoFunctors ( const functor& f1 , 
                  const functor& f2 ) 
      : m_fun1  ( f1 ) 
      , m_fun2  ( f2 ) 
    {}
    /// copy constructor 
    TwoFunctors ( const TwoFunctors&  right ) 
      : m_fun1  ( right.m_fun1 ) 
      , m_fun2  ( right.m_fun2 ) 
    {}    
    /// move  constructor 
    TwoFunctors (       TwoFunctors&& right ) 
      : m_fun1  ( std::move ( right.m_fun1 ) ) 
      , m_fun2  ( std::move ( right.m_fun2 ) )  
    {}
    /// destructor 
    virtual ~TwoFunctors() {}
    // ========================================================================
  public:
    // ========================================================================
    /// evaluate the first functor 
    typename functor::result_type fun1 
    ( argument_a_unless_void ) const { return m_fun1.fun ( a_unless_void ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( argument_a_unless_void ) const { return m_fun2.fun ( a_unless_void ) ; }
    // ========================================================================
  public:
    // ========================================================================
    /// assignement operator 
    TwoFunctors& operator=( const TwoFunctors&  right ) 
    {
      if ( &right == this ) { return *this ; }
      m_fun1 = right.m_fun1 ;
      m_fun2 = right.m_fun2 ;
      return *this ;
    }
    /// move assignement operator 
    TwoFunctors& operator=(       TwoFunctors&& right ) 
    {
      if ( &right == this ) { return *this ; }
      m_fun1 = std::move ( right.m_fun1 ) ;
      m_fun2 = std::move ( right.m_fun2 ) ;
      return *this ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_fun1.func () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_fun2.func () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// no default constructor 
    TwoFunctors () ;                                  // no default constructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the first functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun1 ;          // the first functor 
    /// the second functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun2 ;         // the second functor 
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class And 
   *  helper function to implement logical AND of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>&&</tt> operator for predicates
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A && B             ; // operator form
   *    PfP p2 = LoKi::And( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2> 
  class And<void,TYPE2> : public LoKi::Functor<void,bool> 
#else
  template<class TYPE, class TYPE2=bool> 
  class And : public LoKi::Functor<TYPE,bool> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    And ( const functor& f1 , 
          const functor& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Functor<TYPE,bool>()
      , m_two ( f1 , f2 ) 
    {}
    /// copy constructor
    And ( const And&  right ) 
      : LoKi::AuxFunBase         ( right ) 
      , LoKi::Functor<TYPE,bool> ( right )
      , m_two ( right.m_two ) 
    {}    
    /// move constructor 
    And (       And&& right ) 
      : LoKi::AuxFunBase         ( right ) 
      , LoKi::Functor<TYPE,bool> ( right )
      , m_two ( std::move ( right.m_two ) )
    {}
    /// virtual constructor
    virtual ~And(){}
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: clone method ("")
    virtual And* clone() const { return new And(*this) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return fun1( a_unless_void ) ? fun2 ( a_unless_void )  : false ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << func1() << " & " << func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " && " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    And() ;                               // the default contructor is disabled 
    // ========================================================================
  protected:
    // ========================================================================
    /// evaluate the first functor 
    typename functor::result_type fun1 
    ( argument_a_unless_void )  const { return m_two.fun1 ( a_unless_void ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( argument_a_unless_void )  const { return m_two.fun2 ( a_unless_void ) ; }
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_two.func1 () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_two.func2 () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;       // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  /** @struct Or 
   *  helper function to implement logical OR of 2 predicates 
   *  
   *  It is used by LoKi for implementation of logical 
   *  binary <tt>||</tt> operator for predicates
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A , const Pred& B )
   *  {
   *    PfP p1 = A || B            ;  // operator form
   *    PfP p2 = LoKi::Or( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
  // ==========================================================================
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Or<void,TYPE2> : public LoKi::And<void,TYPE2>
#else
  template<class TYPE, class TYPE2=bool> 
  class Or : public LoKi::And<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the actual type of functor
    typedef LoKi::Functor<TYPE,TYPE2> functor ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Or ( const functor& f1 , const functor& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::And<TYPE,bool>( f1 , f2 ) 
    {}
    /// MANDATORY: virtual constructor
    virtual ~Or(){}
    // ========================================================================
  public:
    // ========================================================================
    /// MANDATORY: clone method ("virtual constructor")
    virtual Or* clone() const { return new Or(*this) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return this->fun1 ( a_unless_void ) ? true : this->fun2 ( a_unless_void ) ; }  
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << " | " << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " || " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    Or() ; // the default contructor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Not
   *
   *  The helper function to implement logical negation 
   *  of the predicate
   *
   *  @code 
   *
   *  typedef Functor<SomeType,bool>            Pred ;
   *  typedef FunctorFromFunctor<SomeType,bool> PfP  ;
   *
   *  void func( const Pred& A )
   *  {
   *    PfP p2 = LoKi::Not( A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Not<void,TYPE2> : public LoKi::Functor<void,bool>
#else
  template<class TYPE, class TYPE2=bool> 
  class Not : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor  
    Not ( const Functor<TYPE,TYPE2>& fun ) 
      : LoKi::AuxFunBase ( std::tie ( fun ) ) 
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
    {} 
    /// virtual destructor 
    virtual ~Not() {}
    /// clone method (mandatory)
    virtual  Not* clone() const { return new Not( *this ); }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    {
      std::logical_not<TYPE2> lnot ;
      return lnot ( this->m_fun.fun ( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (~"  << this->m_fun << ") " ; };
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Not() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; // the functor to be negated 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Negate
   *
   *  The helper function to implement unary negate 
   *  of the function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A )
   *  {
   *    FfF p1 = -A                ; // operator form
   *    FfF p2 = LoKi::Negate( A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Negate<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template<class TYPE,class TYPE2=double> 
  class Negate : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor  
    Negate ( const LoKi::Functor<TYPE,TYPE2>& fun ) 
      : LoKi::AuxFunBase ( std::tie ( fun ) ) 
      , LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
    {} 
    /// virtual destructor 
    virtual ~Negate() {}
    /// clone method (mandatory)
    virtual  Negate* clone() const { return new Negate ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      std::negate<TYPE2> negator ;
      return negator ( m_fun.fun ( a_unless_void ) ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (-"  << this->m_fun << ") " ; };
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Negate() ; // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor to be negated 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; // the functor to be negated 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Less 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A < B               ;  // operator form
   *    PfP p2 = LoKi::Less( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FuctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Less<void,TYPE2> : public LoKi::Functor<void,bool>
#else
  template<class TYPE, class TYPE2=double> 
  class Less : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Less ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
           const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Functor<TYPE,bool>() 
      , m_two ( f1 , f2 )
    {}
    /// copy constructor 
    Less ( const Less&  right ) 
      : LoKi::AuxFunBase        ( right ) 
      , LoKi::Functor<TYPE,bool>( right ) 
      , m_two ( right.m_two )
    {}
    /// move constructor 
    Less (       Less&& right ) 
      : LoKi::AuxFunBase        ( right ) 
      , LoKi::Functor<TYPE,bool>( right ) 
      , m_two ( std::move ( right.m_two ) ) 
    {}
    /// virtual destructor 
    virtual ~Less() {}
    /// clone method (mandatory)
    virtual  Less* clone() const { return new Less( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { 
      std::less<TYPE2> _cmp ;
      return _cmp ( this->fun1 ( a_unless_void ) , this->fun2 ( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "<" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " < " 
        + Gaudi::Utils::toCpp ( this->func2() ) +  ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Less() ; // the default constrictor is disabled 
    // ========================================================================
  protected:
    // ========================================================================
    /// the functor type 
    typedef LoKi::Functor<TYPE,TYPE2>                                 functor ;
    typename functor::result_type fun1 
    ( argument_a_unless_void ) const { return m_two.fun1 ( a_unless_void ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( argument_a_unless_void ) const { return m_two.fun2 ( a_unless_void ) ; }
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_two.func1 () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_two.func2 () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;       // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Equal 
   *  The helper function to implement Equal of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP   ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A == B               ; // operator form
   *    PfP p2 = LoKi::Equal( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Equal<void,TYPE2> : public LoKi::Less<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double> 
  class Equal : public LoKi::Less<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Equal ( const LoKi::Functor<TYPE,TYPE2>& f1          , 
            const LoKi::Functor<TYPE,TYPE2>& f2          ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Less<TYPE,TYPE2> ( f1 , f2 )
    {}
    /// virtual destructor 
    virtual ~Equal() {}
    /// clone method (mandatory)
    virtual  Equal* clone() const { return new Equal ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return equal ( a_unless_void ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "==" << this->func2() << " ) " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " == " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  public:
    // ========================================================================
    /// the actual comparison:
    inline result_type equal ( argument_a_unless_void ) const 
    { 
      LHCb::Math::Equal_To<TYPE2>  _cmp ;                      // the comparator 
      return _cmp ( this->fun1 ( a_unless_void ) , this->fun2 ( a_unless_void ) ) ;
    }
    // ========================================================================
    /// the actual comparison:
    inline result_type not_equal ( argument_a_unless_void ) const 
    { return !this->equal( a_unless_void ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Equal () ;                           // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @struct LessOrEqual 
   *  The helper function to implement Less of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A <= B                ;  // operator form
   *    PfP p2 = LoKi::LessOrEqual( A , B ) ; // explicit form
   *  }
   *  @endcode 
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor

   *  @see LoKi::Function
   *  @see LoKi::PredicateFromPredicate
   *  @see LoKi::PredicateFromTwoFunctions
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class LessOrEqual<void,TYPE2> : public LoKi::Equal<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double> 
  class LessOrEqual : public LoKi::Equal<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from twho functors 
    LessOrEqual ( const Functor<TYPE,TYPE2>& f1 , 
                  const Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Equal<TYPE,TYPE2>( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~LessOrEqual() {}
    /// clone method (mandatory)
    virtual  LessOrEqual* clone() const { return new LessOrEqual ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { 
      std::less_equal<TYPE2> cmp ;
      return cmp ( this->fun1( a_unless_void ) , this->fun2( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "<=" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " <= "
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    LessOrEqual() ;                      // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class NotEqual 
   *  The helper function to implement NotEqual of 2 functions 
   *
   *  It is used by LoKi for implementation of comparison 
   *  operator for functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,bool>   PfP  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    PfP p1 = A != B                  ; // operator form
   *    PfP p2 = LoKi::NotEqual( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class NotEqual<void,TYPE2> : public LoKi::Equal<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double> 
  class NotEqual : public LoKi::Equal<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    typedef typename boost::call_traits<const TYPE2>::param_type T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    NotEqual ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
               const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Equal<TYPE,TYPE2>( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~NotEqual() {}
    /// clone method (mandatory)
    virtual  NotEqual* clone() const { return new NotEqual ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return this -> not_equal ( a_unless_void ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() 
               << "!=" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " ("
        + Gaudi::Utils::toCpp ( this->func1() ) + " != " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    NotEqual() ;                         // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Plus 
   *  The helper function to implement addition of 2 function 
   *
   *  It is used by LoKi for implementation of addition of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A  + B              ; // operator form
   *    FfF f2 = LoKi::Plus( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Plus<void,TYPE2> : public LoKi::Functor<void,double>
#else
  template<class TYPE, class TYPE2=double> 
  class Plus : public LoKi::Functor<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
#ifdef _GEN_LOKI_VOIDPRIMITIVES
    typedef void TYPE;
#endif
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Plus ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
           const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Plus() {}
    /// clone method (mandatory)
    virtual  Plus* clone() const { return new Plus ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      std::plus<TYPE2> _plus ;
      return _plus ( this->fun1( a_unless_void ) , this->fun2( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "+" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " + " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Plus() ;                             // the default constrictor is disabled 
    // ========================================================================
  protected:
    // ========================================================================
    /// the functor type 
    typedef typename LoKi::Functor<TYPE,TYPE2>                        functor ;
    typename functor::result_type fun1 
    ( argument_a_unless_void ) const { return m_two.fun1 ( a_unless_void ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( argument_a_unless_void ) const { return m_two.fun2 ( a_unless_void ) ; }
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_two.func1 () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_two.func2 () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;        // the storage of two functors 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Minus 
   *  The helper function to implement subtraction of 2 function 
   *
   *  It is used by LoKi for implementation of subtraction of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A - B                ; // operator form
   *    FfF f2 = LoKi::Minus( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Minus<void,TYPE2> : public LoKi::Plus<void,double>
#else
  template<class TYPE, class TYPE2=double> 
  class Minus : public LoKi::Plus<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
#ifdef _GEN_LOKI_VOIDPRIMITIVES
    typedef void TYPE;
#endif
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Minus ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
            const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Plus<TYPE,TYPE2>( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Minus() {}
    /// clone method (mandatory)
    virtual  Minus* clone() const { return new Minus ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      std::minus<TYPE2> _minus ;
      return _minus ( this->fun1 ( a_unless_void ) , this->fun2 ( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "-" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " ("
        + Gaudi::Utils::toCpp ( this->func1() ) + " - " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Minus() ;                            // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Divide
   *  The helper function to implement division of 2 function 
   *
   *  It is used by LoKi for implementation of division of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A / B                 ; // operator form
   *    FfF f2 = LoKi::Divide( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Divide<void,TYPE2> : public LoKi::Minus<void,double>
#else
  template<class TYPE, class TYPE2=double> 
  class Divide : public LoKi::Minus<TYPE,TYPE2> 
#endif
  { 
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Divide ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
             const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Minus<TYPE,TYPE2>( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Divide() {}
    /// clone method (mandatory)
    virtual  Divide* clone() const { return new Divide ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      std::divides<TYPE2> _divides ;
      return _divides ( this->fun1 ( a_unless_void ) , this->fun2 ( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "/" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " / " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Divide() ;                           // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Multiply
   *  The helper function to implement multiplication of 2 functions 
   *
   *  It is used by LoKi for implementation of multiplication of  
   *  2 functions:
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = A * B                   ; // operator form
   *    FfF f2 = LoKi::Multiply( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Multiply<void,TYPE2> : public LoKi::Divide<void,double>
#else
  template<class TYPE, class TYPE2=double> 
  class Multiply             : public LoKi::Divide<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from two functors 
    Multiply ( const LoKi::Functor<TYPE,TYPE2>& f1 , 
               const LoKi::Functor<TYPE,TYPE2>& f2 ) 
      : LoKi::AuxFunBase ( std::tie ( f1 , f2 ) ) 
      , LoKi::Divide<TYPE,TYPE2>( f1 , f2 ) 
    {}
    /// virtual destructor 
    virtual ~Multiply() {}
    /// clone method (mandatory)
    virtual  Multiply* clone() const { return new Multiply ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      std::multiplies<TYPE2> _multiplies ;
      return _multiplies ( this->fun1 ( a_unless_void ) , this->fun2 ( a_unless_void ) ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->func1() << "*" << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func1() ) + " * " 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constrictor is disabled
    Multiply() ; // the default constrictor is disabled 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Min 
   *
   *  Simple functor to find the inimum for the functors 
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   * 
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f2 = LoKi::Min( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFomrFunctor
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Min<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double> 
  class Min : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:    
    // ========================================================================
    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , fun2 ) 
    {}
    /// constructor from the function and constant 
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , T2 fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( fun1 , LoKi::Constant<TYPE,TYPE2> ( fun2 )  ) 
    {}
    /// constructor from the function and constant 
    Min ( T2 fun1 , const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( LoKi::Constant<TYPE,TYPE2> ( fun1 ) , fun2 ) 
    {}
    /** constructor from 3 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 , fun3 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Min ( fun1 , fun2 ) , fun3 ) 
    {}
    /** constructor from 4 functions 
     *  @param fun1 the first function
     *  @param fun2 the second function
     *  @param fun3 the third function
     *  @param fun4 the fourth function
     */
    Min ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 , 
          const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 , fun3 , fun4 ) ) 
      , LoKi::Functor<TYPE,TYPE2>()
      , m_two ( Min ( Min ( fun1 , fun2 ) , fun3 ) , fun4 ) 
    {}
    /// virtual destructor 
    virtual ~Min() {}
    /// clone method (mandatory)
    virtual  Min* clone() const { return new Min ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { 
      return  std::min ( this->fun1 ( a_unless_void ) ,
                         this->fun2 ( a_unless_void ) , std::less<TYPE2>() ) ;
    }    
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " min(" << this->func1() << "," << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " LoKi::min(" 
        + Gaudi::Utils::toCpp ( this->func1() ) + "," 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Min () ;                             // the default constructor is disabled
    // ========================================================================
  protected:
    // ========================================================================
    /// the functor type 
    typedef typename LoKi::Functor<TYPE,TYPE2>                        functor ;
    typename functor::result_type fun1 
    ( argument_a_unless_void ) const { return m_two.fun1 ( a_unless_void ) ; }
    /// evaluate the first functor 
    typename functor::result_type fun2 
    ( argument_a_unless_void ) const { return m_two.fun2 ( a_unless_void ) ; }
    // ========================================================================
    /// get the first functor 
    const functor& func1 ()           const { return m_two.func1 () ; }
    /// get the second functor 
    const functor& func2 ()           const { return m_two.func2 () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of two functors
    // ========================================================================
  };
  // ==========================================================================
  /** @class Max 
   *  Return  the maximum from functions 
   *
   *  Simple functor to find the inimum for the functors 
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   * 
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f2 = LoKi::Max( A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFomrFunctor
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-01
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Max<void,TYPE2> : public LoKi::Min<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double> 
  class Max : public LoKi::Min<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:    
    // ========================================================================
    /** constructor from 2 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
      , LoKi::Min<TYPE,TYPE2>( fun1 , fun2 )
    {}
    /// constructor from the function and constant 
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , T2 fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) )  
      , LoKi::Min<TYPE,TYPE2>( fun1 , fun2 )
    {}
    /// constructor from the function and constant 
    Max ( T2 fun1 , const LoKi::Functor<TYPE,TYPE2>& fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) )
      , LoKi::Min<TYPE,TYPE2>( fun1 , fun2 )
    {}
    /** constructor from 3 functions 
     *  @param fun1 the first  function
     *  @param fun2 the second function
     *  @param fun3 the third  function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 , fun3 ) )
      , LoKi::Min<TYPE,TYPE2>( Max ( fun1 , fun2 ) , fun3 ) 
    {}
    /** constructor from 4 functions 
     *  @param fun1 the first function
     *  @param fun2 the second function
     *  @param fun3 the third function
     *  @param fun4 the fourth function
     */
    Max ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
          const LoKi::Functor<TYPE,TYPE2>& fun2 ,
          const LoKi::Functor<TYPE,TYPE2>& fun3 , 
          const LoKi::Functor<TYPE,TYPE2>& fun4 ) 
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 , fun3 , fun4 ) )
      , LoKi::Min<TYPE,TYPE2> ( Max ( Max( fun1 , fun2 ) , fun3 ) , fun4 ) 
    {}
    /// virtual destructor 
    virtual ~Max() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Max* clone() const { return new Max ( *this ) ; }
    /// MANDATORY: the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      return std::max ( this -> fun1 ( a_unless_void ) , 
                        this -> fun2 ( a_unless_void ) , std::less<TYPE2>() ) ;
    }    
    /// OPTIONAL: the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " max(" << this->func1() << "," << this->func2() << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " LoKi::max(" 
        + Gaudi::Utils::toCpp ( this->func1() ) + "," 
        + Gaudi::Utils::toCpp ( this->func2() ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Max () ;                             // the default constructor is disabled
    // ========================================================================
  };
  // ==========================================================================
  /** @class SimpleSwitch
   *  It is a function with acts similar to switch:
   *  
   *  result = condition ? value1 : value2 ;
   *  
   *  In particular it is useful for "conversion" 
   *   of "predicates" into "functions"
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2005-02-11
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class SimpleSwitch<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class SimpleSwitch : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the predicate and 2 constants 
     *
     *  The function return a value of the first constant  
     *  if predicate evaluated to "true", otherwise the 
     *  second constant is returned 
     *
     *  @param cut predicate for branch
     *  @param val1 the first constant 
     *  @param val2 the second constant 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    SimpleSwitch
    ( const LoKi::Functor<TYPE,bool>& cut  , T2 val1 , T2 val2 )
      : LoKi::AuxFunBase ( std::tie ( cut , val1 , val2 ) ) 
      , LoKi::Functor<TYPE,TYPE2> (      ) 
      , m_cut  ( cut  ) 
      , m_val1 ( val1 )
      , m_val2 ( val2 )
    {}
    /// destructor 
    virtual ~SimpleSwitch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  SimpleSwitch* clone() const { return new SimpleSwitch ( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return m_cut( a_unless_void ) ? m_val1 : m_val2 ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " switch("  
               << this->m_cut  << ","  
               << this->m_val1 << "," 
               << this->m_val2 << ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled    
    SimpleSwitch() ;                     // the default constructor is disabled    
    // ========================================================================
  private:
    // ========================================================================
    /// the condition 
    LoKi::FunctorFromFunctor <TYPE,bool> m_cut  ; // the condiftion 
    /// the first value 
    TYPE2                                m_val1 ; // the first value 
    /// the second value 
    TYPE2                                m_val2 ; // the second value 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Switch
   *  
   *  It is a bit advances version fo SimpleSwitch function 
   *   with acts similar to switch:
   *  
   *  result = condition ? function1 : function2 ;
   * 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2005-02-11
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE2>
  class Switch<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template<class TYPE, class TYPE2=double>
  class Switch : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from predicate and 2 functions 
     *
     *  The function return a value of the first function 
     *  if predicate evaluates to "true", otherwise the 
     *  value of second function is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the first  fuction
     *  @param fun2 the second function
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , 
      const LoKi::Functor<TYPE,TYPE2>&  fun1 , 
      const LoKi::Functor<TYPE,TYPE2>&  fun2 )
      : LoKi::AuxFunBase ( std::tie ( cut , fun1 , fun2 ) ) 
      , Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( fun1 , fun2 )  
    {}    
    /** constructor from predicate ,function and constant
     *
     *  The function return a value of the function 
     *  if predicate evaluates to "true", otherwise the 
     *  value of constant is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the function
     *  @param fun2 the constant 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , 
      const LoKi::Functor<TYPE,TYPE2>&  fun1 , T2 fun2 )
      : LoKi::AuxFunBase ( std::tie ( cut , fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( fun1 , LoKi::Constant<TYPE,TYPE2>( fun2 ) )  
    {}    
    /** constructor from predicate ,function and constant 
     *
     *  The function return a value of the constant  
     *  if predicate evaluates to "true", otherwise the 
     *  value of function is returned 
     *
     *  @param cut predicate for branch
     *  @param fun1 the constant 
     *  @param fun2 the function
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>& cut  , 
      T2                              fun1 , 
      const Functor<TYPE,TYPE2>&      fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( cut , fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2>( fun1 ) , fun2 )  
    {}
    /** constructor from predicate and 2 constants 
     *
     *  The function return a value of the first constant  
     *  if predicate evaluates to "true", otherwise the 
     *  second constant is returned 
     *
     *  The constructed fuction object has the same 
     *  functionality as SimpleSwitch, but a bit 
     *  less performant. 
     *  @see SimpleSwitch 
     *  
     *  @param cut predicate for branch
     *  @param fun1 the first constant 
     *  @param fun2 the second constant 
     *
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-11
     */
    Switch
    ( const LoKi::Functor<TYPE,bool>&   cut  , T2 fun1 , T2 fun2 ) 
      : LoKi::AuxFunBase ( std::tie ( cut , fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2>  (      ) 
      , m_cut  ( cut  ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2>( fun1 ) ,
                 LoKi::Constant<TYPE,TYPE2>( fun2 ) )
    {}
    /// destructor 
    virtual ~Switch() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Switch* clone() const { return new Switch ( *this ) ; }
    /// MANDATORY: the only one essential method:
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { return 
        m_cut.fun  ( a_unless_void ) ? 
        m_two.fun1 ( a_unless_void ) :
        m_two.fun2 ( a_unless_void ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " switch("     
               << this->m_cut          << "," 
               << this->m_two.func1()  << "," 
               << this->m_two.func2()  << ") "  ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    Switch () ;                           // the defautl contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the condition 
    LoKi::FunctorFromFunctor<TYPE,bool> m_cut  ;               // the condition 
    /// the actual storage of two functors 
    LoKi::TwoFunctors<TYPE,TYPE2>       m_two ;      // the storage of functors 
    // ========================================================================
  };
  // ==========================================================================
  /** @class ComposeFunction
   *  The helper class to implement function of function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<SomeType,double> FfF  ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = sin( A )                         ; // short form
   *    FfF f2 = LoKi::ComposeFunction( sin , A ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFronFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class ComposeFunction<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class ComposeFunction : public LoKi::Functor<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  protected:
    // ========================================================================
    /// the actual type for the function
    typedef double (*Func) ( double ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor 
    ComposeFunction ( Func                             func           ,  
                      const LoKi::Functor<TYPE,TYPE2>& fun            , 
                      const std::string&               desc = "'fun'" ) 
      : LoKi::Functor<TYPE,TYPE2>()
      , m_fun  ( fun  ) 
      , m_func ( func ) 
      , m_desc ( desc ) 
    {}     
    /// copy contructor
    ComposeFunction ( const ComposeFunction& right ) 
      : LoKi::AuxFunBase          ( right )      
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_fun                     ( right.m_fun  ) 
      , m_func                    ( right.m_func ) 
      , m_desc                    ( right.m_desc ) 
    {}
    /// virtual destructor 
    virtual ~ComposeFunction(){}
    /// clone method (mandatory!)
    virtual ComposeFunction*  clone () const 
    { return new ComposeFunction ( *this ) ; }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return (*m_func) ( m_fun . fun ( a_unless_void ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " " << this->m_desc << "("  << this->m_fun << ") " ; };
    // to C++
    virtual std::string   toCpp() const 
    { return "LoKi::" + this->m_desc + "("  + Gaudi::Utils::toCpp ( this->m_fun ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    ComposeFunction() ; // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun  ; // the functor 
    /// the function:
    Func                                 m_func ; // the function:
    /// the function description:
    std::string                          m_desc ; // description 
    // ========================================================================
  };
  // ==========================================================================
  /** @class ComposeFunction2
   *
   *  The helper structure to implement function of function
   *
   *  @code 
   *
   *  typedef Functor<SomeType,double>            Func ;
   *  typedef FunctorFromFunctor<Sometype,double> FfF ;
   *
   *  void func( const Func& A , const Func& B )
   *  { 
   *    FfF f1 = pow( A , B )                          ; // short form
   *    FfF f2 = LoKi::ComposeFunction2 ( pow , A , B ) ; // explicit form
   *  }
   *
   *  @endcode 
   *
   *  @see LoKi/Operators.h
   *  @see LoKi::Functor
   *  @see LoKi::FunctorFromFunctor
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-15
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class ComposeFunction2<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class ComposeFunction2 : public LoKi::Functor<TYPE,TYPE2> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    /// the actual type of the function 
    typedef double (*Func) ( double , double )      ;
    /// constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor 
    ComposeFunction2 
    ( Func                             func           ,  
      const LoKi::Functor<TYPE,TYPE2>& fun1           , 
      const LoKi::Functor<TYPE,TYPE2>& fun2           , 
      const std::string&               desc = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( fun1 ,fun2 )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      const LoKi::Functor<TYPE,TYPE2>& fun1            , 
      T2                               val2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( fun1 , LoKi::Constant<TYPE,TYPE2> ( val2 ) )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      T2                               val1            , 
      const LoKi::Functor<TYPE,TYPE2>& fun2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2> ( val1 ) , fun2 )
      , m_desc ( desc ) 
    {}
    /// constructor 
    ComposeFunction2 
    ( Func                             func            ,  
      T2                               val1            , 
      T2                               val2            , 
      const std::string&               desc  = "'fun'" )
      : LoKi::Functor<TYPE,TYPE2>() 
      , m_func ( func ) 
      , m_two  ( LoKi::Constant<TYPE,TYPE2> ( val1 ) ,
                 LoKi::Constant<TYPE,TYPE2> ( val2 ) )
      , m_desc ( desc ) 
    {}
    /// copy constructor 
    ComposeFunction2 ( const ComposeFunction2& right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2>( right ) 
      , m_func ( right.m_func ) 
      , m_two  ( right.m_two  ) 
      , m_desc ( right.m_desc ) 
    {}
    /// virtual destructor 
    virtual ~ComposeFunction2() {}
    /// clone method (mandatory!)
    virtual ComposeFunction2*  clone   () const 
    { return new ComposeFunction2( *this ); }
    /// the only one essential method ("function")      
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return (*m_func) ( m_two. fun1 ( a_unless_void ) , m_two.fun2( a_unless_void ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " " 
               << m_desc         << "("  
               << m_two.func1 () << "," 
               << m_two.func2 () << ") " ; }
    /// to C++
    virtual std::string toCpp() const 
    { return "LoKi::" + this->m_desc + "("  
        + Gaudi::Utils::toCpp ( this->m_two.func1 () ) + ", " 
        + Gaudi::Utils::toCpp ( this->m_two.func2 () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the function itself 
    Func m_func        ; // the function itself 
    /// the storage of functors 
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ; // the storage of functors 
    /// the funtion descrition
    std::string m_desc ; // the funtion descrition
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Compose 
   *  the general case of fun2(fun1) function:
   */  
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE1, class TYPE2, class TYPE3>
  class Compose<void,TYPE1,TYPE2,TYPE3> : public LoKi::Functor<void,TYPE2>
#else
  template <class TYPE,class TYPE1, class TYPE2, class TYPE3=TYPE1>
  class Compose : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// contructor
    Compose
    ( const LoKi::Functor<TYPE,TYPE1>&  fun1 , 
      const LoKi::Functor<TYPE3,TYPE2>& fun2 )
      : LoKi::AuxFunBase ( std::tie ( fun1 , fun2 ) ) 
      , LoKi::Functor<TYPE,TYPE2> () 
      , m_fun1 ( fun1 ) 
      , m_fun2 ( fun2 )
    {}
    /// copy constructor
    Compose ( const Compose& right ) 
      : LoKi::AuxFunBase ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right ) 
      , m_fun1 ( right.m_fun1 ) 
      , m_fun2 ( right.m_fun2 )
    {}
    /// MANDATORY: virtual destructor
    virtual ~Compose() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Compose* clone() const { return new Compose ( *this ) ; }    
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { 
      const LoKi::Apply<TYPE,TYPE1>  f1 ( &m_fun1.func() ) ;
      const LoKi::Apply<TYPE3,TYPE2> f2 ( &m_fun2.func() ) ;
      return f2.eval ( f1.eval ( a_unless_void ) ) ;
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (" << this->m_fun1 << ">>" << this->m_fun2  << ") " ; }   
    // ========================================================================
  private:
    // ========================================================================
    /// the first functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE1>  m_fun1  ;        // the first functor 
    /// the second functor 
    LoKi::FunctorFromFunctor<TYPE3,TYPE2> m_fun2  ;       // the second functor 
    // ========================================================================
  } ;
  // ==========================================================================
#ifndef _GEN_LOKI_VOIDPRIMITIVES
  // ==========================================================================
  /** @class Valid 
   *  The trivial predicate to check the validity of argument.
   *  It is OK for any pointer-like types or for types with
   *  implemented implicit conversion to "bool" 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   */
  template <class TYPE>
  class Valid : public LoKi::Functor<TYPE,bool>
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor 
    Valid() : Functor<TYPE,bool> () {} 
    /// copy constructor 
    Valid( const Valid& right ) 
      : LoKi::AuxFunBase   ( right ) 
      , Functor<TYPE,bool> ( right ) 
    {}
    /// virtual destructor 
    virtual ~Valid() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Valid* clone() const { return new Valid( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument_a_unless_void ) const 
    { return LoKi::valid ( a_unless_void ) ? true : false  ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (Valid?)" ; }
    // ========================================================================
  };
  // ==========================================================================
  /** @class TheSame 
   *  Trivial predicate to check if the argument 
   *  is equal to some predefined value. 
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date 2004-02-11
   *  @warning this functor has not valid <c>toCpp</c> method 
   */
  template <class TYPE>
  class TheSame : public LoKi::Functor<TYPE,bool>
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor form the value 
    TheSame ( argument value ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_value ( value ) 
    {}
    /// copy constructor 
    TheSame
    ( const TheSame& right ) 
      : LoKi::AuxFunBase         ( right ) 
      , LoKi::Functor<TYPE,bool> ( right )
      , m_value                  ( right.m_value )
    {}
    /// virtual destructor 
    virtual ~TheSame() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  TheSame* clone() const { return new TheSame( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument object ) const 
    { return LoKi::same ( m_value , object ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " (SAME?) "; }
    // ========================================================================
  private :
    // ========================================================================
    // the default contructor is disabled
    TheSame();
    // ========================================================================
  private:
    // ========================================================================
    // the predefined value 
    TYPE m_value ;
    // ========================================================================
  };
#endif
  // ==========================================================================
  /** @class EqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class EqualToValue<void,TYPE2> : public LoKi::Functor<void,bool>
#else
  template <class TYPE, class TYPE2=double>
  class EqualToValue : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    EqualToValue 
    ( T2 val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /// copy constructor 
    EqualToValue ( const EqualToValue&  right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
    {}    
    /// move constructor 
    EqualToValue (       EqualToValue&& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,bool>  ( right )
      , m_fun ( std::move ( right.m_fun ) ) 
      , m_val ( std::move ( right.m_val ) )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~EqualToValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToValue* clone() const { return new EqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return equal_to ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this->func () 
               << "==" << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " == " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  public:
    // ========================================================================
    inline result_type equal_to ( argument_a_unless_void ) const
    { 
      // the comparator 
      LHCb::Math::Equal_To<TYPE2> _cmp ;
      return _cmp ( this->m_fun.fun ( a_unless_void ) , this->m_val ) ; 
    }    
    // ========================================================================
    inline result_type not_equal_to ( argument_a_unless_void ) const
    { return ! this->equal_to ( a_unless_void ) ; }  
    // ========================================================================
    inline result_type less    ( argument_a_unless_void ) const
    { 
      // the comparator 
      std::less<TYPE2> _cmp ;
      return _cmp ( this->m_fun.fun ( a_unless_void ) , this->m_val ) ; 
    }    
    // ========================================================================
    inline result_type greater ( argument_a_unless_void ) const
    { 
      // the comparator 
      std::less<TYPE2> _cmp ;
      return _cmp ( this->m_val , this->m_fun.fun ( a_unless_void ) ) ; 
    }    
    // ========================================================================
    inline result_type less_or_equal ( argument_a_unless_void ) const
    { 
      // the comparator 
      std::less<TYPE2>            _cmp1 ;
      LHCb::Math::Equal_To<TYPE2> _cmp2 ;
      TYPE2 _r = this->m_fun.fun ( a_unless_void ) ;
      return _cmp1 ( _r , this->m_val ) || _cmp2 ( _r , this->m_val ) ; 
    }    
    // ========================================================================
    inline result_type greater_or_equal ( argument_a_unless_void ) const
    { 
      // the comparator 
      std::less<TYPE2>            _cmp1 ;
      LHCb::Math::Equal_To<TYPE2> _cmp2 ;
      TYPE2 _r = this->m_fun.fun ( a_unless_void ) ;
      return _cmp1 ( this->m_val , _r ) || _cmp2 ( _r , this->m_val ) ; 
    }    
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,TYPE2>& func () const { return m_fun.func() ; }
    const TYPE2&                     val  () const { return m_val        ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    EqualToValue();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ;                 // the functor 
    /// the value 
    TYPE2  m_val ;                                                 // the value 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class NotEqualToValue 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class NotEqualToValue<void,TYPE2> : public LoKi::EqualToValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class NotEqualToValue : public LoKi::EqualToValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    /// the constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor for the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision 
     */
    NotEqualToValue
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , T2 val ) 
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2>( fun , val ) 
    {}
    /** constructor from the function and the value 
     *  @param val the reference value 
     *  @param fun the function
     *  @param eps the relative precision 
     */
    NotEqualToValue
    ( T2 val , const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2>( val , fun ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToValue(){} ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual  NotEqualToValue* clone() const { return new NotEqualToValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->not_equal_to ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this -> func () 
               << "!=" << this -> val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " != " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    NotEqualToValue();
    // ========================================================================
  };
  // ==========================================================================
  /** @class LessThanValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-19
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class LessThanValue<void,TYPE2> : public LoKi::EqualToValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class LessThanValue             : public LoKi::EqualToValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    LessThanValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase   ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    LessThanValue 
    ( const LessThanValue& right )
      : LoKi::AuxFunBase                ( right ) 
      , LoKi::EqualToValue<TYPE,TYPE2>  ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~LessThanValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  LessThanValue* clone() const { return new LessThanValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->less ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this->func () 
               << "<"  << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " < " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    LessThanValue();
    // ========================================================================
  };
  // ==========================================================================
  /** @class LessOrEqualValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-19
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class LessOrEqualValue<void,TYPE2> : public LoKi::EqualToValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class LessOrEqualValue             : public LoKi::EqualToValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    LessOrEqualValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase    ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2> ( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    LessOrEqualValue 
    ( const LessOrEqualValue& right )
      : LoKi::AuxFunBase                ( right ) 
      , LoKi::EqualToValue<TYPE,TYPE2>  ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~LessOrEqualValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  LessOrEqualValue* clone() const { return new LessOrEqualValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->less_or_equal ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("  << this->func () 
               << "<="  << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " <= " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    LessOrEqualValue();
    // ========================================================================
  };
  // ==========================================================================
  /** @class GreaterThanValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-19
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class GreaterThanValue<void,TYPE2> : public LoKi::EqualToValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class GreaterThanValue             : public LoKi::EqualToValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    GreaterThanValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase   ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    GreaterThanValue 
    ( const GreaterThanValue& right )
      : LoKi::AuxFunBase                ( right ) 
      , LoKi::EqualToValue<TYPE,TYPE2>  ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~GreaterThanValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  GreaterThanValue* clone() const { return new GreaterThanValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->greater ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this->func () 
               << ">"  << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " > " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    GreaterThanValue();
    // ========================================================================
  };  
  // ==========================================================================
  /** @class GreaterOrEqualValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-19
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class GreaterOrEqualValue<void,TYPE2> : public LoKi::EqualToValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class GreaterOrEqualValue             : public LoKi::EqualToValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    GreaterOrEqualValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase   ( std::tie ( fun , val ) )
      , LoKi::EqualToValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    GreaterOrEqualValue 
    ( const GreaterOrEqualValue& right )
      : LoKi::AuxFunBase                ( right ) 
      , LoKi::EqualToValue<TYPE,TYPE2>  ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~GreaterOrEqualValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  GreaterOrEqualValue* clone() const { return new GreaterOrEqualValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->greater_or_equal ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("  << this->func () 
               << ">="  << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " >= " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    GreaterOrEqualValue();
    // ========================================================================
  };
  // ==========================================================================
  /** @class MultiplyByValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class MultiplyByValue<void,TYPE2> : public LoKi::Functor<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class MultiplyByValue             : public LoKi::Functor<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    MultiplyByValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    MultiplyByValue 
    ( T2                                val , 
      const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::AuxFunBase ( std::tie ( val , fun ) )
      , LoKi::Functor<TYPE,TYPE2>() 
      , m_fun ( fun ) 
      , m_val ( val ) 
    {}
    // ========================================================================
    /// copy constructor 
    MultiplyByValue 
    ( const MultiplyByValue& right )
      : LoKi::AuxFunBase          ( right ) 
      , LoKi::Functor<TYPE,TYPE2> ( right )
      , m_fun ( right.m_fun ) 
      , m_val ( right.m_val )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~MultiplyByValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  MultiplyByValue* clone() const { return new MultiplyByValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->mult ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->func () 
               << "*"    << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " * " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,TYPE2>& func () const { return m_fun.func() ; }
    const TYPE2&                     val  () const { return m_val        ; }
    // ========================================================================
  protected :
    // ========================================================================
    inline result_type mult    ( argument_a_unless_void ) const
    { return ( this->m_fun.fun ( a_unless_void ) ) * ( this->m_val ) ; }
    // ========================================================================
    inline result_type sum     ( argument_a_unless_void ) const
    { return ( this->m_fun.fun ( a_unless_void ) ) + ( this->m_val ) ; }
    // ========================================================================
    inline result_type divide1 ( argument_a_unless_void ) const
    { return ( this->m_fun.fun ( a_unless_void ) ) / ( this->m_val ) ; }
    // ========================================================================
    inline result_type divide2 ( argument_a_unless_void ) const
    { return ( this->m_val ) / ( this->m_fun.fun ( a_unless_void ) ) ; }
    // ========================================================================
    inline result_type minus1  ( argument_a_unless_void ) const
    { return ( this->m_fun.fun ( a_unless_void ) ) - ( this->m_val ) ; }
    // ========================================================================
    inline result_type minus2  ( argument_a_unless_void ) const
    { return ( this->m_val ) - ( this->m_fun.fun ( a_unless_void ) ) ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    MultiplyByValue();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ;                 // the functor 
    /// the value 
    TYPE2  m_val ;                                                 // the value 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class SumByValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class SumByValue<void,TYPE2> : public LoKi::MultiplyByValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class SumByValue             : public LoKi::MultiplyByValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    SumByValue 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    SumByValue 
    ( T2                                val , 
      const LoKi::Functor<TYPE,TYPE2>&  fun ) 
      : LoKi::AuxFunBase ( std::tie ( val , fun ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( val , fun ) 
    {}
    // ========================================================================
    /// copy constructor 
    SumByValue 
    ( const SumByValue& right )
      : LoKi::AuxFunBase                  ( right ) 
      , LoKi::MultiplyByValue<TYPE,TYPE2> ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~SumByValue(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  SumByValue* clone() const { return new SumByValue(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->sum ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->func () 
               << "+"    << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " + " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    SumByValue();
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Minus1 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class Minus1<void,TYPE2> : public LoKi::MultiplyByValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class Minus1             : public LoKi::MultiplyByValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    Minus1 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    Minus1 
    ( const Minus1& right )
      : LoKi::AuxFunBase                  ( right ) 
      , LoKi::MultiplyByValue<TYPE,TYPE2> ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~Minus1(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  Minus1* clone() const { return new Minus1(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->minus1 ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->func () 
               << "-"    << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " - " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    Minus1();
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Minus2 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class Minus2<void,TYPE2> : public LoKi::MultiplyByValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class Minus2             : public LoKi::MultiplyByValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    Minus2 
    ( T2                                val ,
      const LoKi::Functor<TYPE,TYPE2>&  fun )
      : LoKi::AuxFunBase ( std::tie ( val , fun ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( val , fun ) 
    {}
    // ========================================================================
    /// copy constructor 
    Minus2 
    ( const Minus2& right )
      : LoKi::AuxFunBase                  ( right ) 
      , LoKi::MultiplyByValue<TYPE,TYPE2> ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~Minus2(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  Minus2* clone() const { return new Minus2(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->minus2 ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->val  () 
               << "-"    << this->func () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->val  () ) + " - " 
        + Gaudi::Utils::toCpp ( this->func () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    Minus2();
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Divide1 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class Divide1<void,TYPE2> : public LoKi::MultiplyByValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class Divide1             : public LoKi::MultiplyByValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    Divide1 
    ( const LoKi::Functor<TYPE,TYPE2>&  fun , 
      T2                                val )
      : LoKi::AuxFunBase ( std::tie ( fun , val ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( fun , val ) 
    {}
    // ========================================================================
    /// copy constructor 
    Divide1 
    ( const Divide1& right )
      : LoKi::AuxFunBase                  ( right ) 
      , LoKi::MultiplyByValue<TYPE,TYPE2> ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~Divide1(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  Divide1* clone() const { return new Divide1(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->divide1 ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->func () 
               << "/"    << this->val  () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->func () ) + " / " 
        + Gaudi::Utils::toCpp ( this->val  () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    Divide1();
    // ========================================================================
  };
  // ==========================================================================
  /** @class Divide2 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-01-27
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <class TYPE2>
  class Divide2<void,TYPE2> : public LoKi::MultiplyByValue<void,TYPE2>
#else
  template <class TYPE, class TYPE2=double>
  class Divide2             : public LoKi::MultiplyByValue<TYPE,TYPE2>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument    argument    ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
    // constant type 
    typedef typename LoKi::Constant<TYPE,TYPE2>::T2 T2 ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param val the reference value 
     *  @param eps the relative precision
     */
    Divide2 
    ( T2                                val ,
      const LoKi::Functor<TYPE,TYPE2>&  fun )
      : LoKi::AuxFunBase ( std::tie ( val , fun ) )
      , LoKi::MultiplyByValue<TYPE,TYPE2>( val , fun ) 
    {}
    // ========================================================================
    /// copy constructor 
    Divide2 
    ( const Divide2& right )
      : LoKi::AuxFunBase                  ( right ) 
      , LoKi::MultiplyByValue<TYPE,TYPE2> ( right )
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~Divide2(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  Divide2* clone() const { return new Divide2(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this->divide2 ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " ("   << this->val  () 
               << "/"    << this->func () << ") " ; }
    /// OPTIONAL: C++ print
    virtual std::string   toCpp () const 
    { return " (" 
        + Gaudi::Utils::toCpp ( this->val  () ) + " / " 
        + Gaudi::Utils::toCpp ( this->func () ) + ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    Divide2();
    // ========================================================================
  };
  // ==========================================================================
  /** compare 2 objects using the comparison criteria CMP , 
   *  applied to functions:
   *  <c> cmp( f1(a) , f2(b) ) </c>
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   2002-07-24
   */ 
#ifndef _GEN_LOKI_VOIDPRIMITIVES
  template<class TYPE , class CMP=std::less<double> , class TYPE2=double>
  class Compare
  {
  public:
    // ========================================================================
    typedef_void_TYPE
    /// typedef for actual function 
    typedef LoKi::Functor<TYPE,TYPE2>   function ;
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ;
    // typedef for comparison criteria 
    typedef const CMP            compare  ;
    // ========================================================================
  public:
    // ========================================================================
    /** constructor 
     *  @param fun1 the first functor 
     *  @param fun2 the second functor 
     *  @param cmp the comparison criteria
     */
    Compare ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
              const LoKi::Functor<TYPE,TYPE2>& fun2 ,              
              const compare&  cmp  = compare() )
      : m_two ( fun1 , fun2 ) 
      , m_cmp ( cmp ) 
    {}
    // ========================================================================
    /** constructor 
     *  @param fun1 the first functor 
     *  @param fun2 the second functor 
     *  @param cmp the comparison criteria
     */
    Compare ( const LoKi::Functor<TYPE,TYPE2>& fun1 , 
              const compare&  cmp  = compare() )
      : m_two ( fun1 , fun1 ) 
      , m_cmp ( cmp ) 
    {}
    /// copy constructor 
    Compare ( const Compare& right ) 
      : m_two ( right.m_two ) 
      , m_cmp ( right.m_cmp ) 
    {}
    /// destructor 
    virtual ~Compare() {}
    /// the only one essential method 
    bool operator() ( argument a1 , argument a2 ) const
    { return m_cmp ( m_two.fun1 ( a1 ) , m_two.fun2 ( a2 ) ) ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get the first functor 
    const function& func1 () const { return m_two.func1 () ; }
    /// get the second functor 
    const function& func2 () const { return m_two.func2 () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// no default constructor 
    Compare(){}                                       // no default constructor 
    // ========================================================================
  private:
    // ========================================================================
    LoKi::TwoFunctors<TYPE,TYPE2> m_two ;
    compare   m_cmp  ;
    // ========================================================================
  };
  // ==========================================================================
  template <class TYPE,class TYPE2,bool> struct Cmp ;
  // ==========================================================================
  template <class TYPE,class TYPE2>
  struct Cmp<TYPE,TYPE2,true>
  {
    typedef Compare<TYPE,std::less<TYPE2>,TYPE2>     Type ;
  } ;
  // ==========================================================================
  template <class TYPE,class TYPE2>
  struct Cmp<TYPE,TYPE2,false>
  {
    typedef Compare<TYPE,std::greater<TYPE2>,TYPE2>  Type ;
  } ;
  // ==========================================================================
  /** @class Identity
   *  the simple trivial functor 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE,class TYPE2=TYPE>
  class Identity : public LoKi::Functor<TYPE,TYPE2>
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,TYPE2>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ; 
  public :
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~Identity(){}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Identity* clone () const { return new Identity(*this) ; }
    /// MANDATORY": the only one essential method 
    virtual  result_type operator () ( argument_a_unless_void ) const { return a_unless_void ; }    
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class PrintOut
   *  the simple functor, which "converts" the objects to strings 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-04-07
   */
  template <class TYPE>
  class PrintOut : public LoKi::Functor<TYPE,std::string>
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename LoKi::Functor<TYPE,std::string>::argument argument  ; 
    /// result type 
    typedef typename LoKi::Functor<TYPE,std::string>::result_type result_type ; 
  public:
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~PrintOut(){}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  PrintOut* clone () const { return new PrintOut ( *this ) ; }
    /// MANDATORY": the only one essential method 
    virtual result_type operator () ( argument_a_unless_void ) const 
    { return Gaudi::Utils::toString ( a_unless_void  ) ; }    
    // ========================================================================
  };
#endif
  // ==========================================================================
  /** @class InRange 
   *  Helper predicate to represent that the result of functor 
   *  is within the certain "range" 
   *  @author Vanya BELYAEV Ivan/Belyaev@nikhef.nl
   *  @date 2009-11-21
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<>
  class InRange<void>: public LoKi::Functor<void,bool>
#else
  template<class TYPE> 
  class InRange: public LoKi::Functor<TYPE,bool> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument              argument ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type        result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the functor and edges 
     *  @param fun the functor 
     *  @param low the low edge  
     *  @param high the high edge 
     */
    InRange
    ( const double                      low  , 
      const LoKi::Functor<TYPE,double>& fun  , 
      const double                      high )
      : LoKi::AuxFunBase ( std::tie ( low , fun , high ) ) 
      , LoKi::Functor<TYPE,bool> () 
      , m_low  ( low  ) 
      , m_fun  ( fun  ) 
      , m_high ( high ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~InRange() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  InRange* clone() const { return new InRange ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument_a_unless_void ) const 
    {
      const double r = m_fun.fun ( a_unless_void ) ;
      return m_low <= r && r <= m_high ;
    }
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    {
      return s << " in_range(" << m_low 
               << ","          << m_fun 
               << ","          << m_high << ") " ;
      
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    InRange() ;                           // the default contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the low edge 
    double                                m_low  ;        //       the low edge 
    /// the functor itself 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun  ;        // the functor itself
    /// the high edge 
    double                                m_high ;        //       the low edge 
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class InRange2 
   *  Helper predicate to represent that the result of functor 
   *  is within the certain "range" 
   *  @author Vanya BELYAEV Ivan/Belyaev@nikhef.nl
   *  @date 2009-11-21
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<>
  class InRange2<void>: public LoKi::Functor<void,bool>
#else
  template<class TYPE> 
  class InRange2: public LoKi::Functor<TYPE,bool> 
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument              argument ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type        result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the functor and edges 
     *  @param low the low edge  
     *  @param fun the functor 
     *  @param high the high edge 
     */
    InRange2
    ( const LoKi::Functor<TYPE,double>& low  , 
      const LoKi::Functor<TYPE,double>& fun  , 
      const LoKi::Functor<TYPE,double>& high ) 
      : LoKi::AuxFunBase ( std::tie ( low , fun , high ) ) 
      , LoKi::Functor<TYPE,bool> () 
      , m_low  ( low  ) 
      , m_fun  ( fun  ) 
      , m_high ( high ) 
    {}
    /** constructor from the functor and edges 
     *  @param low the low edge  
     *  @param fun the functor 
     *  @param high the high edge 
     */
    InRange2
    ( const double                      low  , 
      const LoKi::Functor<TYPE,double>& fun  , 
      const LoKi::Functor<TYPE,double>& high ) 
      : LoKi::AuxFunBase ( std::tie ( low , fun , high ) )  
      , LoKi::Functor<TYPE,bool> () 
      , m_low  ( LoKi::Constant<TYPE,double> ( low ) ) 
      , m_fun  ( fun  ) 
      , m_high ( high ) 
    {}
    /** constructor from the functor and edges 
     *  @param low the low edge  
     *  @param fun the functor 
     *  @param high the high edge 
     */
    InRange2
    ( const LoKi::Functor<TYPE,double>& low  , 
      const LoKi::Functor<TYPE,double>& fun  , 
      const double                      high ) 
      : LoKi::AuxFunBase ( std::tie ( low , fun , high ) ) 
      , LoKi::Functor<TYPE,bool> () 
      , m_low  ( low  ) 
      , m_fun  ( fun  ) 
      , m_high ( LoKi::Constant<TYPE,double> ( high ) ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~InRange2() {}
    /// MANDATORY: clone method ("virtual constructor")
    virtual  InRange2* clone() const { return new InRange2 ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument_a_unless_void ) const 
    {
      const double low  = m_low .fun ( a_unless_void ) ;
      const double res  = m_fun .fun ( a_unless_void ) ;
      return  
        low <= res              ? 
        res <= m_high.fun ( a_unless_void ) : false ;
    }
    /// OPTIONAL: the nice printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    {
      return s << " in_range(" << m_low 
               << ","          << m_fun 
               << ","          << m_high << ") " ;
      
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the default contructor is disabled 
    InRange2() ;                          // the default contructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the low edge 
    LoKi::FunctorFromFunctor<TYPE,double> m_low  ;        //       the low edge 
    /// the functor itself 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun  ;        // the functor itself
    /// the high edge 
    LoKi::FunctorFromFunctor<TYPE,double> m_high ;        //      the high edge 
    // ========================================================================
  } ;
  // ==========================================================================
  /** @class EqualToList
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-12-06
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class EqualToList<void> : public LoKi::Functor<void,bool>
#else
  template <class TYPE>
  class EqualToList       : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    EqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<double>&         vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_vct ( vct ) 
    { 
      adjust () ; 
    }
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    EqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<int>&            vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_vct ( vct.begin() , vct.end() ) 
    { 
      adjust () ; 
    }
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    EqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<unsigned int>&   vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_vct ( vct.begin() , vct.end() ) 
    { 
      adjust () ;
    }
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    template <class ITERATOR>
    EqualToList
    ( const LoKi::Functor<TYPE,double>&  fun    , 
      ITERATOR                           first  , 
      ITERATOR                           last   ) 
      : LoKi::Functor<TYPE,bool>() 
      , m_fun ( fun ) 
      , m_vct ( first , last ) 
    { 
      adjust () ;
    }
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~EqualToList(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  EqualToList* clone() const { return new EqualToList(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return equal_to ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this->func() << "==" 
               << Gaudi::Utils::toString ( m_vct ) << ") " ; }
    // ========================================================================
  public:
    // ========================================================================
    inline result_type equal_to ( argument_a_unless_void ) const
    {
      if ( m_vct.empty() ) { return  false ; }
      //
      const double r = m_fun.fun ( a_unless_void ) ;
      //
      LHCb::Math::Equal_To<double> cmp ;
      for  ( std::vector<double>::const_iterator item = m_vct.begin() ;
             m_vct.end() != item ; ++item ) 
      { if ( cmp ( *item , r ) ) { return  true ; } } // RETURN 
      //
      return false ;
    }    
    // ========================================================================
    inline result_type not_equal_to ( argument_a_unless_void ) const
    { return !this->equal_to ( a_unless_void ) ; }
    // ========================================================================
  public:
    // ========================================================================
    const LoKi::Functor<TYPE,double>& func () const { return m_fun.func() ; }
    /// get the vector 
    const std::vector<double>&        vect () const { return m_vct ; }
    // ========================================================================
  private:
    // ========================================================================
    size_t  adjust () 
    {
      std::sort ( m_vct.begin() , m_vct.end() ) ;
      std::vector<double>::iterator ilast = 
        std::unique ( m_vct.begin() , m_vct.end() ) ;
      if ( m_vct.end() != ilast ) { m_vct.erase  ( ilast , m_vct.end() ) ; }
      return m_vct.size() ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    EqualToList ();
    // ========================================================================
  private:
    // ========================================================================
    /// the functor 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun ;                // the functor 
    /// the list 
    std::vector<double>  m_vct ;                                 //    the list 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class NotEqualToList
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-12-06
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class NotEqualToList<void> : public LoKi::EqualToList<void>
#else
  template <class TYPE>
  class NotEqualToList       : public LoKi::EqualToList<TYPE>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    NotEqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<double>&         vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::EqualToList<TYPE>( fun , vct ) 
    {}
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    NotEqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<int>&            vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::EqualToList<TYPE>( fun , vct ) 
    {}
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    NotEqualToList
    ( const LoKi::Functor<TYPE,double>&  fun , 
      const std::vector<unsigned int>&   vct ) 
      : LoKi::AuxFunBase ( std::tie ( fun , vct ) ) 
      , LoKi::EqualToList<TYPE>( fun , vct ) 
    {}
    /** constructor from the function and the value 
     *  @param fun the function
     *  @param vct the vector of values 
     */
    template <class ITERATOR>
    NotEqualToList
    ( const LoKi::Functor<TYPE,double>&  fun    , 
      ITERATOR                           first  , 
      ITERATOR                           last   ) 
      : LoKi::EqualToList<TYPE>( fun , first , last  ) 
    {}
    // ========================================================================
    /// MANDATORY: virtual destructor 
    virtual ~NotEqualToList(){} ;
    // ========================================================================
    /// MANDATORY: clone method ("virtual construcor")
    virtual  NotEqualToList* clone() const 
    { return new NotEqualToList(*this); }
    /// MANDATORY: the only one essential method :
    virtual  result_type operator() ( argument_a_unless_void ) const
    { return this -> not_equal_to ( a_unless_void ) ; }
    /// OPTIONAL: the specific printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " (" << this->func() << "!=" 
               << Gaudi::Utils::toString ( this->vect() ) << ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// The default constructor is disabled 
    NotEqualToList ();
    // ========================================================================
  };  
  // ==========================================================================
  /** @class XScaler
   *  Simple scaler for predicates
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-12-06
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class XScaler<void> : public LoKi::Functor<void,bool>
#else
  template <class TYPE>
  class XScaler : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /** constructor from predicate and scale
     *  @param cut the predicate 
     *  @param scale the scaler 
     */
    XScaler 
    ( const LoKi::Functor<TYPE,bool>& cut   , 
      const LoKi::Functor<void,bool>& scale ) 
      : LoKi::AuxFunBase ( std::tie ( cut , scale ) )  
      , LoKi::Functor<TYPE,bool>()
      , m_cut    ( cut   ) 
      , m_scaler ( scale ) 
    {}
    /// MANDATORY: virtual destructor 
    virtual ~XScaler() {}
    /// MANDATORY: clone method ("virtual constructor") 
    virtual  XScaler* clone() const { return new XScaler ( *this ) ; }
    /// MANDATORY: the only one essential method 
    virtual result_type operator() ( argument_a_unless_void ) const 
    {
      return m_cut.fun ( a_unless_void ) && m_scaler.fun( /* void */ ) ;
    }
    /// OPTIONAL: nice printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return s << " scale(" << m_cut << "," << m_scaler << ") " ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    XScaler() ;                          // the default constructor is disabled    
    // ========================================================================
  private:
    // ========================================================================
    /// the predicate 
    LoKi::FunctorFromFunctor<TYPE,bool>  m_cut    ;            // the predicate 
    /// the scaler 
    LoKi::FunctorFromFunctor<void,bool>  m_scaler ;            // the scaler 
    // ========================================================================
  };
  // ==========================================================================
  /** @class Modulo
   *
   *  The helper function to implement "modulo"-operation 
   *  of the function
   *
   *  http://en.wikipedia.org/wiki/Modulo_operation
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2011-03-30
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template<>
  class Modulo<void> : public LoKi::Functor<void,double>
#else
  template<class TYPE> 
  class Modulo : public LoKi::Functor<TYPE,double>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,double>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,double>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor  
    Modulo ( const LoKi::Functor<TYPE,double>& divident  , 
             const unsigned int                divisor   )
      : LoKi::AuxFunBase ( std::tie ( divident , divisor ) ) 
      , LoKi::Functor<TYPE,double>() 
      , m_divident ( divident  ) 
      , m_divisor  ( divisor   ) 
    {} 
    /// virtual destructor 
    virtual ~Modulo () {}
    /// clone method (mandatory)
    virtual  Modulo* clone() const { return new Modulo ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { return LHCb::Math::round ( m_divident.fun ( a_unless_void ) ) % m_divisor ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " ("  << m_divident << " % "  << m_divisor << ") "; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Modulo () ;                          // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the divident 
    LoKi::FunctorFromFunctor<TYPE,double> m_divident ; // the divident 
    /// the divisor 
    const unsigned int                    m_divisor  ; // the divisor 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class Round 
   *  get the proper rounding for the floating value 
   *  @see LHCb::Math::round 
   *  The actual rounding policy is defined by function LHCb::Math::round
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-04-02
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class Round<void> : public LoKi::Functor<void,double>
#else
  template <class TYPE>
  class Round : public LoKi::Functor<TYPE,double>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,double>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,double>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor and the fake argument  
    Round ( const LoKi::Functor<TYPE,double>&    fun      , 
            const unsigned int                   fake     )
      : LoKi::AuxFunBase ( std::tie ( fun , fake ) )  
      , LoKi::Functor<TYPE,double>() 
      , m_fun  ( fun ) 
    {} 
    /// virtual destructor 
    virtual ~Round () {}
    /// clone method (mandatory)
    virtual  Round* clone() const { return new Round ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    { return LHCb::Math::round ( this->m_fun.fun ( a_unless_void ) ) ; }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " round("  << this->m_fun<< ") "; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    Round () ;                           // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun ;                // the functor
    // ========================================================================
  };  
  // ==========================================================================
  /** @class JBit
   *  get the jth bit of value.
   *  The action :  
   *   - 1. f -> round ( f ) 
   *   - 2. f -> abs   ( f ) 
   *   - 3. bit ( f , j )  
   *  @see Gaudi::Math::bit 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-04-02
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class JBit<void> : public LoKi::Functor<void,bool>
#else
  template <class TYPE>
  class JBit : public LoKi::Functor<TYPE,bool>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,bool>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,bool>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor 
    JBit ( const LoKi::Functor<TYPE,double>&    fun ,  
           const unsigned int                   j   )
      : LoKi::AuxFunBase ( std::tie ( fun , j ) )       
      , LoKi::Functor<TYPE,bool>() 
      , m_fun  ( fun ) 
      , m_j    ( j   ) 
    {
      // 
      BOOST_STATIC_ASSERT( boost::integer_traits<unsigned long>::is_specialized
                           && boost::integer_traits<unsigned long>::is_integral 
                           &&!boost::integer_traits<unsigned long>::is_signed ) ;
      //
      this -> Assert ( j < (unsigned long) boost::integer_traits<unsigned long>::digits , 
                       "Invalid bit index" ) ;
      //
    }
    /// virtual destructor 
    virtual ~JBit () {}
    /// clone method (mandatory)
    virtual  JBit* clone() const { return new JBit ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    {
      const unsigned long _ulv = 
        ::labs ( LHCb::Math::round ( this->m_fun.fun ( a_unless_void ) ) ) ;
      //
      return Gaudi::Math::bit ( _ulv , this->m_j ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " jbit("  << this->m_fun << "," << this->m_j << ") "; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    JBit () ;                           // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun ;                // the functor
    /// the index 
    unsigned int                          m_j   ;                  // the index 
    // ========================================================================
  };  
  // ==========================================================================
  /** @class JBits
   *  get the content between j1 and j2  bit of value.
   *  The action :  
   *   - 1. f -> round ( f ) 
   *   - 2. f -> abs   ( f ) 
   *   - 3. Gaudi::Math::bits ( f , j1, j2  )  
   *  @see Gaudi::Math::bits  
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-04-02
   */
#ifdef _GEN_LOKI_VOIDPRIMITIVES
  template <>
  class JBits<void> : public LoKi::Functor<void,double>
#else
  template <class TYPE>
  class JBits : public LoKi::Functor<TYPE,double>
#endif
  {
  private:
    // ========================================================================
    typedef_void_TYPE
    /// argument type
    typedef typename_v LoKi::Functor<TYPE,double>::argument argument  ; 
    /// result type 
    typedef typename_v LoKi::Functor<TYPE,double>::result_type result_type ; 
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the functor 
    JBits ( const LoKi::Functor<TYPE,double>&    fun ,  
            const unsigned int                   j1  ,
            const unsigned int                   j2  )
      : LoKi::AuxFunBase ( std::tie ( fun , j1 , j2 ) )       
      , LoKi::Functor<TYPE,double>() 
      , m_fun  ( fun )
      , m_j1  ( j1  ) 
      , m_j2  ( j2  ) 
    {
      // 
      BOOST_STATIC_ASSERT( boost::integer_traits<unsigned long>::is_specialized
                           && boost::integer_traits<unsigned long>::is_integral 
                           &&!boost::integer_traits<unsigned long>::is_signed ) ;
      //
      this -> Assert ( j1 <  (unsigned long) boost::integer_traits<unsigned long>::digits , 
                       "Invalid bit index-1" ) ;
      this -> Assert ( j2 <= (unsigned long) boost::integer_traits<unsigned long>::digits , 
                       "Invalid bit index-2" ) ;
      this -> Assert ( j1 < j2 , "Invalid bit indices" ) ;
      //
    }
    /// virtual destructor 
    virtual ~JBits () {}
    /// clone method (mandatory)
    virtual  JBits* clone() const { return new JBits ( *this ) ; }
    /// the only one essential method ("function")      
    virtual  result_type operator() ( argument_a_unless_void ) const 
    {
      const unsigned long _ulv = 
        ::labs ( LHCb::Math::round ( this->m_fun.fun ( a_unless_void ) ) ) ;
      //
      return Gaudi::Math::bits ( _ulv , this->m_j1 , this -> m_j2 ) ; 
    }
    /// the basic printout method 
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << " jbits("  << this->m_fun 
               << "," << this->m_j1
               << "," << this->m_j2
               << ") "               ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    JBits () ;                           // the default constrictor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    LoKi::FunctorFromFunctor<TYPE,double> m_fun ;                // the functor
    /// the index1
    unsigned int                          m_j1  ;                  // the index 
    /// the index2
    unsigned int                          m_j2  ;                  // the index 
    // ========================================================================
  };  
#ifndef _GEN_LOKI_VOIDPRIMITIVES
  // ==========================================================================
  // OPTIONAL: the nice printout 
  // ==========================================================================
  template <class TYPE, class TYPE2>
  std::ostream& Identity<TYPE,TYPE2>::fillStream ( std::ostream& s ) const 
  { return s << "I" ; }
#endif
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                           specializations for void-arguments
// ============================================================================
//#include "LoKi/VoidPrimitives.h"
#ifndef _GEN_LOKI_VOIDPRIMITIVES
#define _GEN_LOKI_VOIDPRIMITIVES 1
#undef typedef_void_TYPE
#undef argument_a_unless_void
#undef typename_v
#undef a_unless_void
#undef class_TYPE_unless_void
#define typedef_void_TYPE typedef void TYPE;
#define argument_a_unless_void
#define typename_v
#define a_unless_void
#define class_TYPE_unless_void
#include "LoKi/Primitives.h"
#undef _GEN_LOKI_VOIDPRIMITIVES
#endif
// ============================================================================
// The END 
// ============================================================================
#undef _GEN_LOKI_PRIMITIVES
#endif // _GEN_LOKI_PRIMITIVES
// ============================================================================
