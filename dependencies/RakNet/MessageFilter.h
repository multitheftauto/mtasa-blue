/// \file
/// \brief Message filter plugin. Assigns systems to FilterSets.  Each FilterSet limits what messages are allowed.  This is a security related plugin.
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __MESSAGE_FILTER_PLUGIN_H
#define __MESSAGE_FILTER_PLUGIN_H

class RakPeerInterface;
#include "RakNetTypes.h"
#include "PluginInterface2.h"
#include "DS_OrderedList.h"
#include "Export.h"

/// MessageIdentifier (ID_*) values shoudln't go higher than this.  Change it if you do.
#define MESSAGE_FILTER_MAX_MESSAGE_ID 256

/// \internal Has to be public so some of the shittier compilers can use it.
int RAK_DLL_EXPORT MessageFilterStrComp( char *const &key,char *const &data );

/// \internal Has to be public so some of the shittier compilers can use it.
struct FilterSet
{
	bool banOnFilterTimeExceed;
	bool kickOnDisallowedMessage;
	bool banOnDisallowedMessage;
	RakNetTime disallowedMessageBanTimeMS;
	RakNetTime timeExceedBanTimeMS;
	RakNetTime maxMemberTimeMS;
	void (*invalidMessageCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData, unsigned char messageID);
	void *disallowedCallbackUserData;
	void (*timeoutCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData);
	void *timeoutUserData;
	int filterSetID;
	bool allowedIDs[MESSAGE_FILTER_MAX_MESSAGE_ID];
	DataStructures::OrderedList<char *, char *, MessageFilterStrComp> allowedRPCs;
};

/// \internal Has to be public so some of the shittier compilers can use it.
int RAK_DLL_EXPORT FilterSetComp( const int &key, FilterSet * const &data );

/// \internal Has to be public so some of the shittier compilers can use it.
struct FilteredSystem
{
	SystemAddress systemAddress;
	FilterSet *filter;
	RakNetTime timeEnteredThisSet;
};

/// \internal Has to be public so some of the shittier compilers can use it.
int RAK_DLL_EXPORT FilteredSystemComp( const SystemAddress &key, const FilteredSystem &data );

/// \defgroup MESSAGEFILTER_GROUP MessageFilter
/// \ingroup PLUGINS_GROUP

/// \brief Assigns systems to FilterSets.  Each FilterSet limits what kinds of messages are allowed.
/// The MessageFilter plugin is used for security where you limit what systems can send what kind of messages.
/// You implicitly define FilterSets, and add allowed message IDs and RPC calls to these FilterSets.
/// You then add systems to these filters, such that those systems are limited to sending what the filters allows.
/// You can automatically assign systems to a filter.
/// You can automatically kick and possibly ban users that stay in a filter too long, or send the wrong message.
/// Each system is a member of either zero or one filters.
/// Add this plugin before any plugin you wish to filter (most likely just add this plugin before any other).
/// \ingroup MESSAGEFILTER_GROUP
class RAK_DLL_EXPORT MessageFilter : public PluginInterface2
{
public:
	MessageFilter();
	virtual ~MessageFilter();

	// --------------------------------------------------------------------------------------------
	// User functions
	// --------------------------------------------------------------------------------------------

	/// Automatically add all new systems to a particular filter
	/// Defaults to -1
	/// \param[in] filterSetID Which filter to add new systems to.  <0 for do not add.
	void SetAutoAddNewConnectionsToFilter(int filterSetID);

	/// Allow a range of message IDs
	/// Always allowed by default: ID_CONNECTION_REQUEST_ACCEPTED through ID_DOWNLOAD_PROGRESS
	/// Usually you specify a range to make it easier to add new enumerations without having to constantly refer back to this function.
	/// \param[in] allow True to allow this message ID, false to disallow. By default, all messageIDs except the noted types are disallowed.  This includes messages from other plugins!
	/// \param[in] messageIDStart The first ID_* message to allow in the range.  Inclusive.
	/// \param[in] messageIDEnd The last ID_* message to allow in the range.  Inclusive.
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	void SetAllowMessageID(bool allow, int messageIDStart, int messageIDEnd,int filterSetID);

	/// Allow an RPC function, by name
	/// \param[in] allow True to allow an RPC call with this function name, false to disallow.  All RPCs are disabled by default.
	/// \param[in] functionName the function name of the RPC call.  Must match the function name exactly, including case.
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	void SetAllowRPC(bool allow, const char *functionName, int filterSetID);

