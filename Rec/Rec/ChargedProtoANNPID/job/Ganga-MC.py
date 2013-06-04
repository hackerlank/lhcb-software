
sim="Sim08"
digi="Digi13"
reco="Reco14"
strp="Stripping20"

refs = { "Pythia6MagUp"   : [],
         "Pythia6MagDown" : [],
         "Pythia8MagUp"   : [],
         "Pythia8MagDown" : [] }
         
for pythia in ["Pythia6","Pythia8"] :
    for polarity in ["MagUp","MagDown"] : 

        datapath = "/MC/2012/Beam4000GeV-2012-"+polarity+"-Nu2.5-"+pythia+"/"+sim+"/"+digi+"/Trig0x409f0045/"+reco+"/"+strp+"NoPrescalingFlagged/10000000/ALLSTREAMS.DST"
        
        datalfns = BKQuery(path=datapath).getDataset()
        print "Extracted", len(datalfns), "LFNS for", datapath

        key = pythia+polarity
        if key in refs.keys() :
            if len(refs[key]) > 0 :
                for id in refs[key]:
                    datalfns = datalfns.difference( jobs(id).inputdata )
                print " -> Reduced to", len(datalfns), "new data files"

        if len(datalfns) > 0 :

            j = Job( application = DaVinci( version = 'v33r2' ) )
            j.name = "MC2012-ANNPID-"+polarity+"-"+pythia+sim+digi+reco+strp

            # Main options
            j.application.optsfile = [ File('options-MC.py') ]
            
            j.splitter = SplitByFiles ( filesPerJob = 1, maxFiles = 999999 )
            
            rootfiles = [ SandboxFile('ProtoPIDANN.MC.tuples.root') ]
            
            j.outputfiles = rootfiles
            
            j.inputdata = datalfns

            j.do_auto_resubmit = True
            
            j.backend = Dirac()
            
            #print "Submitting job", j.name
            j.submit()
            #j.remove()
