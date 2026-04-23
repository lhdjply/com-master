#include "myresource.h"
#include "appcolors.h"

PageSerialTransceiver::PageSerialTransceiver(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi();
  retranslateUi();

  receive_frame_count = 0;
  receive_byte_count = 0;
  send_frame_count = 0;
  send_byte_count = 0;
  receiveFramesCountLabel->setText(QString::number(receive_frame_count));
  receiveBytesCountLabel->setText(QString::number(receive_byte_count));
  sendFramesCountLabel->setText(QString::number(send_frame_count));
  sendBytesCountLabel->setText(QString::number(send_byte_count));

  last_serial_num = 0;
  isSerial_Open = false;
  frameBuffer.clear(); // Initialize frame buffer

  // Initialize frame processing timer
  frameProcessingTimer = new QTimer(this);
  frameProcessingTimer->setSingleShot(true); // Single shot timer
  connect(frameProcessingTimer, &QTimer::timeout, this, &PageSerialTransceiver::onFrameProcessingTimeout);

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PageSerialTransceiver::handleTimeout);
  timer->start(1000);

  AutosendTimer = new QTimer(this);
  connect(AutosendTimer, &QTimer::timeout, this, &PageSerialTransceiver::autosend);
  AutosendTimer->start(1000);
}

PageSerialTransceiver::~PageSerialTransceiver()
{
}

