#include "FileListTransfer.h"
#include "DS_HuffmanEncodingTree.h"
#include "FileListTransferCBInterface.h"
#include "StringCompressor.h"
#include "FileList.h"
#include "DS_Queue.h"
#include "MessageIdentifiers.h"
#include "RakNetTypes.h"
#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "IncrementalReadInterface.h"
#include "RakAssert.h"
#include "RakAlloca.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

struct FLR_MemoryBlock
{
	unsigned int allocatedLength;
	char *block;
};

struct FileListReceiver
{
	FileListReceiver();
	~FileListReceiver();
	FileListTransferCBInterface *downloadHandler;
	SystemAddress allowedSender;
	unsigned short setID;
	unsigned setCount;
	unsigned setTotalCompressedTransmissionLength;
	unsigned setTotalFinalLength;
	bool gotSetHeader;
	bool deleteDownloadHandler;
	bool isCompressed;
	int  filesReceived;
	DataStructures::Map<unsigned int, FLR_MemoryBlock> pushedFiles;

	// Notifications
	unsigned int partLength;

};

FileListReceiver::FileListReceiver() {filesReceived=0; partLength=1; DataStructures::Map<unsigned int, FLR_MemoryBlock>::IMPLEMENT_DEFAULT_COMPARISON();}
FileListReceiver::~FileListReceiver() {
	unsigned int i=0;
	for (i=0; i < pushedFiles.Size(); i++)
		rakFree_Ex(pushedFiles[i].block, __FILE__, __LINE__ );
}

FileListTransfer::FileListTransfer()
{
	setId=0;
	callback=0;
	DataStructures::Map<unsigned short, FileListReceiver*>::IMPLEMENT_DEFAULT_COMPARISON();
}
FileListTransfer::~FileListTransfer()
{
	Clear();
}
unsigned short FileListTransfer::SetupReceive(FileListTransferCBInterface *handler, bool deleteHandler, SystemAddress allowedSender)
{
	if (rakPeerInterface && rakPeerInterface->IsConnected(allowedSender)==false)
		return (unsigned short)-1;
	FileListReceiver *receiver;

	if (fileListReceivers.Has(setId))
	{
		receiver=fileListReceivers.Get(setId);
		receiver->downloadHandler->OnDereference();
		if (receiver->deleteDownloadHandler)
			RakNet::OP_DELETE(receiver->downloadHandler, __FILE__, __LINE__);
		RakNet::OP_DELETE(receiver, __FILE__, __LINE__);
		fileListReceivers.Delete(setId);
	}

	unsigned short oldId;
	receiver = RakNet::OP_NEW<FileListReceiver>( __FILE__, __LINE__ );
	RakAssert(handler);
	receiver->downloadHandler=handler;
	receiver->allowedSender=allowedSender;
	receiver->gotSetHeader=false;
	receiver->deleteDownloadHandler=deleteHandler;
	fileListReceivers.Set(setId, receiver);
	oldId=setId;
	if (++setId==(unsigned short)-1)
		setId=0;
	return oldId;
}

