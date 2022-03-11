/*
 * @Author       : mark
 * @Date         : 2020-06-27
 * @copyleft Apache 2.0
 */ 
#include "httpresponse.h"

using namespace std;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

const unordered_map<string, int> HttpResponse::CustomCode {
    {"GET_SHAREDIR",  0},
    {"DOWNLOAD_FILE", 1},
    {"UPLOAD_FILE",  2}
};

HttpResponse::HttpResponse() {
    code_ = customCode_ = -1;
    path_ = srcDir_ = content_ = "";
    isKeepAlive_ = false;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    requestPost_.clear();
};

HttpResponse::~HttpResponse() {
    UnmapFile();
}

void HttpResponse::Init(const string& srcDir, string& path, int customCode, unordered_map<string, string>& requestPost, bool isKeepAlive, int code){
    assert(srcDir != "");
    if(mmFile_) { UnmapFile(); }
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    path_ = path;
    srcDir_ = srcDir;
    mmFile_ = nullptr; 
    mmFileStat_ = { 0 };
    requestPost_= requestPost;
    content_.clear();
    customCode_ = customCode;
}

void HttpResponse::MakeResponse(Buffer& buff) {
    switch (customCode_)
    {
    case 0:
        AddShareDir2Content_();
        break;
    case 1:
        AddFile2Content_();
        break;
    case 2:
        break;
    default:
        break;
    }

    AddStateLine_(buff);
    AddHeader_(buff);
    AddContent_(buff);
}

char* HttpResponse::File() {
    return mmFile_;
}

size_t HttpResponse::FileLen() const {
    return mmFileStat_.st_size;
}

void HttpResponse::AddStateLine_(Buffer& buff) {
    string status;
    if(CODE_STATUS.count(code_) == 1) {
        status = CODE_STATUS.find(code_)->second;
    }
    else {
        code_ = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buff.Append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::AddHeader_(Buffer& buff) {
    LOG_DEBUG("TEST");
    buff.Append("Connection: ");
    if(isKeepAlive_) {
        buff.Append("keep-alive\r\n");
        buff.Append("keep-alive: max=6, timeout=120\r\n");
    } else{
        buff.Append("close\r\n");
    }

    switch (customCode_)
    {
    case 0:
        buff.Append("Content-Type: MESSAGE\r\n");
        buff.Append("CustomCode: GET_SHAREDIR\r\n");
        break;
    case 1:
        buff.Append("Content-Type: FILE\r\n");
        buff.Append("CustomCode: DOWNLOAD_FILE\r\n");
        buff.Append("File-Length: " + to_string(mmFileStat_.st_size) + "\r\n");
        break;
    default:
        break;
    }
    
    buff.Append("Content-Length: " + to_string(content_.length()) + "\r\n");
    buff.Append("\r\n");

}

void HttpResponse::AddContent_(Buffer& buff) {
    if(content_.size() > 0){
        LOG_DEBUG(content_.c_str());
        buff.Append(content_);
    }
}

void HttpResponse::UnmapFile() {
    if(mmFile_) {
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

string HttpResponse::GetFileType_() {
    /* 判断文件类型 */
    string::size_type idx = path_.find_last_of('.');
    if(idx == string::npos) {
        return "text/plain";
    }
    string suffix = path_.substr(idx);
    if(SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

void HttpResponse::AddShareDir2Content_()
{
    vector<string> str_shareDir;
    content_.append("ShareDir=");
    string dirpath = requestPost_.find("DirPath")->second;
    GetShareDir(srcDir_ + "/" + dirpath, str_shareDir);
    if(str_shareDir.size() > 0)
    { 
        for(auto &s : str_shareDir){
            content_.append(s + ";");
        }
    }
}

void HttpResponse::AddFile2Content_()
{
    string filePath = requestPost_.find("FilePath")->second;
    LOG_DEBUG("%s", filePath.c_str());
        /* 判断请求的资源文件 */
    if(stat((srcDir_ + "/" + filePath).data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)) {
        code_ = 404;
    }
    else if(!(mmFileStat_.st_mode & S_IROTH)) {
        code_ = 403;
    }
    else if(code_ == -1) { 
        code_ = 200; 
    }
    int srcFd = open((srcDir_ + "/" + filePath).data(), O_RDONLY);
    if(srcFd < 0) { 
        LOG_DEBUG("%s", "File NotFound!");
        return; 
    }
    path_ = srcDir_ + "/" + filePath;
    /* 将文件映射到内存提高文件的访问速度 
        MAP_PRIVATE 建立一个写入时拷贝的私有映射*/
    LOG_DEBUG("file path %s", (srcDir_ + "/" + filePath).data());
    int* mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1) {
        LOG_DEBUG("%s", "File NotFound!");
        return; 
    }
    mmFile_ = (char*)mmRet;
    close(srcFd);
}

void HttpResponse::GetShareDir(string path, vector<string>& ret){
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(path.c_str())) == NULL)
    {
        perror("Open dir error...");
    }      
	
    while((ptr = readdir(dir)) != NULL){
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if (ptr->d_type == 4)	// 目录
        {
            ret.push_back(ptr->d_name);
        }
        else if (ptr->d_type == 8)      // 文件
        {
            ret.push_back(ptr->d_name);
        }
    }
    closedir(dir);
}

size_t HttpResponse::SubDirNumber(string dirPath)
{
    DIR *dir;
    struct dirent *ptr;
    size_t num = 0;

    if((dir = opendir(dirPath.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        else if(ptr->d_type == 4 || ptr->d_type == 8)   // 文件或目录
        {
            num++;
        }
    }
    closedir(dir);
    return num;
}

