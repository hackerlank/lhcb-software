import os
import gparser, importUtils

#================================================================================
class genClassDicts(importUtils.importUtils):
#--------------------------------------------------------------------------------
  def __init__(self, cdb, godRoot):
    importUtils.importUtils.__init__(self,cdb)
    self.godRoot = godRoot
    self.classIsAbstract = 0
    self.hasDefaultConstructor = 0
    self.generatedEnums = []
    self.rem = '//' + '-'*78 + '\n'
    self.mNum = 0
    self.cNum = 1
    self.clName = ''
#--------------------------------------------------------------------------------
  def reset(self, godClass):
    importUtils.importUtils.reset(self,godClass)
    self.classIsAbstract = 0
    self.hasDefaultConstructor = 0
    self.generatedEnums = []
    self.include = []
    self.mNum = 0
    self.cNum = 1
    self.clName = ''
#--------------------------------------------------------------------------------
  def generateEnums(self,godClass):
    if godClass.has_key('enum'):
      for enum in godClass['enum']:
        self.generatedEnums.append(enum['attrs']['name'])
#--------------------------------------------------------------------------------
  def genConstructorStubs(self,godClass):
    classname = godClass['attrs']['name']
    s = ''
    if not self.classIsAbstract:
      if godClass.has_key('constructor'):
        for const in godClass['constructor']:
          indent = 0
          paramList = []
          if const['attrs'].has_key('argList') : paramList += self.tools.genParamsFromStrg(const['attrs']['argList'])
          if const.has_key('arg')              : paramList += self.tools.genParamsFromElem(const['arg'])
          if ((not const['attrs'].has_key('argList')) and (not const.has_key('arg'))): self.hasDefaultConstructor = 1 
          s += self.rem
          s += 'static void* %s_constructor_%d(void* mem' % ( classname, self.cNum )
          if len(paramList) : s += ', const std::vector<void*>& argList'
          s += ')\n'
          s += self.rem
          s += '{\n'
          s += '  return new(mem) %s(' % classname
          indent += len(classname) + 19
          if len(paramList) :
            i = 0
            for p in paramList :
              if s[-1] != '(': s += ',\n%s' % (indent*' ')
              if p.find('=') != -1: p = p.split('=')[0]
              p = (' ').join(p.split()[:-1])
              if p in self.generatedEnums : p = 'int'
              p = self.checkTypedefEnums(p,godClass['attrs']['name'])
              if self.tools.isReferenceT(p) : p = p.strip()[:-1]
              if self.tools.isPointerT(p) : s += '(%s)argList[%d]' % (p, i)
              else : s += '*(%s*)argList[%d]' % (p, i)
              i += 1
          s += ');\n}\n\n'
          self.cNum += 1
      if not self.hasDefaultConstructor :                                                                       # there is no default constructor defined
        s += self.rem
        s += 'static void* %s_constructor_0(void* mem)\n' % ( classname )
        s += self.rem
        s += '{\n  return new(mem) %s();\n}\n\n' % classname
    return s
#--------------------------------------------------------------------------------
  def genDestructorStub(self,classname):
    s = self.rem
    s += 'static void %s_destructor(void* mem)\n' % classname
    s += self.rem
    s += '{\n  ((%s*)mem)->~%s();\n}\n\n' % (classname, classname)
    return s  
#--------------------------------------------------------------------------------
  def genBaseStubs(self,godClass):
    s = ''
    if godClass.has_key('base'):
      clName = godClass['attrs']['name']
      for b in godClass['base']:
        bName = b['attrs']['name']
        s += self.rem
        s += 'int %s_to_%s(void* mem)\n' % (clName, self.tools.cppEscape(bName))
        s += self.rem
        s += '{\n'
        s += '  return (int)(%s*)(%s*)mem - (int)(%s*)mem;\n' % (bName, clName, clName)
        s += '}\n\n'
    return s
