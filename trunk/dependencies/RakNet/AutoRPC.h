/// \file
/// \brief Automatically serializing and deserializing RPC system. More advanced RPC, but possibly not cross-platform
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __AUTO_RPC_H
#define __AUTO_RPC_H

class RakPeerInterface;
class NetworkIDManager;
#include "PluginInterface2.h"
#include "DS_Map.h"
#include "PacketPriority.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "Gen_RPC8.h"
#include "RakString.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// \defgroup AUTO_RPC_GROUP AutoRPC
/// \ingroup PLUGINS_GROUP

namespace RakNet
{

/// Maximum amount of data that can be passed on the stack in a function call
#define ARPC_MAX_STACK_SIZE 65536

#if defined (_WIN32)
/// Easier way to get a pointer to a function member of a C++ class
/// \note Recommended you use ARPC_REGISTER_CPP_FUNCTION0 to ARPC_REGISTER_CPP_FUNCTION9 (below)
/// \note ARPC_REGISTER_CPP_FUNCTION is not Linux compatible, and cannot validate the number of parameters is correctly passed.
/// \param[in] autoRPCInstance A pointer to an instance of AutoRPC
/// \param[in] _IDENTIFIER_ C string identifier to use on the remote system to call the function
/// \param[in] _RETURN_ Return value of the function
/// \param[in] _CLASS_ Base-most class of the containing class that contains your function
/// \param[in] _FUNCTION_ Name of the function
/// \param[in] _PARAMS_ Parameter list, include parenthesis
#define ARPC_REGISTER_CPP_FUNCTION(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS_) \
{ \
union \
{ \
	_RETURN_ (AUTO_RPC_CALLSPEC _CLASS_::*__memberFunctionPtr)_PARAMS_; \
	void* __voidFunc; \
}; \
	__memberFunctionPtr=&_CLASS_::_FUNCTION_; \
	(autoRPCInstance)->RegisterFunction(_IDENTIFIER_, __voidFunc, true, -1); \
}

/// \internal Used by ARPC_REGISTER_CPP_FUNCTION0 to ARPC_REGISTER_CPP_FUNCTION9
#define ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS_, _PARAM_COUNT_) \
	{ \
	union \
	{ \
	_RETURN_ (AUTO_RPC_CALLSPEC _CLASS_::*__memberFunctionPtr)_PARAMS_; \
	void* __voidFunc; \
}; \
	__memberFunctionPtr=&_CLASS_::_FUNCTION_; \
	(autoRPCInstance)->RegisterFunction(_IDENTIFIER_, __voidFunc, true, _PARAM_COUNT_); \
}

/// Same as ARPC_REGISTER_CPP_FUNCTION, but specifies how many parameters the function has
#define ARPC_REGISTER_CPP_FUNCTION0(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_) (autoRPCInstance)->RegisterFunction(_IDENTIFIER_, __voidFunc, true, 0);
#define ARPC_REGISTER_CPP_FUNCTION1(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_), 0)
#define ARPC_REGISTER_CPP_FUNCTION2(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_), 1)
#define ARPC_REGISTER_CPP_FUNCTION3(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_), 2)
#define ARPC_REGISTER_CPP_FUNCTION4(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_), 3)
#define ARPC_REGISTER_CPP_FUNCTION5(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_,_PARAMS5_), 4)
#define ARPC_REGISTER_CPP_FUNCTION6(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_,_PARAMS5_,_PARAMS6_), 5)
#define ARPC_REGISTER_CPP_FUNCTION7(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_,_PARAMS5_,_PARAMS6_,_PARAMS7_), 6)
#define ARPC_REGISTER_CPP_FUNCTION8(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_,_PARAMS5_,_PARAMS6_,_PARAMS7_,_PARAMS8_), 7)
#define ARPC_REGISTER_CPP_FUNCTION9(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_, _PARAMS9_) ARPC_REGISTER_CPP_FUNCTIONX(autoRPCInstance,_IDENTIFIER_,_RETURN_,_CLASS_,_FUNCTION_,(_PARAMS1_,_PARAMS2_,_PARAMS3_,_PARAMS4_,_PARAMS5_,_PARAMS6_,_PARAMS7_,_PARAMS8_,_PARAMS9_), 8)

#else

#define ARPC_REGISTER_CPP_FUNCTION0(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*)) &_CLASS_::_FUNCTION_, true, 0 );

#define ARPC_REGISTER_CPP_FUNCTION1(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_ )) &_CLASS_::_FUNCTION_, true, 0 );

#define ARPC_REGISTER_CPP_FUNCTION2(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_ )) &_CLASS_::_FUNCTION_, true, 1 );

#define ARPC_REGISTER_CPP_FUNCTION3(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_ )) &_CLASS_::_FUNCTION_, true, 2 );

#define ARPC_REGISTER_CPP_FUNCTION4(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_ )) &_CLASS_::_FUNCTION_, true, 3 );

#define ARPC_REGISTER_CPP_FUNCTION5(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_ )) &_CLASS_::_FUNCTION_, true, 4 );

#define ARPC_REGISTER_CPP_FUNCTION6(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_ )) &_CLASS_::_FUNCTION_, true, 5 );

#define ARPC_REGISTER_CPP_FUNCTION7(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_ )) &_CLASS_::_FUNCTION_, true, 6 );

#define ARPC_REGISTER_CPP_FUNCTION8(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_ )) &_CLASS_::_FUNCTION_, true, 7 );

#define ARPC_REGISTER_CPP_FUNCTION9(autoRPCInstance, _IDENTIFIER_, _RETURN_, _CLASS_, _FUNCTION_, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_, _PARAMS9_) \
	(autoRPCInstance)->RegisterFunction((_IDENTIFIER_), (void*)(_RETURN_ (*) (_CLASS_*, _PARAMS1_, _PARAMS2_, _PARAMS3_, _PARAMS4_, _PARAMS5_, _PARAMS6_, _PARAMS7_, _PARAMS8_, _PARAMS9_ )) &_CLASS_::_FUNCTION_, true, 8 );

#endif

/// Error codes returned by a remote system as to why an RPC function call cannot execute
/// Follows packet ID ID_RPC_REMOTE_ERROR
/// Name of the function will be appended, if available. Read as follows:
/// char outputBuff[256];
/// stringCompressor->DecodeString(outputBuff,256,&RakNet::BitStream(p->data+sizeof(MessageID)+1,p->length-sizeof(MessageID)-1,false),0);
/// printf("Function: %s\n", outputBuff);
enum RPCErrorCodes
{
	/// AutoRPC::SetNetworkIDManager() was not called, and it must be called to call a C++ object member
	RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE,

