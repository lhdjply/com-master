#include "myresource.h"
#include "appcolors.h"

PageSerialTerminal::PageSerialTerminal(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi();
  retranslateUi();

  last_serial_num = 0;
  isSerial_Open = false;

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PageSerialTerminal::handleTimeout);
  timer->start(1000);

  // 连接终端数据发送信号到串口
  connect(terminal, &QTermWidget::sendData, this, [this](const char * data, int length)
  {
    if(isSerial_Open && serialPort.isOpen())
    {
      serialPort.write(data, length);
    }
  });
}

PageSerialTerminal::~PageSerialTerminal()
{
}

void PageSerialTerminal::setupUi()
{
  if(this->objectName().isEmpty())
    this->setObjectName(QString::fromUtf8("PageSerialTerminal"));
  this->resize(1123, 700);

  this->setStyleSheet(AppColors::getStyleSheet());

  centralwidget = new QWidget(this);
  centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
  this->setCentralWidget(centralwidget);

  // 主布局 - 水平分割
  mainLayout = new QGridLayout(centralwidget);

  // 左侧连接配置组
  configGroup = new QGroupBox(tr("Serial Port Settings"));
  configLayout = new QGridLayout(configGroup);

  portLabel = new QLabel(tr("Port:"));
  baudrateLabel = new QLabel(tr("Baudrate:"));
  databitsLabel = new QLabel(tr("Data bits:"));
  parityLabel = new QLabel(tr("Parity:"));
  stopbitsLabel = new QLabel(tr("Stop bits:"));

  portdroplist = new QComboBox(configGroup);
  portdroplist->setObjectName(QString::fromUtf8("portdroplist"));

  baudratedroplist = new QComboBox(configGroup);
  baudratedroplist->setObjectName(QString::fromUtf8("baudratedroplist"));
  baudratedroplist->addItem(QString::fromUtf8("2400"));
  baudratedroplist->addItem(QString::fromUtf8("4800"));
  baudratedroplist->addItem(QString::fromUtf8("9600"));
  baudratedroplist->addItem(QString::fromUtf8("19200"));
  baudratedroplist->addItem(QString::fromUtf8("38400"));
  baudratedroplist->addItem(QString::fromUtf8("57600"));
  baudratedroplist->addItem(QString::fromUtf8("74880"));
  baudratedroplist->addItem(QString::fromUtf8("115200"));
  baudratedroplist->addItem(QString::fromUtf8("921600"));
  baudratedroplist->addItem(QString::fromUtf8("1000000"));
  baudratedroplist->addItem(QString::fromUtf8("1500000"));
  baudratedroplist->addItem(QString::fromUtf8("2000000"));
  baudratedroplist->addItem(QString::fromUtf8("4500000"));
  baudratedroplist->setCurrentIndex(7);

  databitsdroplist = new QComboBox(configGroup);
  databitsdroplist->setObjectName(QString::fromUtf8("databitsdroplist"));
  databitsdroplist->addItem(QString::fromUtf8("5"));
  databitsdroplist->addItem(QString::fromUtf8("6"));
  databitsdroplist->addItem(QString::fromUtf8("7"));
  databitsdroplist->addItem(QString::fromUtf8("8"));
  databitsdroplist->setCurrentIndex(3);

  paritydroplist = new QComboBox(configGroup);
  paritydroplist->setObjectName(QString::fromUtf8("paritydroplist"));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTerminal", "None", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTerminal", "Odd", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTerminal", "Even", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTerminal", "Mark", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTerminal", "Space", nullptr));

  stopbitsdroplist = new QComboBox(configGroup);
  stopbitsdroplist->setObjectName(QString::fromUtf8("stopbitsdroplist"));
  stopbitsdroplist->addItem(QString::fromUtf8("1"));
  stopbitsdroplist->addItem(QString::fromUtf8("1.5"));
  stopbitsdroplist->addItem(QString::fromUtf8("2"));

  openbutton = new QPushButton(tr("Open"));
  openbutton->setObjectName(QString::fromUtf8("openbutton"));
  openbutton->setStyleSheet(AppColors::getSuccessButtonStyle());

  // 配置布局 - 垂直排列
  configLayout->addWidget(portLabel, 0, 0);
  configLayout->addWidget(portdroplist, 0, 1);
  configLayout->addWidget(baudrateLabel, 1, 0);
  configLayout->addWidget(baudratedroplist, 1, 1);
  configLayout->addWidget(databitsLabel, 2, 0);
  configLayout->addWidget(databitsdroplist, 2, 1);
  configLayout->addWidget(parityLabel, 3, 0);
  configLayout->addWidget(paritydroplist, 3, 1);
  configLayout->addWidget(stopbitsLabel, 4, 0);
  configLayout->addWidget(stopbitsdroplist, 4, 1);
  configLayout->addWidget(openbutton, 5, 0, 1, 2);  // 跨两列

  // 右侧终端组
  terminalGroup = new QGroupBox(tr("Serial Terminal"));
  terminalLayout = new QVBoxLayout(terminalGroup);

  // 创建终端部件
  terminal = new QTermWidget(0, centralwidget);
  terminal->setObjectName(QString::fromUtf8("terminal"));
  terminal->setTerminalFont(QFont("Consolas", 11));
  terminal->setColorScheme("BreezeModified");
  terminal->setHistorySize(1000);
  terminal->setScrollBarPosition(QTermWidgetInterface::ScrollBarRight);
  terminal->setTerminalSizeHint(false);
  terminal->setBlinkingCursor(true);
  terminal->startTerminalTeletype();

  // 设置终端的最小大小，避免左侧设置区域变大
  terminal->setMinimumWidth(600);

  terminalLayout->addWidget(terminal);

  // 添加到主布局 - 左右分布
  mainLayout->addWidget(configGroup, 0, 0);
  mainLayout->addWidget(terminalGroup, 0, 1);

  // 设置列比例 - 左侧较窄，右侧较宽
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 3);

  // 初始化右键菜单
  setupContextMenu();

  QMetaObject::connectSlotsByName(this);

  // 连接信号和槽
  connect(&serialPort, &QSerialPort::readyRead, this, &PageSerialTerminal::readSerialData);

  // 连接终端的右键菜单信号
  // 通过 QTermWidget 的内部 TerminalDisplay 来获取右键菜单信号
  Konsole::TerminalDisplay* terminalDisplay = terminal->findChild<Konsole::TerminalDisplay *>();
  if(terminalDisplay)
  {
    connect(terminalDisplay, &Konsole::TerminalDisplay::configureRequest, this,
            &PageSerialTerminal::showTerminalContextMenu);
  }
}