#--------------------------------------------------------------------------------
  def genMethodStubs(self,godClass):
    s = ''
    if godClass.has_key('method'):
      for method in godClass['method']:
        indent = 0
        if method['attrs']['access'] in ('PRIVATE', 'PROTECTED') : break
        if method['attrs']['friend'] == 'TRUE' : break
        constF = ''
        if method['attrs']['const'] == 'TRUE' : constF = 'const'
        retType = ''
        if method['attrs']['type'].strip() : retType = self.tools.genReturnFromStrg(method['attrs']['type'],[],'')
        elif method.has_key('return')      : retType = self.tools.genReturnFromElem(method['return'],[],'')
        if retType == 'void' : retType = ''
        paramList = []
        if method['attrs'].has_key('argList') : paramList += self.tools.genParamsFromStrg(method['attrs']['argList'])
        if method.has_key('arg')              : paramList += self.tools.genParamsFromElem(method['arg'])
        s += self.rem
        s += 'static void'
        if retType : s += '*'
        s += ' %s_%s_%d%s(void* v' % (godClass['attrs']['name'], self.tools.cppEscape(method['attrs']['name']),self.mNum,constF)
        if len(paramList) : s += ', const std::vector<void*>& argList'
        s += ')\n'
        s += self.rem
        s += '{\n'
        if retType :
          if retType in self.generatedEnums : retType = 'int'
          retType = self.checkTypedefEnums(retType, godClass['attrs']['name'])
          if self.tools.isFundamentalT(retType):
            s += '  static %s ret;\n' % retType
            s += '  ret = '
            indent += 8
          elif self.tools.isReferenceT(retType):
            s += '  %s ret = ' % retType
            indent += len(retType) + 9
          elif self.tools.isPointerT(retType):
            retType2 = self.tools.unQualifyT(retType)
            s += '  return (%s) ' % retType2
            indent += len(retType2) + 12
          else :
            s += '  static %s ret = ' % retType
            indent += len(retType) + 16
        else :
          s += '  '
          indent += 2
        s += '((%s*)v)->%s(' % (godClass['attrs']['name'], method['attrs']['name'])
        indent += len(godClass['attrs']['name']) + len(method['attrs']['name']) + 9
        if len(paramList) :
          i = 0
          for p in paramList :
            if s[-1] != '(': s += ',\n%s' % (indent*' ')
            if p.find('=') != -1: p = p.split('=')[0]
            p = (' ').join(p.split()[:-1])
            if p in self.generatedEnums : p = 'int'
            p = self.checkTypedefEnums(p,godClass['attrs']['name'])
            if self.tools.isPointerT(p) :
              s += '(%s)argList[%d]' % (p, i)
            else :
              if self.tools.isReferenceT(p) : p = p.strip()[:-1] 
              s += '*(%s*)argList[%d]' % (p, i)
            i += 1
        s += ');\n'
        if retType :
          if self.tools.isFundamentalT(retType):
            s += '  return &ret;\n'
          elif self.tools.isReferenceT(retType):
            s += '  return (void*)&ret;\n'
          elif self.tools.isPointerT(retType):
            pass
          else :
            s += '  return (void*)&ret;\n'
        s += '}\n\n'
        self.mNum += 1
    return s
#--------------------------------------------------------------------------------
  def genStubFunction(self,ret,cl,name,params):
    s = self.rem
    s += 'static void'
    if ret != 'void' : s += '*'
    s += ' %s_%s_%d(void* v' % (cl, name, self.mNum)
    if len(params) : s += ', const std::vector<void*>& argList'
    s += ')\n%s{\n' % self.rem
    if ret != 'void' :
      if ret in self.generatedEnums : ret = 'int'
      ret = self.checkTypedefEnums(ret, cl)
      if self.tools.isFundamentalT(ret) : pass
      elif self.tools.isPointerT(ret) : pass
      elif self.tools.isReferenceT(ret):   ret = 'const ' + ret
      else : ret = 'const ' + ret + '&'
      if self.tools.isFundamentalT(ret):
        s += '  static %s ret;\n' % ret
        s += '  ret = '
      elif self.tools.isReferenceT(ret):
        s += '  %s ret = ' % ret
      elif self.tools.isPointerT(ret):
        s += '  return (%s) ' % ret
      else :
        s += '  %s ret = new(' % ret
    else : s += '  '
    s += '((%s*)v)->%s(' % (cl, self.tools.lowerGetterName(name))
    indent = (len(cl) + len(name) + 9) * ' '
    i = 0
    if len(params) :
      for p in params :
        if i : s += '\n' + indent
        #if p in self.generatedEnums : p = 'int'
        p = self.checkTypedefEnums(p, cl)
        if p[:9] == 'SmartRef<' : s += '(%s*)' % p[p.find('<')+1:p.find('>')]
        elif self.tools.isReferenceT(p) : s += '*(%s*)' % p[:-1]
        else : s += '*(%s*)' % p
        s += 'argList[%d]' % i
        i += 1
    s += ');\n'
    if ret != 'void' :
      if self.tools.isFundamentalT(ret):
        s += '  return &ret;\n'
      elif self.tools.isReferenceT(ret):
        s += '  return (void*)&ret;\n'
      elif self.tools.isPointerT(ret):
        pass
      else :
        s[:-1] += ');\n  return (void*)ret;\n'
    s += '}\n\n'
    self.mNum += 1
    return s
