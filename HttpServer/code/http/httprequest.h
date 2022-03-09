#ifndef HTTPREQUESET_H
#define HTTPREQUESET_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>   

#include "../log/log.h"
#include "../buffer/buffer.h"
#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"

class HttpRequest
{
public:
    // http报文解析状态码
    enum PARSE_STATE {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,        
    };

    // http请求状态码
    enum HTTP_CODE {
        NO_REQUEST = 0,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURSE,
        FORBIDDENT_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION,
    };

    HttpRequest() { }
    ~HttpRequest() = default;

    void Init(const std::string& srcDir);
    bool parse(Buffer& buff);

    std::string path() const;
    std::string& path();
    std::string method() const;
    std::string version() const;
    std::string GetPost(const std::string& key) const;
    std::string GetPost(const char* key) const;
    std::unordered_map<std::string, std::string>& GetPost();
    int GetCustomCode() const;

    bool IsKeepAlive() const;
    bool IsDone() const;

private:
    // 解析请求行
    bool ParseRequestLine_(const std::string& line);

    // 解析请求头
    void ParseHeader_(const std::string& line);

    // 解析请求体
    bool ParseBody_(const std::string& line);

    void ParsePath_();
    void ParsePost_();
    void ParseFromUrlencoded_();    // 解析数据


    void DownloadFile_();
    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);

    PARSE_STATE state_;
    std::string method_, path_, version_, body_;
    std::unordered_map<std::string, std::string> header_;
    std::unordered_map<std::string, std::string> post_;     // 解析post请求
    bool isDone_;

    std::string srcDir_;
    int customCode_;
    static const std::unordered_map<std::string, int> CustomCode;        // 自定义协议代码

    static int ConverHex(char ch);
};

#endif //HTTPREQUESET_H