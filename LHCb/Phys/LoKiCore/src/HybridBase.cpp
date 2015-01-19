// $Id$
// ============================================================================
// Include files
// ============================================================================
// Python
// ============================================================================
#include "Python.h"
// ============================================================================
// STD & STL
// ============================================================================
#include <sstream>
#include <fstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/System.h"
// ============================================================================
// local
// ============================================================================
#include "LoKi/HybridBase.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/CacheFactory.h"
// ============================================================================
// Boots
// ============================================================================
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::HybridTool
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
 *  @date 2004-06-29
 */
// ============================================================================
namespace
{
  // ==========================================================================
  /// replace all new lines with "newline+comments"
  std::string addComment ( std::string str )
  {
    static const std::string comm = "\n##! " ;
    std::string::size_type pos = str.find ( '\n' ) ;
    while ( std::string::npos != pos )
    {
      str.replace ( pos , 1 , comm );
      pos = str.find ( '\n' , pos + 2 ) ;
    }
    return str ;
  }
  // ==========================================================================
  /// prepare the actual code: trim and remove the paired quotes
  std::string trimCode ( std::string code )
  {
    // trim leading and trailing blanks:
    boost::trim ( code ) ;
    if ( code.empty() )                { return code ; }
    // check for paired quotes:
    std::string::const_iterator ifront = code.begin () + 1 ;
    std::string::const_iterator iback  = code.end   () - 1 ;
    //
    if ( *ifront != *iback ) { return code ; }
    //
    if ( '\'' != *ifront && '"' != *ifront ) { return code ; }
    //
    return trimCode ( std::string ( ifront , iback ) ) ;
  }
  // ==========================================================================
}
// ============================================================================
// Standard constructor
// ============================================================================
LoKi::Hybrid::Base::Base
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool ( type , name , parent )
    //
  , m_pyInit     ( false )
  , m_showCode   ( false )
  , m_makeCpp    ( false )
  , m_cppname    ()
  , m_cpplines   ( { "#include \"GaudiKernel/Kernel.h\"" , 
                     "#include \"GaudiKernel/System.h\"" ,
                     "#include \"LoKi/Functors.h\""      , 
                     "#include \"LoKi/CacheFactory.h\""  , } )
  , m_allfuncs   () 
{
  declareProperty ( "ShowCode" , 
                    m_showCode , 
                    "Flag to display the prepared python code") ;
  //
  // use Environment variable 
  //
  m_makeCpp = "UNKNOWN" != System::getEnv  ( "LOKI_GENERATE_CPPCODE" ) ;
  //
  declareProperty ( "MakeCpp"  , 
                    m_makeCpp  , 
                    "Generate C++ code for created functors ") ;
  //
  // make reasonable default name
  //
  m_cppname = this->name() ;
  if ( 0 == m_cppname.find ( "ToolSvc." ) ) { m_cppname.erase ( 0 , 8 ) ; }
  while ( std::string::npos != m_cppname.find ( '.' ) ) 
  { m_cppname.replace ( m_cppname.find ( '.' ) , 1 , "_" ) ; }
  m_cppname.insert ( 0 , "FUNCTORS_" ) ;
  m_cppname.append (     ".cpp"      ) ;
  //
  declareProperty ( "CppFileName", 
                    m_cppname    , 
                    "File name for C++ code for created functors ") ;
  //
  declareProperty ( "CppLines" , 
                    m_cpplines , 
                    "C++ (header) lines to be included") ;
}
// ============================================================================
// Destructor (virtual and protected)
// ============================================================================
LoKi::Hybrid::Base::~Base() {}
// ============================================================================
// initialization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::Base::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // force the loading/initialization of  LoKi Service
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  // Initialize python if it not yet done
  if ( !Py_IsInitialized() )
  {
    info() << "Initialization of Python is triggered" << endmsg ;
    Py_Initialize () ;
    m_pyInit = true ;
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalization of the tool
// ============================================================================
StatusCode LoKi::Hybrid::Base::finalize  ()
{
  // finalize python (if the owner)
  if ( Py_IsInitialized() && m_pyInit )
  {
    info() << "Finalization   of Python is triggered" << endmsg ;
    std::string line ;
    line += "                                                           # " + name() + "\n" ;
    line += "import atexit                                              # " + name() + "\n" ;
    line += "if atexit._exithandlers :                                  # " + name() + "\n" ;
    line += "\tprint 'ATTENTION: Clear non-empty atexit._exithandlers'  # " + name() + "\n" ;
    line += "\tprint 'len(_exithandlers)=',len(atexit._exithandlers)    # " + name() + "\n" ;
    line += "\tatexit._exithandlers=[]                                  # " + name() + "\n" ;
    line += "                                                           # " + name() + "\n" ;
    const int result =  PyRun_SimpleString ( line.c_str() ) ;
    if ( 0 != result )
    {
      Warning ( "Error code from PyRun_SimpleString" , 1000 + result ).ignore() ;
      warning() << "The problematic code is \n" << line << endmsg ;
      if ( PyErr_Occurred() ) { PyErr_Print() ; }
    }
    Py_Finalize () ;
  }
  //
  // Write C++ code 
  //
  if ( m_makeCpp ) { writeCpp () ; }
  //
  // finalize the base
  return GaudiTool::finalize() ;
}
// ============================================================================
/* execute the python code
 * @attention the method must be invoked only with appropriate scope!!!
 * @param code python code
 * @return status code
 */
// ============================================================================
namespace
{
  // ==========================================================================
  std::string toString ( PyObject* o )
  {
    if ( 0 == o ) { return "NULL"; }
    PyObject* str = PyObject_Str ( o ) ;
    std::string tmp ;
    if ( 0 != str ) { tmp = PyString_AS_STRING (str ) ; }
    Py_XDECREF ( str ) ;
    return tmp ;
  }
  // ==========================================================================
}
// ============================================================================
StatusCode LoKi::Hybrid::Base::executeCode ( const std::string& pycode ) const
{
  // Check the proper python environment:
  if ( !Py_IsInitialized() ) { return Error("Python is not initialized yet!") ; }

  // play with raw Python

  // local scope
  PyObject* locals  = PyEval_GetLocals  () ;
  if ( 0 == locals )
  {
    debug() << "PyEval_GetLocals:  'locals'  points to NULL" << endmsg ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
  }
  // global scope
  PyObject* globals = PyEval_GetGlobals () ;
  bool globnew = false ;
  if ( 0 == globals )
  {
    debug() << "PyEval_GetGlobals: 'globals' points to NULL" << endmsg ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
    globals  = PyDict_New() ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
    globnew  = true ;
    PyDict_SetItemString ( globals, "__builtins__" , PyEval_GetBuiltins() ) ;
    if ( PyErr_Occurred() ) { PyErr_Print() ; }
  }
  // ===========
  // ATTENTION!
  // ===========
  // execute Python 'code'
  PyObject* result = PyRun_String
    ( const_cast<char*> ( pycode.c_str() ) ,                // EXECUTE CODE
      Py_file_input  , globals  , locals ) ;

  bool ok = true ;
  if ( 0 == result )
  {
    ok = false ;

    PyObject* o1 = 0 ;
    PyObject* o2 = 0 ;
    PyObject* o3 = 0 ;
    PyErr_Fetch              ( &o1 , &o2 , &o3 ) ;
    PyErr_NormalizeException ( &o1 , &o2 , &o3 ) ;

    if ( 0 != o1 )
    { error () << " PyError Type      : " << toString ( o1 ) << endmsg ; }
    if ( 0 != o2 )
    { error () << " PyError Value     : " << toString ( o2 ) << endmsg ; }
    if ( 0 != o3 )
    { error () << " PyError Traceback : " << toString ( o3 ) << endmsg ; }

    PyObject* pyErr = o2 ;

    // SyntaxError
    if ( PyErr_GivenExceptionMatches ( o2 , PyExc_SyntaxError    ) ||
         PyErr_GivenExceptionMatches ( o2 , PyExc_TypeError      ) ||
         PyErr_GivenExceptionMatches ( o2 , PyExc_NameError      ) ||
         PyErr_GivenExceptionMatches ( o2 , PyExc_IndexError     ) ||
         PyErr_GivenExceptionMatches ( o2 , PyExc_ImportError    ) ||
         PyErr_GivenExceptionMatches ( o2 , PyExc_AttributeError )  )
    {
      //
      PyObject* filename = PyObject_GetAttrString  ( pyErr , "filename"            ) ;
      PyObject* lineno   = PyObject_GetAttrString  ( pyErr , "lineno"              ) ;
      PyObject* offset   = PyObject_GetAttrString  ( pyErr , "offset"              ) ;
      PyObject* text     = PyObject_GetAttrString  ( pyErr , "text"                ) ;
      PyObject* prntfal  = PyObject_GetAttrString  ( pyErr , "print_file_and_line" ) ;
      PyObject* msg      = PyObject_GetAttrString  ( pyErr , "msg"                 ) ;
      PyObject* message  = PyObject_GetAttrString  ( pyErr , "message"             ) ;
      //
      long        _offset = -1 ;
      long        _lineno = -1 ;
      std::string _text        ;
      std::string _msg         ;
      std::string _message     ;

      if ( 0 != filename )
      { info () << "Filename: " << toString ( filename ) << endmsg ; }
      if ( 0 != lineno   )
      {
        info () << "Lineno  : " << toString ( lineno   ) << endmsg ;
        if ( PyInt_Check ( lineno ) ) { _lineno = PyInt_AsLong ( lineno ) ;  }
      }
      if ( 0 != offset   )
      {
        info () << "offset  : " << toString ( offset   ) << endmsg ;
        if ( PyInt_Check ( offset ) ) { _offset = PyInt_AsLong ( offset ) ;  }
      }
      if ( 0 != text     )
      {
        _text = toString ( text ) ;
        info () << "text    : " << _text << endmsg ;
      }
      if ( 0 != msg      )
      {
        _msg  = toString ( msg ) ;
        info () << "msg     : " << _msg  << endmsg ;
      }
      if ( 0 != message  )
      {
        _message = toString ( message ) ;
        info () << "message : " << _message << endmsg ;
      }
      if ( 0 != prntfal  )
      {
        info () << "prntfal : " << toString( prntfal )  << endmsg ;
      }

      {
        MsgStream& stream = error ()  ;
        stream
          << " Python error in Code\n" ;
        if ( 0 != pyErr  )
        { stream << "Python error  : " << toString ( pyErr ) << '\n' ; }
        if ( !_msg.empty()     )
        { stream << "PyErr msg     : " << _msg               << '\n' ; }
        if ( !_message.empty() )
        { stream << "PyErr message : " << _message           << '\n' ; }
        if ( 0 <= _lineno   ) { stream << " Line #" << _lineno << '\n'; }
        if ( !_text.empty() ) { stream << "       " << _text          ; }
        if ( 0 < _offset )
        { stream  << "      " << std::string(_offset,' ') << '^'      ; }
        stream<< endmsg ;
      }

      Py_XDECREF ( message  ) ;
      Py_XDECREF ( msg      ) ;
      Py_XDECREF ( prntfal  ) ;
      Py_XDECREF ( text     ) ;
      Py_XDECREF ( offset   ) ;
      Py_XDECREF ( lineno   ) ;
      Py_XDECREF ( filename ) ;

      // restore for printout
      PyErr_Restore ( o1 , o2 , o3 ) ;
    }
    else
    {
      // restore for printout
      PyErr_Restore ( o1 , o2 , o3 ) ;
    }
    //
    error ()  << "Native Python printout to stderr"        << endmsg ;
    PyErr_Print   () ;
    error ()  << "End of native Python printout to stderr" << endmsg ;
  }

  if ( 0 != globals && globnew ) { Py_XDECREF( globals ) ; }

  if ( 0 != result             ) { Py_XDECREF ( result )      ; }
  else if ( PyErr_Occurred()   ) { PyErr_Print() ; ok = false ; }
  else                           {                 ok = false ; }

  if ( !ok )
  {
    err () << " Error has occured in Python: the problematic code is : "
           << endmsg
           << pycode
           << endmsg ;
    return Error( " Error has occured in Python " ) ;
  }

  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  prepare the actual python code from the functor:
 *  @param modules list of modules
 *  @param actor the actual actor
 *  @param code the code for the functor
 *  @param lines more python lines to be used
 *  @return the valid python code
 */
// ============================================================================
std::string LoKi::Hybrid::Base::makeCode
( const LoKi::Hybrid::Base::Strings& modules ,
  const std::string&                 actor   ,
  const std::string&                 code    ,
  const LoKi::Hybrid::Base::Strings& lines   ,
  const std::string&                 context ) const
{
  //
  std::string _code = code ;
  boost::algorithm::replace_all ( _code , "\n"  , " " ) ;
  boost::algorithm::replace_all ( _code , "\\n" , " " ) ;
  boost::algorithm::trim        ( _code ) ;
  //
  // trim and remove the paired quotes:
  _code = trimCode ( _code ) ;
  //
  std::ostringstream stream ;
  // start the code:
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# python pseudomodule, generated for the tool '"
         << name() << "'" << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# Arguments: "  << std::endl ;
  stream << "# \tcode    = " << Gaudi::Utils::toString ( _code   )  << std::endl ;
  stream << "# \tactor   = " << Gaudi::Utils::toString ( actor   )  << std::endl ;
  stream << "# \tmodules = " << Gaudi::Utils::toString ( modules )  << std::endl ;
  stream << "# \tlines   = " << Gaudi::Utils::toString ( lines   )  << std::endl ;
  stream << "# \tcontext = " << Gaudi::Utils::toString ( addComment ( "\n" + context ) )  << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  // define imported modules:
  stream << "##        MODULES :" << std::endl ;
  for ( Strings::const_iterator imodule = modules.begin() ;
        modules.end() != imodule ; ++imodule )
  {
    stream << "from " << (*imodule) << " import *" << std::endl ;
  }
  stream << "## End of MODULES  " << std::endl ;
  stream << "## The ACTOR :"              << std::endl ;
  stream << "_actor=" << actor << std::endl ;
  // put additional lines:
  stream << "##        LINES :" << std::endl ;
  for ( Strings::const_iterator iline = lines.begin() ;
        lines.end() != iline ; ++iline )
  { stream << (*iline) << std::endl ; }
  stream << "## end of LINES  " << std::endl ;
  // put the context
  stream << "##        CONTEXT :" << std::endl ;
  if ( !context.empty() ) { stream << context << std::endl ; }
  stream << "## End of CONTEXT  " << std::endl ;
  stream << "##        CODE :"    << std::endl ;
  stream << "_code="  << _code    << std::endl ;
  stream << "## End of CODE :"    << std::endl ;
  stream << "sc=_actor.process('" << name() << "',_code)" << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  stream << "# The END "                << std::endl ;
  stream << "# " << std::string(78,'=') << std::endl ;
  //
  std::string result = stream.str() ;
  if ( msgLevel ( MSG::DEBUG ) || showCode() )
  {
    debug() << "Generated Python code:" << std::endl
            << result
            << endmsg ;
  }
  ///
  return result ;
}
// ============================================================================
// write C++ code 
// ============================================================================
void LoKi::Hybrid::Base::writeCpp () const 
{
  std::ofstream file ( m_cppname ) ;
  
  //
  // 1) write header line
  //
  for ( std::vector<std::string>::const_iterator il = m_cpplines.begin() ;
        m_cpplines.end() != il ; ++il ) 
  { file << (*il) << std::endl ; }
  //
  // 2 ) loop over basic types 
  //
  for ( ALLFUNCS::const_iterator ia = m_allfuncs.begin() ;
        m_allfuncs.end() != ia ; ++ia ) 
  {
    //
    file << "//"          << std::endl 
         << "// TYPE is " << ia->first 
         << "//"          << std::endl ;
    //
    const std::string& cpptype = ia->first  ;
    const FUNCTIONS&   funcs   = ia->second ;
    for ( FUNCTIONS::const_iterator ic = funcs.begin() ; funcs.end() != ic ; ++ic ) 
    {
      //
      const std::string& cppcode = ic->second.first  ;
      const std::string& pytype  = ic->second.second ;
      const std::string& pycode  = ic->first         ;
      //
      // write actual C++ code 
      LoKi::Cache::makeCode ( file       , 
                              cpptype    ,
                              cppcode    ,
                              pycode     , 
                              pytype     ) ;
      file << std::endl ;
    }
  }
}
// ============================================================================



// ============================================================================
// The END
// ============================================================================
