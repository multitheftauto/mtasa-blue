/// \file
/// \brief A simple flat database included with RakNet, useful for a server browser or a lobby server.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __LIGHTWEIGHT_DATABASE_SERVER_H
#define __LIGHTWEIGHT_DATABASE_SERVER_H

#include "Export.h"
#include "PluginInterface2.h"
#include "LightweightDatabaseCommon.h"
#include "DS_Map.h"

class RakPeerInterface;
struct Packet;

/// \brief A simple flat database included with RakNet, useful for a server browser or a lobby server.
/// A flat database interface.  Adds the ability to track IPs of row updaters and passwords for table read and write operations,
/// Best used for data in which queries which do not need to be updated in real-time
/// \ingroup SIMPLE_DATABSE_GROUP
class RAK_DLL_EXPORT LightweightDatabaseServer : public PluginInterface2
	{
	public:
		/// Constructor
		LightweightDatabaseServer();

		/// Destructor
		virtual ~LightweightDatabaseServer();

		/// Returns a table by name.
		/// It is valid to read and write to the returned pointer.
		/// \param[in] tableName The name of the table that was passed to AddTable
		/// \return The requested table, or 0 if \a tableName cannot be found.
		DataStructures::Table *GetTable(const char *tableName);

		/// Adds a new table
		/// It is valid to read and write to the returned pointer.
		/// \param[in] tableName Name of the new table to create.  Remote systems will refer to this table by this name.
		/// \param[in] allowRemoteQuery true to allow remote systems to query the table. false to not allow this.
		/// \param[in] allowRemoteUpdate true to allow remote systems to update rows in the table. false to not allow this.
		/// \param[in] allowRemoteRemove true to allow remote systems to remove rows from the table. false to not allow this.
		/// \param[in] queryPassword The password required to query the table.  Pass an empty string for no password.
		/// \param[in] updatePassword The password required to update rows in the table.  Pass an empty string for no password.
		/// \param[in] removePassword The password required to remove rows from the table.  Pass an empty string for no password.
		/// \param[in] oneRowPerSystemAddress Only used if allowRemoteUpdate==true.  This limits remote systems to one row.
		/// \param[in] onlyUpdateOwnRows Only used if allowRemoteUpdate==true.  This limits remote systems to only updating rows they created.
		/// \param[in] removeRowOnPingFailure Only used if allowRemoteUpdate==true and removeRowOnDisconnect==false. If true, this will periodically ping disconnected systems and remove rows created by that system if that system does not respond to pings.
		/// \param[in] removeRowOnDisconnect Only used if allowRemoteUpdate==true. This removes rows created by a system when that system disconnects.
		/// \param[in] autogenerateRowIDs true to automatically generate row IDs.  Rows are stored in order by row ID.  If false, the clients must specify a unique row ID when adding rows. If they specify a row that already exists the addition is ignored.
		/// \return The newly created table, or 0 on failure.
		DataStructures::Table* AddTable(const char *tableName,
			bool allowRemoteQuery,
			bool allowRemoteUpdate,
			bool allowRemoteRemove,
			const char *queryPassword,
			const char *updatePassword,
			const char *removePassword,
			bool oneRowPerSystemAddress,
			bool onlyUpdateOwnRows,
			bool removeRowOnPingFailure,
			bool removeRowOnDisconnect,
			bool autogenerateRowIDs);

		/// Removes a table by name.
		/// \param[in] tableName The name of the table that was passed to AddTable
		/// \return true on success, false on failure.
		bool RemoveTable(const char *tableName);

		/// Clears all memory.
		void Clear(void);

		// Gets the next valid auto-generated rowId for a table and increments it.
		unsigned GetAndIncrementRowID(const char *tableName);

		/// Returns a linked list of ordered lists containing the rows of a table, by name.
		/// The returned structure is internal to the BPlus tree.  See DS_BPlusTree
		/// This is a convenience accessor, as you can also get this from the table returned from GetTable()
		/// \param[in] tableName The name of the table that was passed to AddTable
		/// \return The requested rows, or 0 if \a tableName cannot be found.
		DataStructures::Page<unsigned, DataStructures::Table::Row*, _TABLE_BPLUS_TREE_ORDER> *GetTableRows(const char *tableName);

		/// \internal For plugin handling
		virtual void Update(void);
		/// \internal For plugin handling
		virtual PluginReceiveResult OnReceive(Packet *packet);
		/// \internal For plugin handling
		virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );

		struct DatabaseTable
			{
			bool allowRemoteUpdate;
			bool allowRemoteQuery;
			bool allowRemoteRemove;

			char updatePassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];
			char queryPassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];
			char removePassword[_SIMPLE_DATABASE_PASSWORD_LENGTH];
			bool oneRowPerSystemAddress;
			bool onlyUpdateOwnRows;
			bool removeRowOnPingFailure;
			bool removeRowOnDisconnect;
			bool autogenerateRowIDs;
			char tableName[_SIMPLE_DATABASE_TABLE_NAME_LENGTH];

			// Used if autogenerateRowIDs==true
			unsigned nextRowId;

			unsigned SystemAddressColumnIndex;
			unsigned SystemGuidColumnIndex;
			unsigned lastPingResponseColumnIndex;
			unsigned nextPingSendColumnIndex;
			RakNetTime nextRowPingCheck;
			DataStructures::Table table;
			};

		static int DatabaseTableComp( const char* const &key1, const char* const &key2 );

	protected:
		DataStructures::Map<const char *, LightweightDatabaseServer::DatabaseTable*, LightweightDatabaseServer::DatabaseTableComp> database;
		void OnQueryRequest(Packet *packet);
		void OnUpdateRow(Packet *packet);
		void OnRemoveRow(Packet *packet);
		void OnPong(Packet *packet);
		// mode 0 = query, mode 1 = update, mode 2 = remove
		DatabaseTable * DeserializeClientHeader(RakNet::BitStream *inBitstream, RakPeerInterface *peer, Packet *packet, int mode);
		DataStructures::Table::Row * GetRowFromIP(DatabaseTable *databaseTable, SystemAddress systemAddress, unsigned *rowId);
		bool RowHasIP(DataStructures::Table::Row *row, SystemAddress systemAddress, unsigned SystemAddressColumnIndex);
		DataStructures::Table::Row * AddRow(LightweightDatabaseServer::DatabaseTable *databaseTable, SystemAddress systemAddress, RakNetGUID guid, bool hasRowId, unsigned rowId);
		void RemoveRowsFromIP(SystemAddress systemAddress);

	};

#endif
