#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QPushButton>

class SwitchButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SwitchButton(const QString &text, int index, QWidget* parent = nullptr);
    QSize sizeHint() const override;

signals:
    void clicked(int);

protected slots:
    void onClicked();

private:
    int index_;
};

#endif // MYBUTTON_H
