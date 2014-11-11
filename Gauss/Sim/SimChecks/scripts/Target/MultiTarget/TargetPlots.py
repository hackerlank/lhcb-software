from string import find
from ROOT import *
from TargetSummary import *
from array import array
from copy import *
import os

def ScalarProd(ncomp, v, c) :

	res = []
	for i in v :
		res.append(i*c)

	return res


def Plot( dataTree, xvar, finalPlot, outputPath, models = [], pguns = [], materials = [], E0 = -1 , Dx = -1, plotData = False) :
	
	dict = Plotter()

	leg = TLegend(0.10,0.1,0.9,0.9)
	leg.SetTextSize(0.05);

	var = ""
	if(find(finalPlot,"MULTI") > -1) :
		var = "multi"
	elif(find(finalPlot,"TOTAL") > -1) :
		var = "xsec"
	elif(find(finalPlot,"INEL")> -1) :
		var = "inel_xsec"
	elif(find(finalPlot,"EL")> -1) :
		var = "el_xsec"
	elif(find(finalPlot,"MULTI_NCH")> -1) :
		var = "multiNCh"
	elif(find(finalPlot,"MULTI_NCH_NOGAMMA")> -1) :
		var = "multiNCh_nogamma"
	elif(find(finalPlot,"PERC_NCH")> -1) :
		var = "percNCh"
	elif(find(finalPlot,"PERC_MINUS")> -1) :
		var = "percMinus"
	elif(find(finalPlot,"PERC_PLUS")> -1) :
		var = "percPlus"
	elif(find(finalPlot,"MULTI_GAMMA")> -1) :
		var = "multi_gamma"

	if(find(finalPlot,"RATIO") > -1 or find(finalPlot,"ASYM") > -1) :

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		titleMultigr = finalPlot
		if(finalPlot == "RATIO_TOTAL") :
			titleMultigr = "Ratio of total (inel + el) cross sections" + mystr
		if(finalPlot == "ASYM_TOTAL") :
			titleMultigr = "Asymmetry of total (inel + el) cross sections" + mystr
		if(finalPlot == "RATIO_INEL") :
			titleMultigr = "Ratio of inelastic cross sections" + mystr
		if(finalPlot == "ASYM_INEL") :
			titleMultigr = "Asymmetry of inelastic  cross sections" + mystr

		pdgenergies  = [ 1, 5, 10, 50, 100 ]
		pdgRatios_p  = [ 1./1.67, 1./1.42, 1./1.31, 1./1.14, 1./1.10 ]
		pdgRatios_pi = [ 1./1.22, 1./1.13, 1./1.10, 1./1.05, 1./1.03 ]
		pdgRatios_K  = [ 1./1.61, 1./1.32, 1./1.23, 1./1.10, 1./1.07 ]

		if(find(finalPlot,"RATIO") > -1) :
			ratiotxt = open(outputPath+"/ratio_inAl.txt","w")
		else :
			ratiotxt = open(outputPath+"/asym_inAl.txt","w")

		
		grs = []
		
		for pg in range(0, len(pguns)-1, 2) :
		
			ratiotxt.write( "\\multicolumn{2}{c}{ratio " + str(dict._all_pguns[pguns[pg+1]].GetLatex("$")) + "/" + str(dict._all_pguns[pguns[pg]].GetLatex("$")) + "} \\\\ \\hline \n" )
			nm = 0
			for m in models :
			
				ratiotxt.write( "\\multicolumn{2}{c}{ratio " + m + "} \\\\ \\hline" )

				varexp = "h_"+str(pg)+m
				select = "model == " + str(ord(m[0])) + " && material == " + str(ord(materials[0][0])) + " && pGun == " + str(dict._all_pguns[pguns[pg]].GetPDG())
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list = gDirectory.Get(varexp)
				entries = list.GetN()
				dataTree.SetEntryList(list)
				dataTree.Draw(xvar+":"+var+":"+var+"_err")

				errx = array( 'd' , [0.] * entries )
				ty1 = dataTree.GetV2()
				terry1 = dataTree.GetV3()
				y1=[]
				erry1=[]
				for i in range(0,entries) :
					y1.append(ty1[i])
					erry1.append(terry1[i])
									
				varexp = "h_"+str(pg+1)+m
				select = "model == " + str(ord(m[0])) + " && material == " + str(ord(materials[0][0])) + " && pGun == " + str(dict._all_pguns[pguns[pg+1]].GetPDG())
				if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
				elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

				dataTree.SetEntryList(0)
				dataTree.Draw(">>"+varexp,select, "entrylist")
				list2 = gDirectory.Get(varexp)
				dataTree.SetEntryList(list2)
				dataTree.Draw(xvar+":"+var+":"+var+"_err")

				tx = dataTree.GetV1() 
				ty2 = dataTree.GetV2() 
				terry2 = dataTree.GetV3() 
				x = []
				y2=[]
				erry2=[]
				for i in range(0,entries) :
					y2.append(ty2[i])
					erry2.append(terry2[i])
					x.append(tx[i])

				y = []
				erry = []
				for ee in range(0,entries) : 

					ratiotxt.write( str(x[ee]) )
					if(find(finalPlot,"RATIO")>-1) :
						y.append(y2[ee] / y1[ee])
					else :
						y.append(100*TMath.Abs(y1[ee] - y2[ee])/(2. - y1[ee] - y2[ee]))

					totErr2 = TMath.Power(erry1[ee]/y1[ee],2) + TMath.Power(erry2[ee]/y2[ee],2)
					erry.append(y[ee] * TMath.Sqrt(totErr2))

					ratiotxt.write(' & $ {:4.2} \\pm {:4.2} $ \\\\ \n'.format(y[ee],erry[ee]) )

				gr = TGraphErrors(entries,array('d',x),array('d',y),array('d',errx),array('d',erry))
				gr.SetMarkerColor(1+pg/2)
				gr.SetMarkerStyle(20+nm)

				label = dict._all_pguns[pguns[pg+1]].GetLatex("LEG") + " / " + dict._all_pguns[pguns[pg]].GetLatex("LEG")
				if(len(models) > 1) :
					label += " (" + m + ")"
				leg.AddEntry(gr,label,"P")

				if(plotData and (find(finalPlot,"TOTAL")>-1 or find(finalPlot,"INEL")>-1) and find(finalPlot,"RATIO")>-1 ) :

					grPDG = 0
					if(dict._all_pguns[pguns[pg]].GetName()=="p" and dict._all_pguns[pguns[pg+1]].GetName()=="pbar") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_p))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Piplus" and dict._all_pguns[pguns[pg+1]].GetName()=="Piminus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_pi))
					elif(dict._all_pguns[pguns[pg]].GetName()=="Kplus" and dict._all_pguns[pguns[pg+1]].GetName()=="Kminus") :
						grPDG = TGraphErrors(5,array('d',pdgenergies),array('d',pdgRatios_K))

					if grPDG :
						if(len(pguns)>2) :
							grPDG.SetMarkerColor(1+pg/2)
						else :
							grPDG.SetMarkerColor(4)
						grPDG.SetMarkerStyle(34)
						grPDG.SetMarkerSize(1.2)
						if nm == len(models)-1 :
							grs.append(grPDG)
							leg.AddEntry(grPDG,dict._all_pguns[pguns[pg+1]].GetLatex("LEG") + " / " + dict._all_pguns[pguns[pg]].GetLatex("LEG") +" PDG","P")
			
				nm+=1
				grs.append(gr)

			#ratiotxt << "\\hline" << endl
		
		#ratiotxt << "\\end{tabular}" << endl

		c = TCanvas()
		leg_pad = TPad("leg_pad","",0.73,0,1.,1.)
		gr_pad = TPad("gr_pad","",0.03,0,0.8,1.)
		gr_pad.cd()


		gr_pad.SetLogx()
		gr_pad.SetGrid()

		for gg in grs :
			
			gg.GetYaxis().SetTitleOffset(1.5)
			gg.SetTitle(titleMultigr)
			if(finalPlot.find("RATIO") > -1) :
				gg.GetYaxis().SetTitle("Ratio")
				gg.GetYaxis().SetRangeUser(0,2.)
			else :
				gg.GetYaxis().SetTitle("Asym (%)")
	
			if(xvar=="energy") :
				gg.GetXaxis().SetTitle("|p| (GeV)")
				gg.GetXaxis().SetRangeUser(0.5,300)
			elif(xvar=="thickness") :
				gg.GetXaxis().SetTitle("#Delta x (mm)")
	
			if(gg==grs[0]) :
				gg.Draw("APL")
			else :
				gg.Draw("PL SAME")

		leg_pad.cd()
		leg.Draw()

		c.cd()
		gr_pad.Draw()
		leg_pad.Draw()
		c.Print(outputPath + "/" + finalPlot + mystr.replace(" ","_") +".pdf")	
		c.Clear()
	
	else :
	
		c = TCanvas()
		leg_pad = TPad("leg_pad","",0.73,0,1.,1.)
		gr_pad = TPad("gr_pad","",0.03,0,0.8,1.)
		gr_pad.cd()

		mystr = " in " +str(Dx) + " mm"
		if(xvar=="thickness") :
			mystr = " for " + str(E0) + " GeV"

		nameMultigr = "mgr" + mystr
		titleMultigr = finalPlot
		if(finalPlot == "TOTAL") :
			titleMultigr = "Total (inel + el) probability of interaction" + mystr
		elif(finalPlot == "INEL") :
			titleMultigr = "Inelastic probability of interaction" + mystr
		elif(finalPlot == "EL") :
			titleMultigr = "Elastic probability of interaction" + mystr 
		elif(finalPlot == "PERC_PLUS") :
			titleMultigr = "Percentage of positive particles"  + mystr
		elif(finalPlot == "PERC_MINUS") :
			titleMultigr = "Percentage of negative particles"  + mystr
		elif(finalPlot == "MULTI") :
			titleMultigr = "Multiplicity of secondaries"  + mystr
		elif(finalPlot == "MULTI_NCH") :
			titleMultigr = "Multiplicity (neutral)"  + mystr
		elif(finalPlot == "MULTI_NCH_NOGAMMA") :
			titleMultigr = "Multiplicity (neutral minus gamma)"  + mystr
		elif(finalPlot == "PERC_NCH") :
			titleMultigr = "Percentage of neutral particles"  + mystr
		elif(finalPlot == "MULTI_NOGAMMA") :
			titleMultigr = "Multiplicity excluding gammas" + mystr

		grs = []

		PintOverSigmaFactor = Dx /(1000. * dict._all_materials[materials[0]].GetSigmaDxOverPintFactor() * 1000.)	

			
		#COMPAS Inelastic Xsec data in Al
		COMPAS_p_x           = [ 1.52, 5., 9., 20., 30., 60. ]
		COMPAS_p_sigmaErr    = [ 10., 4., 4., 5., 5., 7. ]
		COMPAS_p_sigma       = [ 445., 445., 465., 446., 445., 455. ]
		COMPAS_p_y           = ScalarProd(6,COMPAS_p_sigma,PintOverSigmaFactor)
		COMPAS_p_yErr        = ScalarProd(6,COMPAS_p_sigmaErr,PintOverSigmaFactor)
		COMPAS_pbar_x        = [ 1.45,6.65,13.3,25.,30.,60. ]
		COMPAS_pbar_sigma    = [ 617., 558., 536., 480., 457., 439. ]
		COMPAS_pbar_sigmaErr = [ 17., 10., 10., 9., 11., 13. ]
		COMPAS_pbar_y        = ScalarProd(6,COMPAS_pbar_sigma,PintOverSigmaFactor)
		COMPAS_pbar_yErr     = ScalarProd(6,COMPAS_pbar_sigmaErr,PintOverSigmaFactor)

		#COMPAS Inelastic Xsec data in Be

		COMPAS_inBe_p_x           = [ 3., 5., 9., 30.,50.,60. ]
		COMPAS_inBe_p_sigmaErr    = [ 4., 3., 3., 3., 3., 2. ]
		COMPAS_inBe_p_sigma       = [ 236., 207., 210., 210., 210., 216. ]
		COMPAS_inBe_p_y           = ScalarProd(6,COMPAS_inBe_p_sigma,PintOverSigmaFactor)
		COMPAS_inBe_p_yErr        = ScalarProd(6,COMPAS_inBe_p_sigmaErr,PintOverSigmaFactor)
		COMPAS_inBe_pbar_x        = [ 6.65,13.3,20.,30.,40. ]
		COMPAS_inBe_pbar_sigma    = [ 296., 275., 240., 235., 226., 190. ]
		COMPAS_inBe_pbar_sigmaErr = [ 6., 4., 10., 6., 7. ]
		COMPAS_inBe_pbar_y        = ScalarProd(5,COMPAS_inBe_pbar_sigma,PintOverSigmaFactor)
		COMPAS_inBe_pbar_yErr     = ScalarProd(5,COMPAS_inBe_pbar_sigmaErr,PintOverSigmaFactor)

		#COMPAS Total Xsec data in Al

		COMPASTot_p_x               = [ 1.52, 1.8, 19.3, 20. ]  
		COMPASTot_p_sigmaErr        = [ 22., 27., 10., 10. ] 
		COMPASTot_p_sigma           = [ 687., 694., 687., 687. ] 
		COMPASTot_p_y               = ScalarProd(4,COMPASTot_p_sigma,PintOverSigmaFactor) 
		COMPASTot_p_yErr            = ScalarProd(4,COMPASTot_p_sigmaErr,PintOverSigmaFactor) 
		COMPASTot_pbar_x            = [ 1.45,1.8 ] 
		COMPASTot_pbar_sigma        = [ 1034., 1066. ] 
		COMPASTot_pbar_sigmaErr     = [ 40., 40. ] 
		COMPASTot_pbar_y            = ScalarProd(2,COMPASTot_pbar_sigma,PintOverSigmaFactor) 
		COMPASTot_pbar_yErr         = ScalarProd(2,COMPASTot_pbar_sigmaErr,PintOverSigmaFactor) 

		COMPAS_p_gr = TGraphErrors(6,array('d', COMPAS_p_x ), array('d', COMPAS_p_y ), array('d', [0.]*6 ), array('d', COMPAS_p_yErr))
		COMPAS_pbar_gr = TGraphErrors(6, array('d', COMPAS_pbar_x), array('d', COMPAS_pbar_y), array ('d', [0.]*6 ), array('d', COMPAS_pbar_yErr) )
		COMPAS_p_gr.SetMarkerColor(4)
		COMPAS_p_gr.SetMarkerStyle(29)
		COMPAS_p_gr.SetMarkerSize(1.1)
		COMPAS_pbar_gr.SetMarkerColor(4)
		COMPAS_pbar_gr.SetMarkerStyle(30)
		COMPAS_pbar_gr.SetMarkerSize(1.2)

		COMPASTot_p_gr = TGraphErrors(4, array('d', COMPASTot_p_x), array('d', COMPASTot_p_y), array('d', [0.]*4 ), array('d', COMPASTot_p_yErr))
		COMPASTot_pbar_gr = TGraphErrors(2, array('d', COMPASTot_pbar_x), array('d', COMPASTot_pbar_y), array('d', [0.]*2 ), array('d', COMPASTot_pbar_yErr))
		COMPASTot_p_gr.SetMarkerColor(4)
		COMPASTot_p_gr.SetMarkerStyle(29)
		COMPASTot_p_gr.SetMarkerSize(1.1)
		COMPASTot_pbar_gr.SetMarkerColor(4)
		COMPASTot_pbar_gr.SetMarkerStyle(30)
		COMPASTot_pbar_gr.SetMarkerSize(1.2)

		COMPAS_inBe_p_gr = TGraphErrors(6, array('d', COMPAS_inBe_p_x), array('d', COMPAS_inBe_p_y), array('d', [0.]*6 ), array('d', COMPAS_inBe_p_yErr))
		COMPAS_inBe_pbar_gr = TGraphErrors(5, array('d', COMPAS_inBe_pbar_x), array('d', COMPAS_inBe_pbar_y), array('d', [0.]*5 ), array('d', COMPAS_inBe_pbar_yErr))
		COMPAS_inBe_p_gr.SetMarkerColor(4)
		COMPAS_inBe_p_gr.SetMarkerStyle(29)
		COMPAS_inBe_p_gr.SetMarkerSize(1.1)
		COMPAS_inBe_pbar_gr.SetMarkerColor(4)
		COMPAS_inBe_pbar_gr.SetMarkerStyle(30)
		COMPAS_inBe_pbar_gr.SetMarkerSize(1.2)
		

		#Plotting Gauss values
		n0 = 0
		nh = 0
		for model in models : 
			for material in materials :

				n2 = 0

				for pg in pguns : 
				
					varexp = "h_"+str(nh)
					nh+=1
				
					select = "model == " + str(ord(model[0])) + " && material == " + str(ord(material[0])) + " && pGun == " + str(dict._all_pguns[pg].GetPDG())
					if(xvar=="energy") : 
						select += " && thickness == " + str(Dx)
					elif(xvar=="thickness") :
						select += " && energy == " + str(E0)

					dataTree.SetEntryList(0)
					dataTree.Draw(">>"+varexp,select, "entrylist")
					list = gDirectory.Get(varexp)
					entries = list.GetN()
					dataTree.SetEntryList(list)

					dataTree.SetEstimate(entries)
					if(finalPlot == "MULTI" or finalPlot == "TOTAL" or finalPlot == "INEL" or finalPlot == "EL") :
						dataTree.Draw(xvar+":"+var+":"+var+"_err","","colz")	
					else :
						dataTree.Draw("energy:"+var)

					errx = array( 'd' , [0.] * entries )	
					gr = TGraphErrors(entries,dataTree.GetV1(),dataTree.GetV2(),errx,dataTree.GetV3())

					gr.SetMarkerColor(1+n2)
					if(n0==0) :
						gr.SetMarkerStyle(20+n0)
					else :
						gr.SetMarkerStyle(24+n0)

					if(n0==0) :
						gr.SetMarkerSize(1.1)
					n2+=1

					label = dict._all_pguns[pg].GetLatex("LEG") + " in " + material
					if(len(models) > 1) :
						label += " (" + model + ")"
					leg.AddEntry(gr,label,"P")
					
					grs.append(gr)

					if plotData and n0 == len(models)-1 and materials[0] == "Al" :
						if find(finalPlot,"TOTAL")>-1 :
							if dict._all_pguns[pg].GetName()=="p":
								grs.append(COMPASTot_p_gr)
								leg.AddEntry(COMPASTot_p_gr,"COMPAS p total in Al","P")
							elif dict._all_pguns[pg].GetName()=="pbar":
								grs.append(COMPASTot_pbar_gr)
								leg.AddEntry(COMPASTot_pbar_gr,"COMPAS #bar{p} total in Al","P")
						elif find(finalPlot,"INEL")>-1 :
							if dict._all_pguns[pg].GetName()=="p":
								grs.append(COMPAS_p_gr)
								leg.AddEntry(COMPAS_p_gr,"COMPAS p inel in Al","P")
							elif dict._all_pguns[pg].GetName()=="pbar":
								grs.append(COMPAS_pbar_gr)
								leg.AddEntry(COMPAS_pbar_gr,"COMPAS #bar{p} inel in Al","P")
					elif plotData and n0 == len(models)-1 and materials[0] == "Be" and find(finalPlot,"INEL")>-1 :
						if dict._all_pguns[pg].GetName()=="p":
							grs.append(COMPAS_inBe_p_gr)
							leg.AddEntry(COMPAS_inBe_p_gr,"COMPAS p inel in Be","P")
						elif dict._all_pguns[pg].GetName()=="pbar":
							grs.append(COMPAS_inBe_pbar_gr)
							leg.AddEntry(COMPAS_inBe_pbar_gr,"COMPAS #bar{p} inel in Be","P")

			
							
			n0+=1


		gStyle.SetOptStat(0)
		gr_pad.SetGrid()
		if(xvar=="energy") :
			gr_pad.SetLogx()
		gr_pad.cd()

		ngg = 0
		for gg in grs :

			if(xvar=="energy") :
				gg.GetXaxis().SetTitle("|p| (GeV)")
			elif (xvar=="thickness") :
				gg.GetXaxis().SetTitle("#Delta x (mm)")
				gg.GetYaxis().SetTitleOffset(1.2)

			if(ngg==0) :
				gg.Draw("APL")	
				ngg+=1
			else :
				gg.Draw("PL SAME")	

			gr_pad.Update()

			if(finalPlot == "TOTAL") :
				gg.GetYaxis().SetTitle("P^{tot}_{int} = N^{inel+el}/N^{gen}")
				gg.GetYaxis().SetRangeUser(0.002,0.007)
			elif(finalPlot == "INEL") :
				gg.GetYaxis().SetRangeUser(0.001,0.007)
				gg.GetYaxis().SetTitle("P^{inel}_{int} = N^{inel}/N^{gen}")
			elif(finalPlot == "EL") :
				gg.GetYaxis().SetTitle("P^{el}_{int} = N^{el}/N^{gen}")
				gg.GetYaxis().SetRangeUser(0.0003,0.0025)
			elif(find(finalPlot,"MULTI") > -1) :
				gg.GetYaxis().SetTitle("< Multi >")
				gg.GetYaxis().SetRangeUser(0.,100.)
			elif(find(finalPlot,"PERC") > -1) :
				gg.GetYaxis().SetTitle("%")
				gg.GetYaxis().SetRangeUser(0.,1.)
	
			gg.SetTitle(titleMultigr)
			gg.GetYaxis().SetTitleOffset(1.5)
		
			gr_pad.Update()
		leg_pad.cd()
		leg.Draw()

		printname = outputPath+"/" + finalPlot + mystr.replace(" ","_") + ".pdf"
		c.cd()
		gr_pad.Draw()
		leg_pad.Draw()
		c.Print(printname)
		c.Clear()




