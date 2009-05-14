#include "SystemAddressList.h"
#include "Rand.h"
#include "RakAssert.h"
#include "BitStream.h"
#include <stdio.h>

SystemAddressList::SystemAddressList()
{

}
SystemAddressList::SystemAddressList(SystemAddress system)
{
	systemList.Insert(system, __FILE__, __LINE__);
}
void SystemAddressList::AddSystem(SystemAddress system)
{
	systemList.Insert(system, __FILE__, __LINE__);
}
void SystemAddressList::RandomizeOrder(void)
{
	unsigned index, size, randIndex;
	SystemAddress temp;
	size = systemList.Size();
	for (index=0; index < size; index++)
	{
		randIndex=index + (randomMT() % (size-index));
		if (randIndex!=index)
		{
			temp=systemList[index];
			systemList[index]=systemList[randIndex];
			systemList[randIndex]=temp;
		}
	}
}
void SystemAddressList::Serialize(RakNet::BitStream *out)
{
	out->Write((unsigned short) systemList.Size());
	unsigned index;
	for (index=0; index < systemList.Size(); index++)
		out->Write(systemList[index]);
}
bool SystemAddressList::Deserialize(RakNet::BitStream *in)
{
	unsigned short systemListSize;
	SystemAddress systemAddress;
	unsigned index;
	if (in->Read(systemListSize)==false)
	{
		RakAssert(0);
		return false;
	}
	systemList.Clear();
	for (index=0; index < systemListSize; index++)
	{
		if (in->Read(systemAddress)==false)
		{
			RakAssert(0);
			systemList.Clear();
			return false;
		}
		systemList.Insert(systemAddress, __FILE__, __LINE__);

	}
	return true;
}
void SystemAddressList::RemoveSystem(SystemAddress system)
{
	unsigned i;
	for (i=0; i < systemList.Size(); i++)
	{
		if (systemList[i]==system)
		{
			systemList.RemoveAtIndex(i);
			return;
		}
	}
}
DataStructures::List<SystemAddress> * SystemAddressList::GetList(void)
{
	return &systemList;
}
bool SystemAddressList::Save(const char *filename)
{
	RakNet::BitStream temp;
	Serialize(&temp);
	FILE *fp = fopen(filename, "wb");
	if (fp)
	{
		fwrite(temp.GetData(), (size_t) temp.GetNumberOfBytesUsed(), 1, fp);
		fclose(fp);
		return true;
	}
	return false;
}
bool SystemAddressList::Load(const char *filename)
{
	FILE *fp = NULL;
	unsigned long fileSize;

	if ( ( fp = fopen( filename, "rb" ) ) == 0 )
		return false;

	fseek( fp, 0, SEEK_END );
	fileSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	if (fileSize==0)
	{
		fclose(fp);
		return false;
	}
	unsigned char *filedata = (unsigned char*) rakMalloc_Ex( fileSize, __FILE__, __LINE__ );
	fread(filedata, fileSize, 1, fp);
	fclose(fp);

	RakNet::BitStream bs(filedata, fileSize, false);
	Deserialize(&bs);
	rakFree_Ex(filedata, __FILE__, __LINE__ );
	return true;
}
unsigned SystemAddressList::Size(void) const
{
	return systemList.Size();
}
SystemAddress& SystemAddressList::operator[] ( const unsigned int position ) const
{
	return systemList[position];
}
void SystemAddressList::Clear(void)
{
	systemList.Clear();
}
