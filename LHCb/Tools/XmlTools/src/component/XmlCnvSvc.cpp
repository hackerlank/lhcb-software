// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Tools/XmlTools/src/component/XmlCnvSvc.cpp,v 1.1.1.1 2003-04-23 13:38:46 sponce Exp $

// Include Files
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"

#include "XmlCnvSvc.h"
#include "XmlParserSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static SvcFactory<XmlCnvSvc> xmlcnvsvc_factory;
const ISvcFactory& XmlCnvSvcFactory = xmlcnvsvc_factory;


// -----------------------------------------------------------------------
// Standard Constructor
// -----------------------------------------------------------------------
XmlCnvSvc::XmlCnvSvc (const std::string& name, ISvcLocator* svc) :
  ConversionSvc(name, svc, XML_StorageType) {
  
  // gets the AllowGenericConversion property value
  declareProperty ("AllowGenericConversion", m_genericConversion = false);

  // gets the DtdLocation property value
  declareProperty ("DtdLocation", m_dtdLocation = "");

  // Whether to check parameters for units or not
  declareProperty ("CheckUnits", m_checkUnits = true);
}


// -----------------------------------------------------------------------
// Standard Destructor
// -----------------------------------------------------------------------
XmlCnvSvc::~XmlCnvSvc() {
}


// -----------------------------------------------------------------------
// Initialize the service.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::initialize() {
  // Before anything we have to initialize grand mother
  StatusCode status = ConversionSvc::initialize();

  // Service MUST be initialized BEFORE!
  MsgStream log (msgSvc(), "XmlCnvSvc");

  if (!status.isSuccess()) {
    return status;  
  }

  // Initialize the xerces system
  try  {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    char *message = XMLString::transcode (toCatch.getMessage());
    log << MSG::FATAL << "Error during initialization! :\n"
        << message << endreq;
    status = StatusCode::FAILURE;
    delete [] message;
  } 
  
  // creation of a parser service
  status = serviceLocator()->service("XmlParserSvc", m_parserSvc, true);
  if (status.isFailure()) {
    return status;
  }
  
  //setProperties();

  // Initialize numerical expressions parser with the standard math functions
  // and the system of units used by Gaudi (Geant4)
  m_xp.setStdMath();
  m_xp.setSystemOfUnits (1.e+3,
                         1./1.60217733e-25,
                         1.e+9,
                         1./1.60217733e-10,
                         1.0,
                         1.0,
                         1.0);
  return status;
}


