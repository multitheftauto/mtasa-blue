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
#include <dpp/export.h>
#include <dpp/version.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <dpp/exception.h>
#include <dpp/snowflake.h>
#include <dpp/misc-enum.h>
#include <dpp/stringops.h>
#include <dpp/managed.h>
#include <dpp/utility.h>
#include <dpp/voicestate.h>
#include <dpp/permissions.h>
#include <dpp/role.h>
#include <dpp/user.h>
#include <dpp/channel.h>
#include <dpp/thread.h>
#include <dpp/guild.h>
#include <dpp/invite.h>
#include <dpp/dtemplate.h>
#include <dpp/emoji.h>
#include <dpp/ban.h>
#include <dpp/prune.h>
#include <dpp/voiceregion.h>
#include <dpp/integration.h>
#include <dpp/webhook.h>
#include <dpp/presence.h>
#include <dpp/intents.h>
#include <dpp/message.h>
#include <dpp/appcommand.h>
#include <dpp/stage_instance.h>
#include <dpp/auditlog.h>
#include <dpp/application.h>
#include <dpp/scheduled_event.h>
#include <dpp/discordclient.h>
#include <dpp/dispatcher.h>
#include <dpp/cluster.h>
#include <dpp/cache.h>
#include <dpp/httpsclient.h>
#include <dpp/queues.h>
#include <dpp/commandhandler.h>
#include <dpp/once.h>
#include <dpp/sync.h>
#include <dpp/colors.h>
#include <dpp/discordevents.h>
#include <dpp/timed_listener.h>
#include <dpp/collector.h>
