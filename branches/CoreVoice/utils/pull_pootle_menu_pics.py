##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/pull_pootle_menu_pics.py
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
import polib
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-u", "--url", dest="url",
            help="Pootle homepage URL", default="http://translate.mtasa.com/")
parser.add_option("-o", "--output", dest="output",
            help="Output directory", default="../Shared/data/MTA San Andreas/MTA/locale")
parser.add_option("-p", "--project", dest="project",
            help="Internal name of the pootle project", default="mainmenu")
parser.add_option("-G", "--gnu",action="store_true", dest="gnu", default=False,
            help="Whether the project is a GNU directory layout")
parser.add_option("-L", "--languages",dest="languages", 
            help="A comma delimited list of languages to pull from Pootle", default='ar,zh_CN,hr,cs,de,es,et,fr,hu,it,lt,lv,nb,nl,pl,pt_BR,ro,ru,sk,sl,sv,tr')
parser.add_option("-r", "--rmdir",action="store_true", dest="rmdir", default=False,
            help="Clear the output directory before starting")

(options, args) = parser.parse_args()

# e.g http://translate.multitheftauto.com/export/mainmenu/ar/mainmenu.po
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

    # Show HTTP headers if required
    if False:
        print ( "Trying '%s'"%(url) )
        opener = urllib2.build_opener(urllib2.HTTPHandler(debuglevel=1))
        opener.open(url)

    # Get file twice and compare to defeat truncated downloads
    while True:
        header = {"pragma-directive" : "no-cache","PRAGMA" : "NO-CACHE","CACHE-CONTROL" : "NO-CACHE"}
        req = urllib2.Request(url, headers=header)
        u = urllib2.urlopen(req)
        content = u.read()

        path,tail = os.path.split(output)
        if ( not os.path.exists(path) ):
            os.makedirs(path)

        localFile = open(output, 'wb')
        localFile.write(content)
        localFile.close()

        u = urllib2.urlopen(req)
        if ( content == u.read() ):
            break

    po = polib.pofile(content)

    for entry in po.translated_entries():
        # Make save file name
        pngname = entry.msgid.split("/")[-1]
        output = os.path.join(options.output,"%s/%s"%(lang,pngname))

        path,tail = os.path.split(output)
        if ( not os.path.exists(path) ):
            os.makedirs(path)

        url = entry.msgstr
        try:
            # Get pic
            header = {"pragma-directive" : "no-cache","PRAGMA" : "NO-CACHE","CACHE-CONTROL" : "NO-CACHE"}
            req = urllib2.Request(url, headers=header)
            u = urllib2.urlopen(req)
            content = u.read()
            if ( content[:1] == "<" ):
                print ( "Content error with '%s'"%(url) )
            else:
                # Save pic
                localFile = open(output, 'wb')
                localFile.write(content)
                localFile.close()
                print ( "Read '%s' and written to '%s'"%(url,output) )
        except urllib2.HTTPError as e:
            print ( "HTTPError %d '%s' '%s'"%(e.code,e.reason,url) )
