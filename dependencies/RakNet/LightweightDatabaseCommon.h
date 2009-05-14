#ifndef __SIMPLE_DATABASE_COMMON_H
#define  __SIMPLE_DATABASE_COMMON_H

#include "DS_Table.h"

namespace RakNet
{
	class BitStream;
};

#define _SIMPLE_DATABASE_PASSWORD_LENGTH 32
#define _SIMPLE_DATABASE_TABLE_NAME_LENGTH 32

#define SYSTEM_ID_COLUMN_NAME "__SystemAddress"
#define SYSTEM_GUID_COLUMN_NAME "__SystemGuid"
#define LAST_PING_RESPONSE_COLUMN_NAME "__lastPingResponseTime"
#define NEXT_PING_SEND_COLUMN_NAME "__nextPingSendTime"

struct DatabaseFilter
{
	void Serialize(RakNet::BitStream *out);
	bool Deserialize(RakNet::BitStream *in);

	DataStructures::Table::Cell cellValue;
	DataStructures::Table::FilterQueryType operation;
	DataStructures::Table::ColumnType columnType;
	char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
};

/// The value to write to a cell in a remote database.
struct DatabaseCellUpdate
{
	void Serialize(RakNet::BitStream *out);
	bool Deserialize(RakNet::BitStream *in);

	DataStructures::Table::Cell cellValue;
	DataStructures::Table::ColumnType columnType;
	char columnName[_TABLE_MAX_COLUMN_NAME_LENGTH];
};

enum RowUpdateMode
{
	// Only update an existing row.  rowId is required.
	RUM_UPDATE_EXISTING_ROW,

	// Update an existing row if present - otherwise add a new row.  rowId is required.
	RUM_UPDATE_OR_ADD_ROW,

	// Add a new row.  If rowId is passed then the row will only be added if this id doesn't already exist.
	// If rowId is not passed and the table requires a rowId the creation fails.
	RUM_ADD_NEW_ROW,
};

#endif