void FileListTransfer::Send(FileList *fileList, RakPeerInterface *rakPeer, SystemAddress recipient, unsigned short setID, PacketPriority priority, char orderingChannel, bool compressData, IncrementalReadInterface *_incrementalReadInterface, unsigned int _chunkSize)
{
	(void) compressData;

	if (callback)
		fileList->SetCallback(callback);

	unsigned int i, totalLength;
	RakNet::BitStream outBitstream;
	bool sendReference;
	char *dataBlocks[2];
	int lengths[2];
	totalLength=0;
	for (i=0; i < fileList->fileList.Size(); i++)
	{
		const FileListNode &fileListNode = fileList->fileList[i];
		totalLength+=fileListNode.fileLengthBytes;
	}

	// Write the chunk header, which contains the frequency table, the total number of files, and the total number of bytes
	bool anythingToWrite;
	outBitstream.Write((MessageID)ID_FILE_LIST_TRANSFER_HEADER);
	outBitstream.Write(setID);
	anythingToWrite=fileList->fileList.Size()>0;
	outBitstream.Write(anythingToWrite);
	if (anythingToWrite)
	{
		outBitstream.WriteCompressed(fileList->fileList.Size());
		outBitstream.WriteCompressed(totalLength);

		if (rakPeer)
			rakPeer->Send(&outBitstream, priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
		else
			SendUnified(&outBitstream, priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
	
		for (i=0; i < fileList->fileList.Size(); i++)
		{
			outBitstream.Reset();
			sendReference = fileList->fileList[i].isAReference && _incrementalReadInterface!=0;
			if (sendReference)
			{
				StoreForPush(fileList->fileList[i].context, setID, fileList->fileList[i].filename, fileList->fileList[i].fullPathToFile, i, fileList->fileList[i].fileLengthBytes, fileList->fileList[i].dataLengthBytes, recipient, priority, orderingChannel, _incrementalReadInterface, _chunkSize);
				continue;
			}

			outBitstream.Write((MessageID)ID_FILE_LIST_TRANSFER_FILE);
			
			outBitstream.Write(fileList->fileList[i].context);
			outBitstream.Write(setID);
			stringCompressor->EncodeString(fileList->fileList[i].filename, 512, &outBitstream);
			
			outBitstream.WriteCompressed(i);
			outBitstream.WriteCompressed(fileList->fileList[i].dataLengthBytes); // Original length in bytes

			outBitstream.AlignWriteToByteBoundary();

			dataBlocks[0]=(char*) outBitstream.GetData();
			lengths[0]=outBitstream.GetNumberOfBytesUsed();
			dataBlocks[1]=fileList->fileList[i].data;
			lengths[1]=fileList->fileList[i].dataLengthBytes;
			if (rakPeer)
				rakPeer->SendList(dataBlocks,lengths,2,priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
			else
				SendListUnified(dataBlocks,lengths,2,priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
		}
	}
	else
	{
		if (rakPeer)
			rakPeer->Send(&outBitstream, priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
		else
			SendUnified(&outBitstream, priority, RELIABLE_ORDERED, orderingChannel, recipient, false);
	}
}

bool FileListTransfer::DecodeSetHeader(Packet *packet)
{
	bool anythingToWrite=false;
	unsigned short setID;
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);
	inBitStream.Read(setID);
	FileListReceiver *fileListReceiver;
	if (fileListReceivers.Has(setID)==false)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return false;
	}
	fileListReceiver=fileListReceivers.Get(setID);
	if (fileListReceiver->allowedSender!=packet->systemAddress)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return false;
	}

#ifdef _DEBUG
	RakAssert(fileListReceiver->gotSetHeader==false);
#endif

	inBitStream.Read(anythingToWrite);

	if (anythingToWrite)
	{
		inBitStream.ReadCompressed(fileListReceiver->setCount);
		if (inBitStream.ReadCompressed(fileListReceiver->setTotalFinalLength))
		{
			fileListReceiver->setTotalCompressedTransmissionLength=fileListReceiver->setTotalFinalLength;
			fileListReceiver->gotSetHeader=true;
			return true;
		}

	}
	else
	{
		FileListTransferCBInterface::OnFileStruct s;
		memset(&s,0,sizeof(FileListTransferCBInterface::OnFileStruct));
		s.setID=setID;

		if (fileListReceiver->downloadHandler->OnDownloadComplete()==false)
		{
			fileListReceiver->downloadHandler->OnDereference();
			fileListReceivers.Delete(setID);
			if (fileListReceiver->deleteDownloadHandler)
				RakNet::OP_DELETE(fileListReceiver->downloadHandler, __FILE__, __LINE__);
			RakNet::OP_DELETE(fileListReceiver, __FILE__, __LINE__);
		}

		return true;
	}

	return false;
}

bool FileListTransfer::DecodeFile(Packet *packet, bool fullFile)
{
	FileListTransferCBInterface::OnFileStruct onFileStruct;
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);

	unsigned int partCount=0;
	unsigned int partTotal=0;
	unsigned int partLength=0;
	onFileStruct.fileData=0;
	if (fullFile==false)
	{
		// Disable endian swapping on reading this, as it's generated locally in ReliabilityLayer.cpp
		inBitStream.ReadBits( (unsigned char* ) &partCount, BYTES_TO_BITS(sizeof(partCount)), true );
		inBitStream.ReadBits( (unsigned char* ) &partTotal, BYTES_TO_BITS(sizeof(partTotal)), true );
		inBitStream.ReadBits( (unsigned char* ) &partLength, BYTES_TO_BITS(sizeof(partLength)), true );
		inBitStream.IgnoreBits(8);
		// The header is appended to every chunk, which we continue to read after this statement block
	}
	inBitStream.Read(onFileStruct.context);
	inBitStream.Read(onFileStruct.setID);
	FileListReceiver *fileListReceiver;
	if (fileListReceivers.Has(onFileStruct.setID)==false)
	{
		return false;
	}
	fileListReceiver=fileListReceivers.Get(onFileStruct.setID);
	if (fileListReceiver->allowedSender!=packet->systemAddress)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return false;
	}

#ifdef _DEBUG
	RakAssert(fileListReceiver->gotSetHeader==true);
#endif

	if (stringCompressor->DecodeString(onFileStruct.fileName, 512, &inBitStream)==false)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return false;
	}

	inBitStream.ReadCompressed(onFileStruct.fileIndex);
	inBitStream.ReadCompressed(onFileStruct.finalDataLength);

	// Read header uncompressed so the data is byte aligned, for speed
	onFileStruct.compressedTransmissionLength=(unsigned int) onFileStruct.finalDataLength;

	if (fullFile)
	{
		// Support SendLists
		inBitStream.AlignReadToByteBoundary();

		onFileStruct.fileData = (char*) rakMalloc_Ex( (size_t) onFileStruct.finalDataLength, __FILE__, __LINE__ );

		inBitStream.Read((char*)onFileStruct.fileData, onFileStruct.finalDataLength);
	}
	

	onFileStruct.setCount=fileListReceiver->setCount;
	onFileStruct.setTotalCompressedTransmissionLength=fileListReceiver->setTotalCompressedTransmissionLength;
	onFileStruct.setTotalFinalLength=fileListReceiver->setTotalFinalLength;

	// User callback for this file.
	if (fullFile)
	{
		if (fileListReceiver->downloadHandler->OnFile(&onFileStruct))
			rakFree_Ex(onFileStruct.fileData, __FILE__, __LINE__ );

		fileListReceiver->filesReceived++;

		// If this set is done, free the memory for it.
		if ((int) fileListReceiver->setCount==fileListReceiver->filesReceived)
		{
			if (fileListReceiver->downloadHandler->OnDownloadComplete()==false)
			{
				fileListReceiver->downloadHandler->OnDereference();
				if (fileListReceiver->deleteDownloadHandler)
					RakNet::OP_DELETE(fileListReceiver->downloadHandler, __FILE__, __LINE__);
				fileListReceivers.Delete(onFileStruct.setID);
				RakNet::OP_DELETE(fileListReceiver, __FILE__, __LINE__);
			}
		}

	}
	else
	{
		inBitStream.AlignReadToByteBoundary();

		bool usedAlloca=false;
		char *firstDataChunk;
		unsigned int unreadBits = inBitStream.GetNumberOfUnreadBits();
		unsigned int unreadBytes = BITS_TO_BYTES(unreadBits);
		if (unreadBytes>0)
		{
#if !defined(_XBOX) && !defined(_X360)
			if (unreadBits < MAX_ALLOCA_STACK_ALLOCATION)
			{
				firstDataChunk = ( char* ) alloca( unreadBytes );
				usedAlloca=true;
			}
			else
#endif
				firstDataChunk = (char*) rakMalloc_Ex( unreadBytes, __FILE__, __LINE__ );

			// Read partLength bytes, reported to OnFileProgress

			inBitStream.Read((char*)firstDataChunk, unreadBytes );

		}
		else
			firstDataChunk=0;

		fileListReceiver->downloadHandler->OnFileProgress(&onFileStruct, partCount, partTotal, unreadBytes, firstDataChunk);

		if (usedAlloca==false)
			RakNet::OP_DELETE_ARRAY(firstDataChunk, __FILE__, __LINE__);
	}

	return true;
}
PluginReceiveResult FileListTransfer::OnReceive(Packet *packet)
{
	switch (packet->data[0]) 
	{
	case ID_FILE_LIST_TRANSFER_HEADER:
		DecodeSetHeader(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FILE_LIST_TRANSFER_FILE:
		DecodeFile(packet, true);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FILE_LIST_REFERENCE_PUSH:
		OnReferencePush(packet, true);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_FILE_LIST_REFERENCE_PUSH_ACK:
		OnReferencePushAck(packet);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
	case ID_DOWNLOAD_PROGRESS:
		if (packet->length>sizeof(MessageID)+sizeof(unsigned int)*3)
		{
			if (packet->data[sizeof(MessageID)+sizeof(unsigned int)*3]==ID_FILE_LIST_TRANSFER_FILE)
			{
				DecodeFile(packet, false);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
			if (packet->data[sizeof(MessageID)+sizeof(unsigned int)*3]==ID_FILE_LIST_REFERENCE_PUSH)
			{
				OnReferencePush(packet, false);
				return RR_STOP_PROCESSING_AND_DEALLOCATE;
			}
		}
		break;
	}

	return RR_CONTINUE_PROCESSING;
}
void FileListTransfer::OnShutdown(void)
{
	Clear();	
}
void FileListTransfer::Clear(void)
{
	unsigned i;
	for (i=0; i < fileListReceivers.Size(); i++)
	{
		fileListReceivers[i]->downloadHandler->OnDereference();
		if (fileListReceivers[i]->deleteDownloadHandler)
			RakNet::OP_DELETE(fileListReceivers[i]->downloadHandler, __FILE__, __LINE__);
		RakNet::OP_DELETE(fileListReceivers[i], __FILE__, __LINE__);
	}
	fileListReceivers.Clear();

	for (i=0; i < filesToPush.Size(); i++)
	{
		RakNet::OP_DELETE_ARRAY(filesToPush[i].fileListNode.filename, __FILE__, __LINE__);
	}
	filesToPush.Clear();
}
void FileListTransfer::OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
	(void) lostConnectionReason;
	(void) rakNetGUID;

	RemoveReceiver(systemAddress);
}
void FileListTransfer::CancelReceive(unsigned short setId)
{
	if (fileListReceivers.Has(setId)==false)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return;
	}
	FileListReceiver *fileListReceiver=fileListReceivers.Get(setId);
	fileListReceiver->downloadHandler->OnDereference();
	if (fileListReceiver->deleteDownloadHandler)
		RakNet::OP_DELETE(fileListReceiver->downloadHandler, __FILE__, __LINE__);
	RakNet::OP_DELETE(fileListReceiver, __FILE__, __LINE__);
	fileListReceivers.Delete(setId);
}
void FileListTransfer::RemoveReceiver(SystemAddress systemAddress)
{
	unsigned i;
	i=0;
	while (i < fileListReceivers.Size())
	{
		if (fileListReceivers[i]->allowedSender==systemAddress)
		{
			fileListReceivers[i]->downloadHandler->OnDereference();
			if (fileListReceivers[i]->deleteDownloadHandler)
				RakNet::OP_DELETE(fileListReceivers[i]->downloadHandler, __FILE__, __LINE__);
			RakNet::OP_DELETE(fileListReceivers[i], __FILE__, __LINE__);
			fileListReceivers.RemoveAtIndex(i);
		}
		else
			i++;
	}

	i=0;
	while (i < filesToPush.Size())
	{
		if (filesToPush[i].systemAddress==systemAddress)
		{
			RakNet::OP_DELETE_ARRAY(filesToPush[i].fileListNode.filename, __FILE__, __LINE__);
			filesToPush.RemoveAtIndex(i);
		}
		else
			i++;
	}
	
}
bool FileListTransfer::IsHandlerActive(unsigned short setId)
{
	return fileListReceivers.Has(setId);
}
void FileListTransfer::SetCallback(FileListProgress *cb)
{
	callback=cb;
}
FileListProgress *FileListTransfer::GetCallback(void) const
{
	return callback;
}
void FileListTransfer::Update(void)
{
	unsigned i;
	i=0;
	while (i < fileListReceivers.Size())
	{
		if (fileListReceivers[i]->downloadHandler->Update()==false)
		{
			fileListReceivers[i]->downloadHandler->OnDereference();
			if (fileListReceivers[i]->deleteDownloadHandler)
				RakNet::OP_DELETE(fileListReceivers[i]->downloadHandler, __FILE__, __LINE__);
			RakNet::OP_DELETE(fileListReceivers[i], __FILE__, __LINE__);
			fileListReceivers.RemoveAtIndex(i);
		}
		else
			i++;
	}

}
void FileListTransfer::OnReferencePush(Packet *packet, bool fullFile)
{
	// fullFile is always true for TCP, since TCP does not return SPLIT_PACKET_NOTIFICATION

	RakNet::BitStream refPushAck;
	refPushAck.Write((MessageID)ID_FILE_LIST_REFERENCE_PUSH_ACK);
	SendUnified(&refPushAck,HIGH_PRIORITY, RELIABLE, 0, packet->systemAddress, false);

	FileListTransferCBInterface::OnFileStruct onFileStruct;
	RakNet::BitStream inBitStream(packet->data, packet->length, false);
	inBitStream.IgnoreBits(8);

	unsigned int partCount=0;
	unsigned int partTotal=1;
	unsigned int partLength=0;
	onFileStruct.fileData=0;
	if (fullFile==false)
	{
		// Disable endian swapping on reading this, as it's generated locally in ReliabilityLayer.cpp
		inBitStream.ReadBits( (unsigned char* ) &partCount, BYTES_TO_BITS(sizeof(partCount)), true );
		inBitStream.ReadBits( (unsigned char* ) &partTotal, BYTES_TO_BITS(sizeof(partTotal)), true );
		inBitStream.ReadBits( (unsigned char* ) &partLength, BYTES_TO_BITS(sizeof(partLength)), true );
		inBitStream.IgnoreBits(8);
		// The header is appended to every chunk, which we continue to read after this statement block
	}

	inBitStream.Read(onFileStruct.context);
	inBitStream.Read(onFileStruct.setID);
	FileListReceiver *fileListReceiver;
	if (fileListReceivers.Has(onFileStruct.setID)==false)
	{
		return;
	}
	fileListReceiver=fileListReceivers.Get(onFileStruct.setID);
	if (fileListReceiver->allowedSender!=packet->systemAddress)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return;
	}

