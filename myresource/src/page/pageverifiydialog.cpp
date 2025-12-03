#include "myresource.h"

PageVerifiyDialog::PageVerifiyDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi();
  retranslateUi();
  setFixedSize(this->width(), this->height());
  selectbuttonbox->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("OK"));
  selectbuttonbox->button(QDialogButtonBox::StandardButton::Cancel)->setText(tr("Cancel"));
}

PageVerifiyDialog::~PageVerifiyDialog()
{
}

void PageVerifiyDialog::setupUi()
{
  if(this->objectName().isEmpty())
    this->setObjectName(QString::fromUtf8("PageVerifiyDialog"));
  this->resize(480, 420);
  this->setMinimumSize(QSize(480, 420));
  this->setMaximumSize(QSize(600, 500));

  QString styleSheet = QString::fromUtf8(
                         "/* 主窗口样式 */\n"
                         "QDialog {\n"
                         "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
                         "                                stop: 0 #f6f9fc, stop: 1 #eef2f7);\n"
                         "    font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;\n"
                         "}\n"
                         "/* 按钮盒样式 */\n"
                         "QDialogButtonBox {\n"
                         "    background: transparent;\n"
                         "}\n"
                         "\n"
                         "QDialogButtonBox QPushButton {\n"
                         "    background-color: #3498db;\n"
                         "    border: none;\n"
                         "    border-radius: 6px;\n"
                         "    color: white;\n"
                         "    padding: 8px 16px;\n"
                         "    font-weight: 500;\n"
                         "    min-width: 80px;\n"
                         "}\n"
                         "\n"
                         "QDialogButtonBox QPushButton:hover {\n"
                         "    background-color: #2980b9;\n"
                         "}\n"
                         "\n"
                         "QDialogButtonBox QPushButton:pressed {\n"
                         "    background-color: #21618c;\n"
                         "}\n"
                         "\n"
                         "QDialogButtonBox QPushButton:focus {\n"
                         "    outline: none;\n"
                         "    border: 2px solid #8ba3f0;\n"
                         "}\n"
                         "\n"
                         "/* 取消按钮样式 */\n"
                         "QDialogButtonBox QPushButton[text=\"Cancel\"] {\n"
                         "    background-color: #6c757d;\n"
                         "}\n"
                         "\n"
                         "QDialogButtonBox QPushButton[text=\"Cancel\"]:hover {\n"
                         "    background-color: #5a6268;\n"
                         "}\n"
                         "\n"
                         "/* 组框样式 */\n"
                         "QGroupBox {\n"
                         "    background-color: white;\n"
                         "    border: 1px solid #d1d9e6;\n"
                         "    border-radius: 10px;\n"
                         "    margin-top: 10px;\n"
                         "    padding-top: 10px;\n"
                         "    font-weight: bold;\n"
                         "    color: #2c3e50;\n"
                         "}\n"
                         "\n"
                         "QGroupBox::title {\n"
                         "    subcontrol-origin: margin;\n"
                         "    subcontrol-position: top center;\n"
                         "    padding: 0 8px;\n"
                         "    background-color: white;\n"
                         "    color: #4a6ee0;\n"
                         "    font-weight: 600;\n"
                         "}\n"
                         "\n"
                         "QRadioButton {\n"
                         "    spacing: 6px;\n"
                         "}\n"
                         "QRadioButton::indicator {\n"
                         "    width: 16px;\n"
                         "    height: 16px;\n"
                         "}\n"
                         "QRadioButton::indicator:unchecked {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 2px;\n"
                         "    background-color: white;\n"
                         "}\n"
                         "QRadioButton::indicator:checked {\n"
                         "    border: 1px solid #3498db;\n"
                         "    border-radius: 2px;\n"
                         "    background-color: #3498db;\n"
                         "}"
                       );
  this->setStyleSheet(styleSheet);

  verticalLayout_2 = new QVBoxLayout(this);
  verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

  groupBox = new QGroupBox(this);
  groupBox->setObjectName(QString::fromUtf8("groupBox"));

  verticalLayout = new QVBoxLayout(groupBox);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

  radioButton_1 = new QRadioButton(groupBox);
  radioButton_1->setObjectName(QString::fromUtf8("radioButton_1"));
  radioButton_1->setChecked(true);

  verticalLayout->addWidget(radioButton_1);

  radioButton_2 = new QRadioButton(groupBox);
  radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));

  verticalLayout->addWidget(radioButton_2);

  radioButton_3 = new QRadioButton(groupBox);
  radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));

  verticalLayout->addWidget(radioButton_3);

  radioButton_4 = new QRadioButton(groupBox);
  radioButton_4->setObjectName(QString::fromUtf8("radioButton_4"));

  verticalLayout->addWidget(radioButton_4);

  radioButton_5 = new QRadioButton(groupBox);
  radioButton_5->setObjectName(QString::fromUtf8("radioButton_5"));

  verticalLayout->addWidget(radioButton_5);

  radioButton_6 = new QRadioButton(groupBox);
  radioButton_6->setObjectName(QString::fromUtf8("radioButton_6"));

  verticalLayout->addWidget(radioButton_6);

  radioButton_7 = new QRadioButton(groupBox);
  radioButton_7->setObjectName(QString::fromUtf8("radioButton_7"));

  verticalLayout->addWidget(radioButton_7);

  verticalLayout_2->addWidget(groupBox);

  selectbuttonbox = new QDialogButtonBox(this);
  selectbuttonbox->setObjectName(QString::fromUtf8("selectbuttonbox"));
  selectbuttonbox->setOrientation(Qt::Horizontal);
  selectbuttonbox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
  selectbuttonbox->setCenterButtons(true);

  verticalLayout_2->addWidget(selectbuttonbox);

  // 手动连接按钮信号
  connect(selectbuttonbox, &QDialogButtonBox::accepted, this, &PageVerifiyDialog::accept);
  connect(selectbuttonbox, &QDialogButtonBox::rejected, this, &PageVerifiyDialog::reject);

  QMetaObject::connectSlotsByName(this);
}

void PageVerifiyDialog::retranslateUi()
{
  this->setWindowTitle(QCoreApplication::translate("PageVerifiyDialog", "Calculate Checksum", nullptr));
  radioButton_1->setText(QCoreApplication::translate("PageVerifiyDialog", "CRC16/MODBUS, Little Endian", nullptr));
  radioButton_2->setText(QCoreApplication::translate("PageVerifiyDialog", "CRC16/MODBUS, Big Endian", nullptr));
  radioButton_3->setText(QCoreApplication::translate("PageVerifiyDialog", "CRC8", nullptr));
  radioButton_4->setText(QCoreApplication::translate("PageVerifiyDialog", "Sum", nullptr));
  radioButton_5->setText(QCoreApplication::translate("PageVerifiyDialog", "XOR", nullptr));
  radioButton_6->setText(QCoreApplication::translate("PageVerifiyDialog", "CRC32, Little Endian", nullptr));
  radioButton_7->setText(QCoreApplication::translate("PageVerifiyDialog", "CRC32, Big Endian", nullptr));
}

void PageVerifiyDialog::on_selectbuttonbox_accepted()
{
  int selectedIndex = -1;
  QList<QRadioButton *> radios = groupBox->findChildren<QRadioButton *>();
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
