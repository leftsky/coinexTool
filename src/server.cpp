
#include "event2/event.h"
#include "event2/util.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/thread.h"
#include "CompleteConfidence/Socket.h"
#include "CompleteConfidence/WebSocket.h"

#include <regex>
#include "easylogging/easylogging++.h"
#include "LeftMyCodes/MyCodes.h"
#include "curlPost.h"
#include "nlohmann/json.hpp"
#include "server.h"

#define LEFT_DEBUG 

namespace {
    char *dealQuest(char *msg, char *rtMsg, int len) {
        if (strlen(msg) < 10)
            return NULL;
        using namespace coinexTool;
        using namespace std;
        using json = nlohmann::json;
        json j;
        try
        {
            j = json::parse(msg);

        }
        catch (const std::exception&)
        {
            LOG(WARNING) << "message isn't json type";
            LOG(WARNING) << msg;
            return NULL;
        }
        string url;
        string pm;
        bool post_get = true;
        try {
            url = j["url"];
            pm = j["pm"];
            post_get = j["post_get"];
        }
        catch (const std::exception&)
        {
            LOG(WARNING) << "message isn't full exist";
            LOG(WARNING) << msg;
            return NULL;
        }
        if (url.empty() || url.length() < 5) {
            LOG(WARNING) << "url is not accept";
            LOG(WARNING) << url;
            return NULL;
        }
        LOG(INFO) << "url: " << url;
        LOG(INFO) << "pm: " << pm;
        LOG(INFO) << "post_get: " << post_get;

        EasyCurl a;
        if (post_get) {
            a.post(url.c_str(), pm.c_str(), rtMsg, len);
        }
        else {
            a.get(url.c_str(), pm.c_str(), rtMsg, len);
        }
        return rtMsg;
    }
}

namespace websocketServer {

    void socket_event_cb(bufferevent *bev, short events, void *arg) {
        if (events & BEV_EVENT_EOF) {
            LOG(INFO) << "connection closed";
        }
        else if (events & BEV_EVENT_ERROR)
            LOG(INFO) << "some other error";
        bufferevent_free(bev);
    }

    void socket_read_cb(bufferevent *bev, void *arg) {
        size_t len = 0;
        char msg[10240];
        char RtMsg[10240];

        memset(msg, 0, sizeof(msg));
        memset(RtMsg, 0, sizeof(RtMsg));
        len = bufferevent_read(bev, msg, sizeof(msg) - 1);
        msg[len] = '\0';
        std::regex MagicKey("[\\S,\\s]+Sec-WebSocket-Key[\\S,\\s]+");
        if (std::regex_match(msg, MagicKey)) {
            CCWebSocket::GetAnswerStr(msg, RtMsg, 10240);
        }
        else {
            if (CCWebSocket::ReadPacket(msg, len, RtMsg, 10240) == NULL)
            {
                CCWebSocket::PackageAddHead("ReadPacket is NULL", strlen("ReadPacket is NULL"), RtMsg, 10240);
                LOG(INFO) << "ReadPacket is NULL";
                bufferevent_write(bev, RtMsg, strlen(RtMsg));
                return;
            }
            RtMsg[10240 - 1] = '\0';
            LOG(INFO) << "Recv >>> " << strlen(RtMsg) << " :: " << RtMsg;
            if (!dealQuest(RtMsg, RtMsg, 10240)) {
                LOG(INFO) << "{ \"leftError\": 1}";
                snprintf(RtMsg, 10240, "{ \"leftError\": 1}");
                //bufferevent_write(bev, "{ \"leftError\": 1}", strlen("{ \"leftError\": 1}"));
                //return;
            }
            //snprintf(msg, 10240, "Recv: %d %s \r\n %s", strlen(RtMsg), leftName::GetTimeStr(time, 20), RtMsg);
            snprintf(msg, 10240, "%s", RtMsg);
            CCWebSocket::PackageAddHead(msg, strlen(msg), RtMsg, 10240);
        }
        //LOG(INFO) << "RtMsg >>> " << strlen(RtMsg) << " :: " << RtMsg;
        bufferevent_write(bev, RtMsg, strlen(RtMsg));
    }

