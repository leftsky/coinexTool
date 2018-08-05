
#include "curl/curl.h"
#include <iostream>
#include <map>
#include "curlPost.h"
#include "easylogging/easylogging++.h"
#include "LeftMyCodes/MyCodes.h"
#include "MD5.h"
#include "nlohmann/json.hpp"


//#define SECRET_KEY "BD176C892AEC4434B48951E716FA7B041CB9AA54BDF22F87"


namespace coinexTool {
    using namespace std;
    using json = nlohmann::json;

    string get_sign(const char *params, const char *secret_key) {
        json pm;
        try
        {
            pm = json::parse(params);

        }
        catch (const std::exception&)
        {
            LOG(WARNING) << "get_sign params isn't json type";
            LOG(WARNING) << params;
            return string("");
        }
        string paramsFront;
        {
            map<string, string> a;
            for (json::iterator it = pm.begin(); it != pm.end(); ++it) {
                a.insert(pair<string, string>(it.key(), it.value().dump()));
            }
            for (auto it = a.begin(); it != a.end(); ++it) {
                LOG(INFO) << it->first << " : " << it->second;
                paramsFront += it->first + "=" + it->second + "&";
            }
        }
        STRING_CUTCHAR(paramsFront, '\"');
        json access_id, tonce;
        try {
            access_id = pm["access_id"];
            tonce = pm["tonce"];
        }
        catch (const std::exception&)
        {
            LOG(ERROR) << "get sign error params not full exist";
            LOG(WARNING) << params;
            return string("");
        }
        char sourceStr[1024];
        string str = access_id.dump();
        str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
        //snprintf(sourceStr, 1024, "access_id=%s&tonce=%s&secret_key=%s",
        //    str.c_str(), tonce.dump().c_str(), secret_key);
        snprintf(sourceStr, 1024, "%ssecret_key=%s",
            paramsFront.c_str(), secret_key);
        LOG(INFO) << "sourceStr: " << sourceStr;

        unsigned char decrypt[16];
        MD5_CTX md5;
        MD5Init(&md5);
        MD5Update(&md5, (unsigned char*)sourceStr, (int)strlen(sourceStr));//只是个中间步骤
        MD5Final(&md5, decrypt);//32位
        char auth[200];
        char hexStr[40];
        leftName::HexToStr((const char*)decrypt, 16, hexStr, 40);
        snprintf(auth, 200, "authorization: %s", hexStr);
        LOG(INFO) << "final: " << auth;
        return string(auth);
    }

    // reply of the requery  
    size_t req_reply(void *ptr, size_t size, size_t nmemb, void *stream)
    {
        //cout << "----->reply" << endl;
        string *str = (string*)stream;
        //cout << *str << endl;
        (*str).append((char*)ptr, size*nmemb);
        return size * nmemb;
    }

    // http GET  
    CURLcode curl_get_req(const std::string &url, std::string &response, curl_slist *headers)
    {
        // init curl  
        CURL *curl = curl_easy_init();
        // res code  
        CURLcode res;
        // set handles
        //struct curl_slist *headers = NULL;
        if (curl)
        {
            // set params  
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // set transport and time out time  
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
            // set handles
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            // start req  
            res = curl_easy_perform(curl);
        }
        // release curl  
        //curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res;
    }

