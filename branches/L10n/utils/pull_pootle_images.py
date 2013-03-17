##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/build_gettext_catalog.py
#  PURPOSE:     Pull translation files from a Pootle web server into a directory
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################

import os
import json
import shutil
import urllib2
import urlparse
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-u", "--url", dest="url",
            help="Project URL", default="http://pootlecdn.mtasa.com/mainmenu/")
parser.add_option("-o", "--output", dest="output",
            help="Output directory", default="../output/MTA San Andreas/mta/locale")
parser.add_option("-L", "--languages",dest="languages", 
            help="A JSON list of languages to pull from Pootle", default='["nl", "ru", "templates"]')

(options, args) = parser.parse_args()

image_names = [
    "menu_disconnect.png",
    "menu_quick_connect.png",
    "menu_browse_servers.png",
    "menu_host_game.png",
    "menu_map_editor.png",
    "menu_settings.png",
    "menu_about.png",
    "menu_quit.png"
]

# Let's decode our JSON list into a Python list, and loop it
for lang in json.loads(options.languages):
    for image in image_names:
        # Let's create our full Pootle export URL depending on whether we're GNU or not
        url = urlparse.urljoin(options.url, "%s/%s"%(lang,image))
        output = os.path.join(options.output,"%s/%s"%(lang,image))

        u = None
        try:
            u = urllib2.urlopen(url)
        except urllib2.HTTPError, e:
            continue
                    
        path,tail = os.path.split(output)
        if ( not os.path.exists(path) ):
            os.makedirs(path)

        localFile = open(output, 'wb')
        localFile.write(u.read())
        localFile.close()
        print ( "Read '%s' and written to '%s'"%(url,output) )
