#include "TableSerializer.h"
#include "LightweightDatabaseCommon.h"
#include "BitStream.h"
#include "StringCompressor.h"

void DatabaseFilter::Serialize(RakNet::BitStream *out)
{
	stringCompressor->EncodeString(columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
	out->Write((unsigned char)columnType);
	out->Write((unsigned char)operation);
	if (operation!=DataStructures::Table::QF_IS_EMPTY && operation!=DataStructures::Table::QF_NOT_EMPTY)
	{
		RakAssert(cellValue.isEmpty==false);
		TableSerializer::SerializeCell(out, &cellValue, columnType);
	}
}
bool DatabaseFilter::Deserialize(RakNet::BitStream *in)
{
	unsigned char temp;
	stringCompressor->DecodeString(columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, in);
	in->Read(temp);
	columnType=(DataStructures::Table::ColumnType)temp;
	if (in->Read(temp)==false)
		return false;
	operation=(DataStructures::Table::FilterQueryType)temp;
	if (operation!=DataStructures::Table::QF_IS_EMPTY && operation!=DataStructures::Table::QF_NOT_EMPTY)
	{
		return TableSerializer::DeserializeCell(in, &cellValue, columnType);
	}
	return true;
}
void DatabaseCellUpdate::Serialize(RakNet::BitStream *out)
{
	stringCompressor->EncodeString(columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, out);
	out->Write((unsigned char)columnType);
	TableSerializer::SerializeCell(out, &cellValue, columnType);
}
bool DatabaseCellUpdate::Deserialize(RakNet::BitStream *in)
{
	unsigned char temp;
	stringCompressor->DecodeString(columnName, _TABLE_MAX_COLUMN_NAME_LENGTH, in);
	in->Read(temp);
	columnType=(DataStructures::Table::ColumnType)temp;
	return TableSerializer::DeserializeCell(in, &cellValue, columnType);
}
