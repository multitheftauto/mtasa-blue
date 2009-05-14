/// \file
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __AUTOPATCHER_SERVER_H
#define __AUTOPATCHER_SERVER_H

#include "RakNetTypes.h"
#include "Export.h"
#include "PluginInterface2.h"
#include "PacketPriority.h"
#include "ThreadPool.h"
#include "BitStream.h"
#include "RakString.h"

class RakPeerInterface;
class FileList;
struct Packet;
class AutopatcherRepositoryInterface;
class FileListTransfer;

/// \addtogroup PLUGINS_GROUP
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
class RAK_DLL_EXPORT AutopatcherServer : public PluginInterface2
{
public:
	/// Constructor
	AutopatcherServer();

	/// Destructor
	~AutopatcherServer();

	/// What parameters to use for the RakPeerInterface::Send() call when uploading files.
	/// \param[in] _priority See RakPeerInterface::Send()
	/// \param[in] _orderingChannel See RakPeerInterface::Send()
	void SetUploadSendParameters(PacketPriority _priority, char _orderingChannel);

	/// This plugin has a dependency on the FileListTransfer plugin, which it uses to actually send the files.
	/// So you need an instance of that plugin registered with RakPeerInterface, and a pointer to that interface should be passed here.
	/// \param[in] flt A pointer to a registered instance of FileListTransfer
	void SetFileListTransferPlugin(FileListTransfer *flt);

	/// This class only does the network transfers for the autopatcher.  All the data is stored in a repository.
	/// Pass the interface to your repository with this function.  RakNet comes with AutopatcherPostgreRepository if you wish to use that.
	/// \param[in] ari An implementation of the interface for the autopatcher repository.
	void SetAutopatcherRepositoryInterface(AutopatcherRepositoryInterface *ari);

	/// Clear buffered input and output
	void Clear(void);

	/// \internal For plugin handling
	virtual void OnAttach(void);
	/// \internal For plugin handling
	virtual void OnDetach(void);;
	/// \internal For plugin handling
	virtual void Update(void);
	/// \internal For plugin handling
	virtual PluginReceiveResult OnReceive(Packet *packet);
	/// \internal For plugin handling
	virtual void OnShutdown(void);
	/// \internal For plugin handling
	virtual void OnStartup(RakPeerInterface *peer);
	/// \internal For plugin handling
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );

	struct ThreadData
	{
		AutopatcherServer *server;
		RakNet::RakString applicationName;
		RakNet::RakString lastUpdateDate;
		SystemAddress systemAddress;
		FileList *clientList;
		unsigned short setId;
	};

	struct ResultTypeAndBitstream
	{
		ResultTypeAndBitstream() {patchList=0; deletedFiles=0; addedFiles=0;}
		int resultType;
		SystemAddress systemAddress;
		RakNet::BitStream bitStream1;
		RakNet::BitStream bitStream2;
		FileList *patchList;
		FileList *deletedFiles, *addedFiles;
		bool fatalError;
		unsigned short setId;
		RakNet::RakString currentDate;
		enum
		{
			GET_CHANGELIST_SINCE_DATE,
			GET_PATCH,
		} operation;
	};

protected:
	friend AutopatcherServer::ResultTypeAndBitstream* GetChangelistSinceDateCB(AutopatcherServer::ThreadData pap, bool *returnOutput, void* perThreadData);
	friend AutopatcherServer::ResultTypeAndBitstream* GetPatchCB(AutopatcherServer::ThreadData pap, bool *returnOutput, void* perThreadData);
	void OnGetChangelistSinceDate(Packet *packet);
	void OnGetPatch(Packet *packet);

	void RemoveFromThreadPool(SystemAddress systemAddress);
	AutopatcherRepositoryInterface *repository;
	FileListTransfer *fileListTransfer;
	PacketPriority priority;
	char orderingChannel;

	// The point of the threadPool is so that SQL queries, which are blocking, happen in the thread and don't slow down the rest of the application
	// The threadPool has a queue for incoming processing requests.  As systems disconnect their pending requests are removed from the list.
	ThreadPool<ThreadData, ResultTypeAndBitstream*> threadPool;
};

#endif