void PageSerialTransceiver::setupUi()
{
  if(this->objectName().isEmpty())
    this->setObjectName(QString::fromUtf8("PageSerialTransceiver"));
  this->resize(1123, 700);

  this->setStyleSheet(AppColors::getStyleSheet());

  centralwidget = new QWidget(this);
  centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
  this->setCentralWidget(centralwidget);

  mainLayout = new QHBoxLayout(centralwidget);
  mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
  mainLayout->setStretch(0, 0);
  mainLayout->setStretch(1, 0);

  configFrame = new QFrame(centralwidget);
  configFrame->setObjectName(QString::fromUtf8("configFrame"));
  configFrame->setMinimumSize(QSize(280, 0));
  configFrame->setMaximumSize(QSize(280, 16777215));

  configFrameLayout = new QVBoxLayout(configFrame);
  configFrameLayout->setObjectName(QString::fromUtf8("configFrameLayout"));
  configFrameLayout->setSpacing(15);

  configGroupBox = new QGroupBox(configFrame);
  configGroupBox->setObjectName(QString::fromUtf8("configGroupBox"));

  configLayout = new QVBoxLayout(configGroupBox);
  configLayout->setObjectName(QString::fromUtf8("configLayout"));
  configLayout->setSpacing(12);

  portLayout = new QHBoxLayout();
  portLayout->setObjectName(QString::fromUtf8("portLayout"));

  portLabel = new QLabel(configGroupBox);
  portLabel->setObjectName(QString::fromUtf8("portLabel"));
  portLabel->setMinimumSize(QSize(80, 0));

  portLayout->addWidget(portLabel);

  portdroplist = new QComboBox(configGroupBox);
  portdroplist->setObjectName(QString::fromUtf8("portdroplist"));
  portdroplist->setMinimumSize(QSize(146, 0));

  portLayout->addWidget(portdroplist);

  configLayout->addLayout(portLayout);

  baudrateLayout = new QHBoxLayout();
  baudrateLayout->setObjectName(QString::fromUtf8("baudrateLayout"));

  baudrateLabel = new QLabel(configGroupBox);
  baudrateLabel->setObjectName(QString::fromUtf8("baudrateLabel"));
  baudrateLabel->setMinimumSize(QSize(80, 0));

  baudrateLayout->addWidget(baudrateLabel);

  baudratedroplist = new QComboBox(configGroupBox);
  baudratedroplist->setObjectName(QString::fromUtf8("baudratedroplist"));
  baudratedroplist->setMinimumSize(QSize(146, 0));
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
  baudratedroplist->setCurrentIndex(2);

  baudrateLayout->addWidget(baudratedroplist);

  configLayout->addLayout(baudrateLayout);

  databitsLayout = new QHBoxLayout();
  databitsLayout->setObjectName(QString::fromUtf8("databitsLayout"));

  databitsLabel = new QLabel(configGroupBox);
  databitsLabel->setObjectName(QString::fromUtf8("databitsLabel"));
  databitsLabel->setMinimumSize(QSize(80, 0));

  databitsLayout->addWidget(databitsLabel);

  databitsdroplist = new QComboBox(configGroupBox);
  databitsdroplist->setObjectName(QString::fromUtf8("databitsdroplist"));
  databitsdroplist->setMinimumSize(QSize(146, 0));
  databitsdroplist->addItem(QString::fromUtf8("5"));
  databitsdroplist->addItem(QString::fromUtf8("6"));
  databitsdroplist->addItem(QString::fromUtf8("7"));
  databitsdroplist->addItem(QString::fromUtf8("8"));
  databitsdroplist->setCurrentIndex(3);

  databitsLayout->addWidget(databitsdroplist);

  configLayout->addLayout(databitsLayout);

  parityLayout = new QHBoxLayout();
  parityLayout->setObjectName(QString::fromUtf8("parityLayout"));

  parityLabel = new QLabel(configGroupBox);
  parityLabel->setObjectName(QString::fromUtf8("parityLabel"));
  parityLabel->setMinimumSize(QSize(80, 0));

  parityLayout->addWidget(parityLabel);

  paritydroplist = new QComboBox(configGroupBox);
  paritydroplist->setObjectName(QString::fromUtf8("paritydroplist"));
  paritydroplist->setMinimumSize(QSize(146, 0));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTransceiver", "None", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTransceiver", "Odd", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTransceiver", "Even", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTransceiver", "Mark", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageSerialTransceiver", "Space", nullptr));

  parityLayout->addWidget(paritydroplist);

  configLayout->addLayout(parityLayout);

  stopbitsLayout = new QHBoxLayout();
  stopbitsLayout->setObjectName(QString::fromUtf8("stopbitsLayout"));

  stopbitsLabel = new QLabel(configGroupBox);
  stopbitsLabel->setObjectName(QString::fromUtf8("stopbitsLabel"));
  stopbitsLabel->setMinimumSize(QSize(80, 0));

  stopbitsLayout->addWidget(stopbitsLabel);

  stopbitsdroplist = new QComboBox(configGroupBox);
  stopbitsdroplist->setObjectName(QString::fromUtf8("stopbitsdroplist"));
  stopbitsdroplist->setMinimumSize(QSize(146, 0));
  stopbitsdroplist->addItem(QString::fromUtf8("1"));
  stopbitsdroplist->addItem(QString::fromUtf8("1.5"));
  stopbitsdroplist->addItem(QString::fromUtf8("2"));

  stopbitsLayout->addWidget(stopbitsdroplist);

  configLayout->addLayout(stopbitsLayout);

  openButton = new QPushButton(configGroupBox);
  openButton->setObjectName(QString::fromUtf8("openButton"));
  openButton->setStyleSheet(AppColors::getSuccessButtonStyle());

  configLayout->addWidget(openButton);

  configFrameLayout->addWidget(configGroupBox);

  statsGroupBox = new QGroupBox(configFrame);
  statsGroupBox->setObjectName(QString::fromUtf8("statsGroupBox"));

  statsLayout = new QVBoxLayout(statsGroupBox);
  statsLayout->setObjectName(QString::fromUtf8("statsLayout"));
  statsLayout->setSpacing(10);

  receiveFramesLayout = new QHBoxLayout();
  receiveFramesLayout->setObjectName(QString::fromUtf8("receiveFramesLayout"));

  receiveFramesLabel = new QLabel(statsGroupBox);
  receiveFramesLabel->setObjectName(QString::fromUtf8("receiveFramesLabel"));

  receiveFramesLayout->addWidget(receiveFramesLabel);

  receiveFramesCountLabel = new QLabel(statsGroupBox);
  receiveFramesCountLabel->setObjectName(QString::fromUtf8("receiveFramesCountLabel"));
  receiveFramesCountLabel->setStyleSheet(AppColors::getReceiveLabelStyle());
  receiveFramesCountLabel->setText(QString::fromUtf8("0"));

  receiveFramesLayout->addWidget(receiveFramesCountLabel);

  statsLayout->addLayout(receiveFramesLayout);

  receiveBytesLayout = new QHBoxLayout();
  receiveBytesLayout->setObjectName(QString::fromUtf8("receiveBytesLayout"));

  receiveBytesLabel = new QLabel(statsGroupBox);
  receiveBytesLabel->setObjectName(QString::fromUtf8("receiveBytesLabel"));

  receiveBytesLayout->addWidget(receiveBytesLabel);

  receiveBytesCountLabel = new QLabel(statsGroupBox);
  receiveBytesCountLabel->setObjectName(QString::fromUtf8("receiveBytesCountLabel"));
  receiveBytesCountLabel->setStyleSheet(AppColors::getReceiveLabelStyle());
  receiveBytesCountLabel->setText(QString::fromUtf8("0"));

  receiveBytesLayout->addWidget(receiveBytesCountLabel);

  statsLayout->addLayout(receiveBytesLayout);

  sendFramesLayout = new QHBoxLayout();
  sendFramesLayout->setObjectName(QString::fromUtf8("sendFramesLayout"));

  sendFramesLabel = new QLabel(statsGroupBox);
  sendFramesLabel->setObjectName(QString::fromUtf8("sendFramesLabel"));

  sendFramesLayout->addWidget(sendFramesLabel);

  sendFramesCountLabel = new QLabel(statsGroupBox);
  sendFramesCountLabel->setObjectName(QString::fromUtf8("sendFramesCountLabel"));
  sendFramesCountLabel->setStyleSheet(AppColors::getSendLabelStyle());
  sendFramesCountLabel->setText(QString::fromUtf8("0"));

  sendFramesLayout->addWidget(sendFramesCountLabel);

  statsLayout->addLayout(sendFramesLayout);

  sendBytesLayout = new QHBoxLayout();
  sendBytesLayout->setObjectName(QString::fromUtf8("sendBytesLayout"));

  sendBytesLabel = new QLabel(statsGroupBox);
  sendBytesLabel->setObjectName(QString::fromUtf8("sendBytesLabel"));

  sendBytesLayout->addWidget(sendBytesLabel);

  sendBytesCountLabel = new QLabel(statsGroupBox);
  sendBytesCountLabel->setObjectName(QString::fromUtf8("sendBytesCountLabel"));
  sendBytesCountLabel->setStyleSheet(AppColors::getSendLabelStyle());
  sendBytesCountLabel->setText(QString::fromUtf8("0"));

  sendBytesLayout->addWidget(sendBytesCountLabel);

  statsLayout->addLayout(sendBytesLayout);

  clearcountButton = new QPushButton(statsGroupBox);
  clearcountButton->setObjectName(QString::fromUtf8("clearcountButton"));

  statsLayout->addWidget(clearcountButton);

  configFrameLayout->addWidget(statsGroupBox);

  mainLayout->addWidget(configFrame);

  dataLayout = new QVBoxLayout();
  dataLayout->setObjectName(QString::fromUtf8("dataLayout"));
  dataLayout->setStretch(0, 1);
  dataLayout->setStretch(1, 0);
  dataLayout->setSpacing(15);

  receiveGroupBox = new QGroupBox();
  receiveGroupBox->setObjectName(QString::fromUtf8("receiveGroupBox"));
  receiveGroupBox->setMinimumSize(QSize(0, 320));

  receiveLayout = new QVBoxLayout(receiveGroupBox);
  receiveLayout->setObjectName(QString::fromUtf8("receiveLayout"));
  receiveLayout->setSpacing(10);

  receiveEdit = new CustomTextEdit(receiveGroupBox);
  receiveEdit->setObjectName(QString::fromUtf8("receiveEdit"));
  receiveEdit->setStyleSheet(QString::fromUtf8("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;"));

  receiveLayout->addWidget(receiveEdit);

  receiveOptionsLayout = new QHBoxLayout();
  receiveOptionsLayout->setObjectName(QString::fromUtf8("receiveOptionsLayout"));
  receiveOptionsLayout->setSpacing(15);

  receiveHexCheckbox = new QCheckBox(receiveGroupBox);
  receiveHexCheckbox->setObjectName(QString::fromUtf8("receiveHexCheckbox"));

  receiveOptionsLayout->addWidget(receiveHexCheckbox);

  displaySendCheckbox = new QCheckBox(receiveGroupBox);
  displaySendCheckbox->setObjectName(QString::fromUtf8("displaySendCheckbox"));

  receiveOptionsLayout->addWidget(displaySendCheckbox);

  displayTimeCheckbox = new QCheckBox(receiveGroupBox);
  displayTimeCheckbox->setObjectName(QString::fromUtf8("displayTimeCheckbox"));

  receiveOptionsLayout->addWidget(displayTimeCheckbox);

  lineBreakCheckbox = new QCheckBox(receiveGroupBox);
  lineBreakCheckbox->setObjectName(QString::fromUtf8("lineBreakCheckbox"));

  receiveOptionsLayout->addWidget(lineBreakCheckbox);

  clearReceiveButton = new QPushButton(receiveGroupBox);
  clearReceiveButton->setObjectName(QString::fromUtf8("clearReceiveButton"));

  receiveOptionsLayout->addWidget(clearReceiveButton);
  receiveOptionsLayout->addStretch();

  receiveLayout->addLayout(receiveOptionsLayout);

  dataLayout->addWidget(receiveGroupBox);

  sendGroupBox = new QGroupBox();
  sendGroupBox->setObjectName(QString::fromUtf8("sendGroupBox"));
  sendGroupBox->setMaximumSize(QSize(16777215, 230));

  sendLayout = new QVBoxLayout(sendGroupBox);
  sendLayout->setObjectName(QString::fromUtf8("sendLayout"));
  sendLayout->setSpacing(10);

  sendEdit = new CustomTextEdit(sendGroupBox);
  sendEdit->setObjectName(QString::fromUtf8("sendEdit"));
  sendEdit->setStyleSheet(QString::fromUtf8("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;"));

  sendLayout->addWidget(sendEdit);

  sendOptionsLayout = new QHBoxLayout();
  sendOptionsLayout->setObjectName(QString::fromUtf8("sendOptionsLayout"));
  sendOptionsLayout->setSpacing(10);

  sendButton = new QPushButton(sendGroupBox);
  sendButton->setObjectName(QString::fromUtf8("sendButton"));
  sendButton->setMinimumSize(QSize(130, 36));

  sendOptionsLayout->addWidget(sendButton);

  verifyButton = new QPushButton(sendGroupBox);
  verifyButton->setObjectName(QString::fromUtf8("verifyButton"));
  verifyButton->setMinimumSize(QSize(130, 36));

  sendOptionsLayout->addWidget(verifyButton);

  clearSendButton = new QPushButton(sendGroupBox);
  clearSendButton->setObjectName(QString::fromUtf8("clearSendButton"));
  clearSendButton->setMinimumSize(QSize(130, 36));

  sendOptionsLayout->addWidget(clearSendButton);

  sendHexCheckbox = new QCheckBox(sendGroupBox);
  sendHexCheckbox->setObjectName(QString::fromUtf8("sendHexCheckbox"));

  sendOptionsLayout->addWidget(sendHexCheckbox);

  autoSendCheckbox = new QCheckBox(sendGroupBox);
  autoSendCheckbox->setObjectName(QString::fromUtf8("autoSendCheckbox"));

  sendOptionsLayout->addWidget(autoSendCheckbox);

  sendDutyFrame = new QFrame(sendGroupBox);
  sendDutyFrame->setObjectName(QString::fromUtf8("sendDutyFrame"));
  sendDutyFrame->setMaximumSize(QSize(200, 50));

  sendDutyLayout = new QHBoxLayout(sendDutyFrame);
  sendDutyLayout->setSpacing(5);
  sendDutyLayout->setObjectName(QString::fromUtf8("sendDutyLayout"));
  sendDutyLayout->setContentsMargins(0, 0, 0, 0);

  sendDutyLabel = new QLabel(sendDutyFrame);
  sendDutyLabel->setObjectName(QString::fromUtf8("sendDutyLabel"));

  sendDutyLayout->addWidget(sendDutyLabel);

  sendDutyEdit = new CustomLineEdit(sendDutyFrame);
  sendDutyEdit->setObjectName(QString::fromUtf8("sendDutyEdit"));
  sendDutyEdit->setMaximumSize(QSize(60, 16777215));
  sendDutyEdit->setText(QString::fromUtf8("1000"));
  sendDutyEdit->setAlignment(Qt::AlignCenter);

  sendDutyLayout->addWidget(sendDutyEdit);

  sendDutyUnitLabel = new QLabel(sendDutyFrame);
  sendDutyUnitLabel->setObjectName(QString::fromUtf8("sendDutyUnitLabel"));

  sendDutyLayout->addWidget(sendDutyUnitLabel);

  sendOptionsLayout->addWidget(sendDutyFrame);
  sendOptionsLayout->addStretch();

  sendLayout->addLayout(sendOptionsLayout);

  dataLayout->addWidget(sendGroupBox);

  mainLayout->addLayout(dataLayout);

  QMetaObject::connectSlotsByName(this);
}