	/// Cannot execute C++ object member call because the object specified by SetRecipientObject() does not exist on this system
	RPC_ERROR_OBJECT_DOES_NOT_EXIST,

	/// Internal error, index optimization for function lookup does not exist
	RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE,

	/// Named function was not registered with RegisterFunction(). Check your spelling.
	RPC_ERROR_FUNCTION_NOT_REGISTERED,

	/// Named function was registered, but later unregistered with UnregisterFunction() and can no longer be called.
	RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED,

	/// SetRecipientObject() was not called before Call(), but RegisterFunction() was called with isObjectMember=true
	/// If you intended to call a CPP function, call SetRecipientObject() with a valid object first.
	RPC_ERROR_CALLING_CPP_AS_C,

	/// SetRecipientObject() was called before Call(), but RegisterFunction() was called with isObjectMember=false
	/// If you intended to call a C function, call SetRecipientObject(UNASSIGNED_NETWORK_ID) first.
	RPC_ERROR_CALLING_C_AS_CPP,

	/// Internal error, passed stack is bigger than current stack. Check that the version is the same on both systems.
	RPC_ERROR_STACK_TOO_SMALL,

	/// Internal error, formatting error with how the stack was serialized
	RPC_ERROR_STACK_DESERIALIZATION_FAILED,

	/// The \a parameterCount parameter passed to RegisterFunction() on this system does not match the \a parameterCount parameter passed to SendCall() on the remote system.
	RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS,
};

/// The AutoRPC plugin allows you to call remote functions as if they were local functions, using the standard function call syntax
/// No serialization or deserialization is needed.
/// Advantages are that this is easier to use than regular RPC system.
/// Disadvantages is that all parameters must be passable on the stack using memcpy (shallow copy). For other types of parameters, use SetOutgoingExtraData() and GetIncomingExtraData()
/// Pointers are automatically dereferenced and the contents copied with memcpy
/// Use the old system, or regular message passing, if you need greater flexibility
/// \ingroup AUTO_RPC_GROUP
class AutoRPC : public PluginInterface2
{
public:
	/// Constructor
	AutoRPC();

	/// Destructor
	virtual ~AutoRPC();

	/// Sets the network ID manager to use for object lookup
	/// Required to call C++ object member functions via SetRecipientObject()
	/// \param[in] idMan Pointer to the network ID manager to use
	void SetNetworkIDManager(NetworkIDManager *idMan);