void PageSerialTerminal::retranslateUi()
{
  configGroup->setTitle(tr("Serial Port Settings"));
  terminalGroup->setTitle(tr("Serial Terminal"));
  portLabel->setText(QCoreApplication::translate("PageSerialTerminal", "Port", nullptr));
  baudrateLabel->setText(QCoreApplication::translate("PageSerialTerminal", "Baudrate", nullptr));
  databitsLabel->setText(QCoreApplication::translate("PageSerialTerminal", "Data bits", nullptr));
  parityLabel->setText(QCoreApplication::translate("PageSerialTerminal", "Parity", nullptr));
  stopbitsLabel->setText(QCoreApplication::translate("PageSerialTerminal", "Stop bits", nullptr));
  openbutton->setText(QCoreApplication::translate("PageSerialTerminal", "Open", nullptr));
}

void PageSerialTerminal::on_openbutton_clicked()
{
  const auto serialPortInfos = QSerialPortInfo::availablePorts();

  if(portdroplist->count() == 0)
  {
    return;
  }
#ifdef Q_OS_WIN
  Serial_Name = serialPortInfos.at(portdroplist->currentIndex()).portName();
#else
  Serial_Name = serialPortInfos.at(portdroplist->currentIndex()).systemLocation();
#endif
  Current_BaudRate = baudratedroplist->currentText().toInt();
  Current_DataBits = (QSerialPort::DataBits)databitsdroplist->currentText().toInt();
  Current_Parity = paritydroplist->currentIndex();
  Current_StopBits = stopbitsdroplist->currentIndex();
  serialPort.setPortName(Serial_Name);
  serialPort.setBaudRate(Current_BaudRate);
  serialPort.setDataBits(Current_DataBits);
  switch(Current_Parity)
  {
    case 0:
      serialPort.setParity(QSerialPort::NoParity);
      break;
    case 1:
      serialPort.setParity(QSerialPort::OddParity);
      break;
    case 2:
      serialPort.setParity(QSerialPort::EvenParity);
      break;
    case 3:
      serialPort.setParity(QSerialPort::MarkParity);
      break;
    case 4:
      serialPort.setParity(QSerialPort::SpaceParity);
      break;
    default:
      break;
  }
  switch(Current_StopBits)
  {
    case 0:
      serialPort.setStopBits(QSerialPort::OneStop);
      break;
    case 1:
      serialPort.setStopBits(QSerialPort::OneAndHalfStop);
      break;
    case 2:
      serialPort.setStopBits(QSerialPort::TwoStop);
      break;
    default:
      break;
  }
  serialPort.setFlowControl(QSerialPort::NoFlowControl);

  if(isSerial_Open)
  {
    isSerial_Open = false;
    openbutton->setText(tr("Open"));
    openbutton->setStyleSheet(QString::fromUtf8(
                                "QPushButton {\n"
                                "    background-color: #27ae60;\n"
                                "    padding: 10px;\n"
                                "}\n"
                                "QPushButton:hover {\n"
                                "    background-color: #219653;\n"
                                "}\n"
                                "QPushButton:pressed {\n"
                                "    background-color: #1e874b;\n"
                                "}"
                              ));
    serialPort.close();
    qDebug() << "串口已关闭";
  }
  else
  {
    if(serialPort.open(QIODevice::ReadWrite))
    {
      isSerial_Open = true;
      openbutton->setText(tr("Close"));
      openbutton->setStyleSheet(AppColors::getDangerButtonStyle());
      terminal->setFocus();
      qDebug() << "串口已打开";
    }
    else
    {
      qDebug() << "串口无法打开, 错误原因:" << serialPort.errorString();
    }
  }
}

