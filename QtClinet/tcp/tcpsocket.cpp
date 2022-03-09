#include "tcpsocket.h"

#include <QFile>
#include <QDataStream>
#include <QHostAddress>
#include <QThread>
#include <string.h>
#include <QMimeDatabase>
#include <QDebug>

TcpSocket::TcpSocket(QObject *parent)
    : QObject(parent)
{
    readedLength_ = 0;
}

TcpSocket::~TcpSocket()
{
    readBuf_.clear();
    readPostData_.clear();
    writeHeader_.clear();
    writeBody_.clear();
}

void TcpSocket::tcpInit()
{
    if(!socket_)
        delete socket_;
    socket_ = new QTcpSocket();
    connect(socket_, SIGNAL(readyRead()), this, SLOT(read()));
    connect(socket_, SIGNAL(connected()), this, SLOT(onconnected()));
    init_();
}

void TcpSocket::init_()
{
    readBuf_.clear();
    readPostData_.clear();
    writeHeader_.clear();
    writeBody_.clear();
}

bool TcpSocket::connectServer(const QString hostName, int port)
{
    hostName_ = hostName;
    port_ = port;

    socket_->connectToHost(hostName_, port_);
    if(!socket_->waitForConnected(Timeout)){
        emit error(socket_->error(), socket_->errorString());
        return false;
    }
    http.setHost(hostName, port);
    return true;
}

void TcpSocket::read()
{
    readBuf_.clear();
    readBuf_ = socket_->readAll();
    readedLength_ += readBuf_.size();
    emit readedLength(readedLength_);
    qDebug() << "readed len: " << readedLength_;

    if(!http.parse(readBuf_, readPostData_)){
        return;
    }
    qDebug() << readBuf_;
    qDebug() << "read finised";
    readedLength_ = 0;
    emit readFinished();
}

void TcpSocket::write(QString method)
{
    if(socket_->state() == QTcpSocket::UnconnectedState)
    {
        socket_->connectToHost(hostName_, port_);
        if(!socket_->waitForConnected(Timeout)){
            emit error(socket_->error(), socket_->errorString());
            return;
        }
    }

    QByteArray ret = http.encode(writeHeader_, writeBody_, method);
    qDebug() << ret;
    int len = socket_->write(ret);
    if(len == -1){
        emit error(socket_->error(), socket_->errorString());
        return;
    }
    writeHeader_.clear();
    writeBody_.clear();
}

void TcpSocket::addHeader(const QString &key, const QByteArray& value)
{
    writeHeader_[key] = value;
}

void TcpSocket::addContent(const QByteArray &content)
{
    writeBody_ = content;
}

QByteArray TcpSocket::get(const QString& key)
{
    if(readPostData_.contains(key)){
        return readPostData_[key];
    }
    return "";
}

void TcpSocket::sendMsg(const QString &content)
{
    addHeader("Content-Type", "MESSAGE");
    addHeader("Content-Length", QString::number(content.length()).toUtf8());
    addContent(content.toUtf8());
}

void TcpSocket::sendFile(const QString &filepath, const QByteArray &content)
{
    addHeader("Content-Type", "FILE");
    addHeader("Content-Length", QString::number(content.size()).toUtf8());
    addHeader("FilePath", filepath.toUtf8());
    addContent(content);
}

void TcpSocket::onconnected()
{
    emit connected();
}