    // http POST  
    CURLcode curl_post_req(const string &url, const string &postParams, string &response, curl_slist *headers)
    {
        LOG(INFO) << "final post: " << postParams.c_str();
        // init curl  
        CURL *curl = curl_easy_init();
        // res code  
        CURLcode res;
        // set handles
        //struct curl_slist *headers = NULL;
        if (curl)
        {
            // set params  
            curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url  
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str()); // params  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https  
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false  
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
            curl_easy_setopt(curl, CURLOPT_HEADER, 1);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
            // set handles
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            // start req  
            res = curl_easy_perform(curl);
        }
        // release curl  
        //curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res;
    }

    void EasyCurl::init() {
#ifdef LEFT_OS_LINUX
        curl_global_init(CURL_GLOBAL_ALL);
#else
        LOG(WARNING) << "全局初始化:暂不支持当前系统";
#endif
    }

    void EasyCurl::clean() {
#ifdef LEFT_OS_LINUX
        curl_global_cleanup();
#else
        LOG(WARNING) << "全局析构释放:暂不支持当前系统";
#endif
    }

    char *EasyCurl::post(const char *url, const char *param, char *buf, int len, const char *key) {
#ifdef LEFT_OS_WIN
        LOG(WARNING) << "POST: 暂不支持当前系统";
        return NULL;
#endif
        string postResponseStr;

        struct curl_slist *headers = NULL;
        { // 设置HTTP头信息
            headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
            string sign = get_sign(param, key).c_str();
            if (sign.length() <= 0) {
                snprintf(buf, len, "function get_sign failed");
                return buf;
            }
            headers = curl_slist_append(headers, sign.c_str());
            headers = curl_slist_append(headers,
                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36");
        }
        auto res = curl_post_req(url, param, postResponseStr, headers);
        // 释放header
        curl_slist_free_all(headers);

        if (res != CURLE_OK)
            cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
        else {
            string str = postResponseStr.substr(postResponseStr.find("\r\n\r\n") + strlen("\r\n\r\n"));
            snprintf(buf, len, str.c_str());
        }
    }

    template<typename T> string toString(const T& t) {
        ostringstream oss;  //创建一个格式化输出流
        oss << t;             //把值传递如流中
        return oss.str();
    }

    string getUrlUri(const char *url, const char *params) {
        json pm;
        try
        {
            pm = json::parse(params);

        }
        catch (const std::exception&)
        {
            LOG(WARNING) << "getUrlUri params isn't json type";
            LOG(WARNING) << params;
            return string("");
        }
        string paramsFront;
        {
            for (json::iterator it = pm.begin(); it != pm.end(); ++it) {
                if (it != pm.begin())
                    paramsFront += "&";
                paramsFront += it.key() + "=" + it.value().dump();
            }
        }
        STRING_CUTCHAR(paramsFront, '\"');
        json access_id, tonce;
        try {
            access_id = pm["access_id"];
            tonce = pm["tonce"];
        }
        catch (const std::exception&)
        {
            LOG(ERROR) << "get sign error params not full exist";
            return string("");
        }
        char urlUri[1024];
        string str = access_id.dump();
        str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
        //snprintf(urlUri, 1024, "%s?access_id=%s&tonce=%s", 
        //    url, str.c_str(), tonce.dump().c_str());
        snprintf(urlUri, 1024, "%s?%s", 
            url, paramsFront.c_str());
        return string(urlUri);
    }

    char *EasyCurl::get(const char *url, const char *param, char *buf, int len, const char *key) {
#ifdef LEFT_OS_WIN
        LOG(WARNING) << "GET: 暂不支持当前系统";
        return NULL;
#endif
        string postResponseStr;
        string furl;
        struct curl_slist *headers = NULL;
        { // 设置HTTP头信息
            headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
            string sign = get_sign(param, key).c_str();
            if (sign.length() <= 0) {
                snprintf(buf, len, "function get_sign failed");
                return buf;
            }
            headers = curl_slist_append(headers, sign.c_str());
            headers = curl_slist_append(headers,
                "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.71 Safari/537.36");
        }
        string urlUri = getUrlUri(url, param).c_str();
        if (urlUri.length() <= 0) {
            snprintf(buf, len, "function urlUri failed");
            return buf;
        }
        auto res = curl_get_req(urlUri.c_str(), postResponseStr, headers);
        // 释放header
        curl_slist_free_all(headers);

        if (res != CURLE_OK)
            cerr << "curl_easy_perform() failed: " + string(curl_easy_strerror(res)) << endl;
        else {
            string str = postResponseStr.substr(postResponseStr.find("\r\n\r\n") + strlen("\r\n\r\n"));
            snprintf(buf, len, str.c_str());
        }
        return buf;
    }

}
