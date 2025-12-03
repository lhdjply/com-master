#include "myresource.h"

PageTerminal::PageTerminal(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi();
  retranslateUi();

  last_serial_num = 0;
  isSerial_Open = false;

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PageTerminal::handleTimeout);
  timer->start(1000);
}

PageTerminal::~PageTerminal()
{
}

void PageTerminal::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("PageTerminal"));
    this->resize(1123, 700);
    
    QString styleSheet = QString::fromUtf8(
        "QMainWindow {\n"
        "    background-color: #f5f7fa;\n"
        "    color: #333333;\n"
        "    font-family: 'Segoe UI', Arial, sans-serif;\n"
        "}\n"
        "QGroupBox {\n"
        "    font-weight: bold;\n"
        "    border: 1px solid #c0c6d0;\n"
        "    border-radius: 6px;\n"
        "    margin-top: 12px;\n"
        "    padding-top: 12px;\n"
        "    background-color: white;\n"
        "}\n"
        "QGroupBox::title {\n"
        "    subcontrol-origin: margin;\n"
        "    subcontrol-position: top center;\n"
        "    padding: 0 8px;\n"
        "    background-color: white;\n"
        "    color: #2c3e50;\n"
        "}\n"
        "QPushButton {\n"
        "    background-color: #3498db;\n"
        "    border: none;\n"
        "    border-radius: 4px;\n"
        "    color: white;\n"
        "    padding: 8px 16px;\n"
        "    font-weight: bold;\n"
        "    min-height: 20px;\n"
        "}\n"
        "QPushButton:hover {\n"
        "    background-color: #2980b9;\n"
        "}\n"
        "QPushButton:pressed {\n"
        "    background-color: #21618c;\n"
        "}\n"
        "QPushButton:disabled {\n"
        "    background-color: #bdc3c7;\n"
        "    color: #7f8c8d;\n"
        "}\n"
        "QComboBox {\n"
        "    border: 1px solid #c0c6d0;\n"
        "    border-radius: 4px;\n"
        "    padding: 6px 12px;\n"
        "    background-color: white;\n"
        "    selection-background-color: #3498db;\n"
        "    selection-color: white;\n"
        "    min-width: 120px;\n"
        "    color: black;\n"
        "}\n"
        "QComboBox::drop-down {\n"
        "    subcontrol-origin: padding;\n"
        "    subcontrol-position: top right;\n"
        "    width: 20px;\n"
        "    border-left: 1px solid #c0c6d0;\n"
        "}\n"
        "QComboBox::down-arrow {\n"
        "    image: none;\n"
        "    border-left: 4px solid transparent;\n"
        "    border-right: 4px solid transparent;\n"
        "    border-top: 4px solid #7f8c8d;\n"
        "    width: 0;\n"
        "    height: 0;\n"
        "}\n"
        "QTextEdit {\n"
        "    border: 1px solid #c0c6d0;\n"
        "    border-radius: 4px;\n"
        "    padding: 8px;\n"
        "    background-color: white;\n"
        "    font-family: 'Consolas', 'Monaco', monospace;\n"
        "}\n"
        "QCheckBox {\n"
        "    spacing: 6px;\n"
        "}\n"
        "QCheckBox::indicator {\n"
        "    width: 16px;\n"
        "    height: 16px;\n"
        "}\n"
        "QCheckBox::indicator:unchecked {\n"
        "    border: 1px solid #c0c6d0;\n"
        "    border-radius: 2px;\n"
        "    background-color: white;\n"
        "}\n"
        "QCheckBox::indicator:checked {\n"
        "    border: 1px solid #3498db;\n"
        "    border-radius: 2px;\n"
        "    background-color: #3498db;\n"
        "}\n"
        "QLabel {\n"
        "    color: #2c3e50;\n"
        "    font-weight: 500;\n"
        "}\n"
        "QLineEdit {\n"
        "    border: 1px solid #c0c6d0;\n"
        "    border-radius: 4px;\n"
        "    padding: 6px 8px;\n"
        "    background-color: white;\n"
        "}\n"
        "QLineEdit:focus {\n"
        "    border-color: #3498db;\n"
        "}"
    );
    this->setStyleSheet(styleSheet);
    
    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    this->setCentralWidget(centralwidget);
    
    horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    horizontalLayout->setStretch(0, 0);
    horizontalLayout->setStretch(1, 0);
    
    groupBox = new QGroupBox(centralwidget);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setMinimumSize(QSize(260, 0));
    
    verticalLayout = new QVBoxLayout(groupBox);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    
    horizontalLayout_1 = new QHBoxLayout();
    horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
    
    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));
    
    horizontalLayout_1->addWidget(label);
    
    portdroplist = new QComboBox(groupBox);
    portdroplist->setObjectName(QString::fromUtf8("portdroplist"));
    
    horizontalLayout_1->addWidget(portdroplist);
    
    verticalLayout->addLayout(horizontalLayout_1);
    
    horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    
    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    
    horizontalLayout_2->addWidget(label_2);
    
    baudratedroplist = new QComboBox(groupBox);
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
    
    horizontalLayout_2->addWidget(baudratedroplist);
    
    verticalLayout->addLayout(horizontalLayout_2);
    
    horizontalLayout_3 = new QHBoxLayout();
    horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
    
    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    
    horizontalLayout_3->addWidget(label_3);
    
    databitsdroplist = new QComboBox(groupBox);
    databitsdroplist->setObjectName(QString::fromUtf8("databitsdroplist"));
    databitsdroplist->addItem(QString::fromUtf8("5"));
    databitsdroplist->addItem(QString::fromUtf8("6"));
    databitsdroplist->addItem(QString::fromUtf8("7"));
    databitsdroplist->addItem(QString::fromUtf8("8"));
    databitsdroplist->setCurrentIndex(3);
    
    horizontalLayout_3->addWidget(databitsdroplist);
    
    verticalLayout->addLayout(horizontalLayout_3);
    
    horizontalLayout_4 = new QHBoxLayout();
    horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
    
    label_4 = new QLabel(groupBox);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    
    horizontalLayout_4->addWidget(label_4);
    
    paritydroplist = new QComboBox(groupBox);
    paritydroplist->setObjectName(QString::fromUtf8("paritydroplist"));
    paritydroplist->addItem(QCoreApplication::translate("PageTerminal", "None", nullptr));
    paritydroplist->addItem(QCoreApplication::translate("PageTerminal", "Odd", nullptr));
    paritydroplist->addItem(QCoreApplication::translate("PageTerminal", "Even", nullptr));
    paritydroplist->addItem(QCoreApplication::translate("PageTerminal", "Mark", nullptr));
    paritydroplist->addItem(QCoreApplication::translate("PageTerminal", "Space", nullptr));
    
    horizontalLayout_4->addWidget(paritydroplist);
    
    verticalLayout->addLayout(horizontalLayout_4);
    
    horizontalLayout_5 = new QHBoxLayout();
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
    
    label_5 = new QLabel(groupBox);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    
    horizontalLayout_5->addWidget(label_5);
    
    stopbitsdroplist = new QComboBox(groupBox);
    stopbitsdroplist->setObjectName(QString::fromUtf8("stopbitsdroplist"));
    stopbitsdroplist->addItem(QString::fromUtf8("1"));
    stopbitsdroplist->addItem(QString::fromUtf8("1.5"));
    stopbitsdroplist->addItem(QString::fromUtf8("2"));
    
    horizontalLayout_5->addWidget(stopbitsdroplist);
    
    verticalLayout->addLayout(horizontalLayout_5);
    
    openbutton = new QPushButton(groupBox);
    openbutton->setObjectName(QString::fromUtf8("openbutton"));
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
    
    verticalLayout->addWidget(openbutton);
    
    horizontalLayout->addWidget(groupBox);
    
    textEdit = new QTextEdit(centralwidget);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    textEdit->setStyleSheet(QString::fromUtf8("background-color:rgb(0, 0, 0);color:rgb(255, 255, 255)"));
    textEdit->setReadOnly(true);
    
    horizontalLayout->addWidget(textEdit);
    
    QMetaObject::connectSlotsByName(this);
}

