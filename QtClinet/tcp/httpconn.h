#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <QMetatype>
#include <QString>
#include <QByteArray>
#include <QMap>
#include <QRegExp>
#include <string>
#include <regex>

using namespace std;

class HttpConn
{
public:
    enum PARSE_STATE{
        STATE_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    HttpConn();
    void init();
    void setHost(QString ip, int port);
    QByteArray encode(const QMap<QString, QByteArray>& data, const QByteArray& body, QString method = "GET");
    bool parse(const QByteArray& data, QMap<QString, QByteArray>& buf);

private:
    void parseStateLine_(const QByteArray& line, QMap<QString, QByteArray>& buf);
    void parseHeaders_(const QByteArray& line, QMap<QString, QByteArray>& buf);
    bool parseBody_(const QByteArray& line, QMap<QString, QByteArray>& buf);
    void parseMsg_(const QByteArray& line, QMap<QString, QByteArray>& buf);
    void parseFile_(const QByteArray& line, QMap<QString, QByteArray>& buf);

    int state_;
    QString ip_;
    int port_;
    QByteArray body_;
    QMap<QString, QByteArray> postData_;

    int total_;
};

#endif // HTTPCONN_H
