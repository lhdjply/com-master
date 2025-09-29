#include "myresource.h"
#include "ui_pageverifiydialog.h"

PageVerifiyDialog::PageVerifiyDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::PageVerifiyDialog)
{
  ui->setupUi(this);
  setFixedSize(this->width(), this->height());
  ui->selectbuttonbox->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("OK"));
  ui->selectbuttonbox->button(QDialogButtonBox::StandardButton::Cancel)->setText(tr("Cancel"));
}

PageVerifiyDialog::~PageVerifiyDialog()
{
  delete ui;
}

void PageVerifiyDialog::on_selectbuttonbox_accepted()
{
  int selectedIndex = -1;
  QList<QRadioButton *> radios = ui->groupBox->findChildren<QRadioButton *>();
  for(int i = 0; i < radios.size(); ++i)
  {
    if(radios[i]->isChecked())
    {
      selectedIndex = i;
      break;
    }
  }
  qDebug() << "选中的RadioButton序号:" << selectedIndex;
  emit selectedIndexChanged(selectedIndex);
}
