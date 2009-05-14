/// \file
/// \brief A plugin to provide a simple way to compress and incrementally send the files in the FileList structure.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __FILE_LIST_TRANFER_H
#define __FILE_LIST_TRANFER_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "DS_Map.h"
#include "RakNetTypes.h"
#include "PacketPriority.h"
#include "RakMemoryOverride.h"
#include "FileList.h"

class IncrementalReadInterface;
class FileListTransferCBInterface;
class FileListProgress;
struct FileListReceiver;

/// \defgroup FILE_LIST_TRANSFER_GROUP FileListTransfer
/// \ingroup PLUGINS_GROUP

/// \brief A plugin to provide a simple way to compress and incrementally send the files in the FileList structure.
/// Similar to the DirectoryDeltaTransfer plugin, except that it doesn't send deltas based on pre-existing files or actually write the files to disk.
///
/// Usage:
/// Call SetupReceive to allow one file set to arrive.  The value returned by FileListTransfer::SetupReceive()
/// is the setID that is allowed.
/// It's up to you to transmit this value to the other system, along with information indicating what kind of files you want to get.
/// The other system should then prepare a FileList and call FileListTransfer::Send(), passing the return value of FileListTransfer::SetupReceive()
/// as the \a setID parameter to FileListTransfer::Send()
/// \ingroup FILE_LIST_TRANSFER_GROUP
class RAK_DLL_EXPORT FileListTransfer : public PluginInterface2
{
public:
	FileListTransfer();
	virtual ~FileListTransfer();
	
	/// Allows one corresponding Send() call from another system to arrive.
	/// \param[in] handler The class to call on each file
	/// \param[in] deleteHandler True to delete the handler when it is no longer needed.  False to not do so.
	/// \param[in] allowedSender Which system to allow files from.
	/// \return A set ID value, which should be passed as the \a setID value to the Send() call on the other system.  This value will be returned in the callback and is unique per file set.  Returns 65535 on failure (not connected to sender)
    unsigned short SetupReceive(FileListTransferCBInterface *handler, bool deleteHandler, SystemAddress allowedSender);

	/// Send the FileList structure to another system, which must have previously called SetupReceive()
	/// \param[in] fileList A list of files.  The data contained in FileList::data will be sent incrementally and compressed among all files in the set
	/// \param[in] rakPeer The instance of RakNet to use to send the message. Pass 0 to use the instance the plugin is attached to
	/// \param[in] recipient The address of the system to send to
	/// \param[in] setID The return value of SetupReceive() which was previously called on \a recipient
	/// \param[in] priority Passed to RakPeerInterface::Send()
	/// \param[in] orderingChannel Passed to RakPeerInterface::Send()
	/// \param[in] compressData Depreciated, unsupported
	/// \param[in] _incrementalReadInterface If a file in \a fileList has no data, filePullInterface will be used to read the file in chunks of size \a chunkSize
	/// \param[in] _chunkSize How large of a block of a file to send at once
	void Send(FileList *fileList, RakPeerInterface *rakPeer, SystemAddress recipient, unsigned short setID, PacketPriority priority, char orderingChannel, bool compressData, IncrementalReadInterface *_incrementalReadInterface=0, unsigned int _chunkSize=262144);

	/// Stop a download.
	void CancelReceive(unsigned short setId);

	/// Remove all handlers associated with a particular system address
	void RemoveReceiver(SystemAddress systemAddress);

	/// Is a handler passed to SetupReceive still running?
	bool IsHandlerActive(unsigned short setId);

	/// Set a callback to get progress reports about what the file list instances do
	/// \param[in] cb A pointer to an externally defined instance of FileListProgress. This pointer is held internally, so should remain valid as long as this class is valid.
	void SetCallback(FileListProgress *cb);


	/// \returns what was sent to SetCallback
	/// \return What was sent to SetCallback
	FileListProgress *GetCallback(void) const;

	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(void);
	/// \internal For plugin handling
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	/// \internal For plugin handling
	virtual void Update(void);

protected:
	bool DecodeSetHeader(Packet *packet);
	bool DecodeFile(Packet *packet, bool fullFile);

	void Clear(void);

	void OnReferencePush(Packet *packet, bool fullFile);
	void OnReferencePushAck(Packet *packet);
	void PushReference(SystemAddress systemAddress);
	void StoreForPush(FileListNodeContext context, unsigned short _setID, const char *fileName, const char *fullPathToFile, unsigned int _setIndex, unsigned fileLengthBytes, unsigned dataLengthBytes, SystemAddress recipient, PacketPriority packetPriority, char orderingChannel, IncrementalReadInterface *_incrementalReadInterface, unsigned int _chunkSize);

	DataStructures::Map<unsigned short, FileListReceiver*> fileListReceivers;
	unsigned short setId;
	FileListProgress *callback;

	struct FileToPush
	{
		FileListNode fileListNode;
		SystemAddress systemAddress;
		PacketPriority packetPriority;
		char orderingChannel;
		unsigned int currentOffset;
		unsigned short setID;
		unsigned int setIndex;
		IncrementalReadInterface *incrementalReadInterface;
		unsigned int chunkSize;
	};
	DataStructures::List<FileToPush> filesToPush;
};

#endif