#--------------------------------------------------------------------------------
  def genGetSetBitfieldMethodStubs(self, att, cl) :
    s = ''
    if att.has_key('bitfield'):
      for bf in att['bitfield']:
        bfAtt = bf['attrs']
        bfType = 'bool'
        if bfAtt.has_key('type')    : bfType = bfAtt['type']
        elif bfAtt['length'] != '1' : bfType = att['attrs']['type']
        if bfAtt['setMeth'] == 'TRUE':
          metName = 'set' + self.tools.firstUp(bfAtt['name'])
          s += self.genStubFunction('void',cl,metName,[bfType])
        if bfAtt['getMeth'] == 'TRUE':
          if bfType in self.generatedEnums : bfType = 'int'
          s += self.genStubFunction(bfType,cl,bfAtt['name'],'')
        if bfAtt['checkMeth'] == 'TRUE':
          metName = 'check' + self.tools.firstUp(bfAtt['name'])
          s += self.genStubFunction('bool',cl,metName,[bfType])
    return s
#--------------------------------------------------------------------------------
  def genGetSetMethodStubs(self,godClass):
    s = ''
    cl = godClass['attrs']['name']
    if godClass.has_key('attribute'):
      for att in godClass['attribute']:
        attAtt = att['attrs']
        attType = attAtt['type']
        if att.has_key('bitfield'):
          s += self.genGetSetBitfieldMethodStubs(att,godClass['attrs']['name'])
          attType = 'unsigned int'
        if attAtt['getMeth'] == 'TRUE' :
          ret = self.tools.genReturnFromStrg(attAtt['type'],[],'')
          s += self.genStubFunction(ret,cl,attAtt['name'],[])
        if attAtt['setMeth'] == 'TRUE':
          funName = 'set%s%s' % (attAtt['name'][0].upper(), attAtt['name'][1:])
          par = self.tools.genParamFromStrg(attAtt['type']) 
          s += self.genStubFunction('void',cl,funName,[par])
    if godClass.has_key('relation'):
      for rel in godClass['relation']:
        relAtt = rel['attrs']
        mult = 0
        if relAtt['multiplicity'] != '1' : mult = 1
        sType = 'SmartRef<%s>' % relAtt['type']
        mType = 'SmartRefVector<%s>' % relAtt['type']
        uName = '%s%s' % (relAtt['name'][0].upper(), relAtt['name'][1:])
        if relAtt['getMeth'] == 'TRUE':
          if mult : s += self.genStubFunction(self.tools.genReturnFromStrg(mType,[],''),cl,relAtt['name'],[])
          else    : s += self.genStubFunction(self.tools.genReturnFromStrg(relAtt['type']+'*',[],''),cl,relAtt['name'],[])
        if relAtt['setMeth'] == 'TRUE':
          if mult : s += self.genStubFunction('void',cl,'set'+uName,[self.tools.genParamFromStrg(mType)])
          else    : s += self.genStubFunction('void',cl,'set'+uName,[self.tools.genParamFromStrg(sType)])
        if mult :
          if relAtt['addMeth'] == 'TRUE':
            s += self.genStubFunction('void',cl,'addTo'+uName,[self.tools.genParamFromStrg(sType)])
          if relAtt['remMeth'] == 'TRUE':
            s += self.genStubFunction('void',cl,'removeFrom'+uName,[self.tools.genParamFromStrg(sType)])
          if relAtt['clrMeth'] == 'TRUE':
            s += self.genStubFunction('void',cl,'clear'+uName,[])
    return s
#--------------------------------------------------------------------------------
  def classIsVirtual(self,godClass):
    if godClass['attrs'].has_key('id') : return 1
    if not godClass.has_key('destructor') : return 1
    if godClass.has_key('base') :
      for base in godClass['base']:
        if base['attrs']['virtual'] == 'TRUE' : return 1
    if godClass.has_key('method') :
      for method in godClass['method']:
        if method['attrs']['virtual'] == 'TRUE' : return 1
    return 0
