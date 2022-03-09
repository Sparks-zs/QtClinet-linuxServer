#include "switchbutton.h"

SwitchButton::SwitchButton(const QString &text, int index, QWidget* parent)
    : QPushButton(parent), index_(index)
{
    setText(text);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

void SwitchButton::onClicked()
{
    emit clicked(index_);
}

QSize SwitchButton::sizeHint() const
{
    QSize size = QPushButton::sizeHint();
    size.setHeight(size.width());
    return size;
}

