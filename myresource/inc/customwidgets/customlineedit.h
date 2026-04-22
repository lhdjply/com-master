#ifndef __CUSTOMLINEEDIT_H
#define __CUSTOMLINEEDIT_H

#include <QLineEdit>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

  public:
    explicit CustomLineEdit(QWidget *parent = nullptr);
    explicit CustomLineEdit(const QString &text, QWidget *parent = nullptr);

  protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif
