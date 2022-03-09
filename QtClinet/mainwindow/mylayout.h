#ifndef MYLAYOUT_H
#define MYLAYOUT_H

#include <QLayout>

class MyLayout : public QLayout
{
    Q_OBJECT
public:
    MyLayout(int vSpace = 0, int hSpace = 0, QWidget* parent = nullptr);
    ~MyLayout();

    void addItem(QLayoutItem *) override;
    void setGeometry(const QRect &) override;
    QSize sizeHint() const override;
    int count() const override;
    QLayoutItem * takeAt(int index) override;
    QLayoutItem * itemAt(int index) const override;
    QSize minimumSize() const override;

private:
    void doLayout(const QRect &rect);
    int verticalSpacing() const;
    int horizontalSpacing() const;

    QList<QLayoutItem*>itemList;
    int m_vSpace;
    int m_hSpace;
};

#endif // MYLAYOUT_H