void PageSerialTransceiver::retranslateUi()
{
  configGroupBox->setTitle(QCoreApplication::translate("PageSerialTransceiver", "Serial Port Settings", nullptr));
  portLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Port", nullptr));
  baudrateLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Baudrate", nullptr));
  databitsLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Data bits", nullptr));
  parityLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Parity", nullptr));
  stopbitsLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Stop bits", nullptr));
  openButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Open", nullptr));
  statsGroupBox->setTitle(QCoreApplication::translate("PageSerialTransceiver", "Statistics", nullptr));
  receiveFramesLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Received frames", nullptr));
  receiveBytesLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Received bytes", nullptr));
  sendFramesLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Send frames", nullptr));
  sendBytesLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Send bytes", nullptr));
  clearcountButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Clear count", nullptr));
  receiveGroupBox->setTitle(QCoreApplication::translate("PageSerialTransceiver", "Received Data", nullptr));
  receiveHexCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Hex", nullptr));
  displaySendCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Display send", nullptr));
  displayTimeCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Display time", nullptr));
  lineBreakCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Frame line break", nullptr));
  clearReceiveButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Clear Receive", nullptr));
  sendGroupBox->setTitle(QCoreApplication::translate("PageSerialTransceiver", "Send Data", nullptr));
  sendButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Send", nullptr));
  verifyButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Verification", nullptr));
  clearSendButton->setText(QCoreApplication::translate("PageSerialTransceiver", "Clear Send", nullptr));
  sendHexCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Hex", nullptr));
  autoSendCheckbox->setText(QCoreApplication::translate("PageSerialTransceiver", "Auto send", nullptr));
  sendDutyLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "Send duty", nullptr));
  sendDutyUnitLabel->setText(QCoreApplication::translate("PageSerialTransceiver", "ms", nullptr));
}