	/// Registers a function pointer to be callable given an identifier for the pointer
	/// \param[in] uniqueIdentifier String identifying the function. Recommended that this is the name of the function
	/// \param[in] functionPtr Pointer to the function. For C, just pass the name of the function. For C++, use ARPC_REGISTER_CPP_FUNCTION
	/// \param[in] isObjectMember false if a C function. True if a member function of an object (C++)
	/// \param[in] parameterCount Optional parameter to tell the system how many parameters this function has. If specified, and the wrong number of parameters are called by the remote system, the call is rejected. -1 indicates undefined
	/// \return True on success, false on uniqueIdentifier already used
	bool RegisterFunction(const char *uniqueIdentifier, void *functionPtr, bool isObjectMember, char parameterCount=-1);

	/// Unregisters a function pointer to be callable given an identifier for the pointer
	/// \note This is not safe to call while connected
	/// \param[in] uniqueIdentifier String identifying the function.
	/// \param[in] isObjectMember false if a C function. True if a member function of an object (C++)
	/// \return True on success, false on function was not previously or is not currently registered.
	bool UnregisterFunction(const char *uniqueIdentifier, bool isObjectMember);

	/// Send or stop sending a timestamp with all following calls to Call()
	/// Use GetLastSenderTimestamp() to read the timestamp.
	/// \param[in] timeStamp Non-zero to pass this timestamp using the ID_TIMESTAMP system. 0 to clear passing a timestamp.
	void SetTimestamp(RakNetTime timeStamp);

	/// Set parameters to pass to RakPeer::Send() for all following calls to Call()
	/// Deafults to HIGH_PRIORITY, RELIABLE_ORDERED, ordering channel 0
	/// \param[in] priority See RakPeer::Send()
	/// \param[in] reliability See RakPeer::Send()
	/// \param[in] orderingChannel See RakPeer::Send()
	void SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel);

	/// Set system to send to for all following calls to Call()
	/// Defaults to UNASSIGNED_SYSTEM_ADDRESS, broadcast=true
	/// \param[in] systemAddress See RakPeer::Send()
	/// \param[in] broadcast See RakPeer::Send()
	void SetRecipientAddress(SystemAddress systemAddress, bool broadcast);

	/// Set the NetworkID to pass for all following calls to Call()
	/// Defaults to UNASSIGNED_NETWORK_ID (none)
	/// If set, the remote function will be considered a C++ function, e.g. an object member function
	/// If set to UNASSIGNED_NETWORK_ID (none), the remote function will be considered a C function
	/// If this is set incorrectly, you will get back either RPC_ERROR_CALLING_C_AS_CPP or RPC_ERROR_CALLING_CPP_AS_C
	/// \sa NetworkIDManager
	/// \param[in] networkID Returned from NetworkIDObject::GetNetworkID()
	void SetRecipientObject(NetworkID networkID);

	/// Write extra data to pass for all following calls to Call()
	/// Use BitStream::Reset to clear extra data. Don't forget to do this or you will waste bandwidth.
	/// \return A bitstream you can write to to send extra data with each following call to Call()
	RakNet::BitStream *SetOutgoingExtraData(void);

	/// If the last received function call has a timestamp included, it is stored and can be retrieved with this function.
	/// \return 0 if the last call did not have a timestamp, else non-zero
	RakNetTime GetLastSenderTimestamp(void) const;

	/// Returns the system address of the last system to send us a received function call
	/// Equivalent to the old system RPCParameters::sender
	/// \return Last system to send an RPC call using this system
	SystemAddress GetLastSenderAddress(void) const;

	/// Returns the instance of RakPeer this plugin was attached to
	RakPeerInterface *GetRakPeer(void) const;

	/// Returns the currently running RPC call identifier, set from RegisterFunction::uniqueIdentifier
	/// Returns an empty string "" if none
	/// \return which RPC call is currently running
	const char *GetCurrentExecution(void) const;

