/// \file
/// \brief The server plugin for the autopatcher.  Must be running for the client to get patches.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#include "AutopatcherServer.h"
#include "DirectoryDeltaTransfer.h"
#include "FileList.h"
#include "StringCompressor.h"
#include "RakPeerInterface.h"
#include "FileListTransfer.h"
#include "FileListTransferCBInterface.h"
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "AutopatcherRepositoryInterface.h"
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

AutopatcherServer::AutopatcherServer()
{
	fileListTransfer=0;
	priority=HIGH_PRIORITY;
	orderingChannel=0;
	repository=0;
}
AutopatcherServer::~AutopatcherServer()
{
	Clear();
}
void AutopatcherServer::SetUploadSendParameters(PacketPriority _priority, char _orderingChannel)
{
	priority=_priority;
	orderingChannel=_orderingChannel;
}
void AutopatcherServer::SetFileListTransferPlugin(FileListTransfer *flt)
{
	fileListTransfer=flt;
}
void AutopatcherServer::SetAutopatcherRepositoryInterface(AutopatcherRepositoryInterface *ari)
{
	RakAssert(ari);
	repository=ari;
}
void AutopatcherServer::OnAttach(void)
{
	threadPool.StartThreads(1, 0);
}
void AutopatcherServer::OnDetach(void)
{
	Clear();
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::Update(void)
{
	ResultTypeAndBitstream* rtab;
	while (threadPool.HasOutputFast() && threadPool.HasOutput())
	{
		rtab = threadPool.GetOutput();
		if (rtab->operation==ResultTypeAndBitstream::GET_PATCH)
		{
			if (rtab->fatalError==false)
			{
				if (rtab->patchList->fileList.Size())
					fileListTransfer->Send(rtab->patchList, 0, rtab->systemAddress, rtab->setId, priority, orderingChannel, false, repository);

				rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_FINISHED_INTERNAL);
				stringCompressor->EncodeString(rtab->currentDate.C_String(),64,&rtab->bitStream1);
			}
			else
			{
				rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
				stringCompressor->EncodeString(repository->GetLastError(), 256, &rtab->bitStream1);
			}
			RakNet::OP_DELETE(rtab->patchList, __FILE__, __LINE__);
		}
		else if (rtab->operation==ResultTypeAndBitstream::GET_CHANGELIST_SINCE_DATE)
		{
			if (rtab->fatalError==false)
			{
				if (rtab->deletedFiles->fileList.Size())
				{
					rtab->bitStream1.Write((unsigned char) ID_AUTOPATCHER_DELETION_LIST);
					rtab->deletedFiles->Serialize(&rtab->bitStream1);
				}

				if (rtab->addedFiles->fileList.Size())
				{
					rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_CREATION_LIST);
					rtab->addedFiles->Serialize(&rtab->bitStream2);
					stringCompressor->EncodeString(rtab->currentDate.C_String(),64,&rtab->bitStream2);
					rtab->addedFiles->Clear();
				}
				else
				{
					rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_FINISHED);
					stringCompressor->EncodeString(rtab->currentDate.C_String(),64,&rtab->bitStream2);
				}
			}
			else
			{
				rtab->bitStream2.Write((unsigned char) ID_AUTOPATCHER_REPOSITORY_FATAL_ERROR);
				stringCompressor->EncodeString(repository->GetLastError(), 256, &rtab->bitStream2);	
			}
			RakNet::OP_DELETE(rtab->deletedFiles, __FILE__, __LINE__);
			RakNet::OP_DELETE(rtab->addedFiles, __FILE__, __LINE__);
		}
		if (rtab->bitStream1.GetNumberOfBitsUsed()>0)
			SendUnified(&(rtab->bitStream1), priority, RELIABLE_ORDERED, orderingChannel, rtab->systemAddress, false);
		if (rtab->bitStream2.GetNumberOfBitsUsed()>0)
			SendUnified(&(rtab->bitStream2), priority, RELIABLE_ORDERED, orderingChannel, rtab->systemAddress, false);
		RakNet::OP_DELETE(rtab, __FILE__, __LINE__);
	}	
}
PluginReceiveResult AutopatcherServer::OnReceive(Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_AUTOPATCHER_GET_CHANGELIST_SINCE_DATE:
		OnGetChangelistSinceDate(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_AUTOPATCHER_GET_PATCH:
		OnGetPatch(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}

	return RR_CONTINUE_PROCESSING;
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnShutdown(void)
{
	Clear();
}
void AutopatcherServer::Clear(void)
{
	// Clear the waiting input and output from the thread pool.
	unsigned i;
	threadPool.StopThreads();
	for (i=0; i < threadPool.InputSize(); i++)
	{
		RakNet::OP_DELETE(threadPool.GetInputAtIndex(i).clientList, __FILE__, __LINE__);
	}
	threadPool.ClearInput();
	for (i=0; i < threadPool.OutputSize(); i++)
	{
		RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->patchList, __FILE__, __LINE__);
		RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->deletedFiles, __FILE__, __LINE__);
		RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->addedFiles, __FILE__, __LINE__);
	}
	threadPool.ClearOutput();
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnStartup(RakPeerInterface *peer)
{
}
#ifdef _MSC_VER
#pragma warning( disable : 4100 ) // warning C4100: <variable name> : unreferenced formal parameter
#endif
void AutopatcherServer::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	RemoveFromThreadPool(systemAddress);
}
void AutopatcherServer::RemoveFromThreadPool(SystemAddress systemAddress)
{
	unsigned i;
	i=0;
	threadPool.LockInput();
	while (i < threadPool.InputSize())
	{
		if (threadPool.GetInputAtIndex(i).systemAddress==systemAddress)
		{
			RakNet::OP_DELETE(threadPool.GetInputAtIndex(i).clientList, __FILE__, __LINE__);
			threadPool.RemoveInputAtIndex(i);
		}
		else
			i++;
	}
	threadPool.UnlockInput();

	i=0;
	threadPool.LockOutput();
	while (i < threadPool.OutputSize())
	{
		if (threadPool.GetOutputAtIndex(i)->systemAddress==systemAddress)
		{
			RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->patchList, __FILE__, __LINE__);
			RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->deletedFiles, __FILE__, __LINE__);
			RakNet::OP_DELETE(threadPool.GetOutputAtIndex(i)->addedFiles, __FILE__, __LINE__);
			threadPool.RemoveOutputAtIndex(i);
		}
		else
			i++;
	}
	threadPool.UnlockOutput();
}
AutopatcherServer::ResultTypeAndBitstream* GetChangelistSinceDateCB(AutopatcherServer::ThreadData threadData, bool *returnOutput, void* perThreadData)
{
	
	FileList addedFiles, deletedFiles;
	char currentDate[64];
	currentDate[0]=0;
	AutopatcherServer *server = threadData.server;

	AutopatcherServer::ResultTypeAndBitstream *rtab = RakNet::OP_NEW<AutopatcherServer::ResultTypeAndBitstream>( __FILE__, __LINE__ );
	rtab->systemAddress=threadData.systemAddress;
	rtab->deletedFiles=RakNet::OP_NEW<FileList>( __FILE__, __LINE__ );
	rtab->addedFiles=RakNet::OP_NEW<FileList>( __FILE__, __LINE__ );

	// Query the database for a changelist since this date
	RakAssert(server);
	if (server->repository->GetChangelistSinceDate(threadData.applicationName.C_String(), rtab->addedFiles, rtab->deletedFiles, threadData.lastUpdateDate.C_String(), currentDate))
	{
		rtab->fatalError=false;
	}
	else
	{
		rtab->fatalError=true;
	}

	rtab->operation=AutopatcherServer::ResultTypeAndBitstream::GET_CHANGELIST_SINCE_DATE;
	rtab->currentDate=currentDate;
	*returnOutput=true;
	return rtab;
}
void AutopatcherServer::OnGetChangelistSinceDate(Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	ThreadData threadData;
	inBitStream.IgnoreBits(8);
	inBitStream.ReadCompressed(threadData.applicationName);
	inBitStream.ReadCompressed(threadData.lastUpdateDate);

	threadData.server=this;
	threadData.systemAddress=packet->systemAddress;
	threadPool.AddInput(GetChangelistSinceDateCB, threadData);
}
AutopatcherServer::ResultTypeAndBitstream* GetPatchCB(AutopatcherServer::ThreadData threadData, bool *returnOutput, void* perThreadData)
{
	AutopatcherServer *server = threadData.server;

	AutopatcherServer::ResultTypeAndBitstream *rtab = RakNet::OP_NEW<AutopatcherServer::ResultTypeAndBitstream>( __FILE__, __LINE__ );
	rtab->systemAddress=threadData.systemAddress;
	rtab->patchList=RakNet::OP_NEW<FileList>( __FILE__, __LINE__ );
	RakAssert(server);
	RakAssert(server->repository);
	char currentDate[64];
	currentDate[0]=0;
	if (server->repository->GetPatches(threadData.applicationName.C_String(), threadData.clientList, rtab->patchList, currentDate))
		rtab->fatalError=false;
	else
		rtab->fatalError=true;
	rtab->operation=AutopatcherServer::ResultTypeAndBitstream::GET_PATCH;
	rtab->setId=threadData.setId;
	rtab->currentDate=currentDate;

	RakNet::OP_DELETE(threadData.clientList, __FILE__, __LINE__);

	*returnOutput=true;
	return rtab;
}
void AutopatcherServer::OnGetPatch(Packet *packet)
{
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	
	ThreadData threadData;
	inBitStream.IgnoreBits(8);
	inBitStream.Read(threadData.setId);
	inBitStream.ReadCompressed(threadData.applicationName);
	threadData.systemAddress=packet->systemAddress;
	threadData.server=this;
	threadData.clientList=RakNet::OP_NEW<FileList>( __FILE__, __LINE__ );

	if (threadData.clientList->Deserialize(&inBitStream)==false)
	{
		RakNet::OP_DELETE(threadData.clientList, __FILE__, __LINE__);
		return;
	}
	if (threadData.clientList->fileList.Size()==0)
	{
		RakAssert(0);
		RakNet::OP_DELETE(threadData.clientList, __FILE__, __LINE__);
		return;
	}

	threadPool.AddInput(GetPatchCB, threadData);
	return;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
