// $Id: DaDiPackage.h,v 1.12 2003-12-18 14:05:35 mato Exp $
#ifndef DADIPACKAGE_H
#define DADIPACKAGE_H 1

// Include files
#include "DaDiNamespace.h"
#include "DaDiClass.h"
#include "DaDiAssociation.h"
#include "xercesc/util/XMLString.hpp"

#include <list>

/** @class DaDiPackage DaDiPackage.h
 *
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiPackage {
public:
  /// Standard constructor
  DaDiPackage() :
    m_packageName(0),
    m_impSoftList(std::list<std::string>()),
    m_impStdList(std::list<std::string>()),
    m_importList(std::list<std::string>()),
    m_noImports(std::list<std::string>()),
    m_daDiClass(std::list<DaDiClass*>()),
    m_daDiNamespace(std::list<DaDiNamespace*>()),
    m_daDiAssociation(std::list<DaDiAssociation*>())
  {};

  virtual ~DaDiPackage();

  const XMLCh* packageName();
  void setPackageName(const XMLCh* value);

  std::string popImpSoftList();
  std::list<std::string> impSoftList();
  void pushImpSoftList(std::string value);
  int sizeImpSoftList();
  void remDblImpSoftList();

  std::string popImpStdList();
  std::list<std::string> impStdList();
  void pushImpStdList(std::string value);
  int sizeImpStdList();
  void remDblImpStdList();

  std::string popImportList();
  std::list<std::string> importList();
  void pushImportList(std::string value);
  int sizeImportList();
  void remDblImportList();

  std::string popNoImports();
  std::list<std::string> noImports();
  void pushNoImports(std::string value);
  int sizeNoImports();

  DaDiClass* popDaDiClass();
  void pushDaDiClass(DaDiClass* value);
  int sizeDaDiClass();

  DaDiNamespace* popDaDiNamespace();
  void pushDaDiNamespace(DaDiNamespace* value);
  int sizeDaDiNamespace();

  DaDiAssociation* popDaDiAssociation();
  void pushDaDiAssociation(DaDiAssociation* value);
  int sizeDaDiAssociation();

protected:

private:

  XMLCh                       *m_packageName;
  std::list<std::string>       m_impSoftList;
  std::list<std::string>       m_impStdList;
  std::list<std::string>       m_importList;
  std::list<std::string>       m_noImports;
  std::list<DaDiClass*>        m_daDiClass;
  std::list<DaDiNamespace*>    m_daDiNamespace;
  std::list<DaDiAssociation*>  m_daDiAssociation;

};

inline DaDiPackage::~DaDiPackage()
{
  delete [] m_packageName;
  std::list<DaDiClass*>::iterator cIter;
  for (cIter = m_daDiClass.begin(); cIter != m_daDiClass.end(); ++cIter)
  {
    delete *cIter;
  }
  std::list<DaDiNamespace*>::iterator nIter;
  for (nIter = m_daDiNamespace.begin(); nIter != m_daDiNamespace.end(); ++nIter)
  {
    delete *nIter;
  }
  std::list<DaDiAssociation*>::iterator asIter;
  for (asIter = m_daDiAssociation.begin(); asIter != m_daDiAssociation.end(); ++asIter)
  { 
    delete *asIter; 
  }
};


inline const XMLCh* DaDiPackage::packageName()
{
  return m_packageName;
}

inline void DaDiPackage::setPackageName(const XMLCh* value)
{
  m_packageName = new XMLCh[xercesc::XMLString::stringLen(value)+1];
  xercesc::XMLString::copyString(m_packageName, value);
}

inline std::string DaDiPackage::popImpSoftList()
{
  std::string pt =  m_impSoftList.front();
  m_impSoftList.push_back(pt);
  m_impSoftList.pop_front();
  return pt;
}

inline std::list<std::string> DaDiPackage::impSoftList()
{
  return m_impSoftList;
}

inline void DaDiPackage::pushImpSoftList(std::string value)
{
  bool doImport = true;
  for (int i=0; i<sizeImportList(); ++i)
  {
    if (value == popImportList())
    {
      doImport = false;
    }
  }
  if (doImport)
  {
    m_impSoftList.push_back(value);
  }
}

inline int DaDiPackage::sizeImpSoftList()
{
  return m_impSoftList.size();
}

inline void DaDiPackage::remDblImpSoftList()
{
  m_impSoftList.sort();
  m_impSoftList.unique();
}

inline std::string DaDiPackage::popImpStdList()
{
  std::string pt =  m_impStdList.front();
  m_impStdList.push_back(pt);
  m_impStdList.pop_front();
  return pt;
}

inline std::list<std::string> DaDiPackage::impStdList()
{
  return m_impStdList;
}

inline void DaDiPackage::pushImpStdList(std::string value)
{
  m_impStdList.push_back(value);
}

inline int DaDiPackage::sizeImpStdList()
{
  return m_impStdList.size();
}

inline void DaDiPackage::remDblImpStdList()
{
  m_impStdList.sort();
  m_impStdList.unique();
}

inline std::string DaDiPackage::popImportList()
{
  std::string pt = m_importList.front();
  m_importList.push_back(pt);
  m_importList.pop_front();
  return pt;
}

inline std::list<std::string> DaDiPackage::importList()
{
  return m_importList;
}

inline void DaDiPackage::pushImportList(std::string value)
{
  int lastspace;
  std::string import, lastword;

  while (value != "")
  {
    int i = value.find_first_of(":,<>");

    if (i == -1)
    {
      import = value;
      value = "";
    }
    else
    {
      import = value.substr(0,i);
      value = value.substr(i+1,std::string::npos);
    }

    lastspace = import.find_last_of(" ");
    lastword = import.substr(lastspace+1, import.size()-lastspace);
    if ((lastword != "bool")   && (lastword != "short")    &&
        (lastword != "long")   && (lastword != "int")      &&
        (lastword != "float")  && (lastword != "double")   &&
        (lastword != "char")   && (lastword != "unsigned") &&
        (lastword != "signed") && (lastword != "")         &&
        (lastword != "string") && (lastword != "std")      &&
        (lastword != "longlong"))
    {
      if ((import == "vector") || (import == "list")   ||
          (import == "deque")  || (import == "queue")    ||
          (import == "stack")  || (import == "map")      ||
          (import == "set")    || (import == "bitset")   ||
          (import == "pair"))
      {
        m_impStdList.push_back(import);
      }
      else
      {
        for (int i=0; i<sizeImpSoftList(); ++i)
        {
          if (import == m_impSoftList.front())
          {
            m_impSoftList.pop_front();
          }
        }
        m_importList.push_back(import);
      }
    }
  }
}

inline int DaDiPackage::sizeImportList()
{
  return m_importList.size();
}

inline void DaDiPackage::remDblImportList()
{
  m_importList.sort();
  m_importList.unique();
}

inline std::string DaDiPackage::popNoImports()
{
  std::string pt = m_noImports.front();
  m_noImports.push_back(pt);
  m_noImports.pop_front();
  return pt;
}

inline std::list<std::string> DaDiPackage::noImports()
{
  return m_noImports;
}

inline void DaDiPackage::pushNoImports(std::string value)
{
  m_noImports.push_back(value);
}

inline int DaDiPackage::sizeNoImports()
{
  return m_noImports.size();
}

inline DaDiClass* DaDiPackage::popDaDiClass()
{
  DaDiClass* pt = m_daDiClass.front();
  m_daDiClass.push_back(pt);
  m_daDiClass.pop_front();
  return pt;
}

inline void DaDiPackage::pushDaDiClass(DaDiClass* value)
{
  m_daDiClass.push_back(value);
}

inline int DaDiPackage::sizeDaDiClass()
{
  return m_daDiClass.size();
}

inline DaDiNamespace* DaDiPackage::popDaDiNamespace()
{
  DaDiNamespace* pt = m_daDiNamespace.front();
  m_daDiNamespace.push_back(pt);
  m_daDiNamespace.pop_front();
  return pt;
}

inline void DaDiPackage::pushDaDiNamespace(DaDiNamespace* value)
{
  m_daDiNamespace.push_back(value);
}

inline int DaDiPackage::sizeDaDiNamespace()
{
  return m_daDiNamespace.size();
}

inline DaDiAssociation* DaDiPackage::popDaDiAssociation()
{
  DaDiAssociation* pt = m_daDiAssociation.front();
  m_daDiAssociation.push_back(pt);
  m_daDiAssociation.pop_front();
  return pt;
}

inline void DaDiPackage::pushDaDiAssociation(DaDiAssociation* value)
{
  m_daDiAssociation.push_back(value);
}

inline int DaDiPackage::sizeDaDiAssociation()
{
  return m_daDiAssociation.size();
}

#endif // DADIPACKAGE_H

