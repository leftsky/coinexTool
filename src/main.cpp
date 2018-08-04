

#include <event2/event_struct.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string>
//#include "httplib.h"
#include "CompleteConfidence/Socket.h"

#include <regex>
#include "LeftMyCodes/MyCodes.h"
#include "easylogging/easylogging++.h"
#include "curl/curl.h"
#include "nlohmann/json.hpp"
#include "curlPost.h"
#include "server.h"
#include "client.h"
#include "MD5.h"


INITIALIZE_EASYLOGGINGPP
using namespace std;

template<typename T> string toString(const T& t) {
    ostringstream oss;  //创建一个格式化输出流
    oss << t;             //把值传递如流中
    return oss.str();
}

#define access_id "7B3E854643CF4ACBAD9F91270A2E2721"
#define market_list "https://api.coinex.com/v1/market/list"
#define order_mining_difficulty "https://api.coinex.com/v1/order/mining/difficulty"
#define balance_info "https://api.coinex.com/v1/balance/info"

using json = nlohmann::json;

int globalArgc = 0;
char **globalArgv = NULL;
LeftThrReturn thr1(LeftThrArgv null) {
    websocketServer::WebSocketMain(globalArgc, globalArgv);
    LOG(ERROR) << "WebSocket server Exit!";
    EndThread;
    return 0;
}

LeftThrReturn thr2(LeftThrArgv null) {
    toolServer::serverMain(globalArgc, globalArgv);
    LOG(ERROR) << "TCP server Exit!";
    EndThread;
    return 0;
}

int main(int argc, char **argv) {
    if(0) { // 测试MD5算法
        int i;
        char encrypt[] = "BD176C892AEC4434B48951E716FA7B041CB9AA54BDF22F87";//"admin";//21232f297a57a5a743894a0e4a801fc3
        unsigned char decrypt[16];

        MD5_CTX md5;

        MD5Init(&md5);
        MD5Update(&md5, (unsigned char*)encrypt, (int)strlen(encrypt));//只是个中间步骤
        MD5Final(&md5, decrypt);//32位

        printf("\n加密前:%s\n加密后32位:", encrypt);
        for (i = 0; i < 16; i++) {
            printf("%02X", decrypt[i]);
        }
        for (;;) LeftSleep(500);
    }
    
    globalArgc = argc;
    globalArgv = argv;
    using namespace coinexTool;
    EasyCurl::init();

   
#ifdef CC_OS_LINUX
    LeftThrNo no = 0;
    CURRENCY_StartThread(thr1, NULL, no);
    CURRENCY_StartThread(thr2, NULL, no);
    for (;;) LeftSleep(500);
#else
    using namespace toolClient;
    using namespace toolServer;
    ctS a;
    a.url = string(balance_info);
    //json postPm = {
    //    { "code", 0 },
    //    { "data",{
    //        "BTCBCH", "LTCBCH", "ETHBCH", "ZECBCH", "DASHBCH"
    //        } },
    //    { "message", "Ok" }
    //};
    //a.pm = postPm.dump();
    json postPm = {
        { "access_id", access_id },
        { "tonce", time(NULL) * 1000 },
    };
    a.pm = postPm.dump();
    //a.pm = "";
    a.post_get = false;
    LOG(INFO) << "please talk msg";
    PleaseTalkMsg(a);
#endif

#if 0
    EasyCurl a;
    char buf[10240];

    string postUrlStr = order_mining_difficulty;
    //json postPm = {
    //    { "code", 0 },
    //    { "data",{
    //        "BTCBCH", "LTCBCH", "ETHBCH", "ZECBCH", "DASHBCH"
    //        } },
    //    { "message", "Ok" }
    //};
    json postPm = {
        {"tonce", time(NULL) * 1000}
    };
    LOG(INFO) << postPm;
    string postParams = postPm.dump();

    string endUrl;
    endUrl = postUrlStr + "?tonce=" + toString((time(NULL) * 1000));
    
    a.get(endUrl.c_str(), buf, 10240);
    LOG(INFO) << buf;
#endif

#ifdef CC_OS_WIN
    for (;;) LeftSleep(500);
#endif

    EasyCurl::clean();
}