if __name__ == "__main__" :

	###### Possible types of plots are :
	##          INEL:                                   inelastic cross section
	##          EL:                                     elastic cross section
	##          TOTAL:                                  total cross section
	##          MULTI:                                  total multiplicity 
	##          MULTI_PLUS(MINUS / NCH / NCH_NOGAMMA):  multiplicity of positive (negative, neutral, neutral but no gammas) secondary particles produced
	##          PERC_PLUS(MINUS / NCH / NCH_NOGAMMA):   percentage of positive (negative, neutral, neutral but no gammas) secondary particles produced
	##
	##		For each of the plots above you can have them is form of a ratio of particles (the consecutive ones in the "pguns" array, see below)
	##      or as asymmetries adding RATIO or ASYM to the plot type. e.g. RATIO_TOTAL or RATIO_MULTI_NCH or ASYM_INEL, etc

	plots = [ "RATIO_TOTAL", "RATIO_INEL", "TOTAL", "INEL", "EL", "MULTI"]#, "MULTI_NCH", "MULTI_NCH_NOGAMMA" ]

	### N.B.: Options need to have been generated with Targets_RunAll.py!
	### In your output directory a file options.txt has been created where the options you generated are listed.

	models = ["QGSP_BERT","FTFP_BERT"]  # any you generated, by default "QGSP_BERT","FTFP_BERT"
	thicks = [1] #1,5,10
	materials = ["Al"] ### Al,Si,Be
	energies = [1,5,10,100] # any you generated, by default 1,5,10,100
	pguns = ["p","pbar","Kplus","Kminus","Piplus","Piminus"] # "p","pbar","Kplus","Kminus","Piplus","Piminus"
	#pguns = ["pbar","p"]
	path = "TargetOutput"

	file = TFile(path+"/TargetsPlots.root")
	dataTree = file.Get("summaryTree")
	
	os.system("mkdir -p "+path+"/Kaons")
	os.system("mkdir -p "+path+"/Protons")
	os.system("mkdir -p "+path+"/Pions")

	for p in plots :
		Plot( dataTree, "energy", p, path, models , pguns , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Protons", models , ["p","pbar"] , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Kaons", models , ["Kplus","Kminus"] , materials , 2 , 1, True )
		Plot( dataTree, "energy", p, path+"/Pions", models , ["Piplus","Piminus"] , materials , 2 , 1, True )



