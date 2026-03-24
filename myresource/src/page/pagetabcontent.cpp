#include "myresource.h"

PageTabContent::PageTabContent(QWidget *parent)
  : QWidget(parent)
{
  setupUi();
}

void PageTabContent::setupUi()
{
  // 主垂直布局
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 添加顶部弹簧
  mainLayout->addStretch(1);

  // 创建按钮水平布局
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  // 创建收发器按钮
  transceiverButton = new QToolButton();
  transceiverButton->setIcon(QIcon(":/icons/serial_transceiver.svg")); // 设置图标
  transceiverButton->setIconSize(QSize(150, 150)); // 设置图标大小
  transceiverButton->setText(tr("Serial Transceiver"));
  transceiverButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 字在图标下面

  // 创建终端按钮
  terminalButton = new QToolButton();
  terminalButton->setIcon(QIcon(":/icons/serial_terminal.svg")); // 设置图标
  terminalButton->setIconSize(QSize(150, 150)); // 设置图标大小
  terminalButton->setText(tr("Serial Terminal"));
  terminalButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 字在图标下面

  // 创建IEC61850按钮
  iec61850Button = new QToolButton();
  iec61850Button->setIcon(QIcon(":/icons/iec61850_client.svg")); // 设置图标
  iec61850Button->setIconSize(QSize(150, 150)); // 设置图标大小
  iec61850Button->setText(tr("IEC61850 Client"));
  iec61850Button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 字在图标下面

  // 创建SSH客户端按钮
  QToolButton *sshClientButton = new QToolButton();
  sshClientButton->setIcon(QIcon(":/icons/ssh_terminal.svg")); // 设置图标
  sshClientButton->setIconSize(QSize(150, 150)); // 设置图标大小
  sshClientButton->setText(tr("SSH Client"));
  sshClientButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 字在图标下面

  // 创建网络工具按钮
  QToolButton *networkToolButton = new QToolButton();
  networkToolButton->setIcon(QIcon(":/icons/network_tool.svg")); // 使用网络工具图标
  networkToolButton->setIconSize(QSize(150, 150)); // 设置图标大小
  networkToolButton->setText(tr("Network Tool"));
  networkToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // 字在图标下面

  QString transparentStyle = "QToolButton { background: transparent; border: none; }";
  transceiverButton->setStyleSheet(transparentStyle);
  terminalButton->setStyleSheet(transparentStyle);
  iec61850Button->setStyleSheet(transparentStyle);
  sshClientButton->setStyleSheet(transparentStyle);
  networkToolButton->setStyleSheet(transparentStyle);

  // 添加按钮到布局
  buttonLayout->addWidget(transceiverButton);
  buttonLayout->addSpacing(30); // 按钮间距
  buttonLayout->addWidget(terminalButton);
  buttonLayout->addSpacing(30); // 按钮间距
  buttonLayout->addWidget(iec61850Button);
  buttonLayout->addSpacing(30); // 按钮间距
  buttonLayout->addWidget(sshClientButton);
  buttonLayout->addSpacing(30); // 按钮间距
  buttonLayout->addWidget(networkToolButton);
  buttonLayout->setAlignment(Qt::AlignCenter);

  // 将按钮布局添加到主布局
  mainLayout->addLayout(buttonLayout);

  // 添加底部弹簧
  mainLayout->addStretch(1);

  // 添加版本号标签（页面底部居中）
  QLabel *versionLabel = new QLabel(QString("v%1").arg(PROJECT_VERSION));
  versionLabel->setAlignment(Qt::AlignCenter);
  versionLabel->setStyleSheet("QLabel { color: #7f8c8d; font-size: 14px; }");
  mainLayout->addSpacing(10);
  mainLayout->addWidget(versionLabel);
  mainLayout->addSpacing(10);

  // 设置整体布局
  setLayout(mainLayout);

  // 连接按钮的点击信号
  connect(transceiverButton, &QPushButton::clicked, [this]()
  {
    emit buttonClicked(transceiverButton->text());
  });

  connect(terminalButton, &QPushButton::clicked, [this]()
  {
    emit buttonClicked(terminalButton->text());
  });

  connect(iec61850Button, &QPushButton::clicked, [this]()
  {
    emit buttonClicked(iec61850Button->text());
  });

  connect(sshClientButton, &QPushButton::clicked, [ = ]()
  {
    emit buttonClicked(sshClientButton->text());
  });

  connect(networkToolButton, &QPushButton::clicked, [ = ]()
  {
    emit buttonClicked(networkToolButton->text());
  });
}
