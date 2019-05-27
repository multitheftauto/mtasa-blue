//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwRights gtaRwRights;

struct gtaRwRights {
    gtaRwBool enabled;
    gtaRwUInt32 pluginId;
    gtaRwUInt32 pluginData;
};

void gtaRwRightsInit(gtaRwRights *rightsObj, gtaRwUInt32 PluginId, gtaRwUInt32 PluginData);
void gtaRwRightsDestroy(gtaRwRights *rightsObj);
gtaRwBool gtaRwRightsWrite(gtaRwRights *rightsObj, gtaRwStream *stream);
gtaRwBool gtaRwRightsRead(gtaRwRights *rightsObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwRightsSize(gtaRwRights *rightsObj);
