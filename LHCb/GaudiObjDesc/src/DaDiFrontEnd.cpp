// $Id: DaDiFrontEnd.cpp,v 1.5 2001-10-19 17:28:31 mato Exp $

#include "GaudiKernel/Kernel.h"

#include <iostream>
#include <string>
#include <list>
#include <ctime>
#include <vector>

#include <util/PlatformUtils.hpp>
#include <util/XMLUniDefs.hpp>
#include <parsers/DOMParser.hpp>
#include <dom/DOM_NodeList.hpp>
#include <dom/DOM_Element.hpp>
#include <dom/DOM_DOMException.hpp>
#include <dom/DOM_NamedNodeMap.hpp>
#include <dom/DOMString.hpp>

#include "DaDiTools.h"
#include "DaDiFrontEnd.h"
#include "DaDiPackage.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Dadi
//
// 18/05/2001 : 
//-----------------------------------------------------------------------------

extern std::string argV0;



std::vector<std::string> findWords(std::string value, std::string delim)
{
  std::vector<std::string> words;
  int i,j = 0;

  while ((i = value.find(delim)) != std::string::npos)
  {
    while((i = value.find(" ")) == 0)
    {
      value = value.substr(1, value.size()-1);
    }
    words.push_back(value.substr(0,i));
    value = value.substr(i+1,value.size()-i);
  }
  if (value.size() != 0)
  {
    words.push_back(value.substr(0,value.size()));
  }
  return words;
}

bool isPointer(DOMString value)
{
  while (value.substringData(value.length()-1,1).equals(" "))
  {
    value = value.substringData(value.length()-1,1);
  }
  return value.substringData(value.length()-1,1).equals("*");
}

DaDiPackage* DDFE::DaDiFrontEnd(char* filename)
{

  DaDiPackage* gddPackage = new DaDiPackage();

  static char* gXmlFile = 0;

// 
// Initialization of XML-Parser
//
  try 
  {
    XMLPlatformUtils::Initialize();
  }

  catch(const XMLException& toCatch)
    {
    std::cerr << "Error during Xerces-c Initialization.\n"
      << " Exception message: "
      << toCatch.getMessage() << std::endl;
      exit(1);
    }

  gXmlFile = filename;

  DOMParser *parser = new DOMParser;
  parser->setValidationScheme(DOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  ErrorHandler* errReporter = new DaDiTools();
  parser->setErrorHandler(errReporter);
  parser->setCreateEntityReferenceNodes(false);
  parser->setToCreateXMLDeclTypeNode(true);

  bool errorsOccured = false;

//
// parse file, catch errors
//

  try
  {
    parser->parse(gXmlFile);
  }

  catch(const XMLException& e)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile << "\n Message: "
      << e.getMessage() << std::endl;
    errorsOccured = true;
    }

  catch(const DOM_DOMException& e)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile << "\n Message: "
      << e.msg.transcode() << std::endl;
      errorsOccured = true;
    }

  catch(...)
    {
    std::cerr << "An error occured during parsing file " << gXmlFile << std::endl;
    errorsOccured=true;
    }

  if (errorsOccured)
  {
    std::cerr << argV0 << ": Errors occured, so exiting" << std::endl;
    exit(1);
  }
//
// If no errors occured start walking DOMtree
//
  else
  {
    DOM_Node doc = parser->getDocument();
    DOM_Node top = doc.getFirstChild();

    while (!top.getNodeName().equals("gdd"))
        {
      top = top.getNextSibling();
        }
      
    DOM_Node gdd_node = top.getNextSibling().getFirstChild();
    while (!gdd_node.isNull())
    {
      if (gdd_node.getNodeName().equals("import"))
      {
        DDFE::parseImport(gdd_node,gddPackage);
      }
      if (gdd_node.getNodeName().equals("package"))
      {
        DDFE::parsePackage(gdd_node,gddPackage);
      } 
      if (gdd_node.getNodeName().equals("class"))
      {
        gddPackage->setPackageName(DOMString::transcode("__NO_PACKAGE__"));
        DaDiClass* gddClass = new DaDiClass();
        gddPackage->pushDaDiClass(gddClass);
        DDFE::parseClass(gdd_node,gddClass);
      }
      gdd_node = gdd_node.getNextSibling();
    }
  }
  return gddPackage;
}

