#ifndef __FILE_LIST_TRANSFER_CALLBACK_INTERFACE_H
#define __FILE_LIST_TRANSFER_CALLBACK_INTERFACE_H

#include "RakMemoryOverride.h"
#include "FileListNodeContext.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// \brief Used by FileListTransfer plugin as a callback for when we get a file.
/// You get the last file when fileIndex==setCount
/// \sa FileListTransfer
class FileListTransferCBInterface
{
public:
	struct OnFileStruct
	{
		/// The index into the set of files, from 0 to setCount
		unsigned fileIndex;

		/// The name of the file
		char fileName[512];

		/// The data pointed to by the file
		char *fileData;

		/// How many bytes this file took to send, if using compression.
		unsigned compressedTransmissionLength;

		/// The actual length of this file.
		BitSize_t finalDataLength;

		/// Files are transmitted in sets, where more than one set of files can be transmitted at the same time.
		/// This is the identifier for the set, which is returned by FileListTransfer::SetupReceive
		unsigned short setID;

		/// The number of files that are in this set.
		unsigned setCount;

		/// The total length of the transmitted files for this set, with compression.
		unsigned setTotalCompressedTransmissionLength;

		/// The total length of the transmitted files for this set, after being uncompressed
		unsigned setTotalFinalLength;

		/// User data passed to one of the functions in the FileList class.
		/// However, on error, this is instead changed to one of the enumerations in the PatchContext structure.
		FileListNodeContext context;
	};

	FileListTransferCBInterface() {}
	virtual ~FileListTransferCBInterface() {}

	/// Got a file
	/// This structure is only valid for the duration of this function call.
	/// \return Return true to have RakNet delete the memory allocated to hold this file for this function call.
	virtual bool OnFile(OnFileStruct *onFileStruct)=0;

	/// Got part of a big file.
	/// You can get these notifications by calling RakPeer::SetSplitMessageProgressInterval
	/// Otherwise you will only get complete files.
	/// \param[in] onFileStruct General information about this file, such as the filename and the first \a partLength bytes. You do NOT need to save this data yourself. The complete file will arrive normally.
	/// \param[in] partCount The zero based index into partTotal. The percentage complete done of this file is 100 * (partCount+1)/partTotal
	/// \param[in] partTotal The total number of parts this file was split into. Each part will be roughly the MTU size, minus the UDP header and RakNet headers
	/// \param[in] partLength How many bytes long firstDataChunk is
	/// \param[in] firstDataChunk The first \a partLength of the final file. If you store identifying information about the file in the first \a partLength bytes, you can read them while the download is taking place. If this hasn't arrived yet, firstDataChunk will be 0
	virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength, char *firstDataChunk) {
		(void) onFileStruct;
		(void) partCount;
		(void) partTotal;
		(void) partLength;
		(void) firstDataChunk;
	}

	/// Called while the handler is active by FileListTransfer
	/// Return false when you are done with the class
	/// At that point OnDereference will be called and the class will no longer be maintained by the FileListTransfer plugin.
	virtual bool Update(void) {return true;}

	/// Called when the download is completed.
	/// If you are finished with this class, return false
	/// At that point OnDereference will be called and the class will no longer be maintained by the FileListTransfer plugin.
	/// Otherwise return true, and Update will continue to be called.
	virtual bool OnDownloadComplete(void) {return false;}

	/// This function is called when this instance is about to be dereferenced by the FileListTransfer plugin.
	/// Update will no longer be called.
	/// It will will be deleted automatically if true was passed to FileListTransfer::SetupReceive::deleteHandler
	/// Otherwise it is up to you to delete it yourself.
	virtual void OnDereference(void) {}
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif

