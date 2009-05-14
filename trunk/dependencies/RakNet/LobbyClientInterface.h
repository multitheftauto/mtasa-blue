#ifndef __LOBBY_CLIENT_INTERFACE_H
#define __LOBBY_CLIENT_INTERFACE_H

enum LobbyEvent
{
	LOBBY_EVENT_COUNT
};

enum LobbyClientEvents
{
	LCE_ADDED_FRIEND,
	LCE_SENT_MESSAGE_SUCCESS,
	LCE_BLOCK_USER_SUCCESS,
	LCE_ALL_FRIENDS_DOWNLOADED,
	LCE_FRIEND_REMOVED_FROM_LIST,
	LCE_FRIEND_ADDED_DIFFERENT_GAME,
	LCE_FRIEND_ADDED_SAME_GAME,
	LCE_FRIEND_OFFLINE,
	LCE_SAME_GAME_INSTANT_MESSAGE,
	LCE_GAME_INVITATION,
	LCE_GAME_GOT_MESSAGE_WITH_ATTACHMENT_GUI
};

#if defined(WIN32)
#include "[PC]LobbyClientTypes.h"
#else
#include "[PS3]LobbyClientTypes.h"
#endif

class LobbyClientCBInterface
{
public:
	LobbyClientCBInterface() {}
	virtual ~LobbyClientCBInterface() {}
	virtual void AddedFriendSuccessfully(void) {}
	virtual void SentMessageSuccessfully(void) {}
	virtual void BlockedUserSuccessfully(void) {}
	virtual void AllFriendsDownloaded(void) {}
	virtual void FriendRemovedFromFriendsList(void *friendId, const char *friendName) {}
	virtual void FriendAddedToFriendsList(void *friendId, const char *friendName, bool inSameGame) {}
	virtual void FriendOffline(void *friendId, const char *friendName) {}
	virtual void GotInstantMessage(void *friendId, const char *friendName, char *data, int dataLength) {}
	virtual void GotGameInvitation(void *friendId, const char *friendName) {}
	virtual void GameMessageWithAttachmentArrived(void *friendId, const char *friendName) {}
	virtual void GotGameMessageWithAttachmentThroughGui(void *friendId, const char *friendName, char *data, int dataLength) {}

	virtual void LeaveRoomComplete(void) {}
	virtual void NewMemberJoinedRoom(void *memberId, const char *memberName) {}
	virtual void MemberLeftRoom(void *memberId, const char *memberName) {}
	virtual void RoomDisappeared(void) {}
	virtual void RoomOwnershipGranted(void *newOwnerId, const char *newOwnerName) {}
	virtual void RoomOwnershipChanged(void *newOwnerId, const char *newOwnerName) {}
	virtual void RoomMemberKicked(void *memberId, const char *memberName) {}
	virtual void KickedFromRoom(void) {}
	virtual void RoomJoined(void) {}
	virtual void RoomCreated(void) {}
	virtual void RoomSearchJoined(void) {}
	virtual void RoomQuickMatchJoined(void) {}
	virtual void RoomQuickMatchCancelled(void) {}
	virtual void RoomJoinInvitationSentConfirmation(void) {}
	virtual void RoomJoinInvitationAccepted(void *roomId) {}
	virtual void ReceivedRoomList(void) {}
	virtual void ScoreSubmitted(unsigned int newRank);
	virtual void GotScore(void *memberId, char *onlineName, unsigned int rank, unsigned int highestRank, unsigned long long score );

};

class LobbyClientInterface
{
public:	
	virtual bool StartLogin(const char *titleIdentifier){return false;}
	virtual void Logoff(){}
	virtual bool IsLoggedIn(void) const{return false;}
	virtual bool IsFriendsListOpen(void) const{return false;}
	virtual bool IsSendingMessage(void) const{return false;}
	virtual bool IsReceivingStoredMessage(void) const{return false;}
	virtual bool IsAddingFriend(void) const{return false;}
	virtual bool IsCreatingRoom(void) const{return false;}
	virtual bool IsSearchingAndJoiningRoom(void) const{return false;}
	virtual bool IsDoingQuickMatch(void) const{return false;}
	virtual bool IsGettingRoomList(void) const{return false;}
	virtual bool IsInRoom(void) const{return false;}
	virtual bool RecordScore(unsigned int scoreboardId, unsigned long long int score, const char *censoredComment, const char *gameData, unsigned int gamedataLength){return false;}
	virtual bool GetScoreRanking(unsigned int scoreboardId, LobbyClientUserId* userId){return false;}
	virtual void AbortScoreTransaction(void){}
	virtual void Update(void){}
	virtual bool ShowFriendsList(void){return false;}
	virtual bool AddFriend(LobbyClientUserId* to, const char *body){return false;}
	virtual unsigned GetNumFriends(void) const{return 0;}
	virtual const LobbyClientUserId& GetFriendIDFromIndex(unsigned idx) const{return LobbyClientUserId();}
	virtual bool GetFriendSameGameFromIndex(unsigned idx) const{return false;}
	virtual unsigned GetIndexFromFriendId(LobbyClientUserId* id){return 0;}
	virtual bool SendGameMessage(LobbyClientUserId* to, const void *data, int dataSize){return false;}
	virtual bool SendStoredMessage(LobbyClientUserId* to, const char *subject, const char *body, const void *data, int dataSize){return false;}
	virtual bool ReceiveStoredMessageWithAttachment(void){return false;}
	virtual bool CreateRoom(int totalSlots, int privateSlots, bool kickMembersOnOwnerLeave, bool allowSearchesForRoom){return false;}
	virtual unsigned GetNumRoomMembers(void) const{return 0;}
	virtual const LobbyClientUserId& GetRoomMemberIDFromIndex(unsigned idx) const{return LobbyClientUserId();}
	virtual unsigned GetIndexFromRoomMemberID(LobbyClientUserId* id) const{return 0;}
	virtual unsigned GetNumRooms(void) const{return 0;}
	virtual const LobbyClientRoomId & GetRoomIDFromIndex(unsigned idx) const{return LobbyClientRoomId();}
	virtual unsigned GetIndexFromRoomID(LobbyClientRoomId *id) const{return 0;}
	virtual bool GetRoomList(int requiredSlots){return false;}
	virtual void SetRoomStealth(bool isHidden){}
	virtual bool LeaveRoom(void){return false;}
	virtual bool GrantRoomOwnership(LobbyClientUserId * userId){return false;}
	virtual bool KickRoomMember(LobbyClientUserId * userId){return false;}
	virtual bool SendRoomInvitation(LobbyClientUserId * userId, const char *subject, const char *body, bool inviteToPrivateSlot){return false;}
	virtual bool SearchAndJoinRoom(int requiredSlots){return false;}
	virtual bool QuickMatch(int requiredSlots, int timeoutInSeconds){return false;}
	virtual bool BlockPlayer(LobbyClientUserId* to){return false;}
	virtual unsigned GetNumBlockedPlayers(void) const{return 0;}
	virtual const LobbyClientUserId& GetBlockedPlayerAtIndex(unsigned idx){return LobbyClientUserId();}
	virtual bool AddToMetPlayerHistory(LobbyClientUserId* id, const char *description){return false;}
	virtual void SetCallbackInterface(LobbyClientCBInterface *cbi){}
	virtual const LobbyClientUserId* GetMyID(void) const{return 0;}
};

#if defined(WIN32)
#include "[PC]LobbyClient.h"
#else
#include "[PS3]LobbyClient.h"
#endif

#endif
