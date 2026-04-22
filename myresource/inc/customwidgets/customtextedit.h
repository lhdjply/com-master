#ifndef __CUSTOMTEXTEDIT_H
#define __CUSTOMTEXTEDIT_H

#include <QTextEdit>

class CustomTextEdit : public QTextEdit
{
    Q_OBJECT

  public:
    explicit CustomTextEdit(QWidget *parent = nullptr);

  protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif
