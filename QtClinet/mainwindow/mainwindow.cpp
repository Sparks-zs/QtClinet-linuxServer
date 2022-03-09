#include "mainwindow.h"
#include <QDebug>
#include <iostream>
using namespace std;

MainWindow* MainWindow::instance_ = nullptr;

MainWindow::MainWindow(QWidget *parent)
    :  QMainWindow(parent), m_preIndex(-1), m_pageIndex(MainWindowPage)
{
    init();
}

MainWindow* MainWindow::getInstance()
{
    if(!instance_)
        instance_ = new MainWindow;
    return instance_;
}

void MainWindow::init()
{
    m_widget = new QWidget;
    m_layout = new MyLayout;
    m_widget->setLayout(m_layout);

    m_fileTransfer = new SwitchButton("File Transfer", FileWidgetPage);
    connect(m_fileTransfer, SIGNAL(clicked(int)), this, SLOT(onSwitchPage(int)));

    m_serverM = new SwitchButton("Server Monitoring", ServerMPage);
    connect(m_serverM, SIGNAL(clicked(int)), this, SLOT(onSwitchPage(int)));

    m_layout->addWidget(m_fileTransfer);
    m_layout->addWidget(m_serverM);

    m_stackWidget = new QStackedWidget;
    setCentralWidget(m_stackWidget);
    m_stackWidget->insertWidget(MainWindowPage, m_widget);
}

void MainWindow::onSwitchWidget(QWidget* widget)
{
    // 确保当前界面在动画结束前不被隐藏
    m_preIndex = m_stackWidget->currentIndex();
    if(m_stackWidget->indexOf(widget) == -1)  // 部件不在stackwidget中则加入
        m_stackWidget->addWidget(widget);
    m_stackWidget->setCurrentWidget(widget);
    m_stackWidget->widget(m_preIndex)->show();

    // 前一个界面
    QPropertyAnimation *animation1 = new QPropertyAnimation(m_stackWidget->widget(m_preIndex), "geometry");
    animation1->setDuration(200);
    animation1->setStartValue(QRect(0, 0, this->width(), this->height()));
    animation1->setEndValue(QRect(-this->width(), 0, this->width(), this->height()));

    // 下一个界面
    QPropertyAnimation *animation2 = new QPropertyAnimation(widget, "geometry");
    animation2->setDuration(200);
    animation2->setStartValue(QRect(this->width(), 0, this->width(), this->height()));
    animation2->setEndValue(QRect(0, 0, this->width(), this->height()));

    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    group->addAnimation(animation1);
    group->addAnimation(animation2);
    group->start();

    connect(group, SIGNAL(finished()), this, SLOT(onAnimationFinished()));

}

void MainWindow::onAnimationFinished()
{
    QWidget* preWidget = m_stackWidget->widget(m_preIndex);
    // 隐藏移除的画面
    if(m_preIndex > 0){
        m_stackWidget->removeWidget(preWidget);
    }
}

bool MainWindow::event(QEvent *event)
{
    if(m_pageIndex != MainWindowPage)
        return QMainWindow::event(event);
    QEvent::Type t = event->type();
    switch(t)
    {
    case QEvent::KeyRelease:
        if(((static_cast<QKeyEvent*>(event))->key() == Qt::Key_Back)
            || ((static_cast<QKeyEvent*>(event))->key() == Qt::Key_F1))
        {
            event->accept();
            int index = m_stackWidget->currentIndex();
            if(index == MainWindowPage){   // 主界面程序退出
                QCoreApplication::quit();
            }
            return true;
        }
        break;
    default:
        break;
    }
    return QMainWindow::event(event);
}

void MainWindow::onSwitchPage(int index)
{
    if(index == MainWindowPage){
        onSwitchWidget(m_widget);
    }
    else if(index == FileWidgetPage){
        onSwitchWidget(FileTransfer::getInstance());
    }
    else if(index == ServerMPage){
        onSwitchWidget(ServerM::getInstance());
    }
}
