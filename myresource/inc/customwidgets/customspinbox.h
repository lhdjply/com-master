#ifndef __CUSTOMSPINBOX_H
#define __CUSTOMSPINBOX_H

#include <QSpinBox>

class CustomSpinBox : public QSpinBox
{
    Q_OBJECT

  public:
    explicit CustomSpinBox(QWidget *parent = nullptr);

  protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif
