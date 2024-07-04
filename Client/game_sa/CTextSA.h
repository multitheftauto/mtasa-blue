#pragma once

#include <cstdint>

struct CKeyArraySAInterface {
    std::uint32_t data;
    std::uint32_t size;
};

struct CDataSAInterface {
    std::uint32_t data;
    std::uint32_t size;
};

struct TablEntrySAInterface {
    const char name[8];
    std::uint32_t offset;
};

struct CTextSAInterface {
    struct Tabl {
        TablEntrySAInterface data[200];
        std::uint16_t size;
        std::uint16_t __pad;
    };

    CKeyArraySAInterface tkeyMain;
    CDataSAInterface tdatMain;
    std::uint32_t tkeyMission;
    std::uint8_t pad[4];
    CDataSAInterface tdatMission;
    std::uint8_t field_20;
    std::uint8_t haveTabl;
    std::uint8_t cderrorInitialized;
    std::uint8_t missionLoaded;
    const char missionName[8];
    const char cderrorText[256];
    Tabl tabl;
};