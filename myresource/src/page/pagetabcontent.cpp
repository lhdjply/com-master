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
  transceiverButton = new QPushButton(tr("Transceiver"));
  transceiverButton->setFixedSize(150, 150);

  // 创建终端按钮
  terminalButton = new QPushButton(tr("Terminal"));
  terminalButton->setFixedSize(150, 150);

  // 添加按钮到布局
  buttonLayout->addWidget(transceiverButton);
  buttonLayout->addSpacing(100); // 按钮间距
  buttonLayout->addWidget(terminalButton);
  buttonLayout->setAlignment(Qt::AlignCenter);

  // 将按钮布局添加到主布局
  mainLayout->addLayout(buttonLayout);

  // 添加底部弹簧
  mainLayout->addStretch(1);

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
}