#--------------------------------------------------------------------------------
  def classIsAbstrct(self,godClass):
    if godClass.has_key('method'):
      for method in godClass['method']:
        if method['attrs']['virtual'] == 'PURE' : return 1
    return 0
#--------------------------------------------------------------------------------
  def genClassBuilder(self,godClass):
    clAtt = godClass['attrs']
    s = ''
    indent = '\n' + 20 * ' '
    s += '  ClassBuilder metaC("%s",' % clAtt['name']
    s += '%s "%s",' % (indent, clAtt['desc'])
    s += '%s typeid(%s),' % (indent, clAtt['name'])
    s += '%s sizeof(%s),' % (indent, clAtt['name'])
    s += '%s std::vector<const std::type_info*>(),' % indent
    s += '%s false,' % indent
    s += '%s NOCONTAINER,' % indent
    s += '%s 0,' % indent
    s += '%s PUBLIC' % indent
    if self.classIsAbstrct(godClass)  : s += ' | ABSTRACT'
    if self.classIsVirtual(godClass)  : s += ' | VIRTUAL'
    s += ');\n\n'
    return s
#--------------------------------------------------------------------------------
  def genClassID(self,godClass):
    s = ''
    if godClass['attrs'].has_key('id'):
      s += '  static char clid_txt[64];\n'
      s += '  sprintf(clid_txt,"%%08X-0000-0000-0000-000000000000",%s);\n' % godClass['attrs']['id']
      s += '  metaC.addProperty("ClassID", (char*)clid_txt);\n'
    return s
#--------------------------------------------------------------------------------
  def genProperties(self,godClass):
    s = '  metaC.addProperty("Author", "%s");\n' % godClass['attrs']['author']
    return s
#--------------------------------------------------------------------------------
  def genConstructors(self,godClass):
    s = ''
    if not self.classIsAbstract:
      clName = godClass['attrs']['name']
      indent = '\n' + 18 * ' '
      if godClass.has_key('constructor'):
        for const in godClass['constructor']:
          constAtt = const['attrs']
          par = []
          if constAtt.has_key('argList') : par = self.tools.genParamsFromStrg(constAtt['argList'])
          elif const.has_key('arg') : par = self.tools.genParamsFromElem(const['arg'])
          if ((not const['attrs'].has_key('argList')) and (not const.has_key('arg'))): self.hasDefaultConstructor = 1
          parList = ''
          if len(par) :
            par = [p.split('=')[0].strip() for p in par]
            for p in par :
              for e in self.generatedEnums :
                if p.find(e) != -1 : p = p.replace(e,'int')
            for parTyp in [(' ').join(x.split()[:-1]) for x in par]: parList += self.demangle(parTyp,self.clName) + '+";"+'
            parList = parList[:-5]
          mod = 'PUBLIC'
          if constAtt['explicit'] == 'TRUE' : mod = 'EXPLICIT'
          s += self.genMethod(clName, constAtt['desc'], clName, parList, '%s_constructor_%d'%(clName,self.cNum), mod) 
          self.cNum += 1
      if not self.hasDefaultConstructor :
        s += self.genMethod(clName,'default constructor', clName, '', '%s_constructor_0' % ( clName ), 'PUBLIC')
    return s
#--------------------------------------------------------------------------------
  def genDestructor(self,godClass):
    s = ''
    clName = godClass['attrs']['name']
    indent = '\n' + 18 * ' '
    if godClass.has_key('destructor'):
      dest = godClass['destructor'][0]
      destAtt = dest['attrs']
      s += self.genMethod('~'+clName,destAtt['desc'],'','',clName+'_destructor','PUBLIC')
    else :
      s += self.genMethod('~'+clName,'default destructor','','',clName+'_destructor','PUBLIC | VIRTUAL')
    return s
#--------------------------------------------------------------------------------
  def genBases(self,godClass):
    s = ''
    if godClass.has_key('base'):
      clName = godClass['attrs']['name']
      if not self.classIsAbstract : s += '  %s* cl = new %s();\n' % (clName, clName)
      for b in godClass['base']:
        bName = b ['attrs']['name']
        #if self.classIsAbstract : baseStub = '0'
        #else :
        baseStub = '%s_to_%s' % (clName, self.tools.cppEscape(bName))
        s += '  metaC.addSuperClass("%s", 0, %s);\n' % (bName, baseStub)
      if not self.classIsAbstract : s += '  delete cl;\n\n'
    return s