	/// What action to take on a disallowed message.  You can kick or not.  You can add them to the ban list for some time
	/// By default no action is taken.  The message is simply ignored.
	/// param[in] 0 for permanent ban, >0 for ban time in milliseconds.
	/// \param[in] kickOnDisallowed kick the system that sent a disallowed message.
	/// \param[in] banOnDisallowed ban the system that sent a disallowed message.  See \a banTimeMS for the ban duration
	/// \param[in] banTimeMS Passed to the milliseconds parameter of RakPeer::AddToBanList.
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	void SetActionOnDisallowedMessage(bool kickOnDisallowed, bool banOnDisallowed, RakNetTime banTimeMS, int filterSetID);

	/// Set a user callback to be called on an invalid message for a particular filterSet
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	/// \param[in] userData A pointer passed with the callback
	/// \param[in] invalidMessageCallback A pointer to a C function to be called back with the specified parameters.
	void SetDisallowedMessageCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData, unsigned char messageID));

	/// Set a user callback to be called when a user is disconnected due to SetFilterMaxTime
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	/// \param[in] userData A pointer passed with the callback
	/// \param[in] invalidMessageCallback A pointer to a C function to be called back with the specified parameters.
	void SetTimeoutCallback(int filterSetID, void *userData, void (*invalidMessageCallback)(RakPeerInterface *peer, SystemAddress systemAddress, int filterSetID, void *userData));

	/// Limit how long a connection can stay in a particular filterSetID. After this time, the connection is kicked and possibly banned.
	/// By default there is no limit to how long a connection can stay in a particular filter set.
	/// \param[in] allowedTimeMS How many milliseconds to allow a connection to stay in this filter set.
	/// \param[in] banOnExceed True or false to ban the system, or not, when \a allowedTimeMS is exceeded
	/// \param[in] banTimeMS Passed to the milliseconds parameter of RakPeer::AddToBanList.
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.
	void SetFilterMaxTime(int allowedTimeMS, bool banOnExceed, RakNetTime banTimeMS, int filterSetID);

	/// Get the filterSetID a system is using.  Returns -1 for none.
	/// \param[in] systemAddress The system we are referring to
	int GetSystemFilterSet(SystemAddress systemAddress);

	/// Assign a system to a filter set.
	/// Systems are automatically added to filter sets (or not) based on SetAutoAddNewConnectionsToFilter()
	/// This function is used to change the filter set a system is using, to add it to a new filter set, or to remove it from all existin filter sets.
	/// \param[in] systemAddress The system we are referring to
	/// \param[in] filterSetID A user defined ID to represent a filter set.  If no filter with this ID exists, one will be created with default settings.  If -1, the system will be removed from all filter sets.
	void SetSystemFilterSet(SystemAddress systemAddress, int filterSetID);

	/// Returns the number of systems subscribed to a particular filter set
	/// Using anything other than -1 for \a filterSetID is slow, so you should store the returned value.
	/// \param[in] filterSetID The filter set to limit to.  Use -1 for none (just returns the total number of filter systems in that case).
	unsigned GetSystemCount(int filterSetID) const;

	/// Returns a system subscribed to a particular filter set,by index.
	/// index should be between 0 and the GetSystemCount(filterSetID)-1;
	/// \param[in] filterSetID The filter set to limit to.  Use -1 for none (just indexes all the filtered systems in that case).
	/// \param[in] index A number between 0 and GetSystemCount(filterSetID)-1;
	SystemAddress GetSystemByIndex(int filterSetID, unsigned index);

	/// Returns the total number of filter sets.
	/// \return The total number of filter sets.
	unsigned GetFilterSetCount(void) const;

	/// Returns the ID of a filter set, by index
	/// \param[in] An index between 0 and GetFilterSetCount()-1 inclusive
	int GetFilterSetIDByIndex(unsigned index);

    /// Delete a FilterSet.  All systems formerly subscribed to this filter are now unrestricted.
	/// \param[in] filterSetID The ID of the filter set to delete.
	void DeleteFilterSet(int filterSetID);

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual void Update(void);
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnNewConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, bool isIncoming);
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );

protected:

	void Clear(void);
	void DeallocateFilterSet(FilterSet *filterSet);
	FilterSet* GetFilterSetByID(int filterSetID);
	void OnInvalidMessage(FilterSet *filterSet, SystemAddress systemAddress, unsigned char messageID);

	DataStructures::OrderedList<int, FilterSet*, FilterSetComp> filterList;
	DataStructures::OrderedList<SystemAddress, FilteredSystem, FilteredSystemComp> systemList;

	int autoAddNewConnectionsToFilter;
};

#endif
