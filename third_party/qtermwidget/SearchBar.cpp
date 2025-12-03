/*
    Copyright 2013 Christian Surlykke

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301  USA.
*/
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QLineEdit>

#include "SearchBar.h"

SearchBar::SearchBar(QWidget *parent) : QWidget(parent)
{
  setWindowTitle("SearchBar");
  setGeometry(0, 0, 399, 40);

  // Create layout
  QHBoxLayout *horizontalLayout = new QHBoxLayout(this);

  // Create widgets
  closeButton = new QToolButton(this);
  closeButton->setText("X");
  closeButton->setIcon(QIcon::fromTheme("dialog-close"));

  findLabel = new QLabel("Find:", this);

  searchTextEdit = new QLineEdit(this);

  findPreviousButton = new QToolButton(this);
  findPreviousButton->setText("<");
  findPreviousButton->setIcon(QIcon::fromTheme("go-previous"));

  findNextButton = new QToolButton(this);
  findNextButton->setText(">");
  findNextButton->setIcon(QIcon::fromTheme("go-next"));

  optionsButton = new QToolButton(this);
  optionsButton->setText("...");
  optionsButton->setIcon(QIcon::fromTheme("preferences-system"));
  optionsButton->setPopupMode(QToolButton::InstantPopup);

  // Add widgets to layout
  horizontalLayout->addWidget(closeButton);
  horizontalLayout->addWidget(findLabel);
  horizontalLayout->addWidget(searchTextEdit);
  horizontalLayout->addWidget(findPreviousButton);
  horizontalLayout->addWidget(findNextButton);
  horizontalLayout->addWidget(optionsButton);

  setAutoFillBackground(true); // make it always opaque, especially inside translucent windows
  connect(closeButton, &QAbstractButton::clicked, this, &SearchBar::hide);
  connect(searchTextEdit, SIGNAL(textChanged(QString)), this, SIGNAL(searchCriteriaChanged()));
  connect(findPreviousButton, SIGNAL(clicked()), this, SIGNAL(findPrevious()));
  connect(findNextButton, SIGNAL(clicked()), this, SIGNAL(findNext()));

  connect(this, SIGNAL(searchCriteriaChanged()), this, SLOT(clearBackgroundColor()));

  QMenu *optionsMenu = new QMenu(optionsButton);
  optionsButton->setMenu(optionsMenu);

  m_matchCaseMenuEntry = optionsMenu->addAction(tr("Match case"));
  m_matchCaseMenuEntry->setCheckable(true);
  m_matchCaseMenuEntry->setChecked(true);
  connect(m_matchCaseMenuEntry, SIGNAL(toggled(bool)), this, SIGNAL(searchCriteriaChanged()));


  m_useRegularExpressionMenuEntry = optionsMenu->addAction(tr("Regular expression"));
  m_useRegularExpressionMenuEntry->setCheckable(true);
  connect(m_useRegularExpressionMenuEntry, SIGNAL(toggled(bool)), this, SIGNAL(searchCriteriaChanged()));

  m_highlightMatchesMenuEntry = optionsMenu->addAction(tr("Highlight all matches"));
  m_highlightMatchesMenuEntry->setCheckable(true);
  m_highlightMatchesMenuEntry->setChecked(true);
  connect(m_highlightMatchesMenuEntry, SIGNAL(toggled(bool)), this, SIGNAL(highlightMatchesChanged(bool)));
}

SearchBar::~SearchBar()
{
}

QString SearchBar::searchText()
{
  return searchTextEdit->text();
}


bool SearchBar::useRegularExpression()
{
  return m_useRegularExpressionMenuEntry->isChecked();
}

bool SearchBar::matchCase()
{
  return m_matchCaseMenuEntry->isChecked();
}

bool SearchBar::highlightAllMatches()
{
  return m_highlightMatchesMenuEntry->isChecked();
}

void SearchBar::show()
{
  QWidget::show();
  searchTextEdit->setFocus();
  searchTextEdit->selectAll();
}

void SearchBar::hide()
{
  QWidget::hide();
  if(QWidget *p = parentWidget())
  {
    p->setFocus(Qt::OtherFocusReason); // give the focus to the parent widget on hiding
  }
}

void SearchBar::noMatchFound()
{
  QPalette palette;
  palette.setColor(searchTextEdit->backgroundRole(), QColor(255, 128, 128));
  searchTextEdit->setPalette(palette);
}


void SearchBar::keyReleaseEvent(QKeyEvent* keyEvent)
{
  if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
  {
    if(keyEvent->modifiers() == Qt::ShiftModifier)
    {
      Q_EMIT findPrevious();
    }
    else
    {
      Q_EMIT findNext();
    }
  }
  else if(keyEvent->key() == Qt::Key_Escape)
  {
    hide();
  }
}

void SearchBar::clearBackgroundColor()
{
  searchTextEdit->setPalette(QWidget::window()->palette());

}
