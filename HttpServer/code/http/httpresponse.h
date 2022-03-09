#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>

#include "./httprequest.h"
#include "../buffer/buffer.h"
#include "../log/log.h"

class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    void Init(const std::string& srcDir, std::string& path, int customCode, std::unordered_map<std::string, std::string>& requestPost, bool isKeepAlive, int code);
    void MakeResponse(Buffer &buff);    // 相应客户端
    char* File();
    size_t FileLen() const;
    void UnmapFile();
    int Code() const;

private:
    void AddStateLine_(Buffer &buff);   // 添加状态行
    void AddHeader_(Buffer &buff);      // 添加消息报头
    void AddContent_(Buffer &buff);      // 添加相应正文


    std::string GetFileType_();         // 获取文件类型
    void GetShareDir(std::string dirPath, std::vector<std::string>& ret, std::string subDir="");
    size_t SubDirNumber(std::string dirPath);

    void AddShareDir2Content_();
    void AddFile2Content_();

    int code_;
    int customCode_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;
    std::string content_;

    char* mmFile_; 
    struct stat mmFileStat_;

    std::unordered_map<std::string, std::string> requestPost_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;  // 文件后缀对应的编码类型
    static const std::unordered_map<int, std::string> CODE_STATUS;      // code对应状态
    static const std::unordered_map<int, std::string> CODE_PATH;        // code对应html路径
    static const std::unordered_map<std::string, int> CustomCode;        // 自定义协议代码
};

#endif //HTTPRESPONSE_H