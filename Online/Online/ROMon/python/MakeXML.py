import os, sys, socket
HLT = 1
STORAGE = 2
MONITORING = 3
RECONSTRUCTION = 5
RECCTRL = 6
TOP_LOGGER = 4

db_dir = '..'+os.sep+'xml'+os.sep

def getNodes(host):
  lines = os.popen('ssh -q '+host+' "tmLs | grep \"Node:\" | sort" | grep Node')
  return [l[:-1].replace('"','').replace('.','').replace('  ',' ').split(' ')[3] for l in lines]

def nodeType(typ):
  if typ == HLT:
    return ('HLTControl','HLTWorker')
  elif typ == STORAGE:
    return ('StorageControl','StorageWorker')
  elif typ == MONITORING:
    return ('MonitoringControl','MonitoringWorker')
  elif typ == RECCTRL:
    return ('MonitoringControlTest','MonitoringWorker')
  elif typ == RECONSTRUCTION:
    return ('ReconstructionControl','ReconstructionWorker')
  return ('','')

def create(typ,controlsPC):
  ctr = controlsPC.upper()
  nodes = getNodes(ctr)
  ctr_type,wrk_type = nodeType(typ)
  txt = '<TaskInventory>\n  <NodeList name="%s">\n   <Name>%s</Name>\n'%(ctr,ctr,)
  for i in nodes:
    if i.upper()==ctr:
      txt = txt + '   <Node name="%s"   type="%s"/>\n'%(i,ctr_type)
    else:
      txt = txt + '   <Node name="%s"   type="%s"/>\n'%(i,wrk_type)
  txt = txt + '  </NodeList>\n</TaskInventory>\n'
  file = open(db_dir+ctr+'.xml','w')
  print >>file, txt
  file.close()
  print '.........> Wrote XML configuration file for sub-cluster:',ctr

def createHLT():
  rows = "a'b'c'd'e"
  columns = "1'2'3'4'6'7'8'9'10'11"
  for row in rows.split("'"):
    for column in columns.split("'"):
      create(HLT,'hlt%s%02d'%(row,int(column)))
if __name__ == "__main__":
  create(STORAGE,'storectl01')
  create(MONITORING,'mona08')
  create(RECONSTRUCTION,'mona09')
  create(RECCTRL,'mona07')
  createHLT()
  print '.........> All Done.'