//
// ParseImport (package)
//
void DDFE::parseImport(DOM_Node node, DaDiPackage* gddPackage)
{  
  std::string import_name, import_soft, import_std;
  
  import_name = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().transcode();
  import_soft = node.getAttributes().
    getNamedItem(DOMString::transcode("soft")).getNodeValue().transcode();
  import_std = node.getAttributes().
    getNamedItem(DOMString::transcode("std")).getNodeValue().transcode();

  if (import_soft == "TRUE")
    {
    gddPackage->pushImpSoftList(import_name);
    }
  else if (import_std == "TRUE")
    {
    gddPackage->pushImpStdList(import_name);
    }
  else
    {
    gddPackage->pushImportList(import_name);
    }
}

//
// ParseImport (class)
//
void DDFE::parseImport(DOM_Node node, DaDiClass* gddClass)
{  
  std::string import_name, import_soft, import_std;
  
  import_name = node.getAttributes().
    getNamedItem(DOMString::transcode("name")).getNodeValue().transcode();
  import_soft = node.getAttributes().
    getNamedItem(DOMString::transcode("soft")).getNodeValue().transcode();
  import_std = node.getAttributes().
    getNamedItem(DOMString::transcode("std")).getNodeValue().transcode();
  
  if (import_soft == "TRUE")
    {
    gddClass->pushImpSoftList(import_name);
    }
  else if (import_std == "TRUE")
    {
    gddClass->pushImpStdList(import_name);
    }
  else
    {
    gddClass->pushImportList(import_name);
    }
}


//
// ParsePackage
//
void DDFE::parsePackage(DOM_Node node, DaDiPackage* gddPackage)
{
  gddPackage->setPackageName(node.getAttributes().    
    getNamedItem(DOMString::transcode("name")).
        getNodeValue());
  
  node = node.getFirstChild();
  
  while(!node.isNull())
    {
    switch(node.getNodeType())
      {
    case DOM_Node::ELEMENT_NODE:
      {
        if (node.getNodeName().equals("import"))
              {
          DDFE::parseImport(node,gddPackage);
        }
        else if (node.getNodeName().equals("class"))
        {
          DaDiClass* gddClass = new DaDiClass();
          gddPackage->pushDaDiClass(gddClass);
          DDFE::parseClass(node,gddClass);
        }
        else 
        {
          node = node.getNextSibling();
        }
    default:
        {
          node = node.getNextSibling();
        }
      }
    }
  }
}