    void listener_cb(evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *sock, int socklen, void *arg) {
        char ip[16];
        event_base *base = (event_base*)arg;
        sockaddr_in *clientAddress = (sockaddr_in*)sock;
        unsigned short port = clientAddress->sin_port;
        inet_ntop(AF_INET, IPFromAddr((*clientAddress)), ip, 16);
        LOG(INFO) << "Accept " << ip << "::" << port;
        bufferevent *bev = bufferevent_socket_new(base, fd,
            BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
    }

    int WebSocketMain(int argc, char* argv[]) {

        LeftSocket::InitializeSocket();
        char ip[20];
        short port;
        LeftThrNo ThrNo = 0;
        switch (argc) {
#ifdef LEFT_DEBUG
        case 1:
#endif
        case 3:
#ifdef LEFT_DEBUG
            switch (argc) {
            case 1:
                snprintf(ip, 20, "127.0.0.1");
                port = 1996;
                break;
            case 3:
#endif
                snprintf(ip, 20, argv[1]);
                port = atoi(argv[2]) + 1;
#ifdef LEFT_DEBUG
            }
#endif
            LOG(INFO) << "You chose " << ip << ":" << port;
            LOG(INFO) << "I will try to build websocket server on it ";
            {
#ifdef __linux__
                evthread_use_pthreads();
#endif
#ifdef _WIN32
                evthread_use_windows_threads();
#endif
                struct sockaddr_in sin;
                memset(&sin, 0, sizeof(struct sockaddr_in));
                sin.sin_family = AF_INET;
                sin.sin_port = htons(port);
                inet_pton(AF_INET, ip, IPFromAddr(sin));
                event_base *base = event_base_new();
                evconnlistener *listener
                    = evconnlistener_new_bind(base, listener_cb, base,
                        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                        500, (struct sockaddr*)&sin,
                        sizeof(struct sockaddr_in));
                event_base_dispatch(base);
                evconnlistener_free(listener);
                event_base_free(base);
            }
            break;
        default:
            LOG(INFO) << "coinexTool Version 1.0.0 (c) left";
            LOG(INFO) << "Use like coinexTool (ip port)";
            break;
        }
        LeftSocket::CleanSocket();
        return 0;
    }
}

namespace toolServer {

    void socket_event_cb(bufferevent *bev, short events, void *arg) {
        if (events & BEV_EVENT_EOF) {
            //LOG(INFO) << "connection closed";
        }
        else if (events & BEV_EVENT_ERROR)
            LOG(INFO) << "some other error";
        bufferevent_free(bev);
    }


    void socket_read_cb(bufferevent *bev, void *arg) {
        using namespace coinexTool;
        using namespace std;
        char msg[10240];
        char rtMsg[10240];
        int len = bufferevent_read(bev, msg, sizeof(msg) - 1);
        msg[len] = '\0';
        {
            LOG(INFO) << msg;
            //bufferevent_write(bev, "Hi", strlen("Hi"));
        }
        if (!dealQuest(msg, rtMsg, 10240)) {
            LOG(INFO) << "{ \"leftError\": 1}";
            bufferevent_write(bev, "{ \"leftError\": 1}", strlen("{ \"leftError\": 1}"));
            return;
        }
        LOG(INFO) << "len: " << strlen(rtMsg) << "\r\n" << rtMsg;
        if (strlen(rtMsg) > 2000) {
            using string = std::string;
            string str = string(rtMsg);
            STRING_CUTCHAR(str, '\r');
            STRING_CUTCHAR(str, '\n');
            STRING_CUTCHAR(str, ' ');
            snprintf(rtMsg, 10240, str.c_str());
        }
        bufferevent_write(bev, rtMsg, strlen(rtMsg));
    }

    void listener_cb(evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *sock, int socklen, void *arg) {
        char ip[16];
        event_base *base = (event_base*)arg;
        sockaddr_in *clientAddress = (sockaddr_in*)sock;
        unsigned short port = clientAddress->sin_port;
        inet_ntop(AF_INET, IPFromAddr((*clientAddress)), ip, 16);
        LOG(INFO) << "Accept " << ip << "::" << port;
        bufferevent *bev = bufferevent_socket_new(base, fd,
            BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);
        bufferevent_enable(bev, EV_READ | EV_PERSIST);
    }

    void EasyLoggingInit() {
        //el::Configurations conf("easylogging.conf");
        //el::Loggers::reconfigureLogger("default", conf);
        //el::Loggers::reconfigureAllLoggers(conf);
    }

    void InitAll() {
        //EasyLoggingInit();
        LeftSocket::InitializeSocket();
    }

    int serverMain(int argc, char* argv[]) {

        InitAll();
        char ip[20];
        short port;
        LeftThrNo ThrNo = 0;
        switch (argc) {
#ifdef LEFT_DEBUG
        case 1:
#endif
        case 3:
#ifdef LEFT_DEBUG
            switch (argc) {
            case 1:
                snprintf(ip, 20, "127.0.0.1");
                port = 1995;
                break;
            case 3:
#endif
                snprintf(ip, 20, argv[1]);
                port = atoi(argv[2]);
#ifdef LEFT_DEBUG
        }
#endif
            LOG(INFO) << "You chose " << ip << ":" << port;
            LOG(INFO) << "I will try to build tcp server on it ";
            {
#ifdef __linux__
                evthread_use_pthreads();
#endif
#ifdef _WIN32
                evthread_use_windows_threads();
#endif
                struct sockaddr_in sin;
                memset(&sin, 0, sizeof(struct sockaddr_in));
                sin.sin_family = AF_INET;
                sin.sin_port = htons(port);
                inet_pton(AF_INET, ip, IPFromAddr(sin));
                event_base *base = event_base_new();
                evconnlistener *listener
                    = evconnlistener_new_bind(base, listener_cb, base,
                        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                        500, (struct sockaddr*)&sin,
                        sizeof(struct sockaddr_in));
                event_base_dispatch(base);
                evconnlistener_free(listener);
                event_base_free(base);
            }
            break;
        default:
            LOG(INFO) << "coinexTool Version 1.0.0 (c) left";
            LOG(INFO) << "Use like coinexTool (ip port)";
            break;
    }
        LeftSocket::CleanSocket();
        return 0;
}


}
