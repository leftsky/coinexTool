
#ifndef _LEFT_COINEX_TOOL_SERVER_H_
#define _LEFT_COINEX_TOOL_SERVER_H_

#include <iostream>

namespace websocketServer {
    int WebSocketMain(int argc, char* argv[]);
}

namespace toolServer {

    typedef struct _CHAT {
        // true==>post  false==>get
        bool post_get;
        std::string url;
        std::string pm;
        std::string response;
    } ctS, *pctS;

    int serverMain(int argc, char* argv[]);
}

#endif // !_LEFT_COINEX_TOOL_SERVER_H_

