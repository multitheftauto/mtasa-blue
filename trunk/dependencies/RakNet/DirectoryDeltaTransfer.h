/// \file
/// \brief Simple class to send changes between directories.  In essence, a simple autopatcher that can be used for transmitting levels, skins, etc.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __DIRECTORY_DELTA_TRANSFER_H
#define __DIRECTORY_DELTA_TRANSFER_H

#include "RakMemoryOverride.h"
#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "DS_Map.h"
#include "PacketPriority.h"

class RakPeerInterface;
class FileList;
struct Packet;
struct InternalPacket;
struct DownloadRequest;
class FileListTransfer;
class FileListTransferCBInterface;
class FileListProgress;
class IncrementalReadInterface;

/// \defgroup DIRECTORY_DELTA_TRANSFER_GROUP DirectoryDeltaTransfer
/// \ingroup PLUGINS_GROUP

/// \brief Simple class to send changes between directories.  In essence, a simple autopatcher that can be used for transmitting levels, skins, etc.
/// \sa Autopatcher class for database driven patching, including binary deltas and search by date.
///
/// To use, first set the path to your application.  For example "C:/Games/MyRPG/"
/// To allow other systems to download files, call AddUploadsFromSubdirectory, where the parameter is a path relative
/// to the path to your application.  This includes subdirectories.
/// For example:
/// SetApplicationDirectory("C:/Games/MyRPG/");
/// AddUploadsFromSubdirectory("Mods/Skins/");
/// would allow downloads from
/// "C:/Games/MyRPG/Mods/Skins/*.*" as well as "C:/Games/MyRPG/Mods/Skins/Level1/*.*"
/// It would NOT allow downloads from C:/Games/MyRPG/Levels, nor would it allow downloads from C:/Windows
/// While pathToApplication can be anything you want, applicationSubdirectory must match either partially or fully between systems.
/// \ingroup DIRECTORY_DELTA_TRANSFER_GROUP
class RAK_DLL_EXPORT DirectoryDeltaTransfer : public PluginInterface2
{
public:
	/// Constructor
	DirectoryDeltaTransfer();

	/// Destructor
	virtual ~DirectoryDeltaTransfer();

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// Set the local root directory to base all file uploads and downloads off of.
	/// \param[in] pathToApplication This path will be prepended to \a applicationSubdirectory in AddUploadsFromSubdirectory to find the actual path on disk.
	void SetApplicationDirectory(const char *pathToApplication);

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// Add all files in the specified subdirectory recursively
	/// \a subdir is appended to \a pathToApplication in SetApplicationDirectory().
	/// All files in the resultant directory and subdirectories are then hashed so that users can download them.
	/// \pre You must call SetFileListTransferPlugin with a valid FileListTransfer plugin
	/// \param[in] subdir Concatenated with pathToApplication to form the final path from which to allow uploads.
	void AddUploadsFromSubdirectory(const char *subdir);

	/// Downloads files from the matching parameter \a subdir in AddUploadsFromSubdirectory.
	/// \a subdir must contain all starting characters in \a subdir in AddUploadsFromSubdirectory
	/// Therefore,
	/// AddUploadsFromSubdirectory("Levels/Level1/"); would allow you to download using DownloadFromSubdirectory("Levels/Level1/Textures/"...
	/// but it would NOT allow you to download from DownloadFromSubdirectory("Levels/"... or DownloadFromSubdirectory("Levels/Level2/"...
	/// \pre You must call SetFileListTransferPlugin with a valid FileListTransfer plugin
	/// \param[in] subdir A directory passed to AddUploadsFromSubdirectory on the remote system.  The passed dir can be more specific than the remote dir.
	/// \param[in] outputSubdir The directory to write the output to.  Usually this will match \a subdir but it can be different if you want.
	/// \param[in] prependAppDirToOutputSubdir True to prepend outputSubdir with pathToApplication when determining the final output path.  Usually you want this to be true.
	/// \param[in] host The address of the remote system to send the message to.
	/// \param[in] onFileCallback Callback to call per-file (optional).  When fileIndex+1==setCount in the callback then the download is done
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	/// \param[in] cb Callback to get progress updates. Pass 0 to not use.
	/// \return A set ID, identifying this download set.  Returns 65535 on host unreachable.
	unsigned short DownloadFromSubdirectory(const char *subdir, const char *outputSubdir, bool prependAppDirToOutputSubdir, SystemAddress host, FileListTransferCBInterface *onFileCallback, PacketPriority _priority, char _orderingChannel, FileListProgress *cb);

	/// Clear all allowed uploads previously set with AddUploadsFromSubdirectory
	void ClearUploads(void);

	/// Returns how many files are available for upload
	/// \return How many files are available for upload
	unsigned GetNumberOfFilesForUpload(void) const;

	/// Set if we should compress outgoing sends or not
	/// Defaults to false, because this results in a noticeable freeze on large requests
	/// You can set this to true if you only send small files though
	/// \param[in] compress True to compress, false to not.
	void SetCompressOutgoingSends(bool compress);

	/// Normally, if a remote system requests files, those files are all loaded into memory and sent immediately.
	/// This function allows the files to be read in incremental chunks, saving memory
	/// \param[in] _incrementalReadInterface If a file in \a fileList has no data, filePullInterface will be used to read the file in chunks of size \a chunkSize
	/// \param[in] _chunkSize How large of a block of a file to send at once
	void SetDownloadRequestIncrementalReadInterface(IncrementalReadInterface *_incrementalReadInterface, unsigned int _chunkSize);
	
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
protected:
	void OnDownloadRequest(Packet *packet);

	char applicationDirectory[512];
	FileListTransfer *fileListTransfer;
	FileList *availableUploads;
	PacketPriority priority;
	char orderingChannel;
	bool compressOutgoingSends;
	IncrementalReadInterface *incrementalReadInterface;
	unsigned int chunkSize;
};

#endif
