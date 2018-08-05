
#ifndef _LEFT_7112_UC_TCP_BASE_UPDATER_H_
#define _LEFT_7112_UC_TCP_BASE_UPDATER_H_

#include "server.h"
#include "nlohmann/json.hpp"

namespace toolClient {
    using json = nlohmann::json;
    int PleaseTalkMsg(json j);
}

#endif

