#include "BasicKeyboardInterfaceTest.h"

BasicKeyboardInterfaceTestWidget::BasicKeyboardInterfaceTestWidget() :
    QWidget()
{
    QLabel *plblNoChange = new QLabel("Changed: ",this);
    plblResultIsKeyDown = new QLabel(this);
    plblResultIsKeyDown->setObjectName("resultIsKeyDown");
    plblResultIsKeyUp = new QLabel(this);
    plblResultIsKeyUp->setObjectName("resultIsKeyUp");

    QLabel *plblModifier = new QLabel("Modifier: ",this);
    plblResultModifier = new QLabel(this);
    plblResultModifier->setObjectName("resultModifier");

    plnEdit= new LEdit(this);
    plnEdit->setObjectName("enabledTextElement");
    connect(plnEdit, SIGNAL(keyDown()), this, SLOT(setResultDown()));
    connect(plnEdit, SIGNAL(keyUp()), this, SLOT(setResultUp()));
    connect(plnEdit, SIGNAL(keyModifier(QString)), this, SLOT(setResultModifier(QString)));

    QVBoxLayout *pvbl = new QVBoxLayout(this);

    QHBoxLayout *phblToLabel = new QHBoxLayout;
    phblToLabel->addWidget(plblNoChange);
    phblToLabel->addWidget(plblResultIsKeyDown);
    phblToLabel->addWidget(plblResultIsKeyUp);
    pvbl->addLayout(phblToLabel);

    QHBoxLayout *phblIsModifier = new QHBoxLayout;
    phblIsModifier->addWidget(plblModifier);
    phblIsModifier->addWidget(plblResultModifier);
    pvbl->addLayout(phblIsModifier);


    pvbl->addWidget(plnEdit);

    this->setLayout(pvbl);
}

BasicKeyboardInterfaceTestWidget::~BasicKeyboardInterfaceTestWidget()
{
}

void BasicKeyboardInterfaceTestWidget::setResultDown() {
    plblResultIsKeyDown->setText("Key Down");
}

void BasicKeyboardInterfaceTestWidget::setResultUp() {
    plblResultIsKeyUp->setText("Key Up");
}

void BasicKeyboardInterfaceTestWidget::setResultModifier(const QString &str) {
    plblResultModifier->setText(str);
}

void LEdit::keyPressEvent(QKeyEvent *pe) {
    emit keyDown();
    currentText += pe->text();
    setText(currentText);
    if (pe->modifiers() & Qt::ShiftModifier)
        emit keyModifier(QString::fromStdString("Shift"));
    else if (pe->modifiers() & Qt::ControlModifier)
        emit keyModifier(QString::fromStdString("Ctrl"));
    else if (pe->modifiers() & Qt::AltModifier)
        emit keyModifier(QString::fromStdString("Alt"));
}

void LEdit::keyReleaseEvent(QKeyEvent *pe) {
    emit keyUp();
    if (pe->modifiers() & Qt::ShiftModifier)
        emit keyModifier(QString::fromStdString("Shift"));
    else if (pe->modifiers() & Qt::ControlModifier)
        emit keyModifier(QString::fromStdString("Ctrl"));
    else if (pe->modifiers() & Qt::AltModifier)
        emit keyModifier(QString::fromStdString("Alt"));

}

LEdit::LEdit(QWidget * parent): QLineEdit(parent)
{
}

LEdit::LEdit(const QString & contents, QWidget * parent):
            QLineEdit(contents, parent)
{
}

LEdit::~LEdit()
{
}
