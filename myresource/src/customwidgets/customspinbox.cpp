#include "qtinclude.h"

CustomSpinBox::CustomSpinBox(QWidget *parent)
  : QSpinBox(parent)
{
}

void CustomSpinBox::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = new QMenu(this);

  QAction *undoAction = menu->addAction(QIcon::fromTheme("edit-undo", QIcon(":/icons/undo.png")), tr("Undo"));
  QAction *redoAction = menu->addAction(QIcon::fromTheme("edit-redo", QIcon(":/icons/redo.png")), tr("Redo"));
  menu->addSeparator();
  QAction *cutAction = menu->addAction(QIcon::fromTheme("edit-cut", QIcon(":/icons/cut.png")), tr("Cut"));
  QAction *copyAction = menu->addAction(QIcon::fromTheme("edit-copy", QIcon(":/icons/copy.png")), tr("Copy"));
  QAction *pasteAction = menu->addAction(QIcon::fromTheme("edit-paste", QIcon(":/icons/paste.png")), tr("Paste"));
  QAction *deleteAction = menu->addAction(QIcon::fromTheme("edit-delete", QIcon(":/icons/delete.png")), tr("Delete"));
  menu->addSeparator();
  QAction *selectAllAction = menu->addAction(QIcon::fromTheme("edit-select-all", QIcon(":/icons/selectall.png")),
                                             tr("Select All"));

  QLineEdit *lineEdit = this->lineEdit();
  bool hasSelectedText = lineEdit->hasSelectedText();
  bool hasText = !lineEdit->text().isEmpty();
  bool isAllSelected = lineEdit->selectedText().length() == lineEdit->text().length() && hasText;

  undoAction->setEnabled(lineEdit->isUndoAvailable());
  redoAction->setEnabled(lineEdit->isRedoAvailable());
  cutAction->setEnabled(hasSelectedText);
  copyAction->setEnabled(hasSelectedText);
  deleteAction->setEnabled(hasSelectedText);
  selectAllAction->setEnabled(hasText && !isAllSelected);

  connect(undoAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->undo();
  });
  connect(redoAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->redo();
  });
  connect(cutAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->cut();
  });
  connect(copyAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->copy();
  });
  connect(pasteAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->paste();
  });
  connect(deleteAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->backspace();
  });
  connect(selectAllAction, &QAction::triggered, this, [this]()
  {
    this->lineEdit()->selectAll();
  });

  menu->exec(event->globalPos());
  delete menu;
}