#--------------------------------------------------------------------------------
  def stripConst(self, name) :
    if name.split()[0] == 'const' : return ' '.join(name.split()[1:])
    return name
#--------------------------------------------------------------------------------
  def genMethod(self, name, desc, ret, par, fp, mod=''):
    indent = '\n' + 17 * ' '
    s  = '  metaC.addMethod("%s",' % name
    s += '%s "%s",' % (indent, desc)
    if ret and ret != 'void' : s += '%s (%s).c_str(),' % (indent, self.demangle(self.stripConst(ret),self.clName))
    if par : s += '%s (%s).c_str(),' % (indent, self.stripConst(par))
    s += '%s %s' % (indent, self.tools.cppEscape(fp))
    if mod : s += ',%s %s' % (indent, mod)
    s += ');\n\n'
    self.mNum += 1
    return s
#--------------------------------------------------------------------------------
  def genMethods(self,godClass):
    s = ''
    if godClass.has_key('method'):
      for met in godClass['method']:
        metAtt = met['attrs']
        if metAtt['access'] in ('PRIVATE', 'PROTECTED') : break
        if metAtt['friend'] == 'TRUE' : break
        if metAtt['virtual'] == 'PURE' : self.classIsAbstract = 1
        constF = ''
        if metAtt['const'] == 'TRUE' : constF = 'const'
        ret = ''
        if metAtt['type'].strip()  : ret = self.tools.genReturnFromStrg(metAtt['type'],[],'')
        elif met.has_key('return') : ret = self.tools.genReturnFromElem(met['return'],[],'')
        if ret in self.generatedEnums : ret = 'int'
        par = []
        if metAtt.has_key('argList') : par = self.tools.genParamsFromStrg(metAtt['argList'])
        elif met.has_key('arg') : par = self.tools.genParamsFromElem(met['arg'])
        parList = ''
        if len(par) :
          for p in par :
            for e in self.generatedEnums :
              if p.find(e) != -1 : p = p.replace(e,'int')
          for parTyp in [(' ').join(x.split()[:-1]) for x in par]: parList += self.demangle(parTyp,self.clName) + '+";"+'
          parList = parList[:-5]
        mod = [metAtt['access']]
        if metAtt['const']   == 'TRUE' : mod.append('CONST')
        if metAtt['virtual'] == 'TRUE' : mod.append('VIRTUAL')
        if metAtt['static']  == 'TRUE' : mod.append('STATIC')
        if metAtt['inline']  == 'TRUE' : mod.append('INLINE')
        if metAtt['friend']  == 'TRUE' : mod.append('FRIEND')
        s += self.genMethod(metAtt['name'],
                            metAtt['desc'],
                            ret,
                            parList,
                            '%s_%s_%d%s'%(godClass['attrs']['name'],metAtt['name'],self.mNum,constF),
                            ' | '.join(mod))
    return s
