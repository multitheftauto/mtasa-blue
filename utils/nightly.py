import optparse
import os.path
prefix = "#define MTA_DM_BUILDTYPE "

parser = optparse.OptionParser()
parser.add_option("-r", "--revision", dest="revision" )
(options,args) = parser.parse_args()

def writeVersion (filePath):
	if (not options.revision) or (options.revision == ""):
		return
		
	if os.path.exists(filePath):
		versionFile = open(filePath,"r+")
		newLines = []
		for line in versionFile:
			#The prefix was found at the beginning of this line
			if line.find (prefix) == 0:
				newLines.append ( '%s "Nightly r%s"\n' % (prefix,options.revision) )
			else:
				newLines.append(line)
		versionFile.seek(0)
		versionFile.writelines(newLines)
		versionFile.close()

writeVersion("../MTA10/version.h")
writeVersion("../MTA10_Server/version.h")