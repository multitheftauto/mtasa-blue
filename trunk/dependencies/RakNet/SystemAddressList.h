/// \file
/// \brief Just a class to hold a list of systems
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __SYSTEM_ID_LIST_H
#define __SYSTEM_ID_LIST_H

#include "RakNetTypes.h"
#include "DS_OrderedList.h"

class SystemAddressList
{
public:
	SystemAddressList();
	SystemAddressList(SystemAddress system);
	void AddSystem(SystemAddress system);
	void RandomizeOrder(void);
	void Serialize(RakNet::BitStream *out);
	bool Deserialize(RakNet::BitStream *in);
	bool Save(const char *filename);
	bool Load(const char *filename);
	void RemoveSystem(SystemAddress system);
	unsigned Size(void) const;
	SystemAddress& operator[] ( const unsigned int position ) const;
	void Clear(void);

	DataStructures::List<SystemAddress> * GetList(void);

protected:
	DataStructures::List<SystemAddress> systemList;
};

#endif
