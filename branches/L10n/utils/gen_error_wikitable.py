##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/gen_error_wikitable.py
#  PURPOSE:     Generates a MediaWiki table for error codes from source code
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################

import os
import re
import sys
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-d", "--dir", dest="dir",
                  help="MTA Source code location", default="../" )
parser.add_option("-o", "--output", dest="output",
                  help="Output .txt file", default="errors_wikicode.txt" )

(options, args) = parser.parse_args()

ErrorCodes = {}

# Scan for .cpp and .h files
for root,dirs,files in os.walk(options.dir):
        for file in files:
                filename,ext = os.path.splitext(file)
                if ext == ".c" or ext == ".cpp" or ext == ".h" or ext == ".hpp":
                        filePath = os.path.abspath(os.path.join(root,file))
                        #Parse the file looking for error codes
                        openedFile = open(filePath,"r")
                        lineNo = 0
                        for line in openedFile:
                                lineNo += 1  
                                startScan = line.find('_E("')
                                if startScan == -1:
                                        continue
                                # Check previous character is not alphanumerical
                                if (startScan > 0 and re.match('^[\w-]+$',line[startScan-1:startScan]) != None ):
                                        continue

                                endScan = line.find('"',startScan+4)
                                errorCode = line[startScan+4:endScan]
                                # Find any other strings and compile for information
                                newStartScan = line.find('"',0)
                                finalIndex = 0
                                information = []
                                while newStartScan != -1:
                                        newEndScan = line.find('"',newStartScan+1)
                                        foundString = line[newStartScan+1:newEndScan]
                                        if foundString != errorCode:
                                                information.append(foundString)

                                        newStartScan = line.find('"',newEndScan+1)
                                        finalIndex = max([ finalIndex, newEndScan+1 ])

                                # Find any line comments and append
                                newStartScan = line.rfind("//",finalIndex)
                                if newStartScan != -1:
                                        information.append(line[newStartScan+2:-1].lstrip())
                                if errorCode in ErrorCodes:
                                        print("WARNING: Error Code conflict: " + errorCode )
                                        
                                ErrorCodes[errorCode] = { "information" : information, "file" : os.path.relpath(filePath,options.dir), "line" : lineNo }

                        openedFile.close()


#### Put into media wiki format
wikicode = "<!-- Automatically generated with %s -->"%(os.path.relpath(sys.argv[0],options.dir)) + """
{| class="wikitable" style="width: auto; text-align: center; table-layout: fixed;"
|-
!Error Code
!Associated strings
!File:LineNumber
"""
for code,data in sorted(ErrorCodes.items()):
        wikicode += "|-\n"
        wikicode += "|'''%s'''\n"%(code)
        wikicode += "|"
        if len(data["information"]) > 0:
                wikicode += ("\n\n").join(data["information"])
        wikicode += "\n"
        wikicode += "|%s:%s\n"%(data["file"],data["line"])
        #print code, data["information"], "%s:%s"%(data["file"],data["line"])

wikicode += "|}"

outputFile = open(options.output,"w")
outputFile.write(wikicode)
outputFile.close()

print( "Operation complete, %i error codes found.  File written to '%s'."%(len(ErrorCodes),os.path.abspath(options.output)) )
