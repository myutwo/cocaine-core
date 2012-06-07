//
// Copyright (C) 2011-2012 Andrey Sibiryov <me@kobology.ru>
//
// Licensed under the BSD 2-Clause License (the "License");
// you may not use this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "cocaine/manifest.hpp"

#include "cocaine/context.hpp"

#include "cocaine/interfaces/storage.hpp"

using namespace cocaine::engine;
using namespace cocaine::storages;

// Application
// -----------

manifest_t::manifest_t(context_t& context, const std::string& app):
    name(app)
{
    objects::value_type object;

    try {
        // Load the app manifest.
        object = context.storage<objects>("core")->get("apps", name);
    } catch(const storage_error_t& e) {
        throw configuration_error_t("the '" + name + "' app is not available");
    }

    root = object.meta;

    // Setup the app configuration.
    type = root["type"].asString();

    // Setup the engine policies.
    policy.heartbeat_timeout = root["engine"].get(
        "heartbeat-timeout",
        defaults::heartbeat_timeout
    ).asDouble();

    policy.suicide_timeout = root["engine"].get(
        "suicide-timeout",
        defaults::suicide_timeout
    ).asDouble();
    
    policy.pool_limit = root["engine"].get(
        "pool-limit",
        defaults::pool_limit
    ).asUInt();
    
    policy.queue_limit = root["engine"].get(
        "queue-limit",
        defaults::queue_limit
    ).asUInt();

    policy.grow_threshold = root["engine"].get(
        "grow-threshold",
        policy.queue_limit / policy.pool_limit
    ).asUInt();

    slave = root["engine"].get(
        "slave",
        defaults::slave
    ).asString();
}
