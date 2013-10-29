#include "RestyledLabel.h"

RestyledLabel::RestyledLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    setFrameShape(QFrame::StyledPanel);
}
