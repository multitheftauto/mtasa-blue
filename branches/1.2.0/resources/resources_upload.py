#!/usr/bin/python
"""Script that checks out files from the MTA SVN, removes .svn files, archives to a zip, and uploads them to google code."""

#Import modules
import os			#basic file/dir stuff
import sys			#basic system stuff
import stat			#used for stat.S_IWRITE, which disables readonly on files
import shutil		#higher level file operations, used to delete directory trees
import logging		#logging
import zipfile		#used to create zip files
import datetime		#used to time how long this script takes to run
import optparse		#used for command line options of this script
import tempfile		#used to create a temp dir where the SVN is checked out to
import subprocess	#executes external programs

#Google's needed modules
import base64
import httplib

#Google's upload function, http://code.google.com/p/support/source/browse/trunk/scripts/googlecode_upload.py
def upload(file, project_name, user_name, password, summary, labels=None):
	"""Upload a file to a Google Code project's file server.
	
	Args:
		file: The local path to the file.
		project_name: The name of your project on Google Code.
		user_name: Your Google account name.
		password: The googlecode.com password for your account.
			Note that this is NOT your global Google Account password!
		summary: A small description for the file.
		labels: an optional list of label strings with which to tag the file.
	
	Returns: a tuple:
		http_status: 201 if the upload succeeded, something else if an error occured.
		http_reason: The human-readable string associated with http_status
		file_url: If the upload succeeded, the URL of the file on Google Code, None otherwise.
	"""
	# The login is the user part of user@gmail.com. If the login provided
	# is in the full user@domain form, strip it down.
	if user_name.endswith('@gmail.com'):
		user_name = user_name[:user_name.index('@gmail.com')]
	
	form_fields = [('summary', summary)]
	if labels is not None:
		form_fields.extend([('label', l.strip()) for l in labels])
	
	content_type, body = encode_upload_request(form_fields, file)
	
	upload_host = '%s.googlecode.com' % project_name
	upload_uri = '/files'
	auth_token = base64.b64encode('%s:%s'% (user_name, password))
	headers = {
		'Authorization': 'Basic %s' % auth_token,
		'User-Agent': 'Googlecode.com uploader v0.9.4',
		'Content-Type': content_type,
		}
	
	server = httplib.HTTPSConnection(upload_host)
	server.request('POST', upload_uri, body, headers)
	resp = server.getresponse()
	server.close()
	
	if resp.status == 201:
		location = resp.getheader('Location', None)
	else:
		location = None
	return resp.status, resp.reason, location

#Google's upload function's encoding function
def encode_upload_request(fields, file_path):
	"""Encode the given fields and file into a multipart form body.

	fields is a sequence of (name, value) pairs. file is the path of
	the file to upload. The file will be uploaded to Google Code with
	the same file name.

	Returns: (content_type, body) ready for httplib.HTTP instance
	"""
	BOUNDARY = '----------Googlecode_boundary_reindeer_flotilla'
	CRLF = '\r\n'
	
	body = []
	
	# Add the metadata about the upload first
	for key, value in fields:
		body.extend(
			['--' + BOUNDARY,
			 'Content-Disposition: form-data; name="%s"' % key,
			 '',
			 value,
			 ])
	
	# Now add the file itself
	file_name = os.path.basename(file_path)
	f = open(file_path, 'rb')
	file_content = f.read()
	f.close()
	
	body.extend(
		['--' + BOUNDARY,
		 'Content-Disposition: form-data; name="filename"; filename="%s"'
		 % file_name,
		 # The upload server determines the mime-type, no need to set it.
		 'Content-Type: application/octet-stream',
		 '',
		 file_content,
		 ])
	
	# Finalize the form body
	body.extend(['--' + BOUNDARY + '--', ''])
	
	return 'multipart/form-data; boundary=%s' % BOUNDARY, CRLF.join(body)

#Function to print messages to screen
def echo(msg):
	"""Centralizes printing to screen."""
	logging.info(msg)
	print msg

#Function to print error message to STDERR and terminate entire script
def error(msg):
	"""Prints message to stderr and quits with a status of one."""
	logging.error(msg)
	logging.error("QUITTING!")
	print >> sys.stderr, ""
	print >> sys.stderr, msg
	print >> sys.stderr, "\nQUITTING!"
	sys.exit(1)
	