void PageSerialTransceiver::on_openButton_clicked()
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
    openButton->setText(tr("Open"));
    openButton->setStyleSheet(
      "QPushButton {"
      "  background-color: #27ae60;"
      "  padding: 10px;"
      "}"
      "QPushButton:hover {"
      "  background-color: #219653;"
      "}"
      "QPushButton:pressed {"
      "  background-color: #1e874b;"
      "}"
    );
    serialPort.close();
    qDebug() << "串口已关闭";
    disconnect(&serialPort, &QSerialPort::readyRead, this, &PageSerialTransceiver::receiveData);
  }
  else
  {
    if(serialPort.open(QIODevice::ReadWrite))
    {
      isSerial_Open = true;
      openButton->setText(tr("Close"));
      openButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #e74c3c;"
        "  padding: 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c0392b;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #a93226;"
        "}"
      );
      qDebug() << "串口已打开";
      // 连接信号槽，接收数据
      connect(&serialPort, &QSerialPort::readyRead, this, &PageSerialTransceiver::receiveData);
    }
    else
    {
      qDebug() << "串口无法打开, 错误原因:" << serialPort.errorString();
    }
  }
}

void PageSerialTransceiver::on_sendButton_clicked()
{
  sendData();
}

void PageSerialTransceiver::on_verifyButton_clicked()
{
  if(sendHexCheckbox->isChecked())
  {
    PageVerifiyDialog pageVerifiyDialog;
    connect(&pageVerifiyDialog, &PageVerifiyDialog::selectedIndexChanged, this, &PageSerialTransceiver::verifysend);

    // 设置对话框为模态
    pageVerifiyDialog.setModal(true);

    // 计算对话框在父窗口中间的位置
    QPoint parentCenter = this->window()->geometry().center();
    QSize dialogSize = pageVerifiyDialog.size();

    // 如果对话框还没有显示过，size()可能返回无效值，所以先调整大小
    pageVerifiyDialog.adjustSize();
    dialogSize = pageVerifiyDialog.size();

    // 计算位置并移动对话框
    QPoint dialogPos = parentCenter - QPoint(dialogSize.width() / 2, dialogSize.height() / 2);
    pageVerifiyDialog.move(dialogPos);

    pageVerifiyDialog.exec();
  }
}