void PageSerialTerminal::handleTimeout()
{
  const auto serialPortInfos = QSerialPortInfo::availablePorts();
  if(last_serial_num != serialPortInfos.size())
  {
    last_serial_num = serialPortInfos.size();
    QString lastSelectedPort; // 用于存储当前应选中的串口

    // 如果有打开的串口，保留当前打开的串口名
    if(isSerial_Open)
    {
      lastSelectedPort = Serial_Name;
    }
    else if(portdroplist->count() > 0)
    {
      lastSelectedPort = portdroplist->currentText();
    }

    portdroplist->clear();
    for(uint32_t i = 0; i < last_serial_num; i++)
    {
#ifdef Q_OS_WIN
      portdroplist->addItem(serialPortInfos.at(i).portName());
#else
      portdroplist->addItem(serialPortInfos.at(i).systemLocation());
#endif
    }

    // 尝试恢复之前选中的串口
    if(!lastSelectedPort.isEmpty())
    {
      int index = portdroplist->findText(lastSelectedPort);
      if(index != -1)
      {
        portdroplist->setCurrentIndex(index);
      }
    }

    if(isSerial_Open)
    {
      bool portFound = false;

      // 检查当前打开的串口是否仍然在可用列表中
      for(const auto &portInfo : serialPortInfos)
      {
#ifdef Q_OS_WIN
        if(portInfo.portName() == Serial_Name)
#else
        if(portInfo.systemLocation() == Serial_Name)
#endif
        {
          portFound = true;
          break;
        }
      }
      if(!portFound)
      {
        isSerial_Open = false;
        openbutton->setText(tr("Open"));
        openbutton->setStyleSheet(QString::fromUtf8(
                                    "QPushButton {\n"
                                    "    background-color: #27ae60;\n"
                                    "    padding: 10px;\n"
                                    "}\n"
                                    "QPushButton:hover {\n"
                                    "    background-color: #219653;\n"
                                    "}\n"
                                    "QPushButton:pressed {\n"
                                    "    background-color: #1e874b;\n"
                                    "}"
                                  ));
        serialPort.close();
        qDebug() << "串口被拔除，已自动关闭";
      }
    }
  }
}

void PageSerialTerminal::readSerialData()
{
  if(isSerial_Open && serialPort.isOpen())
  {
    QByteArray data = serialPort.readAll();
    if(!data.isEmpty())
    {
      // 将接收到的数据显示在terminal中
      terminal->displayData(data);
    }
  }
}

void PageSerialTerminal::setupContextMenu()
{
  contextMenu = new QMenu(this);

  copyAction = new QAction(tr("Copy"), this);
  pasteAction = new QAction(tr("Paste"), this);
  clearAction = new QAction(tr("Clear"), this);

  // 设置图标
  copyAction->setIcon(QIcon::fromTheme("edit-copy", style()->standardIcon(QStyle::SP_FileIcon)));
  pasteAction->setIcon(QIcon::fromTheme("edit-paste", style()->standardIcon(QStyle::SP_FileIcon)));
  clearAction->setIcon(QIcon::fromTheme("edit-clear", style()->standardIcon(QStyle::SP_TrashIcon)));

  // 设置快捷键
  copyAction->setShortcut(QKeySequence::Copy);
  pasteAction->setShortcut(QKeySequence::Paste);

  // 添加到菜单
  contextMenu->addAction(copyAction);
  contextMenu->addAction(pasteAction);
  contextMenu->addSeparator();
  contextMenu->addAction(clearAction);

  // 连接信号
  connect(copyAction, &QAction::triggered, terminal, &QTermWidget::copyClipboard);
  connect(pasteAction, &QAction::triggered, terminal, &QTermWidget::pasteClipboard);
  connect(clearAction, &QAction::triggered, terminal, &QTermWidget::clear);
}

void PageSerialTerminal::showTerminalContextMenu(const QPoint& pos)
{
  // 获取全局坐标
  QPoint globalPos = terminal->mapToGlobal(pos);

  // 显示右键菜单
  contextMenu->popup(globalPos);
}
