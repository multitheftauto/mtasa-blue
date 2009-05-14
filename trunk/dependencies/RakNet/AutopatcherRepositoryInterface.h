/// \file
/// \brief An interface used by AutopatcherServer to get the data necessary to run an autopatcher.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.



#ifndef __AUTOPATCHER_REPOSITORY_INTERFACE_H
#define __AUTOPATCHER_REPOSITORY_INTERFACE_H

class FileList;
namespace RakNet
{
	class BitStream;
}

#include "IncrementalReadInterface.h"

/// An interface used by AutopatcherServer to get the data necessary to run an autopatcher.  This is up to you to implement for custom repository solutions.
class AutopatcherRepositoryInterface : public IncrementalReadInterface
{
public:
	/// Get list of files added and deleted since a certain date.  This is used by AutopatcherServer and not usually explicitly called.
	/// \param[in] applicationName A null terminated string identifying the application
	/// \param[out] addedFiles A list of the current versions of filenames with hashes as their data that were created after \a sinceData
	/// \param[out] deletedFiles A list of the current versions of filenames that were deleted after \a sinceData
	/// \param[in] An input date, in whatever format your repository uses
	/// \param[out] currentDate The current server date, in whatever format your repository uses
	/// \return True on success, false on failure.
	virtual bool GetChangelistSinceDate(const char *applicationName, FileList *addedFiles, FileList *deletedFiles, const char *sinceDate, char currentDate[64])=0;

	/// Get patches (or files) for every file in input, assuming that input has a hash for each of those files.
	/// \param[in] applicationName A null terminated string identifying the application
	/// \param[in] input A list of files with SHA1_LENGTH byte hashes to get from the database.
	/// \param[out] patchList You should return list of files with either the filedata or the patch.  This is a subset of \a input.  The context data for each file will be either PC_WRITE_FILE (to just write the file) or PC_HASH_WITH_PATCH (to patch).  If PC_HASH_WITH_PATCH, then the file contains a SHA1_LENGTH byte patch followed by the hash.  The datalength is patchlength + SHA1_LENGTH
	/// \param[out] currentDate The current server date, in whatever format your repository uses
	/// \return True on success, false on failure.
	virtual bool GetPatches(const char *applicationName, FileList *input, FileList *patchList, char currentDate[64])=0;

	/// \return Whatever this function returns is sent from the AutopatcherServer to the AutopatcherClient when one of the above functions returns false.
	virtual const char *GetLastError(void) const=0;
};

#endif