#ifdef _DEBUG
	RakAssert(fileListReceiver->gotSetHeader==true);
#endif

	if (stringCompressor->DecodeString(onFileStruct.fileName, 512, &inBitStream)==false)
	{
#ifdef _DEBUG
		RakAssert(0);
#endif
		return;
	}

	inBitStream.ReadCompressed(onFileStruct.fileIndex);
	inBitStream.ReadCompressed(onFileStruct.finalDataLength);
	unsigned int offset;
	unsigned int chunkLength;
	inBitStream.ReadCompressed(offset);
	inBitStream.ReadCompressed(chunkLength);
//	if (chunkLength==0)
//		return;
	bool lastChunk;
	inBitStream.Read(lastChunk);
	bool finished = lastChunk && fullFile;

	if (fullFile==false)
		fileListReceiver->partLength=partLength;

	FLR_MemoryBlock mb;
	if (fileListReceiver->pushedFiles.Has(onFileStruct.fileIndex)==false)
	{
		if (chunkLength > 1000000000 || onFileStruct.finalDataLength > 1000000000)
		{
			RakAssert("FileListTransfer::OnReferencePush: file too large" && 0);
			return;
		}

		mb.allocatedLength=onFileStruct.finalDataLength;
		mb.block = (char*) rakMalloc_Ex(onFileStruct.finalDataLength, __FILE__, __LINE__);
		if (mb.block==0)
		{
			notifyOutOfMemory(__FILE__, __LINE__);
			return;
		}

		fileListReceiver->pushedFiles.SetNew(onFileStruct.fileIndex, mb);
	}
	else
		mb=fileListReceiver->pushedFiles.Get(onFileStruct.fileIndex);

	if (offset+chunkLength > mb.allocatedLength)
	{
		// Overrun
		RakAssert("FileListTransfer::OnReferencePush: Write would overrun allocated block" && 0);
		return;
	}

	// Read header uncompressed so the data is byte aligned, for speed
	onFileStruct.compressedTransmissionLength=(unsigned int) onFileStruct.finalDataLength;

	unsigned int unreadBits = inBitStream.GetNumberOfUnreadBits();
	unsigned int unreadBytes = BITS_TO_BYTES(unreadBits);
	unsigned int amountToRead;
	if (fullFile)
		amountToRead=chunkLength;
	else
		amountToRead=unreadBytes;

	inBitStream.AlignReadToByteBoundary();
	if (fullFile || (rakPeerInterface->GetSplitMessageProgressInterval() != 0 && (int)partCount==rakPeerInterface->GetSplitMessageProgressInterval()))
	{
		inBitStream.Read(mb.block+offset, amountToRead);
	}

	onFileStruct.setCount=fileListReceiver->setCount;
	onFileStruct.setTotalCompressedTransmissionLength=fileListReceiver->setTotalCompressedTransmissionLength;
	onFileStruct.setTotalFinalLength=fileListReceiver->setTotalFinalLength;
	onFileStruct.fileData=mb.block;

	if (finished)
	{
		if (fileListReceiver->downloadHandler->OnFile(&onFileStruct))
			rakFree_Ex(onFileStruct.fileData, __FILE__, __LINE__ );
		fileListReceiver->pushedFiles.Delete(onFileStruct.fileIndex);

		fileListReceiver->filesReceived++;

		// If this set is done, free the memory for it.
		if ((int) fileListReceiver->setCount==fileListReceiver->filesReceived)
		{
			if (fileListReceiver->downloadHandler->OnDownloadComplete()==false)
			{
				fileListReceiver->downloadHandler->OnDereference();
				fileListReceivers.Delete(onFileStruct.setID);
				if (fileListReceiver->deleteDownloadHandler)
					RakNet::OP_DELETE(fileListReceiver->downloadHandler, __FILE__, __LINE__);
				RakNet::OP_DELETE(fileListReceiver, __FILE__, __LINE__);
			}
		}
	}
	else
	{
		unsigned int totalNotifications;
		unsigned int currentNotificationIndex;
		unsigned int unreadBytes;

		if (rakPeerInterface==0 || rakPeerInterface->GetSplitMessageProgressInterval()==0)
		{
			totalNotifications = onFileStruct.finalDataLength / chunkLength + 1;
			currentNotificationIndex = offset / chunkLength;
			unreadBytes = mb.allocatedLength - ((offset+1)*chunkLength);
		}
		else
		{
			totalNotifications = onFileStruct.finalDataLength / fileListReceiver->partLength + 1;
			if (fullFile==false)
				currentNotificationIndex = (offset+partCount*fileListReceiver->partLength) / fileListReceiver->partLength ;
			else
				currentNotificationIndex = (offset+chunkLength) / fileListReceiver->partLength ;
			unreadBytes = onFileStruct.finalDataLength - ((currentNotificationIndex+1) * fileListReceiver->partLength);
		}
		fileListReceiver->downloadHandler->OnFileProgress(&onFileStruct, currentNotificationIndex, totalNotifications, unreadBytes, mb.block);
	}

	return;
}
void FileListTransfer::PushReference(SystemAddress systemAddress)
{
	// Was previously using GetStatistics to get outgoing buffer size, but TCP with UnifiedSend doesn't have this
	unsigned int i=0;
	unsigned int bytesRead;	
	char *dataBlocks[2];
	int lengths[2];
	RakNet::BitStream outBitstream;
	while (i < filesToPush.Size())
	{
		if (filesToPush[i].systemAddress==systemAddress)
		{
			outBitstream.Reset();
			outBitstream.Write((MessageID)ID_FILE_LIST_REFERENCE_PUSH);
			outBitstream.Write(filesToPush[i].fileListNode.context);
			outBitstream.Write(filesToPush[i].setID);
			stringCompressor->EncodeString(filesToPush[i].fileListNode.filename, 512, &outBitstream);
			outBitstream.WriteCompressed(filesToPush[i].setIndex);
			outBitstream.WriteCompressed(filesToPush[i].fileListNode.dataLengthBytes); // Original length in bytes

			// Read and send chunk. If done, delete at this index
			void *buff = rakMalloc_Ex(filesToPush[i].chunkSize, __FILE__, __LINE__);
			if (buff==0)
			{
				notifyOutOfMemory(__FILE__, __LINE__);
				continue;
			}
			bytesRead=filesToPush[i].incrementalReadInterface->GetFilePart(filesToPush[i].fileListNode.fullPathToFile, filesToPush[i].currentOffset, filesToPush[i].chunkSize, buff, filesToPush[i].fileListNode.context);
			outBitstream.WriteCompressed(filesToPush[i].currentOffset);
			filesToPush[i].currentOffset+=bytesRead;
			outBitstream.WriteCompressed(bytesRead);
			bool done = bytesRead!=filesToPush[i].chunkSize;
			outBitstream.Write(done);

			if (callback)
			{
				callback->OnFilePush(filesToPush[i].fileListNode.filename, filesToPush[i].fileListNode.fileLengthBytes, filesToPush[i].currentOffset-bytesRead, bytesRead, done, filesToPush[i].systemAddress);
			}

			dataBlocks[0]=(char*) outBitstream.GetData();
			lengths[0]=outBitstream.GetNumberOfBytesUsed();
			dataBlocks[1]=(char*) buff;
			lengths[1]=bytesRead;
			//rakPeerInterface->SendList(dataBlocks,lengths,2,filesToPush[i].packetPriority, RELIABLE_ORDERED, filesToPush[i].orderingChannel, filesToPush[i].systemAddress, false);
			SendListUnified(dataBlocks,lengths,2,filesToPush[i].packetPriority, RELIABLE_ORDERED, filesToPush[i].orderingChannel, filesToPush[i].systemAddress, false);
			if (done)
			{
				// Done
				RakNet::OP_DELETE_ARRAY(filesToPush[i].fileListNode.filename, __FILE__, __LINE__);
				filesToPush.RemoveAtIndex(i);
			}
			rakFree_Ex(buff, __FILE__, __LINE__ );
			return;
		}			
		else
			i++;
	}	
}
void FileListTransfer::OnReferencePushAck(Packet *packet)
{
	PushReference(packet->systemAddress);
}

