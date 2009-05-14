#include "LightweightDatabaseClient.h"
#include "StringCompressor.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "TableSerializer.h"
#include "BitStream.h"
#include "RakAssert.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

LightweightDatabaseClient::LightweightDatabaseClient()
{
}
LightweightDatabaseClient::~LightweightDatabaseClient()
{

}
void LightweightDatabaseClient::QueryTable(const char *tableName, const char *queryPassword, const char **columnNamesSubset, unsigned char numColumnSubset, DatabaseFilter *filter, unsigned char numFilters, unsigned *rowIds, unsigned char numRowIDs, SystemAddress systemAddress, bool broadcast)
{
	if (tableName==0 || tableName[0]==0)
		return;
	if (rakPeerInterface==0)
		return;

	RakNet::BitStream out;
	out.Write((MessageID)ID_DATABASE_QUERY_REQUEST);
	stringCompressor->EncodeString(tableName, _SIMPLE_DATABASE_TABLE_NAME_LENGTH, &out);
	if (queryPassword && queryPassword[0])
	{
        out.Write(true);
		// This is sent in plain text.  I can do this securely but it's not worth the trouble.
		// Use secure connections if you want security.
		stringCompressor->EncodeString(queryPassword, _SIMPLE_DATABASE_PASSWORD_LENGTH, &out);
	}
	else
		out.Write(false);

	out.Write(numColumnSubset);
	unsigned i;
	for (i=0; i < numColumnSubset; i++)
	{
		stringCompressor->EncodeString(columnNamesSubset[i],256,&out);
	}

	out.Write(numFilters);
	for (i=0; i < numFilters; i++)
	{
		RakAssert((int)filter[i].operation<=(int)DataStructures::Table::QF_NOT_EMPTY);
		filter[i].Serialize(&out);
	}

	out.Write(numRowIDs);
	for (i=0; i < numRowIDs; i++)
		out.Write(rowIds[i]);

	SendUnified(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,systemAddress, broadcast);
}
void LightweightDatabaseClient::RemoveRow(const char *tableName, const char *removePassword, unsigned rowId, SystemAddress systemAddress, bool broadcast)
{
	if (tableName==0 || tableName[0]==0)
		return;
	if (rakPeerInterface==0)
		return;

	RakNet::BitStream out;
	out.Write((MessageID)ID_DATABASE_REMOVE_ROW);
	stringCompressor->EncodeString(tableName, _SIMPLE_DATABASE_TABLE_NAME_LENGTH, &out);
	if (removePassword && removePassword[0])
	{
		out.Write(true);
		// This is sent in plain text.  I can do this securely but it's not worth the trouble.
		// Use secure connections if you want security.
		stringCompressor->EncodeString(removePassword, _SIMPLE_DATABASE_PASSWORD_LENGTH, &out);
	}
	else
		out.Write(false);

	out.Write(rowId);

	SendUnified(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,systemAddress, broadcast);
}
void LightweightDatabaseClient::UpdateRow(const char *tableName, const char *updatePassword, RowUpdateMode updateMode, bool hasRowId, unsigned rowId, DatabaseCellUpdate *cellUpdates, unsigned char numCellUpdates, SystemAddress systemAddress, bool broadcast)
{
	if (tableName==0 || tableName[0]==0)
		return;
	if (rakPeerInterface==0)
		return;
	if (cellUpdates==0 || numCellUpdates==0)
		return;

	RakNet::BitStream out;
	out.Write((MessageID)ID_DATABASE_UPDATE_ROW);
	stringCompressor->EncodeString(tableName, _SIMPLE_DATABASE_TABLE_NAME_LENGTH, &out);
	if (updatePassword && updatePassword[0])
	{
		out.Write(true);
		// This is sent in plain text.  I can do this securely but it's not worth the trouble.
		// Use secure connections if you want security.
		stringCompressor->EncodeString(updatePassword, _SIMPLE_DATABASE_PASSWORD_LENGTH, &out);
	}
	else
		out.Write(false);

	out.Write((unsigned char) updateMode);
	out.Write(hasRowId);
	if (hasRowId)
		out.Write(rowId);
	out.Write(numCellUpdates);
	unsigned i;
	for (i=0; i < numCellUpdates; i++)
		cellUpdates[i].Serialize(&out);

	SendUnified(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,systemAddress, broadcast);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
