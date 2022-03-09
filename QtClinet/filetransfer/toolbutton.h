#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class ToolButton : public QPushButton
{
    Q_OBJECT
public:
    ToolButton(QWidget* parent=nullptr);

//    virtual void mousePressEvent(QMouseEvent *event);
//    virtual void mouseReleaseEvent(QMouseEvent *event);
////    virtual void mouseDoubleClickEvent(QMouseEvent *event);
//    virtual void mouseMoveEvent(QMouseEvent *event);

//signals:
////    void moveSignals();
//private:
//    QPoint m_start;//起始点
//    QPoint m_end;//结束点
//    bool m_leftButtonPressed;//鼠标左键按下标记
//    bool isMove = false;
};

#endif // TOOLBUTTON_H
