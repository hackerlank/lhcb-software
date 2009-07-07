#!/usr/bin/env python

# first parse all options
from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] <opts_file> ...")
parser.add_option("-n","--numiter",type="int", dest="numiter",help="number of iterations", default=3)
parser.add_option("-e","--numevents",type="int", dest="numevents",help="number of events", default=1000)
parser.add_option("-p","--numprocesses",type="int", dest="numprocs",help="number of processes", default=8)
parser.add_option("-d", "--aligndb",type="str", dest="aligndb",help="path to file with alignment database layer for first iteration", default = '')
(opts, args) = parser.parse_args()

import os
rundir = os.getcwd()

for i in range( opts.numiter ) :
    print "Iteration nr: ", i
    # make the directory from where to run the job
    iterdir = 'Iter' + str( i )
    os.mkdir ( iterdir )
    os.chdir ( iterdir )

    # beyond the first iteration, add the input database as an option
    theseoptions = ' --numevents ' + str(opts.numevents) + ' --numprocesses ' + str(opts.numprocs) + ' --iter ' + str(i)
    if i>0 :
        previterdb = '../Iter' + str( i-1 ) + '/Alignment.db'
        theseoptions += ' --aligndb ' + previterdb
    elif opts.aligndb :
        theseoptions += ' --aligndb ' + opts.aligndb

    # add the remaining options
    for a in args:
        theseoptions += ' ' + a
            
    # run the job
    thiscommand = '$ESCHEROPTS/gaudipar.py' + theseoptions
    print 'command: ', thiscommand
    os.system( thiscommand )

    os.chdir(rundir)

# create a single alignlog file
os.system('cat Iter?/alignlog.txt > ! alignlog.txt')
