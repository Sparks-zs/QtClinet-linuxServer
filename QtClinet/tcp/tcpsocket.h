#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QTcpSocket>
#include <QFile>
#include <QVariant>
#include <QByteArray>

#include "httpconn.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

#define BUF_SIZE 1024
const int Timeout =  5 * 1000;

class TcpSocket : public QObject
{
    Q_OBJECT

public:
    TcpSocket(QObject *parent = 0);
    ~TcpSocket();

    void addHeader(const QString& key, const QByteArray& value);
    void addContent(const QByteArray& content);
    QByteArray get(const QString& key);
    void sendMsg(const QString& content);
    void sendFile(const QString& filepath, const QByteArray& content);

signals:
    void readFinished();
    void connected();
    void readedLength(int);    // 已传输的长度
    void writeedLength(int);    // 已传输的长度
    void error(int socketError, const QString& message);

public slots:
    void onconnected();
    void tcpInit();
    bool connectServer(const QString hostName, int port);
    void read();
    void write(QString method);

private:
    void init_();        // 初始化缓冲区等
    HttpConn http;
    QTcpSocket *socket_;
    QString hostName_;
    int port_;
    QMap<QString, QByteArray> readPostData_; // http格式数据
    QMap<QString, QByteArray> writeHeader_; // http格式数据
    QByteArray writeBody_;
    QByteArray readBuf_;    // 接收缓冲区
    int readedLength_;
};

#endif // TCPCLIENT_H
