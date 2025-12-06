#include "myresource.h"

PageMainWindow::PageMainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi();
}

void PageMainWindow::setupUi()
{
  // 窗口属性
  this->setWindowTitle(tr("ly Serial Tool"));
  this->resize(1100, 700);
  this->setMinimumSize(1100, 700);
  this->setLayoutDirection(Qt::LeftToRight);

  // 创建中心部件和主布局
  centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setMinimumSize(0, 0);
  gridLayout = new QGridLayout(centralWidget);
  gridLayout->setContentsMargins(0, 0, 0, 0);

  // 创建标签页组件
  tabWidget = new QTabWidget(centralWidget);

  // 创建自定义标签栏
  tabBarWidget = new QWidget();
  tabBarLayout = new QHBoxLayout(tabBarWidget);
  tabBarLayout->setContentsMargins(0, 0, 0, 0);
  tabBarLayout->setSpacing(0);

  // 创建"+"按钮
  addTabButton = new QPushButton("+", tabBarWidget);
  addTabButton->setFixedSize(30, 30);
  addTabButton->setToolTip(tr("New window"));

  // 将按钮添加到标签栏布局
  tabBarLayout->addStretch(); // 添加弹性空间，使按钮靠右
  tabBarLayout->addWidget(addTabButton);

  // 设置标签页的角部件
  tabWidget->setCornerWidget(tabBarWidget, Qt::TopRightCorner);

  // 添加初始标签页
  addNewTab();

  // 连接信号和槽
  connect(addTabButton, &QPushButton::clicked, this, &PageMainWindow::addNewTab);

  // 新增：连接标签关闭请求信号
  connect(tabWidget, &QTabWidget::tabCloseRequested, this, &PageMainWindow::closeTab);

  // 将标签页添加到主布局
  gridLayout->addWidget(tabWidget, 0, 0);
}

// 添加新标签页的槽函数
void PageMainWindow::addNewTab()
{
  // 创建新标签页
  PageTabContent *newTab = new PageTabContent();
  int tabIndex = tabWidget->addTab(newTab, tr("Function selection"));
  tabWidget->setTabToolTip(tabIndex, tr("Function selection"));
  tabWidget->setCurrentIndex(tabIndex); // 切换到新标签
  tabWidget->setTabIcon(tabIndex, QIcon(":/icons/window.png")); // 设置标签图标

  // 当标签页数量大于1时显示关闭按钮，否则隐藏
  tabWidget->setTabsClosable(tabWidget->count() > 1);

  // 连接按钮点击信号到更改标题的槽函数
  connect(newTab, &PageTabContent::buttonClicked,
          this, &PageMainWindow::opentoolpage);
}

// 新增：关闭标签页的功能
void PageMainWindow::closeTab(int index)
{
  if(index < 0 || index >= tabWidget->count())
    return;

  // 获取要关闭的标签页部件
  QWidget* tab = tabWidget->widget(index);

  // 从标签页中移除（不删除）
  tabWidget->removeTab(index);

  // 删除实际的QWidget对象
  delete tab;

  // 当标签页数量大于1时显示关闭按钮，否则隐藏
  tabWidget->setTabsClosable(tabWidget->count() > 1);
}

// 新增：当点击按钮时打开对应工具页面
void PageMainWindow::opentoolpage(const QString &buttonName)
{
  // 获取发送信号的对象
  PageTabContent *senderContent = qobject_cast<PageTabContent *>(sender());
  if(!senderContent) return;

  // 找到包含该内容的标签页索引
  int tabIndex = findTabWithContent(senderContent);
  if(tabIndex < 0) return;

  tabWidget->removeTab(tabIndex);

  QIcon tabIcon;

  if(buttonName.compare(tr("Serial Transceiver")) == 0)
  {
    PageSerialTransceiver *newTab = new PageSerialTransceiver();
    tabIcon = QIcon(":/icons/serial_transceiver.png"); // 串口收发器图标路径
    tabWidget->insertTab(tabIndex, newTab, tabIcon, buttonName);
  }
  else if(buttonName.compare(tr("Serial Terminal")) == 0)
  {
    PageSerialTerminal *newTab = new PageSerialTerminal();
    tabIcon = QIcon(":/icons/serial_terminal.png"); // 串口终端图标路径
    tabWidget->insertTab(tabIndex, newTab, tabIcon, buttonName);
  }
  else if(buttonName.compare(tr("IEC61850 Client")) == 0)
  {
    PageIEC61850Client *newTab = new PageIEC61850Client();
    newTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabIcon = QIcon(":/icons/iec61850_client.png"); // 61850图标路径
    tabWidget->insertTab(tabIndex, newTab, tabIcon, buttonName);
  }
  else if(buttonName.compare(tr("SSH Client")) == 0)
  {
    PageSshClient *newTab = new PageSshClient();
    newTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tabIcon = QIcon(":/icons/ssh_terminal.png"); // ssh终端图标路径
    tabWidget->insertTab(tabIndex, newTab, tabIcon, buttonName);
  }
  tabWidget->setTabToolTip(tabIndex, buttonName);
  tabWidget->setCurrentIndex(tabIndex); // 切换到新标签

  // 当标签页数量大于1时显示关闭按钮，否则隐藏
  tabWidget->setTabsClosable(tabWidget->count() > 1);
}

// 新增：查找包含指定内容的标签页索引
int PageMainWindow::findTabWithContent(PageTabContent *content)
{
  for(int i = 0; i < tabWidget->count(); ++i)
  {
    QWidget *widget = tabWidget->widget(i);
    if(widget == content)
    {
      return i;
    }
  }
  return -1;
}
