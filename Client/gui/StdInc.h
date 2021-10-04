#define MTA_CLIENT
#define SHARED_UTIL_WITH_FAST_HASH_MAP

#include <windows.h>
#undef CreateWindow // mAcrosoft strikes again

#include <list>
#include <map>
#include <string>
#include <vector>

#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>

#include <d3dx9.h>

#include "SharedUtil.h"
#include "CRect2D.h"
#include "CVector2D.h"

#include "../sdk/gui/CGUITypes.h"

#include "CGUI_Impl.h"
#include "CGUIBrowser_Impl.h"
#include "CGUIButton_Impl.h"
#include "CGUICheckbox_Impl.h"
#include "CGUICombobox_Impl.h"
#include "CGUIEdit_Impl.h"
#include "CGUIElement_Impl.h"
#include "CGUIGridList_Impl.h"
#include "CGUILabel_Impl.h"
#include "CGUIMemo_Impl.h"
#include "CGUIProgressBar_Impl.h"
#include "CGUIRadioButton_Impl.h"
#include "CGUIScrollbar_Impl.h"
#include "CGUIScrollpane_Impl.h"
#include "CGUIStaticImage_Impl.h"
#include "CGUITabPanel_Impl.h"
#include "CGUIWindow_Impl.h"
