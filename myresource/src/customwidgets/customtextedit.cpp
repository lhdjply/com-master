#include "qtinclude.h"

CustomTextEdit::CustomTextEdit(QWidget *parent)
  : QTextEdit(parent)
{
}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();

  QList<QAction *> actions = menu->actions();
  for(QAction *action : actions)
  {
    if(action->text() == "&Undo")
    {
      action->setText(tr("Undo"));
      action->setIcon(QIcon::fromTheme("edit-undo", QIcon(":/icons/undo.png")));
    }
    else if(action->text() == "&Redo")
    {
      action->setText(tr("Redo"));
      action->setIcon(QIcon::fromTheme("edit-redo", QIcon(":/icons/redo.png")));
    }
    else if(action->text() == "Cu&t")
    {
      action->setText(tr("Cut"));
      action->setIcon(QIcon::fromTheme("edit-cut", QIcon(":/icons/cut.png")));
    }
    else if(action->text() == "&Copy")
    {
      action->setText(tr("Copy"));
      action->setIcon(QIcon::fromTheme("edit-copy", QIcon(":/icons/copy.png")));
    }
    else if(action->text() == "&Paste")
    {
      action->setText(tr("Paste"));
      action->setIcon(QIcon::fromTheme("edit-paste", QIcon(":/icons/paste.png")));
    }
    else if(action->text() == "Delete")
    {
      action->setText(tr("Delete"));
      action->setIcon(QIcon::fromTheme("edit-delete", QIcon(":/icons/delete.png")));
    }
    else if(action->text() == "Select All")
    {
      action->setText(tr("Select All"));
      action->setIcon(QIcon::fromTheme("edit-select-all", QIcon(":/icons/selectall.png")));
    }
  }

  menu->exec(event->globalPos());
  delete menu;
}
