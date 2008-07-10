#!/usr/bin/env python

import os, sys, stat

def _which(cmd):
    # Absolute paths should be treated differently
    if os.path.isabs(cmd):
        if os.path.exists(cmd):
            return cmd
        else:
            return None
    
    # search path
    search_path = os.environ["PATH"].split(os.pathsep)
    # on win32, the PATHEXT variable defines the extensions to look for
    if "PATHEXT" in os.environ:
        search_pathext = os.environ["PATHEXT"].split(os.pathsep)
    else:
        # fail-safe default
        search_pathext = [".BAT", ".COM", ".EXE"]
    cmd = cmd.strip()
    for p in search_path:
        if sys.platform[0:3] == "win":
            testc = [ (cmd+x).lower() for x in search_pathext ]
            for c in testc:
                c = os.path.join(p,c)
                if os.path.exists(c):
                    return c
        else: # assume UNIX
            c = os.path.join(p,cmd)
            if os.path.exists(c):
                if os.stat(c)[stat.ST_MODE] & ( stat.S_IXGRP | stat.S_IXOTH | stat.S_IXUSR ):
                    return c
            
    return None

def _getMessage():
    # look for an editor
    editor_alternatives = [ "pico", "nano", "emacs", "vi", "edit" ]
    # prefer the environment variables VISUAL and EDITOR 
    for v in [ "EDITOR", "VISUAL" ]:
        if v in os.environ:
            editor_alternatives.insert(0,os.environ[v])
    # this variable will contain the full path to the editor found
    editor = None
    eargs = []
    for e in editor_alternatives:
        eargs = e.strip().split()
        editor = _which(eargs.pop(0))
        if editor:
            break
    
    if editor:
        # prepare temporary file  
        import tempfile
        (tmpfd, tmpn) = tempfile.mkstemp()
        tmpf = os.fdopen(tmpfd,"w")
        marker = "--- Everything below this line will be ignored ---"
        default = "\n\n%s\nEdit comments for the changes and save.\n"%marker
        tmpf.write(default)
        tmpf.close()
        # start editor
        os.system(" ".join([e] + eargs + [tmpn]))
        # read the edited file
        message = [ l.strip() for l in open(tmpn) ]
        os.remove(tmpn)
        
        if marker in message:
            message = message[:message.index(marker)]
        
        # remove trailing empty lines
        while message and not message[-1]:
            message.pop()
        return "\n".join(message)
    else:
        return raw_input("Insert comment: ")
    
