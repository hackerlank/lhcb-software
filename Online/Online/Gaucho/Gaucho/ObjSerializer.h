#pragma once
#include <string>
#include <vector>
#include <map>
class MonObj;
typedef std::map<std::string,MonObj*> ObjMap;
typedef std::pair<std::string, MonObj*> ObjPair;
typedef ObjMap::iterator SysIter;
class ObjSerializer
{
protected:
  int buffersize;
  void *buffer;
public:
  ObjMap *m_objmap;
  bool m_expandservices;
  ObjSerializer(void);
  ObjSerializer(ObjMap *ObjMap, bool expand=false);
  ~ObjSerializer(void);
  void *Allocate(int siz);
  virtual void *SerializeObj(void * &ptr,int &siz, bool clear=false);
  virtual void *SerializeObj(char *name,void * &ptr,int &siz, bool clear=false);
  virtual void *SerializeObj(std::vector<std::string> &nams,void * &ptr,int &siz, bool clear=false);
  virtual void *SerializeDir(void *&ptr, int &siz);
  virtual void DeSerializeObj(char *name,void *&ptr);
  virtual void DeSerializeObj(std::vector<std::string> &nams,void *&ptr);
  virtual MonObj *findobj(const char *n);
  virtual void UpdateExpansions(void);
};