// -----------------------------------------------------------------------
// Reinitialize the service.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::reinitialize() {
  clearCache();
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Stop the service.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::finalize() {
  m_parserSvc->release();
  m_parserSvc = 0;
  return ConversionSvc::finalize();
}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::queryInterface(const IID& riid, void** ppvInterface) {
  if (IID_IXmlSvc.versionMatch(riid))  {
    *ppvInterface = (IXmlSvc*)this;
  } else {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an XML address.
// Converters interpret XML addresses according to the following rules:
// par[0] is an XML file name or an XML string;
// par[1] is the name of the requested element in the XML data;
// ipar[0] is 0 or 1 for files and strings respectively.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::createAddress(unsigned char svc_type,
                                    const CLID& clid,
                                    const std::string* par, 
                                    const unsigned long* ipar,
                                    IOpaqueAddress*& refpAddress) 
{
  // First check that requested address is of type XML_StorageType
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Create an XML address" << endreq;
  if( XML_StorageType != svc_type ) {
    log << MSG::ERROR 
	<< "Cannot create addresses of type " << (int)svc_type 
	<< " which is different from " << (int)XML_StorageType 
	<< endreq;
    return StatusCode::FAILURE;
  }

  // Par[0] is an XML file name or an XML string.
  std::string source = par[0];

  // Par[1] is the name of the requested XML element.
  std::string entryName = par[1];

  // Ipar[0] is 0 for XML files and 1 for XML strings.
  unsigned int isString;

  // To avoid the need of two separate address creators with two different
  // storage types for files and strings, this method also creates XML string 
  // addresses when par[0] begins by "<?xml": in this case ipar[0] is ignored.
  unsigned int pos = source.find_first_not_of(" ");
  if( 0 < pos && pos < source.length() ) source.erase( 0, pos );
  if( source.find("<?xml") == 0 ) {
    isString = 1;
    log << MSG::VERBOSE 
	<< "XML source beginning by \"<?xml\" is interpreted"
	<< " as an XML string" << endreq;
  } else {
    isString = ipar[0];
    if( isString == 0 ) {
      log << MSG::VERBOSE << "XML source is an XML file name" << endreq;
    } else if( isString == 1 ) { 
      log << MSG::VERBOSE << "XML source is an XML string" << endreq;
    } else {
      log << MSG::ERROR 
	  << "Cannot create address: invalid ipar[0] value = "
	  << ipar[0] << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  // Now create the address
  refpAddress = new GenericAddress( XML_StorageType,
				    clid,
				    source,
				    entryName,
				    isString );
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Parses an Xml file and provides the DOM tree representing it
// -----------------------------------------------------------------------
DOM_Document XmlCnvSvc::parse (const char* fileName) {
  if (0 != m_parserSvc) {
    return m_parserSvc->parse(fileName);
  }
  DOM_Document null_result;
  MsgStream log (msgSvc(), "XmlCnvSvc");
  log << MSG::DEBUG << "null result returned in parse" << endreq;
  return null_result;
}


// -----------------------------------------------------------------------
// Parses an Xml file and provides the DOM tree representing it
// -----------------------------------------------------------------------
DOM_Document XmlCnvSvc::parseString (std::string source) {
  MsgStream log (msgSvc(), "XmlCnvSvc");

  // First prepend the proper DTD path where appropriate
  // Only one "relpath/file.dtd" or 'relpath/file.dtd' is expected in string
  if( m_dtdLocation != "" ) {
    unsigned int dtdPos = source.find( ".dtd" );
    if( dtdPos < source.length() ) {
      log << MSG::VERBOSE 
          << "Set correct DTD location in the string to be parsed" << endreq;
      unsigned int quotePos;
      if( source[dtdPos+4] == '\'' ) {
        quotePos = source.substr(0,dtdPos).rfind("\'");
        source.insert( quotePos+1, m_dtdLocation+"/" );
        log << MSG::VERBOSE << "DTD literal is now: " 
            << source.substr(quotePos,dtdPos+6-quotePos+m_dtdLocation.length())
            << endreq;
      } else if ( source[dtdPos+4] == '\"' ) {
        quotePos = source.substr(0,dtdPos).rfind("\"");
        source.insert( quotePos+1, m_dtdLocation+"/" );
        log << MSG::VERBOSE << "DTD literal is now: " 
            << source.substr(quotePos,dtdPos+6-quotePos+m_dtdLocation.length())
            << endreq;
      } else {
        log << MSG::VERBOSE
            << "Bad DTD literal in the string to be parsed: do nothing" 
            << endreq;
      }
    }
  }

  // Then feed the string to the XML parser
  if (0 != m_parserSvc) {
    return m_parserSvc->parseString (source);
  }
  DOM_Document null_result;
  log << MSG::DEBUG << "null result returned in parseString" << endreq;
  return null_result;

}


// -----------------------------------------------------------------------
// clears the cache of previously parsed xml files
// -----------------------------------------------------------------------
void XmlCnvSvc::clearCache() {
  if (0 != m_parserSvc) {
    m_parserSvc->clearCache();
    return;
  }
}


// -----------------------------------------------------------------------
// Evaluate a numerical expresion
// -----------------------------------------------------------------------
double XmlCnvSvc::eval (const std::string& expr, bool check) {
  return eval (expr.c_str(), check);
}


// -----------------------------------------------------------------------
// Evaluate a numerical expresion
// -----------------------------------------------------------------------
double XmlCnvSvc::eval (const char* expr, bool check) {
  MsgStream log (msgSvc(), name());

  // Check if it is needed to be a dimention number
  if (m_checkUnits) {
    if (check) {
      std::string e(expr);
      // remove leading blanks
      if (!sumHasUnit(e, e.find_first_not_of(' '), e.length())) {
        log << MSG::WARNING << "Expression requires correct units ["
            << e << "]" << endreq;
      }
    }
  }
  
  // Call the CLHEP Evaluator
  double value = m_xp.evaluate( expr );
  std::string errtxt;
  switch (m_xp.status()) {
  case HepTool::Evaluator::OK :
    return value;
  case HepTool::Evaluator::ERROR_NOT_A_NAME :
    errtxt = "NOT_A_NAME";
    break;
  case HepTool::Evaluator::ERROR_SYNTAX_ERROR :
    errtxt = "SYNTAX_ERROR";
    break;
  case HepTool::Evaluator::ERROR_UNPAIRED_PARENTHESIS :
    errtxt = "UNPAIRED_PARENTHESIS";
    break;
  case HepTool::Evaluator::ERROR_UNEXPECTED_SYMBOL :
    errtxt = "UNEXPECTED_SYMBOL";
    break;
  case HepTool::Evaluator::ERROR_UNKNOWN_VARIABLE :
    errtxt = "UNKNOWN_VARIABLE";
    break;
  case HepTool::Evaluator::ERROR_UNKNOWN_FUNCTION :
    errtxt = "UNKNOWN_FUNCTION";
    break;
  case HepTool::Evaluator::ERROR_EMPTY_PARAMETER :
    errtxt = "EMPTY_PARAMETER";
    break;
  case HepTool::Evaluator::ERROR_CALCULATION_ERROR :
    errtxt = "CALCULATION_ERROR";
    break;
  default :
    errtxt = "UNKNOWN_ERROR";
    break;
  }
  log << MSG::ERROR << "Expresion evaluation error: " << errtxt << endreq;
  log << MSG::ERROR << "[" << expr << "]" << endreq;
  log << MSG::ERROR << " ";
  for (int i = 0; i < m_xp.error_position(); i++) {
    log << " "; log << "^" << endreq;
  }
  return 0;
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const std::string& name,
                              const std::string& value) {
  return addParameter (name.c_str(), value.c_str());
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const char* name, const char* expr) {
  m_xp.setVariable (name, expr);
  if (m_xp.status() == HepTool::Evaluator::OK) return true;
  else return false;
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const char* name, double value) {
  m_xp.setVariable (name, value);
  if (m_xp.status() == HepTool::Evaluator::OK) {
    return true;
  } else {
    return false;
  }
}


// -----------------------------------------------------------------------
// removeParameter from expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::removeParameter (const std::string& name) {
  return removeParameter (name.c_str());
}


// -----------------------------------------------------------------------
// removeParameter from expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::removeParameter (const char* name) {
  m_xp.removeVariable (name);
  if (m_xp.status() == HepTool::Evaluator::OK) {
    return true;
  } else {
    return false;
  }
}


// -----------------------------------------------------------------------
// skipSum
// -----------------------------------------------------------------------
unsigned int XmlCnvSvc::skipSum (std::string s,
                                 unsigned int start,
                                 unsigned int end) {
  unsigned int result = start;
  while (result < end) {
    result = skipProduct (s, result, end);
    if (result == end) return end;
    result = s.find_first_not_of(' ', result);
    if (result == s.npos) return end;
    if (s[result] == ')') {
      return result;
    } else {
      // skip the + or - and loop
      result = s.find_first_not_of(' ', result + 1);
      if (result == s.npos) return end;
    }
  }
  return result;
}


// -----------------------------------------------------------------------
// skipProduct
// -----------------------------------------------------------------------
unsigned int XmlCnvSvc::skipProduct (std::string s,
                                     unsigned int start,
                                     unsigned int end) {
  unsigned int result = start;
  while (result < end) {
    result = skipExpr (s, result, end);
    if (result == end) return end;
    result = s.find_first_not_of(' ', result);
    if (result == s.npos) return end;
    if ((s[result] == '+') || (s[result] == '-') || (s[result] == ')')) {
      return result;
    } else {
      // skip the * or / and loop
      result = s.find_first_not_of(' ', result + 1);
      if (result == s.npos) return end;
    }
  }
  return result;
}

// -----------------------------------------------------------------------
// skipExpr
// -----------------------------------------------------------------------
unsigned int XmlCnvSvc::skipExpr (std::string s,
                                  unsigned int start,
                                  unsigned int end) {
  MsgStream log (msgSvc(), "DetDescUnit");
  // deal with the unary plus/minus
  unsigned int realStart = s.find_first_not_of(' ', start);
  if (realStart != s.npos) {
    if ((s[realStart] == '-') || (s[realStart] == '+')) {
      realStart = s.find_first_not_of(' ', realStart + 1);
    }
  }
  if (realStart == s.npos) {
    // the expression is empty !
    log << MSG::ERROR
        << "Invalid expression (It's empty !) : \""
        << s.substr (realStart, end - realStart) << "\""
        << endreq;
    return end;
  }
  unsigned int index = s.find_first_of("+-/*()", realStart);
  // if we the expression starts with an opening parenthesis or a
  // function call
  if ((index != s.npos) && (s[index] == '(')) {
    // skip the sum inside the parenthesis
    unsigned int endIndex = skipSum (s, index+1, end);
    if (endIndex != end) {
      endIndex = s.find_first_not_of(' ', endIndex);
      if (endIndex == s.npos) endIndex = end;
    }
    // if no end
    if (endIndex == end) {
      log << MSG::ERROR
          << "Invalid expression (no ')' at end of string) : \""
          << s.substr (realStart, end - realStart) << "\""
          << endreq;
      return end;
    }
    // else test the closing parenthesis
    if (s[endIndex] != ')') {
      log << MSG::ERROR 
          << "Invalid expression (missing ')' at column "
          << endIndex - realStart << ") : \""
          << s.substr (realStart, end - realStart) << "\""
          << endreq;
      return end;
    }
    // else everything is ok
    endIndex = s.find_first_not_of(' ', endIndex + 1);
    if (endIndex == s.npos) endIndex = end;
    if (endIndex < end) return endIndex; else return end;
  } else {
    // if the expression does not start with a parenthesis nor a
    // function call, it should be either a word or a number
    if (isalpha(s[realStart])) {
      if (index != s.npos) {
        return index;
      } else {
        return end;
      }
    } else if ((isdigit(s[realStart])) || (s[realStart] == '.')) {
      // We have a number, look whether we have an exponent
      // if yes, skip it
      if ((index != s.npos) && ((s[index] == '-') || (s[index] == '+'))) {
        if (index > realStart + 1) {
          if ((s[index-1] == 'e') || (s[index-1] == 'E')) {
            index = s.find_first_of("+-/*()", index+1);
          }
        }
      }
      if (index != s.npos) {
        return index;
      } else {
        return end;
      }
    } else {
      // in case there is no alphanumeric character
      log << MSG::ERROR
          << "Invalid expression (Alphanumeric character expected) : \""
          << s.substr (realStart, end - realStart) << "\""
          << endreq;
      return end;
    }
  }
  // We should never reach this point
  log << MSG::ERROR
      << "This should never appear, please report" << endreq;
  return end;
}


// -----------------------------------------------------------------------
// sumHasUnit
// -----------------------------------------------------------------------
bool XmlCnvSvc::sumHasUnit (std::string s,
                 unsigned int baseIndex,
                 unsigned int lastIndex) {
  bool result = true;
  unsigned int index = baseIndex;
  unsigned int oldIndex = index;
  while (oldIndex < lastIndex) {
    // get next product subexpression
    index = skipProduct (s, oldIndex, lastIndex);
    // see whether it has a unit
    result = result && productHasUnit(s, oldIndex, index);
    // skip the + or - sign
    oldIndex = s.find_first_not_of(' ', index + 1);
    if (oldIndex == s.npos) oldIndex = lastIndex;
  }
  return result;
}


// -----------------------------------------------------------------------
// productHasUnit
// -----------------------------------------------------------------------
bool XmlCnvSvc::productHasUnit (std::string s,
                     unsigned int baseIndex,
                     unsigned int lastIndex) {
  bool result = false;
  unsigned int index = baseIndex;
  unsigned int oldIndex = index;
  while (oldIndex < lastIndex) {
    // get next subexpression
    index = skipExpr (s, oldIndex, lastIndex);
    // see whether it has a unit
    result = result || exprHasUnit(s, oldIndex, index);
    // skip the * or / sign
    oldIndex = s.find_first_not_of(' ', index + 1);
    if (oldIndex == s.npos) oldIndex = lastIndex;
  }
  return result;
}


// -----------------------------------------------------------------------
// exprHasUnit
// -----------------------------------------------------------------------
bool XmlCnvSvc::exprHasUnit (std::string s,
                             unsigned int baseIndex,
                             unsigned int lastIndex) {
  // deal with the unary minus
  unsigned int realBaseIndex =  s.find_first_not_of(' ', baseIndex);
  if (realBaseIndex == s.npos) {
    realBaseIndex = lastIndex;
  } else if (s[realBaseIndex] == '-') {
    realBaseIndex = s.find_first_not_of(' ', realBaseIndex + 1);
    if (realBaseIndex == s.npos) realBaseIndex = lastIndex;
  }
  if (realBaseIndex == lastIndex) {
    // empty expression, return false
    return false;
  }
  unsigned int index = s.find_first_of("+-/*()", realBaseIndex);
  if ((index != s.npos) && (s[index] == '(')) {
    if (index == realBaseIndex) {
      // if we the expression starts with an opening parenthesis
      // deal with the sum which is its content
      return sumHasUnit (s, realBaseIndex + 1, lastIndex - 1);
    } else {
      // else we are calling a function, the result has no unit except
      // if the function abs is called.
      int findex = s.find_last_not_of(' ', index-1);
      std::string function = s.substr(realBaseIndex, findex+1-realBaseIndex);
      if ((function == "abs") || (function == "sqrt")) {
        return sumHasUnit(s, index+1, s.find_last_of(')', lastIndex));
      } else {
        return false;
      }
    }
  } else {
    // else we consider that having an alphabetic character at the first
    // place means there is a unit
    return isalpha(s[realBaseIndex]);
  }
}
