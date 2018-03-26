#pragma
#ifndef __CIFPANIMATIONS_H
#define __CIFPANIMATIONS_H
#include "../mods/deathmatch/logic/IFP/IFPLoader.h"

class CIFPAnimations
{
    public:
    // This is set to true when unloadIFP function is called, it means that animations
    // can be unloaded when iReferences reaches zero, if this boolean is set to true
	bool  bUnloadOnZeroReferences; 
    // incremented when a new CAnimBlendAssociationSAInteface is created and decremented 
    // when destructor of that animation is called 
	DWORD iReferences; 
	std::vector < IFP_Animation > vecAnimations;	

public:
            CIFPAnimations   ( void );
            ~CIFPAnimations  ( void );

    void    DeleteAnimations ( void );

};
#endif