#--------------------------------------------------------------------------------
  def genGetSetBitfieldMethods(self, att, clName) :
    s = ''
    if att.has_key('bitfield'):
      for bf in att['bitfield']:
        bfAtt = bf['attrs']
        bfType = 'bool'
        if bfAtt.has_key('type'): bfType = bfAtt['type']
        if bfType in self.generatedEnums : bfType = 'int'
        if bfAtt['setMeth'] == 'TRUE':
          metName = 'set'+self.tools.firstUp(bfAtt['name'])
          s += self.genMethod(metName,'set '+bfAtt['desc'],'void',self.demangle(bfType,clName),'%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
        if bfAtt['getMeth'] == 'TRUE':
          s += self.genMethod(bfAtt['name'],'get '+bfAtt['desc'],bfType,'','%s_%s_%d'%(clName,bfAtt['name'],self.mNum),'PUBLIC')
        if bfAtt['checkMeth'] == 'TRUE':
          metName = 'check'+self.tools.firstUp(bfAtt['name'])
          s += self.genMethod(metName,'check '+bfAtt['desc'],'bool',self.demangle(bfType,self.clName),'%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
    return s
#--------------------------------------------------------------------------------
  def genGetSetMethods(self,godClass):
    s = ''
    clName = godClass['attrs']['name']
    if godClass.has_key('attribute'):
      for att in godClass['attribute']:
        attAtt = att['attrs']
        attType = attAtt['type']
        if attType in self.generatedEnums : attType = 'int'
        if att.has_key('bitfield') :
          s += self.genGetSetBitfieldMethods(att, clName)
          attType = 'unsigned int'
        if attAtt['getMeth'] == 'TRUE' :
          ret = self.tools.genReturnFromStrg(attType,[],'')
          if not ( self.tools.isReferenceT(ret) or self.tools.isPointerT(ret) or self.tools.isFundamentalT(ret) ) : ret += "&"
          s += self.genMethod(attAtt['name'], attAtt['desc'], ret, '', '%s_%s_%d'%(clName,attAtt['name'],self.mNum), 'PUBLIC')
        if attAtt['setMeth'] == 'TRUE' :
          metName = 'set'+self.tools.firstUp(attAtt['name'])
          param = self.tools.genParamFromStrg(attType)
          if not ( self.tools.isReferenceT(param) or self.tools.isPointerT(param) or self.tools.isFundamentalT(param) ) : param += "&"
          s += self.genMethod(metName, attAtt['desc'], '', self.demangle(param,self.clName), '%s_%s_%d'%(clName,metName,self.mNum), 'PUBLIC')
    if godClass.has_key('relation'):
      for rel in godClass['relation']:
        relAtt = rel['attrs']
        mult = 0
        if relAtt['multiplicity'] != '1' : mult = 1
        metNameUp = self.tools.firstUp(relAtt['name'])
        if relAtt['getMeth'] == 'TRUE' :
          ret = ''
          if mult : ret = self.tools.genReturnFromStrg('SmartRefVector<%s>&'%relAtt['type'],[],'')
          else    : ret = self.tools.genReturnFromStrg('%s*'%relAtt['type'],[],'')
          s += self.genMethod(relAtt['name'], relAtt['desc'], ret, '', '%s_%s_%d'%(clName,relAtt['name'],self.mNum),'PUBLIC')
        if relAtt['setMeth'] == 'TRUE' :
          metName = 'set'+metNameUp
          par = ''
          if mult : par = self.tools.genParamFromStrg('SmartRefVector<%s>'%relAtt['type'])
          else    : par = self.tools.genParamFromStrg('%s*'%relAtt['type'])
          s += self.genMethod(metName, relAtt['desc'], '', self.demangle(par,self.clName), '%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
        if mult:
          if relAtt['addMeth'] == 'TRUE' :
            metName = 'addTo'+metNameUp
            par = self.tools.genParamFromStrg('%s*'%relAtt['type'])
            s += self.genMethod(metName,relAtt['desc'],'',self.demangle(par,self.clName),'%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
          if relAtt['remMeth'] == 'TRUE' :
            metName = 'removeFrom'+metNameUp
            par = self.tools.genParamFromStrg('%s*'%relAtt['type'])
            s += self.genMethod(metName,relAtt['desc'],'',self.demangle(par,self.clName),'%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
          if relAtt['clrMeth'] == 'TRUE' :
            metName = 'clear'+metNameUp
            s += self.genMethod(metName,relAtt['desc'],'','','%s_%s_%d'%(clName,metName,self.mNum),'PUBLIC')
    return s
#--------------------------------------------------------------------------------
  def demangle(self, name, clname):
    ref = 0
    name = self.checkTypedefEnums(name.strip(), clname)
    if self.tools.isReferenceT(name) :
      ref = 1
      name = name[:-1]
    uqName = self.tools.unQualifyT(name)
    if self.tools.isFundamentalT(uqName) or uqName.strip()[:5] == 'std::' :
      name = name.replace('std::string','std::basic_string<char> ')
      if ref : return 'std::string("'+name + '&")'
      else   : return 'std::string("'+name+'")'
    else :
      if ref : return 'System::typeinfoName(typeid(%s))+std::string("&")' % name
      else   : return 'System::typeinfoName(typeid(%s))' % name
#--------------------------------------------------------------------------------
  def genFields(self,godClass):
    s = ''
    indent = '\n' + 16 * ' '
    clName = godClass['attrs']['name']
    if godClass.has_key('attribute'):
      for att in godClass['attribute']:
        attAtt = att['attrs']
        attType = attAtt['type'].replace('std::string','std::basic_string<char> ')
        if attType[-1] == ' ' : attType = attType[:-1]
        if attType in self.generatedEnums : attType = 'int'
        if attAtt.has_key('dictalias') : attType = '"%s"' % attAtt['dictalias']
        else                           : attType = '(%s).c_str()' % self.demangle(attType,self.clName)
        mod = attAtt['access']
        if attAtt['transient'] == 'TRUE' : mod += ' | TRANSIENT'
        s += '  metaC.addField("%s",' % attAtt['name']
        s += '%s %s,' % (indent, attType)
        s += '%s "%s",' % (indent, attAtt['desc'])
        s += '%s OffsetOf(%s, m_%s),' % (indent, clName, attAtt['name'])
        s += '%s %s);\n\n' % (indent, mod)
    if godClass.has_key('relation'):
      for rel in godClass['relation']:
        relAtt = rel['attrs']
        type = ''
        if relAtt['multiplicity'] == '1' : type = 'SmartRef<%s>'%relAtt['type']
        else                             : type = 'SmartRefVector<%s>'%relAtt['type']
        s += '  metaC.addField("%s",' % relAtt['name']
        s += '%s "%s",' % (indent, type)
        s += '%s "%s",' % (indent, relAtt['desc'])
        s += '%s OffsetOf(%s, m_%s),' % (indent, clName, relAtt['name'])
        s += '%s %s);\n\n' % (indent, relAtt['access'])                                                                               
    return s
#--------------------------------------------------------------------------------
  def genInitialisations(self,godClass):
    s = ''
    if godClass.has_key('template') :
      for tmpl in godClass['template']:
        dictIncl = 'GaudiKernel/KeyedDictionary'
        if dictIncl not in self.include : self.include.append(dictIncl)
        tmplAtt = tmpl['attrs']
        tmplKey = ''
        for t in ['t1','t2','t3','t4'] :
          if tmplAtt.has_key(t):
            tmplKey += tmplAtt[t].replace('THIS',godClass['attrs']['name']) + ', '
        tmplKey = tmplKey[:-2]
        s += '      GaudiDict::%s<%s>();\n' % (tmplAtt['name'],tmplKey)
    return s[:-1]
#--------------------------------------------------------------------------------
  def doit(self,package,godClasses,outputDir,lname):

    for godClass in godClasses:

      self.reset(godClass)
      self.generateEnums(godClass)

      classDict = package.dict
      classname = godClass['attrs']['name']
      self.clName = classname

      fileName = '%s_dict.cpp' % classname

      print '    File %s' % fileName.ljust(lname),

      classDict['classname']         = classname
      classDict['classBuilder']      = self.genClassBuilder(godClass)
      classDict['classID']           = self.genClassID(godClass)
      classDict['properties']        = self.genProperties(godClass)
      classDict['methods']           = self.genMethods(godClass)
      classDict['getSetMethods']     = self.genGetSetMethods(godClass)
      self.mNum = 0
      classDict['methodStubs']       = self.genMethodStubs(godClass)
      classDict['getSetMethodStubs'] = self.genGetSetMethodStubs(godClass)
      classDict['constructors']      = self.genConstructors(godClass)
      self.cNum = 1
      classDict['constructorStubs']  = self.genConstructorStubs(godClass)
      classDict['destructor']        = self.genDestructor(godClass)
      classDict['destructorStub']    = self.genDestructorStub(classname)
      classDict['baseStubs']         = self.genBaseStubs(godClass)
      classDict['bases']             = self.genBases(godClass)
      classDict['fields']            = self.genFields(godClass)
      classDict['initialisations']   = self.genInitialisations(godClass)
      classDict['includes']          = self.genIncludes()

      g = gparser.gparser()
      g.parse(self.godRoot+'templates/dict.tpl',classDict)

      file = open(outputDir+os.sep+fileName,'w')
      file.write(g.stream)
      file.close()

      print ' - Done'

#================================================================================
#import xparser
#if __name__ == '__main__':
#  x = xparser.xparser('xml_files/Event.xml','xml_files/GODsClassDB.xml')
#  x.parse()
#  g1 = genClasses.genClasses(x.gdd,x.cdb)
#  g1.doit(1)
#  g2 = genClassDicts(x.gdd)
#  g2.doit()
