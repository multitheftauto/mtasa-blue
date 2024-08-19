/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2021 Craig Edwards and D++ contributors 
 * (https://github.com/brainboxdotcc/DPP/graphs/contributors)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/
#pragma once

/* This file contains pragmas to disable warnings on win32 builds with msvc only.
 * It is only included during build of D++ itself, and not when including the headers
 * into a user's project.
 * 
 * Before adding a warning here please be ABSOLUTELY SURE it is one we cannot easily fix
 * and is to be silenced, thrown into the sarlacc pit to be eaten for 1000 years...
 */

_Pragma("warning( disable : 4251 )"); // 4251 warns when we export classes or structures with stl member variables
_Pragma("warning( disable : 5105 )"); // 5105 is to do with macro warnings