def main():
    # Configure the parser
    from optparse import OptionParser
    parser = OptionParser(usage = "%prog [options] changes_source partition reference_tag local_tag")
    parser.add_option("--user-tag", type = "string",
                      help = "Tag to be used to extract the changes from the user " +
                             "database. It make sense only if changes_source is a " +
                             "COOL database. [default is current directory]"
                      )
    parser.add_option("-d","--dir", type = "string",
                      help = "Directory where to put the DB with the differences. [default = %default]"
                      )
    parser.add_option("--rel-notes", type = "string",
                      help = "XML file containing the release notes"
                      )
    parser.add_option("-m", "--message", type = "string",
                      help = "Message to include in the release notes"
                      )
    parser.add_option("-c","--contributor", type = "string",
                      help = "Name of the contributor of the patch"
                      )
    parser.add_option("-p","--provider", type = "string",
                      dest = "contributor",
                      help = "alias for --contributor"
                      )
    parser.add_option("-n","--dry-run", action = "store_true",
                      help = "Skip the actual commit to database and the update of release notes."
                      )
    parser.set_default("user_tag", "HEAD")
    parser.set_default("dir", None)
    parser.set_default("rel_notes", os.path.join(os.environ["SQLDDDBROOT"], "doc", "release_notes.xml"))
    parser.set_default("message", None)
    parser.set_default("contributor", None)
    
    # parse command line
    options, args = parser.parse_args()
    
    # check arguments
    if len(args) != 4:
        print "Error: not enough arguments. Try with --help."
        return 1
    
    # Prepare local logger
    import logging
    log = logging.getLogger(parser.prog or os.path.basename(sys.argv[0]))
    log.setLevel(logging.INFO)
    
    # set the global stream handler
    from CondDBUI import LOG_FORMAT
    hndlr = logging.StreamHandler()
    hndlr.setFormatter(logging.Formatter(LOG_FORMAT))
    logging.getLogger().handlers = [hndlr]
    
    # decrease verbosity of PyCoolDiff
    import CondDBUI.PyCoolDiff
    CondDBUI.PyCoolDiff._log.setLevel(logging.WARNING)
    
    # check arguments
    changes_source, partition, reference_tag, local_tag = args
    
    partitions = ["DDDB", "LHCBCOND", "SIMCOND"]
    if partition not in partitions:
        log.error("'%s' is not a valid partition name. Allowed: %s" % \
                  (partition, partitions))
        return 1
    
    log.info("Preparing database with changes from data in %s for partition %s" % (changes_source, partition))
    log.info("reference tag = %s" % reference_tag)
    if options.user_tag != "HEAD":
        log.info("user tag = %s" % options.user_tag)
    if options.dir:
        log.info("working dir = %s" % options.dir)
        if not os.path.isdir(options.dir):
            os.makedirs(options.dir)
        
    from CondDBUI.Admin import prepareChangesDB, analyzeChanges
    changesURL, pass1URL = prepareChangesDB(changes_source, partition,
                                            reference_tag,
                                            usertag = options.user_tag,
                                            destdir = options.dir)
    log.info("Created changes database: %s" % changesURL)
    
    log.info("Analyzing changes with respect to head version of master DB")
    # I do not like it, but I do not have a better way right now
    import CondDBUI.Admin.Management
    masterURL = CondDBUI.Admin.Management._masterCoolURL(partition)
    log.info("master = %s" % masterURL)
    
    if pass1URL:
        # Check the pass1 diff
        log.info("Analyzing pass1 changes")
        modified, added, problems = analyzeChanges(partition, pass1URL)
        if problems:
            for p in problems:
                log.error("Problem: %s." % p)
            return 1
    
        if len(modified):
            log.info("Modified %d files:\n\t%s" % (len(modified),'\n\t'.join(modified)))
        if len(added):
            log.info("Added %d files:\n\t%s" % (len(added),'\n\t'.join(added)))
        if (len(modified) + len(added)) == 0:
            log.warning("No changes to apply!")
            return 0
        
        log.info("Analyzing final changes")
    
    modified, added, problems = analyzeChanges(partition, changesURL)
    
    if problems:
        for p in problems:
            log.error("Problem: %s." % p)
        return 1
    
    if len(modified):
        log.info("Modified %d files:\n\t%s" % (len(modified),'\n\t'.join(modified)))
    if len(added):
        log.info("Added %d files:\n\t%s" % (len(added),'\n\t'.join(added)))
    
    if (len(modified) + len(added)) == 0:
        log.warning("No changes to apply!")
        return 0
    
    if not options.contributor:
        options.contributor = raw_input("Contributor: ")
    log.info("Changes by: %s" % options.contributor)
        
    # prepare the message
    if not options.message:
        options.message = _getMessage()
    
    if not options.message:
        log.warning("Empty message!")
    else:
        log.info("Message for the changes:\n\n%s\n" % options.message)

    from CondDBUI.Admin import ReleaseNotes
    rel_notes = ReleaseNotes(options.rel_notes)
    rel_notes.addNote(provider = options.contributor,
                      partitions = {partition:(local_tag,modified + added)},
                      description = [options.message])
    
    ans = None
    if options.dry_run:
        ans = "No"
        log.info("Updated release notes written in the local directory")
        rel_notes.write("release_notes.xml")
    
    while ans is None:
        ans = raw_input("Do you really want to commit the changes (Yes,[No])? ")
        if not ans: ans = "No"
        if ans not in [ "Yes", "No" ]:
            print "You have to type exactly 'Yes' or 'No'"
            ans = None
    
    if ans == "No":
        log.info("Not committing the changes")
        return 0
    
    try:
        import time
        log.info("Committing the changes in ...")
        timeout = 10
        while timeout >= 0:
            log.info("%d seconds"%timeout)
            time.sleep(1)
            timeout -= 1
    except KeyboardInterrupt:
        log.info("Commit aborted")
        return 0
    
    if not options.dry_run:
        log.info("Committing the changes")
        
        from CondDBUI.Admin import MergeAndTag
        MergeAndTag(changesURL, masterURL, local_tag, check_addition_db = False)
        
        rel_notes.write()
        log.info("Updated release notes at %s"%options.rel_notes)
    
if __name__ == '__main__':
    import sys
    sys.exit(main())