#Function to execute a local command
def localCmd(cmdStr,cwd=None):
	"""Gets the external command's return status and output. Returns (retcode,stdout,stderr,pid)"""
	handle = subprocess.Popen(cmdStr,cwd=cwd,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	stdout,stderr = handle.communicate()
	retcode = handle.returncode
	pid = handle.pid
	stdout = stdout.strip()
	stderr = stderr.strip()
	logging.debug("localCmd(): cmd (%s) retcode (%d) stdout (%s) stderr (%s) pid (%d)"%(cmdStr,retcode,stdout,stderr,pid))
	return (retcode,stdout,stderr,pid)

#Main function
def main():
	#Variables
	SVNURL = "http://multitheftauto-resources.googlecode.com/svn/trunk/"
	
	#Command line args
	parser = optparse.OptionParser(version="%prog 1.2")
	parser.add_option("-l","--logfile",dest="logfile",metavar="FILE",type="string",default="",help="Log text to specified FILE.")
	parser.add_option("-n","--project",dest="gproject",metavar="PROJECT",type="string",default="",help="Upload zip to specific google project.")
	parser.add_option("-u","--user",dest="guser",metavar="USER",type="string",default="",help="Username to access google project.")
	parser.add_option("-p","--password",dest="gpasswd",metavar="PASSWD",type="string",default="",help="googlecode.com password.")
	(opts,args) = parser.parse_args()
	
	#Enable logging if requested
	if opts.logfile != "":
		#enable logging
		logging.basicConfig(level=logging.DEBUG,format="%(asctime)s %(levelname)-8s %(process)d %(message)s",filename=opts.logfile,filemode='w')
		logging.info('Script started.')
	else:
		#disable logging
		logging.disable(logging.critical)
	
	#Sanity check
	if opts.gproject == "":
		error("Google project name not set, for more information do %s -h"%sys.argv[0])
	if opts.guser == "":
		error("Google account username not set, for more information do %s -h"%sys.argv[0])
	if opts.gpasswd == "":
		error("Googlecode.com password not set, for more information do %s -h"%sys.argv[0])
	
	#Generate a temporary directory for holding SVN files
	tmpSvnDir = tempfile.mkdtemp()
	
	#SVN Checkout
	echo("Checking out SVN to %s"%tmpSvnDir)
	svnResults = localCmd("svn co %s"%SVNURL,tmpSvnDir)
	if svnResults[0] != 0:
		error("Error occured when checking out SVN.")
	else:
		svnRevision = int(svnResults[1].splitlines()[-1].split(" ")[-1][:-1]) #really ugly I know, but avoids the overhead of regexp for just this one thing
	
	#Walk SVN checkout directory and eliminate any .svn directories
	echo("Walking checked out directory and deleting .svn directories...")
	nukeList = list() #this will hold a list (array) of .svn directories (absolute paths) which will be deleted
	#walk the svn checkout directory, looking for .svn dirs
	for root,dirs,files in os.walk(tmpSvnDir):
		if ".svn" in dirs:
			svnPath = os.path.abspath(os.path.join(root,".svn"))
			nukeList.append(svnPath) #add dir to nuke list
			logging.debug("Unsetting readonly for %s"%svnPath)
			os.chmod(svnPath,stat.S_IWRITE) #unsets .svn directory readonly
		if ".svn" in root:
			#all files and dirs are inside of .svn dir, unset readonly on everything so I can nuke it
			for file in files:
				filePath = os.path.abspath(os.path.join(root,file))
				logging.debug("Unsetting readonly for %s"%filePath)
				os.chmod(filePath,stat.S_IWRITE)
			for dir in dirs:
				dirPath = os.path.abspath(os.path.join(root,dir))
				logging.debug("Unsetting readonly for %s"%dirPath)
				os.chmod(dirPath,stat.S_IWRITE)
	#walking is done, now delete all of the .svn dirs
	for dir in nukeList:
		logging.debug("Removing %s"%dir)
		shutil.rmtree(dir)
	
	#Merge the three root directories into one by moving files/dirs up one directory
	for rootDir in os.listdir(os.path.join(tmpSvnDir,"trunk")):
		for subDir in os.listdir(os.path.join(os.path.join(tmpSvnDir,"trunk"),rootDir)):
			#move files to root directory
			srcFile = os.path.join(os.path.join(os.path.join(tmpSvnDir,"trunk"),rootDir),subDir)
			dstFile = os.path.join(tmpSvnDir,subDir)
			logging.debug("Moving %s to %s"%(srcFile,dstFile))
			os.rename(srcFile,dstFile)
	#delete empty trunk directory
	trunkPath = os.path.join(tmpSvnDir,"trunk")
	logging.debug("Removing empty directory tree %s"%trunkPath)
	shutil.rmtree(trunkPath)
	
	#Generate a temporary zipfile path
	tmpZipFileDir = tempfile.mkdtemp()
	tmpZipFile = os.path.join(tmpZipFileDir,"multitheftauto_resources-r%d.zip"%svnRevision)
	
	#Archive files into a zip file
	echo("Archiving to %s"%tmpZipFile)
	zipFileHandle = zipfile.ZipFile(tmpZipFile,'w',zipfile.ZIP_DEFLATED) #ZIP_DEFLATED means to compress, the alternative is ZIP_STORED
	for root,dirs,files in os.walk(tmpSvnDir):
		for file in files:
			absPathToFile = os.path.join(root,file)
			logging.debug("Adding %s to zip file."%absPathToFile)
			zipFileHandle.write(absPathToFile,os.path.relpath(absPathToFile,tmpSvnDir))
	zipFileHandle.close()
	
	#Upload zip to googlecode
	echo("Uploading zip file to project %s"%opts.gproject)
	summary = "Latest Multi Theft Auto resources (revision %d) (required to run)"%svnRevision
	labels = ["Featured","OpSys-All","Type-Archive"]
	uploadResult = upload(tmpZipFile,opts.gproject,opts.guser,opts.gpasswd,summary,labels)
	logging.debug("Uploaded %s, status: %d, reason: %s, location: %s"%(tmpZipFile,uploadResult[0],uploadResult[1],uploadResult[2]))
	if uploadResult[0] != 201:
		#error occured when uploading
		error("Error occured when uploading zip to Googlecode.  HTTP %d %s"%(uploadResult[0],uploadResult[1]))
	
if __name__ == "__main__":
	try:
		#Record start time
		timeStart = datetime.datetime.now()
		#Execute main()
		main()
		#Print footer
		timeEnd = datetime.datetime.now()
		timeDelta = timeEnd - timeStart
		echo("Completed running script in "+str(timeDelta)+".")
	except KeyboardInterrupt:
		echo('Control-C')