void PageSerialTransceiver::on_clearcountButton_clicked()
{
  receive_frame_count = 0;
  receive_byte_count = 0;
  send_frame_count = 0;
  send_byte_count = 0;
  receiveFramesCountLabel->setText(QString::number(receive_frame_count));
  receiveBytesCountLabel->setText(QString::number(receive_byte_count));
  sendFramesCountLabel->setText(QString::number(send_frame_count));
  sendBytesCountLabel->setText(QString::number(send_byte_count));
}

void PageSerialTransceiver::on_clearReceiveButton_clicked()
{
  receiveEdit->clear();
  frameBuffer.clear(); // Also clear the frame buffer
  frameProcessingTimer->stop(); // Stop the timer too
}

void PageSerialTransceiver::on_clearSendButton_clicked()
{
  sendEdit->clear();
}

void PageSerialTransceiver::on_sendDutyEdit_textChanged(const QString &arg1)
{
  int interval = arg1.toInt();
  if(interval < 10)
  {
    interval = 10;
  }
  AutosendTimer->setInterval(interval);
}

void PageSerialTransceiver::handleTimeout()
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
        openButton->setText(tr("Open"));
        openButton->setText(tr("Open"));
        openButton->setStyleSheet(
          "QPushButton {"
          "  background-color: #27ae60;"
          "  padding: 10px;"
          "}"
          "QPushButton:hover {"
          "  background-color: #219653;"
          "}"
          "QPushButton:pressed {"
          "  background-color: #1e874b;"
          "}"
        );
        serialPort.close();
        qDebug() << "串口被拔除，已自动关闭";
      }
    }
  }
}