	/// Gets the bitstream written to via SetOutgoingExtraData().
	/// Data is updated with each incoming function call
	/// \return A bitstream you can read from with extra data that was written with SetOutgoingExtraData();
	RakNet::BitStream *GetIncomingExtraData(void);

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	bool Call(const char *uniqueIdentifier){
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 0);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1>
	bool Call(const char *uniqueIdentifier, P1 p1)	{
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 1);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2)	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 2);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3 )	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 3);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3, class P4>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4 )	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, true, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 4);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3, class P4, class P5>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, true, true, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 5);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3, class P4, class P5, class P6>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, true, true, true, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 6);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )	{
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, p7, true, true, true, true, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 7);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
	bool Call(const char *uniqueIdentifier, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) {
			char stack[ARPC_MAX_STACK_SIZE];
			unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, p7, p8, true, true, true, true, true, true, true, true);
			return SendCall(uniqueIdentifier, stack, bytesOnStack, 8);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID){
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);		
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 0);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1)	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 1);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2)	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 2);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3 )	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 3);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3, class P4>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3, P4 p4 )	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, true, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 4);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3, class P4, class P5>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5 )	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, true, true, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 5);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3, class P4, class P5, class P6>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6 )	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, true, true, true, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 6);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7 )	{
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, p7, true, true, true, true, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 7);
	}

	/// Calls a remote function, using whatever was last passed to SetTimestamp(), SetSendParams(), SetRecipientAddress(), and SetRecipientObject()
	/// Passed parameter(s), if any, are passed via memcpy and pushed on the stack for the remote function
	/// \note This ONLY works with variables that are passable via memcpy! If you need more flexibility, use SetOutgoingExtraData() and GetIncomingExtraData()
	/// \note The this pointer, for this instance of AutoRPC, is pushed as the last parameter on the stack. See AutoRPCSample.ccp for an example of this
	/// \param[in] uniqueIdentifier parameter of the same name passed to RegisterFunction() on the remote system
	/// \param[in] timeStamp See SetTimestamp()
	/// \param[in] priority See SetSendParams()
	/// \param[in] reliability See SetSendParams()
	/// \param[in] orderingChannel See SetSendParams()
	/// \param[in] systemAddress See SetRecipientAddress()
	/// \param[in] broadcast See SetRecipientAddress()
	/// \param[in] networkID See SetRecipientObject()
	template <class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
	bool CallExplicit(const char *uniqueIdentifier, RakNetTime timeStamp, PacketPriority priority, PacketReliability reliability, char orderingChannel, SystemAddress systemAddress, bool broadcast, NetworkID networkID, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8 ) {
		SetTimestamp(timeStamp);
		SetSendParams(priority, reliability, orderingChannel);
		SetRecipientAddress(systemAddress, broadcast);
		SetRecipientObject(networkID);
		char stack[ARPC_MAX_STACK_SIZE];
		unsigned int bytesOnStack = GenRPC::BuildStack(stack, p1, p2, p3, p4, p5, p6, p7, p8, true, true, true, true, true, true, true, true);
		return SendCall(uniqueIdentifier, stack, bytesOnStack, 8);
	}


	// If you need more than 8 parameters, just add it here...

	// ---------------------------- ALL INTERNAL AFTER HERE ----------------------------

	/// \internal
	/// Identifies an RPC function, by string identifier and if it is a C or C++ function
	struct RPCIdentifier
	{
		char *uniqueIdentifier;
		bool isObjectMember;
	};

	/// \internal
	/// The RPC identifier, and a pointer to the function
	struct LocalRPCFunction
	{
		RPCIdentifier identifier;
		void *functionPtr;
		char parameterCount;
	};

	/// \internal
	/// The RPC identifier, and the index of the function on a remote system
	struct RemoteRPCFunction
	{
		RPCIdentifier identifier;
		unsigned int functionIndex;
	};

	/// \internal
	static int RemoteRPCFunctionComp( const RPCIdentifier &key, const RemoteRPCFunction &data );

	/// \internal
	/// Sends the RPC call, with a given serialized stack
	bool SendCall(const char *uniqueIdentifier, const char *stack, unsigned int bytesOnStack, char parameterCount);

protected:

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	void OnAttach(void);
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnAutoRPCCall(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes);
	virtual void OnRPCRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes);
	virtual void OnRPCUnknownRemoteIndex(SystemAddress systemAddress, unsigned char *data, unsigned int lengthInBytes, RakNetTime timestamp);
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );
	virtual void OnShutdown(void);

	void Clear(void);

	void SendError(SystemAddress target, unsigned char errorCode, const char *functionName);
	unsigned GetLocalFunctionIndex(RPCIdentifier identifier);
	bool GetRemoteFunctionIndex(SystemAddress systemAddress, RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex);


	DataStructures::List<LocalRPCFunction> localFunctions;
	DataStructures::Map<SystemAddress, DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, AutoRPC::RemoteRPCFunctionComp> *> remoteFunctions;

	RakNetTime outgoingTimestamp;
	PacketPriority outgoingPriority;
	PacketReliability outgoingReliability;
	char outgoingOrderingChannel;
	SystemAddress outgoingSystemAddress;
	bool outgoingBroadcast;
	NetworkID outgoingNetworkID;
	RakNet::BitStream outgoingExtraData;

	RakNetTime incomingTimeStamp;
	SystemAddress incomingSystemAddress;
	RakNet::BitStream incomingExtraData;

	NetworkIDManager *networkIdManager;
	char currentExecution[512];
};

} // End namespace

#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