void FileListTransfer::StoreForPush(FileListNodeContext context, unsigned short _setID, const char *fileName, const char *fullPathToFile, unsigned int _setIndex, unsigned fileLengthBytes, unsigned dataLengthBytes, SystemAddress recipient, PacketPriority packetPriority, char orderingChannel, IncrementalReadInterface *_incrementalReadInterface, unsigned int _chunkSize)
{
	FileToPush fileToPush;
	fileToPush.fileListNode.context=context;
	fileToPush.setIndex=_setIndex;
	fileToPush.fileListNode.filename=RakNet::OP_NEW_ARRAY<char>((const int) strlen(fileName)+1, __FILE__, __LINE__ );	
	strcpy(fileToPush.fileListNode.filename, fileName);
	fileToPush.fileListNode.fullPathToFile=RakNet::OP_NEW_ARRAY<char>((const int) strlen(fullPathToFile)+1, __FILE__, __LINE__ );	
	strcpy(fileToPush.fileListNode.fullPathToFile, fullPathToFile);
	fileToPush.fileListNode.fileLengthBytes=fileLengthBytes;
	fileToPush.fileListNode.dataLengthBytes=dataLengthBytes;
	fileToPush.systemAddress=recipient;
	fileToPush.setID=_setID;
	fileToPush.packetPriority=packetPriority;
	fileToPush.orderingChannel=orderingChannel;
	fileToPush.currentOffset=0;
	fileToPush.incrementalReadInterface=_incrementalReadInterface;
	fileToPush.chunkSize=_chunkSize;

	// If nothing is transferring, start the process
	for (unsigned int i=0; i < filesToPush.Size(); i++)
	{
		if (filesToPush[i].systemAddress==recipient)
		{
			filesToPush.Insert(fileToPush, __FILE__, __LINE__);
			return;
		}
	}
	filesToPush.Insert(fileToPush, __FILE__, __LINE__);
	PushReference(recipient);

}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
