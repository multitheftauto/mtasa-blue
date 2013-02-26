##############################################################################
#
#  PROJECT:     Multi Theft Auto v1.0
#  LICENSE:     See LICENSE in the top level directory
#  FILE:        utils/build_gettext_catalog.py
#  PURPOSE:     Build a multi-language .NSI script based upon input PO files
#  DEVELOPERS:  Dan Chowdhury <>
#
#  Multi Theft Auto is available from http://www.multitheftauto.com/
#
##############################################################################
import collections
import os
import polib
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-i", "--input", dest="input",
                  help="nightly.nsi location", default="../Shared/installer/nightly.nsi" )
parser.add_option("-o", "--output", dest="output",
                  help="Localized nightly output location", default="../Shared/installer/nightly_localized.nsi")
parser.add_option("-p", "--podir", dest="podir",
                  help="Directory containing PO files", default="../Shared/installer/locale/")
parser.add_option("-l", "--lang", dest="lang",
                  help="Default language of the NSI", default="English" )

(options, args) = parser.parse_args()


# Define a dict to convert locale names to language names
localeToName = {
  "af" :	"Afrikaans",
#  "am" :	"Amharic", #Comment out those unsupported by NSIS
  "ar" :	"Arabic",
  "bg" :	"Bulgarian",
  "br" :	"Breton",
  "ca" :	"Catalan",
  "cs" :	"Czech",
  "da" :	"Danish",
  "de" :	"German",
#  "dz" :	"Dzongkha",
  "el" :	"Greek",
  "en" :	"English",
  "es" :	"Spanish",
  "eu" :	"Basque",
#  "fa" :	"Persian",
  "fi" :	"Finnish",
  "fr" :	"French",
  "ga" :	"Irish",
#  "gu" :	"Gujarati",
  "he" :	"Hebrew",
#  "hi" :	"Hindi",
  "hr" :	"Croatian",
  "hu" :	"Hungarian",
  "id" :	"Indonesian",
  "it" :	"Italian",
  "ja" :	"Japanese",
#  "ka" :	"Georgian",
  "ko" :	"Korean",
  "lt" :	"Lithuanian",
  "lv" :	"Latvian",
  "mk" :	"Macedonian",
#  "ml" :	"Malayalam",
#  "mr" :	"Marathi",
  "ms" :	"Malay",
  "nb" :	"Norwegian",
#  "ne" :	"Nepal",
  "nl" :	"Dutch",
  "nn" :	"NorwegianNynorsk",
#  "oc" :	"Occitan",
#  "pa" :	"Punjabi",
  "pl" :	"Polish",
  "pt" :	"Portuguese",
  "pt_BR" :	"PortugueseBR",
  "ro" :	"Romanian",
  "ru" :	"Russian",
 # "rw" :	"Kinyarwanda",
  "sk" :	"Slovak",
  "sl" :	"Slovenian",
#  "so" :	"Somali",
  "sq" :	"Albanian",
#  "sr" :	"Serbian",
  "sv" :	"Swedish",
#  "ta" :	"Tamil",
  "th" :	"Thai",
  "tr" :	"Turkish",
  "uk" :	"Ukrainian",
#  "ur" :	"Urdu",
#  "vi" :	"Vietnamese",
#  "wa" :	"Walloon",
  "zh" :	"SimpChinese",
  "zh" :	"TradChinese",
}

translationCache = {}

# The purpose of this loop is to go to the podir scanning for PO files for each locale name
# Once we've found a PO file, we use PO lib to read every translated entry
# Using this, for each each language, we store a dict of entries - { nsilabel (comment) : translation (msgstr) }
# For untranslated entries, we use msgid instead of msgstr (i.e. default English string)
for root,dirs,files in os.walk(options.podir):
    for file in files:
        filename,ext = os.path.splitext(file)
        if ext == ".po":
            # Valid locale filename (fr.po, de.po etc)?
            if filename in localeToName:
                language = localeToName[filename]
                translationCache[language] = collections.OrderedDict()
                po = polib.pofile(os.path.join(root,file))
                for entry in po.translated_entries():
                    # Loop through all our labels and add translation (each translation may have multiple labels)
                    for label in entry.comment.split():
                        translationCache[language][label] = polib.unescape(entry.msgstr)
                # For untranslated strings, let's add the English entry
                for entry in po.untranslated_entries():
                    for label in entry.comment.split():
                        print("Warning: Label '%s' for language '%s' remains untranslated"%(label,language))
                        translationCache[language][label] = polib.unescape(entry.msgid)               


# Open our source NSI, dump it to a list and close it
NSISourceFile = open(options.input,"r")
NSISourceLines = NSISourceFile.readlines()
NSISourceFile.close()

incr = 0
def getlineincr(no):
    global incr
    incr += 1
    return incr+no

# Here we scan for ";@INSERT_TRANSLATIONS@" in the NSIS, and add MUI_LANGUAGE macros and LangString's for translation languages
lineNo = 1
for line in NSISourceLines:
    if line.find(";@INSERT_TRANSLATIONS@") == 0:      
        for language,translations in translationCache.iteritems():
			# if the language isn't the default, we add our MUI_LANGUAGE macro
            if language.upper() != options.lang.upper():
                NSISourceLines.insert(getlineincr(lineNo),'!insertmacro MUI_LANGUAGE "%s"\n'%language)
			# For every translation we grabbed from the .po, let's add our LangString
            for label,value in translations.iteritems():
                NSISourceLines.insert(getlineincr(lineNo),'LangString %s ${LANG_%s} "%s"\n' % (label,language,value) )

		# Remove the redundant ";@INSERT_TRANSATIONS@" to prevent this script working again
        del NSISourceLines[lineNo-1]
        break
    
    lineNo += 1

# Finally, let's write our new .nsi to the desired target file
NSIWorkingFile = open(options.output,"w")
NSIWorkingFile.writelines(NSISourceLines)
NSIWorkingFile.close()
    
print ( "NSI Localization Operation Complete" )
