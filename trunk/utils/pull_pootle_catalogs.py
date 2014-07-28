##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/pull_pootle_catalogs.py
#  PURPOSE:     Pull translation files from a Pootle web server into a directory
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################

import os
import shutil
import urllib2
import urlparse
import time
import sys
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-u", "--url", dest="url",
            help="Pootle homepage URL", default="http://translate.mtasa.com/")
parser.add_option("-o", "--output", dest="output",
            help="Output directory", default="../output/MTA San Andreas/mta/locale")
parser.add_option("-p", "--project", dest="project",
            help="Internal name of the pootle project", default="client")
parser.add_option("-G", "--gnu",action="store_true", dest="gnu", default=False,
            help="Whether the project is a GNU directory layout")
parser.add_option("-L", "--languages",dest="languages", 
            help="A comma delimited list of languages to pull from Pootle", default='de,fr')
parser.add_option("-r", "--rmdir",action="store_true", dest="rmdir", default=False,
            help="Clear the output directory before starting")

(options, args) = parser.parse_args()

# e.g http://translate.multitheftauto.com/export/client/nl/client.po
if options.rmdir:
    # Clear our output directory first
    if os.path.exists(options.output):
        shutil.rmtree(options.output)
    print ( "Cleared output directory: '%s'"%(options.output) )

# Let's split our languages string into a Python list, and loop it
for lang in (options.languages).replace(" ","").split(","):
    # Let's create our full Pootle export URL depending on whether we're GNU or not
    url = ""
    output = ""
    if options.gnu:
        url = urlparse.urljoin(options.url, "export/%s/%s.po"%(options.project,lang))
        output = os.path.join(options.output,"%s.po"%(lang))
    else:
        url = urlparse.urljoin(options.url, "export/%s/%s/%s.po"%(options.project,lang,options.project))
        output = os.path.join(options.output,"%s/%s.po"%(lang,options.project))

    print ( "Trying '%s'"%(url) )

    # Get file twice and compare to defeat truncated downloads
    success = False
    for x in range(0, 3):
        content = ""
        content2 = ""
        # Get file first
        try:
            u = urllib2.urlopen(url, timeout = 30)
            content = u.read()
        except urllib2.URLError as e:
            print type(e)
        except socket.timeout as e:
            print type(e)
        if ( len(content) > 1 ):
            # Get file second
            try:
                u = urllib2.urlopen(url, timeout = 30)
                content2 = u.read()
            except urllib2.URLError as e:
                print type(e)
            except socket.timeout as e:
                print type(e)
            # Check both downloads got the same result
            if ( content == content2 ):
                success = True
                break
            print ( "Downloads did no match" )
        else:
            print ( "Download failed" )
        # try again
        time.sleep(1)

    # Win or lose?
    if success != True:
        print ( "ERROR: Download total failure" )
        sys.exit(1)

    print ( "Download success " )

    path,tail = os.path.split(output)
    if ( not os.path.exists(path) ):
        os.makedirs(path)
    
    localFile = open(output, 'w')
    localFile.write(content)
    localFile.close()
    print ( "Read '%s' and written to '%s'"%(url,output) )
