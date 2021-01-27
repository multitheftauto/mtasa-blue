#define PI (3.14159265358979323846f)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mutex>
#include <unordered_map>
#include <functional>
#include "SharedUtil.h"

#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "v8-inspector.h"

#include "v8/CV8Base.h"

#include "classes/CV8BaseClass.h"
#include "classes/CV8Vector2D.h"
#include "classes/CV8Vector3D.h"
#include "classes/CV8Matrix.h"

#include "include/CV8.h"
#include "include/CV8Isolate.h"
#include "include/CV8Module.h"
#include "include/CV8Promise.h"
#include "include/CV8FunctionCallback.h"
