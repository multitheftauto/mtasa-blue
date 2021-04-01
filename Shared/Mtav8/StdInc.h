#define PI (3.14159265358979323846f)

class CV8Class;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mutex>
#include <unordered_map>
#include <functional>
#include "SharedUtil.h"
#include "Common.h"

#include "include/config.h"
#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "v8-inspector.h"

#include "v8/CV8Base.h"

#include "include/CV8Utils.h"

#include "include/CV8.h"
#include "include/CV8Isolate.h"
#include "include/CV8Module.h"
#include "include/CV8Task.h"
#include "include/CV8Promise.h"
#include "include/CV8FunctionCallback.h"
#include "include/CV8ExportClass.h"
#include "include/CV8ExportObject.h"
#include "include/CV8ExportEnum.h"
#include "include/CV8AsyncContext.h"