//
// ParseClass
//
void DDFE::parseClass(DOM_Node node, DaDiClass* gddClass)
{
  bool classHasDesc = false;

  gddClass->setClassName(node.getAttributes().
    getNamedItem(DOMString::transcode("name")).
        getNodeValue());

  if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).isNull())
  {
    gddClass->setClassDesc(node.getAttributes().
      getNamedItem(DOMString::transcode("desc")).
      getNodeValue());
    classHasDesc = true;
  }
  else
  {
    gddClass->setClassDesc(NULL);
  }
  
  gddClass->setClassAuthor(node.getAttributes().
    getNamedItem(DOMString::transcode("author")).
    getNodeValue());

  gddClass->setClassTemplateVector(node.getAttributes().
    getNamedItem(DOMString::transcode("templateVector")).
    getNodeValue());
  if (gddClass->classTemplateVector().equals("TRUE"))
  {
    gddClass->pushImportList("ObjectVector");
  }
  
  gddClass->setClassTemplateList(node.getAttributes().
    getNamedItem(DOMString::transcode("templateList")).
    getNodeValue());
  if (gddClass->classTemplateList().equals("TRUE"))
  {
    gddClass->pushImportList("ObjectList");
  }
  
  if(!node.getAttributes().getNamedItem(DOMString::transcode("id")).isNull())
  {
    gddClass->setClassID(node.getAttributes().
      getNamedItem(DOMString::transcode("id")).
      getNodeValue());
  }
  else
  {
    gddClass->setClassID(NULL);
  }

  gddClass->setLongDesc(NULL);

  node = node.getFirstChild();

  while(!node.isNull())
  {
    switch(node.getNodeType())
    {    
    case DOM_Node::ELEMENT_NODE:
      {
//
// Parse description
//
        if(node.getNodeName().equals("desc"))
        {
          gddClass->setLongDesc(node.getFirstChild().
                  getNodeValue().transcode());
          classHasDesc = true;
        }
//
// ParseImport
//
        if(node.getNodeName().equals("import"))
        {
          DDFE::parseImport(node,gddClass);
        }
//
// Parse baseclasses
//
        else if(node.getNodeName().equals("base"))
        {
          DaDiBaseClass* gddBaseClass = new DaDiBaseClass();

          gddClass->pushDaDiBaseClass(gddBaseClass);

          gddBaseClass->setName(node.getAttributes().getNamedItem(DOMString::transcode("name")).getNodeValue().transcode());

          gddClass->pushImportList(node.getAttributes().getNamedItem(DOMString::transcode("name")).getNodeValue().transcode());

          gddBaseClass->setVirtual_(node.getAttributes().getNamedItem(DOMString::transcode("virtual")).getNodeValue().transcode());

          gddBaseClass->setAccess(node.getAttributes().getNamedItem(DOMString::transcode("access")).getNodeValue().transcode());
        }

//
// Parse methods
//
        else if(node.getNodeName().equals("method"))
        {
          DaDiMethod* gddMethod = new DaDiMethod();

          gddClass->pushDaDiMethod(gddMethod);

          gddMethod->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
                        getNodeValue().transcode());
          
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddMethod->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddMethod->setDesc(NULL);
          }
            
          gddMethod->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
                        getNodeValue().transcode());
            
          gddMethod->setConst_(node.getAttributes().
            getNamedItem(DOMString::transcode("const")).
                        getNodeValue().transcode());
            
          gddMethod->setVirtual_(node.getAttributes().
            getNamedItem(DOMString::transcode("virtual")).
                        getNodeValue().transcode());
            
          gddMethod->setStatic_(node.getAttributes().
            getNamedItem(DOMString::transcode("static")).
                        getNodeValue().transcode());            

          gddMethod->setInline_(node.getAttributes().
            getNamedItem(DOMString::transcode("inline")).
                        getNodeValue().transcode());            

          DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
          gddMethReturn->setConst_("FALSE");
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("type")).isNull())
          {
            gddMethod->setDaDiMethReturn(gddMethReturn);

            std::string typeStr = node.getAttributes().
              getNamedItem(DOMString::transcode("type")).
              getNodeValue().transcode();
            
            if (typeStr.find("const ") == std::string::npos)
            {
              gddMethReturn->setType(node.getAttributes().
                getNamedItem(DOMString::transcode("type")).
                getNodeValue().transcode());
            }
            else
            {
              gddMethReturn->setType(DOMString::transcode(typeStr.substr(
                typeStr.find(' ')+1, std::string::npos).data()));
              gddMethReturn->setConst_("TRUE");
            }
          }
          else
          {
            gddMethReturn->setType(NULL);
          }

          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            int valueNo = 0;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i)," ");
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList," ");
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddMethod->pushDaDiMethArgument(gddMethArgument);

              if (args.size() < 2)
              {
                std::cerr << argV0 << ": Error in 'argList'-description of method "
                  << gddMethod->name().transcode() << " (Class: " << gddClass->className().transcode()
                  << "), you have to provide at least a type-name-pair" << std::endl;
                exit(1);
              }

              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));

              DOMString argType = DOMString::transcode(args[args.size()-2].data());
              if (isPointer(argType))
              {
                gddMethArgument->setIsPointer(true);
                gddMethArgument->setType(argType.substringData(0,argType.length()-1));
              }
              else
              {
                gddMethArgument->setIsPointer(false);
                gddMethArgument->setType(argType);
              }
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
              gddMethArgument->setInout("INPUT");
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddMethod->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(met_child.getNodeName().equals("return"))
              {
                DaDiMethReturn* gddMethReturn = new DaDiMethReturn();
              
                gddMethod->setDaDiMethReturn(gddMethReturn);

                gddMethReturn->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                gddMethReturn->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                          
              }
              if(met_child.getNodeName().equals("code"))
              {
                gddMethod->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddMethod->pushDaDiMethArgument(gddMethArgument);
                DOMString argType = met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode();
                if (isPointer(argType))
                {
                  gddMethArgument->setIsPointer(true);
                  gddMethArgument->setType(argType.substringData(0,argType.length()-1));
                }
                else
                {
                  gddMethArgument->setIsPointer(false);
                  gddMethArgument->setType(argType);
                }
                
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse constructors
//
        else if(node.getNodeName().equals("constructor"))
        {
          DaDiConstructor* gddConstructor = new DaDiConstructor();

          gddClass->pushDaDiConstructor(gddConstructor);

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddConstructor->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddConstructor->setDesc(NULL);
          }
            
          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i)," ");
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList," ");
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddConstructor->pushDaDiMethArgument(gddMethArgument);
              
              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));
              gddMethArgument->setType(DOMString::transcode(args[args.size()-2].data()));
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddConstructor->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(met_child.getNodeName().equals("code"))
              {
                gddConstructor->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddConstructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse destructors
//
        else if(node.getNodeName().equals("destructor"))
        {
          DaDiDestructor* gddDestructor = new DaDiDestructor();

          gddClass->pushDaDiDestructor(gddDestructor);

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("desc")).isNull())
          {
            gddDestructor->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddDestructor->setDesc(NULL);
          }
            
          std::string argList, argInOut;
          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argList")).isNull())
          {
            argList = node.getAttributes().
              getNamedItem(DOMString::transcode("argList")).
              getNodeValue().transcode();

            bool lastElem = false;
            while(!lastElem)
            {
              std::vector<std::string> args;
              int i = argList.find(",");
              if (i != std::string::npos)
              {
                args = findWords(argList.substr(0,i)," ");
                argList = argList.substr(i+1,argList.size()-i);
              }
              else
              {
                args = findWords(argList," ");
                lastElem = true;
              }

              DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
              gddDestructor->pushDaDiMethArgument(gddMethArgument);
              
              gddMethArgument->setName(DOMString::transcode(args[args.size()-1].data()));
              gddMethArgument->setType(DOMString::transcode(args[args.size()-2].data()));
              
              for (int j = (args.size()-3); j>=0; --j)
              {
                if (args[j] == "const")
                {
                  gddMethArgument->setConst_("TRUE");
                }
                else
                {
                std::cerr << argV0 << ": Don't know how to handle modifier: " << args[j] << "." << std::endl;
                }
              }
            }
          }

          if (!node.getAttributes().
            getNamedItem(DOMString::transcode("argInOut")).isNull())
          {
            argInOut = node.getAttributes().
              getNamedItem(DOMString::transcode("argInOut")).
              getNodeValue().transcode();           
          }

          
          gddDestructor->setCode(NULL);
          
          DOM_Node met_child;
          met_child = node.getFirstChild();
          
          while(!met_child.isNull())
          {                
            switch(met_child.getNodeType())    
            {      
            case DOM_Node::ELEMENT_NODE:
              {
              if(met_child.getNodeName().equals("code"))
              {
                gddDestructor->setCode(met_child.getFirstChild().
                  getNodeValue().transcode());
              }
              if(met_child.getNodeName().equals("arg"))
              {
                DaDiMethArgument* gddMethArgument = new DaDiMethArgument();
                gddDestructor->pushDaDiMethArgument(gddMethArgument);
                gddMethArgument->setType(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("type")).
                  getNodeValue().transcode());
                if (!met_child.getAttributes().
                  getNamedItem(DOMString::transcode("name")).isNull())
                {
                  gddMethArgument->setName(met_child.getAttributes().
                    getNamedItem(DOMString::transcode("name")).
                    getNodeValue().transcode());
                }
                else
                {
                  gddMethArgument->setName(NULL);
                }
                gddMethArgument->setConst_(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("const")).
                  getNodeValue().transcode());
                gddMethArgument->setInout(met_child.getAttributes().
                  getNamedItem(DOMString::transcode("inout")).
                  getNodeValue().transcode());
              }
              }
            default:
              {
                met_child = met_child.getNextSibling();
              }
            }
          }
        }

