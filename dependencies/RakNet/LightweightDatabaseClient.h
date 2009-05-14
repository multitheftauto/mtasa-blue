/// \file
/// \brief Contains the client interface to the simple database included with RakNet, useful for a server browser or a lobby server.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __LIGHTWEIGHT_DATABASE_CLIENT_H
#define __LIGHTWEIGHT_DATABASE_CLIENT_H

#include "Export.h"
#include "PluginInterface2.h"
#include "LightweightDatabaseCommon.h"

class RakPeerInterface;
struct Packet;

/// \defgroup SIMPLE_DATABSE_GROUP LightweightDatabase
/// \ingroup PLUGINS_GROUP

/// \brief The client interface to the simple database included with RakNet, useful for a server browser or a lobby server.
/// \ingroup SIMPLE_DATABSE_GROUP
class RAK_DLL_EXPORT LightweightDatabaseClient : public PluginInterface2
{
public:

	/// Constructor
	LightweightDatabaseClient();

	/// Destructor
	virtual ~LightweightDatabaseClient();

	/// Sends a query to a remote table.
	/// The remote system should return ID_DATABASE_QUERY_REPLY, ID_DATABASE_UNKNOWN_TABLE, or ID_DATABASE_INCORRECT_PASSWORD
	/// \note If the remote table uses a password and you send the wrong one you will be silently disconnected and banned for one second.
	/// \param[in] tableName String name of the remote table. Case sensitive.
	/// \param[in] queryPassword Password to query the remote table, if any.
	/// \param[in] columnSubset An array of string names of the columns to query.  The resulting table will return the columns in this same order.  Pass 0 for all columns.
	/// \param[in] numColumnSubset The number of elements in the columnSubset array
	/// \param[in] filter Filters to apply to the query. For each row the filters are applied.  All filters must be passed for the row to be returned.  Pass 0 for no filters.
	/// \param[in] numFilters The number of elements in the filter array
	/// \param[in] rowIds Which particular rows to return.  Pass 0 for all rows.
	/// \param[in] numRowIDs The number of elements in the rowIds array
	/// \param[in] systemAddress Which system to send to.
	/// \param[in] broadcast Broadcast or not.  Same as the parameter in RakPeer::Send
	void QueryTable(const char *tableName, const char *queryPassword, const char **columnNamesSubset, unsigned char numColumnSubset, DatabaseFilter *filter, unsigned char numFilters, unsigned *rowIds, unsigned char numRowIDs, SystemAddress systemAddress, bool broadcast);

	/// Sets one or more values in a new or existing row, assuming the server allows row creation and updates.
	/// No response is returned by the server.
	/// \param[in] tableName String name of the remote table. Case sensitive.
	/// \param[in] updatePassword Password to update the remote table, if any.
	/// \param[in] updateMode See RowUpdateMode in LightweightDatabaseCommon.h .  This determines if to update an existing or new row.
	/// \param[in] hasRowId True if a valid value was passed for \a rowId, false otherwise. If false, will lookup the row by system address.  Required if adding a new row and the remote system does not automatically create rowIDs.
	/// \param[in] rowId The rowID of the new or existing row.
	/// \param[in] cellUpdates An array of DatabaseCellUpdate structures containing the values to write to the remote row.
	/// \param[in] numCellUpdates The number of elements in the cellUpdates array
	/// \param[in] systemAddress Which system to send to.
	/// \param[in] broadcast Broadcast or not.  Same as the parameter in RakPeer::Send
	void UpdateRow(const char *tableName, const char *updatePassword, RowUpdateMode updateMode, bool hasRowId, unsigned rowId, DatabaseCellUpdate *cellUpdates, unsigned char numCellUpdates, SystemAddress systemAddress, bool broadcast);

	/// Removes a remote row, assuming the server allows row removal.
	/// No response is returned by the server.
	/// \param[in] tableName String name of the remote table. Case sensitive.
	/// \param[in] removePassword Password to remove rows from the remote table, if any.
	/// \param[in] rowId The rowID of the existing row.
	/// \param[in] systemAddress Which system to send to.
	/// \param[in] broadcast Broadcast or not.  Same as the parameter in RakPeer::Send
	void RemoveRow(const char *tableName, const char *removePassword, unsigned rowId, SystemAddress systemAddress, bool broadcast);

protected:
};

#endif
