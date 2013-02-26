##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/build_gettext_catalog.py
#  PURPOSE:     Create corresponding runtime .mo files for all localized .po files
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################
import os
import polib
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-d", "--dir", dest="dir",
                  help="Directory to scan for source .po translations", default="../Shared/locale")

(options, args) = parser.parse_args()

# We simply loop through options.dir, looking for .po files.  Then create a corresponding .mo
for root,dirs,files in os.walk(options.dir):
    for file in files:
        filename,ext = os.path.splitext(file)
        if ext == ".po":
            filePath = os.path.abspath(os.path.join(root,file))
            fileNoExtPath,_ = os.path.splitext(filePath)
            print ( filePath )
            targetFile = fileNoExtPath + ".mo"
            polib.pofile(filePath).save_as_mofile(targetFile)
print ( ".mo Output Operation Complete" )