//
// Parse attributes
//
        else if(node.getNodeName().equals("attribute"))
        {
          DaDiAttribute* gddAttribute = new DaDiAttribute();
          gddClass->pushDaDiAttribute(gddAttribute);
          gddAttribute->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
            getNodeValue().transcode());
            
          gddAttribute->setType(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          gddClass->pushImportList(node.getAttributes().
              getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          
          if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).
            isNull())
          {
            gddAttribute->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddAttribute->setDesc(NULL);
          }
            
          gddAttribute->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
            getNodeValue().transcode());
            
          gddAttribute->setSetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
                    getNodeValue().transcode());
            
          gddAttribute->setGetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
              getNodeValue().transcode());

          if(!node.getAttributes().getNamedItem(DOMString::transcode("init")).isNull())
          {
            gddAttribute->setInit(node.getAttributes().
              getNamedItem(DOMString::transcode("init")).
              getNodeValue());
          }
          else
          {
            gddAttribute->setInit(NULL);
          }            
        }

//
// Parse relations
//
        else if(node.getNodeName().equals("relation"))
        {
          DOMString gddRelationRatio;
            
          DaDiRelation* gddRelation = new DaDiRelation();
          gddClass->pushDaDiRelation(gddRelation);
          gddRelation->setName(node.getAttributes().
            getNamedItem(DOMString::transcode("name")).
            getNodeValue().transcode());
            
          gddRelation->setType(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          gddClass->pushImpSoftList(node.getAttributes().
            getNamedItem(DOMString::transcode("type")).
            getNodeValue().transcode());
          
          if (!node.getAttributes().getNamedItem(DOMString::transcode("desc")).
            isNull())
          {
            gddRelation->setDesc(node.getAttributes().
              getNamedItem(DOMString::transcode("desc")).
              getNodeValue().transcode());
          }
          else
          {
            gddRelation->setDesc(NULL);
          }
            
          gddRelation->setAccess(node.getAttributes().
            getNamedItem(DOMString::transcode("access")).
            getNodeValue().transcode());
            
          gddRelationRatio = node.getAttributes().
            getNamedItem(DOMString::transcode("ratio")).
            getNodeValue();
          gddRelation->setRatio(gddRelationRatio);
            
          gddRelation->setSetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("setMeth")).
            getNodeValue().transcode());
            
          gddRelation->setGetMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("getMeth")).
            getNodeValue().transcode());
            
          gddRelation->setAddMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("addMeth")).
            getNodeValue().transcode());
            
          gddRelation->setRemMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("remMeth")).
            getNodeValue().transcode());
            
          gddRelation->setClrMeth(node.getAttributes().
            getNamedItem(DOMString::transcode("clrMeth")).
            getNodeValue().transcode());
            
          if (gddRelationRatio.equals("1-1"))
          {
            gddClass->pushImportList("SmartRef");
          }
          else if (gddRelationRatio.equals("1-M") ||
                   gddRelationRatio.equals("M-N"))
          {
            gddClass->pushImportList("SmartRef");
            gddClass->pushImportList("SmartRefVector");
          }
        }
        else
        {    
          node = node.getNextSibling();
        } 
      }
      default:
      {
        node = node.getNextSibling();
      }
    }
  }
  if (!classHasDesc)
  {
    std::cerr << "Class " <<  gddClass->className().transcode() 
      << " has no description, please add one to the xml-file" << std::endl;
    exit(1);
  }
}