void PageTerminal::retranslateUi()
{
    this->setWindowTitle(QCoreApplication::translate("PageTerminal", "Serial Port Transceiver", nullptr));
    groupBox->setTitle(QString());
    label->setText(QCoreApplication::translate("PageTerminal", "Port", nullptr));
    label_2->setText(QCoreApplication::translate("PageTerminal", "Baudrate", nullptr));
    label_3->setText(QCoreApplication::translate("PageTerminal", "Data bits", nullptr));
    label_4->setText(QCoreApplication::translate("PageTerminal", "Parity", nullptr));
    label_5->setText(QCoreApplication::translate("PageTerminal", "Stop bits", nullptr));
    openbutton->setText(QCoreApplication::translate("PageTerminal", "Open", nullptr));
}

void PageTerminal::on_openbutton_clicked()
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
    serialPort.close();
    qDebug() << "串口已关闭";
  }
  else
  {
    if(serialPort.open(QIODevice::ReadWrite))
    {
      isSerial_Open = true;
      openbutton->setText(tr("Close"));
      qDebug() << "串口已打开";
    }
    else
    {
      qDebug() << "串口无法打开, 错误原因:" << serialPort.errorString();
    }
  }
}

void PageTerminal::handleTimeout()
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
        serialPort.close();
        qDebug() << "串口被拔除，已自动关闭";
      }
    }
  }
}
