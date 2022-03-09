#include "mylayout.h"

#include <QWidget>
#include <QDebug>

MyLayout::MyLayout(int vSpace, int hSpace, QWidget* parent)
    : m_vSpace(vSpace), m_hSpace(hSpace), QLayout(parent)
{

}

MyLayout::~MyLayout()
{
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;
}

void MyLayout::addItem(QLayoutItem *item)
{
    itemList.append(item);
}

void MyLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect);
}

QSize MyLayout::sizeHint() const
{
    return minimumSize();
}

QSize MyLayout::minimumSize() const
{
    QSize size;
    for (const QLayoutItem *item : qAsConst(itemList))
        size = size.expandedTo(item->minimumSize());

    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

int MyLayout::count() const
{
    return itemList.size();
}

QLayoutItem* MyLayout::takeAt(int index)
{
    if(index >= 0 && index < itemList.size())
        return itemList.takeAt(index);
    return NULL;
}

QLayoutItem* MyLayout::itemAt(int index) const
{
    return itemList.value(index);
}

int MyLayout::verticalSpacing() const
{
    if(m_vSpace > 0)
        return m_vSpace;
    return 0;
}

int MyLayout::horizontalSpacing() const
{
    if(m_hSpace > 0)
        return m_hSpace;
    return 0;
}

void MyLayout::doLayout(const QRect &rect)
{
    QSize s = parentWidget()->size();
    int w = s.width();
    int h = s.height();

    int x = 0, y = 0;
    int item_w = 0, item_h = 0;
    item_w = w / 2 - horizontalSpacing() / 2;
//    item_h = h / 2 - verticalSpacing() / 2;

    for(int i = 0; i < itemList.size(); ++i)
    {
        itemList.at(i)->setGeometry(QRect(x, y, item_w, item_w));
        if(i % 2 == 0)
        {
            x += item_w + horizontalSpacing();
        }
        else{
            x = 0;
            y += item_w + verticalSpacing();
        }
    }
}