void PageSerialTransceiver::autosend()
{
  if(autoSendCheckbox->isChecked())
  {
    sendData();
  }
}

void PageSerialTransceiver::sendData()
{
  QByteArray byteArray;
  if(!isSerial_Open)
  {
    qDebug() << "请先打开串口";
    return;
  }
  QString dataToSend = sendEdit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(sendHexCheckbox->isChecked())
  {
    // 十六进制发送
    QString hexInput = dataToSend.trimmed();
    // 如果没有空格且长度大于2，自动每两个字符插入一个空格
    if(!hexInput.contains(' ') && hexInput.length() > 2)
    {
      QString spacedHex;
      for(int i = 0; i < hexInput.length(); i += 2)
      {
        if(i + 2 <= hexInput.length())
          spacedHex += hexInput.mid(i, 2) + " ";
        else
          spacedHex += hexInput.mid(i); // 处理最后一个字符（如果是奇数长度）
      }
      hexInput = spacedHex.trimmed();
    }
    QStringList hexValues = hexInput.split(' ', Qt::SkipEmptyParts);
    bool ok;
    for(const QString &hex : hexValues)
    {
      int byte = hex.toInt(&ok, 16);
      if(ok && byte >= 0 && byte <= 255)
      {
        byteArray.append(static_cast<char>(byte));
      }
      else
      {
        qDebug() << "无效的十六进制数:" << hex;
        return;
      }
    }
  }
  else
  {
    byteArray = dataToSend.toUtf8();
  }
  qint64 bytesWritten = serialPort.write(byteArray);
  if(bytesWritten == -1)
  {
    qDebug() << "写入数据失败, 错误原因:" << serialPort.errorString();
  }
  else if(bytesWritten != byteArray.size())
  {
    qDebug() << "未能写入所有数据, 已写入字节数:" << bytesWritten;
  }
  else
  {
    if(displaySendCheckbox->isChecked())
    {
      receiveEdit->moveCursor(QTextCursor::End);  // 移动光标至末尾
      if(receiveHexCheckbox->isChecked())
      {
        // 十六进制显示
        QString hexString;
        for(char byte : byteArray)
        {
          hexString += QString("%1 ").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')).toUpper();
        }
        dataToSend = hexString.trimmed(); // 去除末尾空格
      }
      dataToSend = dataToSend + "  ";
      if(lineBreakCheckbox->isChecked())
      {
        dataToSend = dataToSend + "\n"; // 插入换行符
      }
      dataToSend = "[发送]" + dataToSend;
      if(displayTimeCheckbox->isChecked())
      {
        QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
        dataToSend = currentTime + dataToSend;  // 插入当前时间
      }
      //插入绿色字体
      receiveEdit->setTextColor(QColor(0, 128, 0)); // 设置为绿色
      receiveEdit->insertPlainText(dataToSend);  // 插入文本
    }
    qDebug() << "数据已成功写入, 字节数:" << bytesWritten;
    send_frame_count++;
    send_byte_count = send_byte_count + bytesWritten;
    sendFramesCountLabel->setText(QString::number(send_frame_count));
    sendBytesCountLabel->setText(QString::number(send_byte_count));
  }
}

// Helper method to get the current configFrame delimiter based on settings
QByteArray PageSerialTransceiver::getFrameDelimiter()
{
  // For now, return commonly used delimiters
  // In a more advanced implementation, this could be configurable from UI
  return QByteArray(); // Return empty to use default logic
}

