##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/build_gettext_catalog.py
#  PURPOSE:     Create a template .pot file from .cpp and .h project files for client and server
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################

import os
import subprocess
import tempfile
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-e", "--exe", dest="exe",
                  help="xgettext executable location", default="C:/Program Files (x86)/Poedit/bin/xgettext.exe" )
#parser.add_option("-f", "--file", dest="output",
#                  help="POT File output directory", default="messages")
parser.add_option("-v", "--version", dest="version",
                  help="MTA:SA Version to write to the POT file", default="1.x")

(options, args) = parser.parse_args()

directories = {
    "client.pot" : [ "../MTA10", "../Shared" ],
    #"../MTA10/locale/client.pot" : [ "../MTA10_Server", "../Shared" ],
}

scanDirsList = []

fd,temp_path = tempfile.mkstemp()

# The objective here is to scan for all our .cpp's and .h's in each root directory
# We then compile a list of these files into a temporary file, which is given to xgettext
# xgettext then reads this list, and produces our template .po file which is renamed to .pot
for output,dirList in directories.iteritems():
    scanDirsFile = open(temp_path, 'w')
    # Scan for .cpp and .h files
    for dir in dirList:
        for root,dirs,files in os.walk(dir):
            for file in files:
                filename,ext = os.path.splitext(file)
                if ext == ".c" or ext == ".cpp" or ext == ".h" or ext == ".hpp":
                    filePath = os.path.abspath(os.path.join(root,file))
                    print ( filePath )
                    # Add each file to a list
                    scanDirsList.append ( filePath + "\n" )
                
    print ( "Files found: " + str(len(scanDirsList)) )

    # Write this to our temporary file
    scanDirsFile.writelines(scanDirsList)
    scanDirsFile.close()

    # If we have .pot in the destination, strip it (xgettext seems to append an extension regardless)
    path,ext = os.path.splitext(output)
    if ext == ".pot":
        output = path

    # Give xgettext our temporary file to produce our .po
    cmdArgs = [options.exe,"-f",os.path.abspath(scanDirsFile.name),"-d",output,
               "--c++","--from-code=UTF-8","--add-comments",
               "--keyword=_", "--keyword=_td", "--keyword=_tn:1,2", "--keyword=_tc:1c,2", "--keyword=_tcn:1c,2,3",
               "--package-name=MTA San Andreas","--package-version="+options.version]

    proc = subprocess.Popen(cmdArgs)
    stdout, stderr = proc.communicate()
    print stdout
    print stderr
    
    #Rename our template to .pot (xgettext always outputs .po)
    if os.path.isfile(output + ".pot"):
        os.remove(output + ".pot")
    if os.path.isfile(output + ".po"):
        os.rename(output + ".po", output + ".pot")

# Delete our temporary file
os.close(fd)
os.remove(temp_path)


print ( "POT Generation Operation Complete" )
