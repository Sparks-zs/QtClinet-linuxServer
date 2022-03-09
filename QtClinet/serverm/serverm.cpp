#include "serverm.h"

ServerM* ServerM::instance_ = nullptr;

ServerM::ServerM(QWidget *parent) : QWidget(parent)
{
    init_();
}

ServerM* ServerM::getInstance()
{
    if(instance_ == nullptr)
        instance_ = new ServerM;
    return instance_;
}

void ServerM::init_()
{
    label = new QLabel("Server Monitoring page", this);
}