void PageSerialTransceiver::receiveData()
{
  QByteArray data = serialPort.readAll();
  if(data.isEmpty())
    return;

  int oldSize = frameBuffer.size();
  // Add received data to the configFrame buffer
  frameBuffer.append(data);

  // Process complete configFrames in the buffer based on delimiter
  bool hasProcessedFrame = true;
  while(hasProcessedFrame)
  {
    hasProcessedFrame = false;
    int configFrameEnd = -1;

    // Look for common configFrame delimiters in order of preference
    // First check for CRLF (carriage return + line feed)
    configFrameEnd = frameBuffer.indexOf("\r\n");
    if(configFrameEnd == -1)
    {
      // Then check for just LF (line feed)
      configFrameEnd = frameBuffer.indexOf('\n');
    }
    if(configFrameEnd == -1)
    {
      // Then check for just CR (carriage return)
      configFrameEnd = frameBuffer.indexOf('\r');
    }

    // If we found a delimiter
    if(configFrameEnd != -1)
    {
      // Handle CRLF case specifically
      int delimiterLength = 1;
      if(configFrameEnd + 1 < frameBuffer.size() && frameBuffer.at(configFrameEnd + 1) == '\n')
      {
        delimiterLength = 2; // CRLF case
      }

      // Extract the complete configFrame (including the delimiter)
      QByteArray completeFrame = frameBuffer.left(configFrameEnd + delimiterLength);
      // Remove the processed configFrame from the buffer
      frameBuffer = frameBuffer.mid(configFrameEnd + delimiterLength);

      // Process the complete configFrame
      processCompleteFrame(completeFrame);
      hasProcessedFrame = true; // Continue processing if more delimiters exist
    }
  }

  // If we still have data in the buffer after processing delimiters,
  // start or restart the timer to process it after a delay
  if(!frameBuffer.isEmpty())
  {
    // Restart the timer to process remaining data if no delimiter appears soon
    frameProcessingTimer->stop();
    frameProcessingTimer->start(100); // Process after 100ms if no more data comes
  }
  else
  {
    // If buffer is empty, stop the timer
    frameProcessingTimer->stop();
  }

  // Optional: Handle case where buffer becomes too large (possible missing configFrame delimiter)
  if(frameBuffer.size() > 10240)    // 10KB threshold
  {
    // Process whatever we have as a configFrame if it gets too large
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
    frameProcessingTimer->stop(); // Stop timer since buffer is now empty
  }
}

void PageSerialTransceiver::processCompleteFrame(const QByteArray &configFrame)
{
  if(configFrame.isEmpty())
    return;

  qDebug() << "完整帧已接收, 字节数:" << configFrame.size();
  receive_frame_count++;
  receive_byte_count += configFrame.size();
  receiveFramesCountLabel->setText(QString::number(receive_frame_count));
  receiveBytesCountLabel->setText(QString::number(receive_byte_count));

  receiveEdit->moveCursor(QTextCursor::End);  // 移动光标至末尾
  QString dataToDisplay;
  if(receiveHexCheckbox->isChecked())
  {
    // 十六进制显示
    QString hexString;
    for(char byte : configFrame)
    {
      hexString += QString("%1 ").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')).toUpper();
    }
    dataToDisplay = hexString.trimmed(); // 去除末尾空格
  }
  else
  {
    dataToDisplay = QString::fromUtf8(configFrame);
  }
  dataToDisplay = dataToDisplay + "  ";
  if(lineBreakCheckbox->isChecked())
  {
    dataToDisplay = dataToDisplay + "\n"; // 插入换行符
  }
  dataToDisplay = "[接收]" + dataToDisplay;
  if(displayTimeCheckbox->isChecked())
  {
    QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    dataToDisplay = currentTime + dataToDisplay;  // 插入当前时间
  }
  //插入蓝色字体
  receiveEdit->setTextColor(QColor(0, 0, 255)); // 设置为蓝色
  receiveEdit->insertPlainText(dataToDisplay);  // 插入文本
}

