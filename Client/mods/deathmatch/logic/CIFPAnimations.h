#ifndef __CIFPANIMATIONS_H
#define __CIFPANIMATIONS_H
#include "CClientIFP.h"

class CIFPAnimations
{
public:
    struct SAnimation
    {
        SString                              Name;
        unsigned int                         uiNameHash;
        std::unique_ptr<CAnimBlendHierarchy> pHierarchy;
        BYTE*                                pSequencesMemory;
    };

    std::vector<SAnimation> vecAnimations;

public:
    ~CIFPAnimations(void);
    void DeleteAnimations(void);
};
#endif
