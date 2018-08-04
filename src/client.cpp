
#include "CompleteConfidence/Socket.h"
#include "event2/event.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/thread.h"
#include "client.h"
#include "LeftMyCodes/MyCodes.h"
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include "nlohmann/json.hpp"
#include "easylogging/easylogging++.h"

namespace toolClient {
    using namespace toolServer;

    void read_cb(struct bufferevent* bev, void* arg) {
        char msg[10240];
        auto len = bufferevent_read(bev, msg, 10240);
        msg[len] = '\0';
        LOG(INFO) << "len: " << len << "\r\n" << msg;
        //bufferevent_free(bev);
    }

    void event_cb(struct bufferevent *bev, short event, void *arg) {
        if (event & BEV_EVENT_EOF) {
#ifdef LEFT_DEBUG
            CURLOG << "connection closed" << std::endl;
#endif
            *(int*)arg = 1;
        }
        else if (event & BEV_EVENT_ERROR) {
#ifdef LEFT_DEBUG
            CURLOG << "some other error" << std::endl;
#endif
            *(int*)arg = 0;
        }
        else if (event & BEV_EVENT_CONNECTED) {
#ifdef LEFT_DEBUG
            CURLOG << "the client has connected to server" << std::endl;
#endif
            *(int*)arg = 1;
            return;
        }
        bufferevent_free(bev);
    }

    int PleaseTalkMsg(ctS ask) {
        LeftSocket::InitializeSocket();
        int rt;
        struct event_base *base = event_base_new();
        struct bufferevent* bev = bufferevent_socket_new(
            base, -1, BEV_OPT_CLOSE_ON_FREE);
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(1995);
        inet_pton(AF_INET, "106.14.133.3", IPFromAddr(server_addr));
        bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr,
            sizeof(server_addr));
        bufferevent_setcb(bev, read_cb, NULL, event_cb, &rt);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
        using json = nlohmann::json;
        json j = {
            { "url", ask.url },
        { "pm", ask.pm },
        { "post_get", ask.post_get }
        };
        char buf[10240];
        snprintf(buf, 10240, j.dump().c_str());
        bufferevent_write(bev, buf, strlen(buf));
        event_base_dispatch(base);
        event_base_free(base);
        LeftSocket::CleanSocket();
        return rt;
    }
}