void PageSerialTransceiver::onFrameProcessingTimeout()
{
  // Process the remaining data in the buffer as a configFrame
  if(!frameBuffer.isEmpty())
  {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageSerialTransceiver::processFrameBuffer()
{
  // Process all data in the buffer as a single configFrame
  if(!frameBuffer.isEmpty())
  {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageSerialTransceiver::verifysend(int selectedIndex)
{
  PageVerifiyDialog::VERIFIY_TYPE type = (PageVerifiyDialog::VERIFIY_TYPE)selectedIndex;
  QString dataToSend = sendEdit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(!sendHexCheckbox->isChecked())
  {
    qDebug() << "字符无法校验";
    return;
  }

  QByteArray byteArray;
  // 十六进制解析
  QString hexInput = dataToSend.trimmed();
  if(!hexInput.contains(' ') && hexInput.length() > 2)
  {
    QString spacedHex;
    for(int i = 0; i < hexInput.length(); i += 2)
    {
      if(i + 2 <= hexInput.length())
        spacedHex += hexInput.mid(i, 2) + " ";
      else
        spacedHex += hexInput.mid(i);
    }
    hexInput = spacedHex.trimmed();
  }
  QStringList hexValues = hexInput.split(' ', Qt::SkipEmptyParts);
  bool ok;
  for(const QString &hex : hexValues)
  {
    int byte = hex.toInt(&ok, 16);
    if(ok && byte >= 0 && byte <= 255)
    {
      byteArray.append(static_cast<char>(byte));
    }
    else
    {
      qDebug() << "无效的十六进制数:" << hex;
      return;
    }
  }

  switch(type)
  {
    case PageVerifiyDialog::CRC16_LOW:
      {
        // CRC16-IBM (Modbus)算法
        quint16 crc = 0xFFFF;
        for(auto b : byteArray)
        {
          crc ^= static_cast<quint8>(b);
          for(int i = 0; i < 8; ++i)
          {
            if(crc & 0x0001)
              crc = (crc >> 1) ^ 0xA001;
            else
              crc >>= 1;
          }
        }
        // 低字节在前
        QString crcStr = QString("%1 %2")
                         .arg(crc & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 8) & 0xFF, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC16_LOW 校验码:" << crcStr;
        break;
      }
    case PageVerifiyDialog::CRC16_HIGH:
      {
        // CRC16-IBM (Modbus)算法
        quint16 crc = 0xFFFF;
        for(auto b : byteArray)
        {
          crc ^= static_cast<quint8>(b);
          for(int i = 0; i < 8; ++i)
          {
            if(crc & 0x0001)
              crc = (crc >> 1) ^ 0xA001;
            else
              crc >>= 1;
          }
        }
        // 高字节在前
        QString crcStr = QString("%1 %2")
                         .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                         .arg(crc & 0xFF, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC16_HIGH 校验码:" << crcStr;
        break;
      }
    case PageVerifiyDialog::CRC8:
      {
        // CRC8算法 (多项式: 0x07)
        quint8 crc = 0x00;
        for(auto b : byteArray)
        {
          crc ^= static_cast<quint8>(b);
          for(int i = 0; i < 8; ++i)
          {
            if(crc & 0x80)
              crc = (crc << 1) ^ 0x07;
            else
              crc <<= 1;
          }
        }
        QString crcStr = QString("%1").arg(crc, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC8 校验码:" << crcStr;
        break;
      }
    case PageVerifiyDialog::TOTAL_SUM:
      {
        // 累加和
        quint8 sum = 0;
        for(auto b : byteArray)
        {
          sum += static_cast<quint8>(b);
        }
        sum &= 0xFF;
        QString sumStr = QString("%1").arg(sum, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + sumStr);
        qDebug() << "TOTAL_SUM 校验码:" << sumStr;
        break;
      }
    case PageVerifiyDialog::SUM_XOR:
      {
        // 异或和
        quint8 xorSum = 0;
        for(auto b : byteArray)
        {
          xorSum ^= static_cast<quint8>(b);
        }
        QString xorStr = QString("%1").arg(xorSum, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + xorStr);
        qDebug() << "SUM_XOR 校验码:" << xorStr;
        break;
      }
    case PageVerifiyDialog::CRC32_LOW:
      {
        // CRC32算法 (多项式: 0xEDB88320)
        quint32 crc = 0xFFFFFFFF;
        for(auto b : byteArray)
        {
          crc ^= static_cast<quint8>(b);
          for(int i = 0; i < 8; ++i)
          {
            if(crc & 0x00000001)
              crc = (crc >> 1) ^ 0xEDB88320;
            else
              crc >>= 1;
          }
        }
        crc ^= 0xFFFFFFFF;
        // 低字节在前 (小端序)
        QString crcStr = QString("%1 %2 %3 %4")
                         .arg((crc >> 0) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 16) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 24) & 0xFF, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC32_LOW 校验码:" << crcStr;
        break;
      }
    case PageVerifiyDialog::CRC32_HIGH:
      {
        // CRC32算法 (多项式: 0x04C11DB7)
        quint32 crc = 0xFFFFFFFF;
        for(auto b : byteArray)
        {
          crc ^= (static_cast<quint8>(b) << 24);
          for(int i = 0; i < 8; ++i)
          {
            if(crc & 0x80000000)
              crc = (crc << 1) ^ 0x04C11DB7;
            else
              crc <<= 1;
          }
        }
        // 高字节在前 (大端序)
        QString crcStr = QString("%1 %2 %3 %4")
                         .arg((crc >> 24) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 16) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                         .arg((crc >> 0) & 0xFF, 2, 16, QChar('0')).toUpper();
        sendEdit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC32_HIGH 校验码:" << crcStr;
        break;
      }
    default:
      qDebug() << "未知的校验类型:" << type;
      break;
  }
}
