#include "httpconn.h"
#include <QDebug>

HttpConn::HttpConn()
{
    state_ = STATE_LINE;
    body_ = "";
    total_ = 0;
}

void HttpConn::init()
{
    state_ = STATE_LINE;
    body_.clear();
    postData_.clear();

    total_ = 0;
    qDebug() << "init";
}

void HttpConn::setHost(QString ip, int port)
{
     ip_ = ip;
     port_ = port;
}

bool HttpConn::parse(const QByteArray &data, QMap<QString, QByteArray>& buf)
{
    total_ += data.size();
    qDebug() << "total: " << total_;
    if(state_ == BODY){             // 继续读取上一次数据
        if(!parseBody_(data, buf)){
            return false;
        }
        init();
        return true;
    }

    int from = 0, to = 0;
    char CRLF[] = "\r\n";
    QByteArray line;
    while(state_ != FINISH){
        if(state_ != BODY){
            to = data.indexOf(CRLF, from);
            line = data.mid(from, to - from);
            from = to + 2;
        }
        else{
            line = data.right(data.size() - from);
        }
        switch (state_) {
        case STATE_LINE:
            parseStateLine_(line, buf);
            break;
        case HEADERS:
            parseHeaders_(line, buf);
            break;
        case BODY:
            if(!parseBody_(line, buf))
                return false;
            break;
        }
    }

    if(state_ != FINISH)
        return false;

    init();
    return true;
}

void HttpConn::parseStateLine_(const QByteArray &line, QMap<QString, QByteArray>& buf)
{
    state_ = HEADERS;
}

void HttpConn::parseHeaders_(const QByteArray& line, QMap<QString, QByteArray>& buf)
{
    QRegExp rx("^([^:]*): ?(.*)$");
    if(rx.indexIn(line) > -1){
        buf[rx.cap(1)] = rx.cap(2).toUtf8().simplified();
    }
    else {
        state_ = BODY;
    }
}

bool HttpConn::parseBody_(const QByteArray &line, QMap<QString, QByteArray>& buf)
{
    body_.append(line);
    qDebug() << "body: " << body_.size();
    if(buf["Content-Type"] == "MESSAGE"){
        if(buf["Content-Length"].toInt() > body_.size()){
            return false;
        }
        parseMsg_(body_, buf);
    }
    else if(buf["Content-Type"] == "FILE"){
        if(buf["File-Length"].toInt() > body_.size()){
            return false;
        }
        parseFile_(body_, buf);
    }

    state_ = FINISH;
    return true;
}

void HttpConn::parseMsg_(const QByteArray& body, QMap<QString, QByteArray>& buf)
{
    QString key;
    QByteArray value;
    int n = body.size();
    int i = 0, j = 0;

    for(; i < n; i++) {
        char ch = body[i];
        switch (ch) {
        case '=':
            key = body.mid(j, i - j);
            j = i + 1;
            break;
        case '&':
            value = body.mid(j, i - j);
            j = i + 1;
            buf[key] = value;
            break;
        default:
            break;
        }
    }
    assert(j <= i);
    if(j <= i) {
        value = body.mid(j, i - j);
        buf[key] = value;
    }
}

void HttpConn::parseFile_(const QByteArray& body, QMap<QString, QByteArray>& buf)
{
    int len = buf["File-Length"].toInt();
    buf["FileContent"] = body.right(len);
}


QByteArray HttpConn::encode(const QMap<QString, QByteArray>& header, const QByteArray& body, QString method)
{
    QString url;
    url =  "/";

    QString headers;
    QMap<QString, QByteArray>::const_iterator iter;
    for(iter = header.begin(); iter != header.end(); iter++){
        headers += iter.key() + ": " + iter.value() + "\r\n";
    }

    QByteArray result;
    // 请求行
    result += method + " " + url + " HTTP/1.1\r\n";

    // 请求头
    result += "Host: " + ip_ + ':' + QString::number(port_) + "\r\n";
    result += "Connection: keep-alive\r\n";
    result += headers;
    result += "\r\n";

    // 请求体
    result += body;
    result += "\r\n";
    return result;
}
