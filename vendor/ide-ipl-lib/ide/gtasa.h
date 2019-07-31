#pragma once
#include "..\types\ideipl_line.h"
#include "..\types\section.h"
#include "..\types\point.h"
#include "..\types\rgba.h"
#include <fstream>

namespace ide
{
    struct gtasa
    {
        enum item_type
        {
            item_na,
            item_objs,
            item_tobj,
            item_hier,
            item_anim,
            item_weap,
            item_txdp,
            item_cars,
            item_peds
        };
        enum object_type
        {
            obj_objs,
            obj_tobj,
            obj_anim
        };

        struct tModelObject
        {
            int  id = -1;
            char modelName[24];
            char txdName[24];
        };

        union tObjectFlags {
            struct
            {
                unsigned int isRoad : 1;
                unsigned int doNotFade : 1;
                unsigned int drawLast : 1;
                unsigned int additive : 1;
                unsigned int isSubway : 1;
                unsigned int dynamicLighting : 1;
                unsigned int noZbufferWrite : 1;
                unsigned int dontReceiveShadows : 1;
                unsigned int ignoreDrawDistance : 1;
                unsigned int glassType : 2;
            };
            unsigned int int_value = 0;
        };

        struct tCarObject : public tModelObject
        {
            char         vehicleType[24];
            char         handlingID[24];
            char         gameName[24];
            char         animName[24];
            char         className[24];
            int          frequency = -1;
            unsigned int flags = 0;
            char         comprules[24];
            int          wheelModelID = -1;
            float        wheelScaleFront = 0.0f;
            float        wheelScaleRear = 0.0f;
            int          WheelUpgradeClass = -1;
            const unsigned int parameterCount = 15;
            tCarObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tPedObject : public tModelObject
        {
            char defaultPedType[24];
            char behaviour[24];
            char animGroup[24];
            char CarsCanDriveMask[24];
            unsigned int flags = 0;
            char         animFile[24];
            int          radio1 = -1;
            int          radio2 = -1;
            char         voiceArchive[24];
            char         voice1[24];
            char         voice2[24];
            const unsigned int parameterCount = 14;

            tPedObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tObject : public tModelObject
        {
            unsigned int meshCount = 1;
            float        drawDistance = 0.0f;
            tObjectFlags flags;
            unsigned int parameterCount = 0;

            tObject();

            tObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tTimeObject : public tObject
        {
            unsigned int  meshCount = 1;
            unsigned char timeOn = 0;
            unsigned char timeOff = 0;

            tTimeObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tHierObject : public tModelObject
        {
            const unsigned int parameterCount = 3;
            tHierObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tAnimObject : public tObject
        {
            char animName[16];
            const unsigned int parameterCount = 6;

            tAnimObject();

            tAnimObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tWeapObject : public tAnimObject
        {
            const unsigned int parameterCount = 6;

            tWeapObject(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        struct tTxdParent
        {
            char txdName[24];
            char parentTxdName[24];
            const unsigned int parameterCount = 3;

            tTxdParent(ideipl_line& line);

            void read(ideipl_line& line);

            void write(ideipl_line& line);
        };

        class ide_file
        {
        public:
            std::string                 m_filePath;
            bool                        m_bFileOpenSuccess;
            const unsigned int          totalSections = 8;
            ideipl_section<tObject>     objs;
            ideipl_section<tTimeObject> tobj;
            ideipl_section<tHierObject> hier;
            ideipl_section<tAnimObject> anim;
            ideipl_section<tWeapObject> weap;
            ideipl_section<tTxdParent>  txdp;
            ideipl_section<tCarObject>  cars;
            ideipl_section<tPedObject>  peds;

            // Used by WriteIDE
            std::vector<char>           tempData;

            ide_file(const std::string& filename) { m_filePath = filename; }
            bool LoadIDE();

            // DO NOT PASS THE .IDE name, only the FOLDER PATH
            // program will do folderLocation + "\\" + ideNameWithExtension
            bool WriteIDE(const std::string& folderLocation);
            void WriteObjsObject(std::ofstream& outputFileStream);
            void WriteTimeObjects(std::ofstream& outputFileStream);
            void WriteHierObjects(std::ofstream& outputFileStream);
            void WriteAnimObjects(std::ofstream& outputFileStream);
            void WriteWeaponObjects(std::ofstream& outputFileStream);
            void WriteTxdParentObjects(std::ofstream& outputFileStream);
            void WriteCarObjects(std::ofstream& outputFileStream);
            void WritePedObjects(std::ofstream& outputFileStream);

            template <typename T>
            void forAllObjects(T callback)
            {
                for (auto& obj : objs)
                    callback(obj, obj_objs);
                for (auto& obj : tobj)
                    callback(obj, obj_tobj);
                for (auto& obj : anim)
                    callback(obj, obj_anim);
            }
        };
    };

} // namespace ide