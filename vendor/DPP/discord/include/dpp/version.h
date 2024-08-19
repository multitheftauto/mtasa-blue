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

#if !defined(DPP_VERSION_LONG)
#define DPP_VERSION_LONG 0x00100030
#define DPP_VERSION_SHORT 100030
#define DPP_VERSION_TEXT "D++ 10.0.30 (11-Dec-2023)"

#define DPP_VERSION_MAJOR ((DPP_VERSION_LONG & 0x00ff0000) >> 16)
#define DPP_VERSION_MINOR ((DPP_VERSION_LONG & 0x0000ff00) >> 8)
#define DPP_VERSION_PATCH (DPP_VERSION_LONG & 0x000000ff)
#endif
