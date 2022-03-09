#include "toolbutton.h"
#include <QDebug>
#include <QScreen>
#include <QApplication>

ToolButton::ToolButton(QWidget* parent)
    : QPushButton(parent)
{
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->resize(200,200);
    setStyleSheet(
        "ToolButton{"
        "background:transparent;"
        "background-image:url(:/images/jiahao.png);"
        "height:200px;"
        "width:200px;"
        "}"
    );

    QScreen *screen = qApp->primaryScreen();
    QSize screenSize = screen->availableSize();
    this->move(screenSize.width() - 400, screenSize.height() - 400);
}

//void ToolButton::mousePressEvent(QMouseEvent *event)
//{
//    // 鼠标左键按下事件
//    if (event->button() == Qt::LeftButton)
//    {
//        isMove = false;
//        // 记录鼠标左键状态
//        m_leftButtonPressed = true;
//        //记录鼠标在屏幕中的位置
//        m_start = event->globalPos();
////        m_start = event->pos();
//    }
//    if(!isMove)
//        QPushButton::mousePressEvent(event);

//}

//void ToolButton::mouseReleaseEvent(QMouseEvent *event)
//{
//    // 鼠标左键释放
//    if (event->button() == Qt::LeftButton)
//    {
//        if(!isMove){
////            emit released();
//            qDebug() << "released";
//            QPushButton::mouseReleaseEvent(event);
//        }
//        // 记录鼠标状态
//        m_leftButtonPressed = false;
//        isMove = false;
//    }
//}

//void ToolButton::mouseMoveEvent(QMouseEvent *event)
//{
//    // 持续按住才做对应事件
//      if(m_leftButtonPressed)
//      {
//          QSize size = this->size();//当前窗口大小
//          QSize parentSize = this->parentWidget()->size();//父窗口大小
//          QPoint curPos = geometry().topLeft() + (event->globalPos() - m_start);//移动后的位置
//          if(curPos.x() < 0)//left
//              curPos.setX(0);

//          if(curPos.y() < 0)//top
//              curPos.setY(0);

//          if( (curPos.x()+size.width()) > parentSize.width())//right
//              curPos.setX(parentSize.width() - size.width());

//          if( (curPos.y()+size.height()) > parentSize.height())//bottom
//              curPos.setY(parentSize.height() - size.height());

//          move(curPos);//移动
//          isMove = true;
////          //将鼠标在屏幕中的位置替换为新的位置
//          m_start = event->globalPos();
//      }
//      QPushButton::mouseMoveEvent(event);
//}
