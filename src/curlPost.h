
#ifndef _LEFT_COINEX_TOOL_H_
#define _LEFT_COINEX_TOOL_H_

namespace coinexTool {

#define STRING_CUTCHAR(str, c) str.erase(std::remove(str.begin(), str.end(), c), str.end())

    class EasyCurl {
    public:
        static void init();
        static void clean();
        char *post(const char *url, const char *param, char *buf, int len);
        char *get(const char *url, const char *param, char *buf, int len);
    private:
    };

}

#endif
