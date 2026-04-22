#include "qtinclude.h"

CustomLineEdit::CustomLineEdit(QWidget *parent)
  : QLineEdit(parent)
{
}

CustomLineEdit::CustomLineEdit(const QString &text, QWidget *parent)
  : QLineEdit(text, parent)
{
}

void CustomLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();

  // 获取标准菜单项并重新设置中文文本
  QList<QAction *> actions = menu->actions();
  for(QAction *action : actions)
  {
    if(action->text() == "&Undo")
    {
      action->setText(tr("Undo"));
    }
    else if(action->text() == "&Redo")
    {
      action->setText(tr("Redo"));
    }
    else if(action->text() == "Cu&t")
    {
      action->setText(tr("Cut"));
    }
    else if(action->text() == "&Copy")
    {
      action->setText(tr("Copy"));
    }
    else if(action->text() == "&Paste")
    {
      action->setText(tr("Paste"));
    }
    else if(action->text() == "Delete")
    {
      action->setText(tr("Delete"));
    }
    else if(action->text() == "Select All")
    {
      action->setText(tr("Select All"));
    }
  }

  menu->exec(event->globalPos());
  delete menu;
}
