#include "myresource.h"

CustomTextEdit::CustomTextEdit(QWidget *parent)
  : QTextEdit(parent)
{
}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    
    // 获取标准菜单项并重新设置中文文本
    QList<QAction*> actions = menu->actions();
    for (QAction *action : actions) {
        if (action->text() == "&Undo") {
            action->setText(tr("Undo"));
        } else if (action->text() == "&Redo") {
            action->setText(tr("Redo"));
        } else if (action->text() == "Cu&t") {
            action->setText(tr("Cut"));
        } else if (action->text() == "&Copy") {
            action->setText(tr("Copy"));
        } else if (action->text() == "&Paste") {
            action->setText(tr("Paste"));
        } else if (action->text() == "Delete") {
            action->setText(tr("Delete"));
        } else if (action->text() == "Select All") {
            action->setText(tr("Select All"));
        }
    }
    
    menu->exec(event->globalPos());
    delete menu;
}

PageTransceiver::PageTransceiver(QWidget *parent)
  : QMainWindow(parent)
{
  setupUi();
  retranslateUi();

  receive_frame_count = 0;
  receive_byte_count = 0;
  send_frame_count = 0;
  send_byte_count = 0;
  receive_frames_count->setText(QString::number(receive_frame_count));
  receive_bytes_count->setText(QString::number(receive_byte_count));
  send_frames_count->setText(QString::number(send_frame_count));
  send_bytes_count->setText(QString::number(send_byte_count));

  last_serial_num = 0;
  isSerial_Open = false;
  frameBuffer.clear(); // Initialize frame buffer

  // Initialize frame processing timer
  frameProcessingTimer = new QTimer(this);
  frameProcessingTimer->setSingleShot(true); // Single shot timer
  connect(frameProcessingTimer, &QTimer::timeout, this, &PageTransceiver::onFrameProcessingTimeout);

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PageTransceiver::handleTimeout);
  timer->start(1000);

  AutosendTimer = new QTimer(this);
  connect(AutosendTimer, &QTimer::timeout, this, &PageTransceiver::autosend);
  AutosendTimer->start(1000);
}

PageTransceiver::~PageTransceiver()
{
}

void PageTransceiver::setupUi()
{
  if(this->objectName().isEmpty())
    this->setObjectName(QString::fromUtf8("PageTransceiver"));
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

  horizontalLayout_10 = new QHBoxLayout(centralwidget);
  horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
  horizontalLayout_10->setStretch(0, 0);
  horizontalLayout_10->setStretch(1, 0);

  frame = new QFrame(centralwidget);
  frame->setObjectName(QString::fromUtf8("frame"));
  frame->setMinimumSize(QSize(280, 0));
  frame->setMaximumSize(QSize(280, 16777215));

  verticalLayout_7 = new QVBoxLayout(frame);
  verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
  verticalLayout_7->setSpacing(15);

  groupBox = new QGroupBox(frame);
  groupBox->setObjectName(QString::fromUtf8("groupBox"));

  verticalLayout = new QVBoxLayout(groupBox);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  verticalLayout->setSpacing(12);

  horizontalLayout_1 = new QHBoxLayout();
  horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));

  label = new QLabel(groupBox);
  label->setObjectName(QString::fromUtf8("label"));
  label->setMinimumSize(QSize(80, 0));

  horizontalLayout_1->addWidget(label);

  portdroplist = new QComboBox(groupBox);
  portdroplist->setObjectName(QString::fromUtf8("portdroplist"));
  portdroplist->setMinimumSize(QSize(146, 0));

  horizontalLayout_1->addWidget(portdroplist);

  verticalLayout->addLayout(horizontalLayout_1);

  horizontalLayout_2 = new QHBoxLayout();
  horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));

  label_2 = new QLabel(groupBox);
  label_2->setObjectName(QString::fromUtf8("label_2"));
  label_2->setMinimumSize(QSize(80, 0));

  horizontalLayout_2->addWidget(label_2);

  baudratedroplist = new QComboBox(groupBox);
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

  horizontalLayout_2->addWidget(baudratedroplist);

  verticalLayout->addLayout(horizontalLayout_2);

  horizontalLayout_3 = new QHBoxLayout();
  horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));

  label_3 = new QLabel(groupBox);
  label_3->setObjectName(QString::fromUtf8("label_3"));
  label_3->setMinimumSize(QSize(80, 0));

  horizontalLayout_3->addWidget(label_3);

  databitsdroplist = new QComboBox(groupBox);
  databitsdroplist->setObjectName(QString::fromUtf8("databitsdroplist"));
  databitsdroplist->setMinimumSize(QSize(146, 0));
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
  label_4->setMinimumSize(QSize(80, 0));

  horizontalLayout_4->addWidget(label_4);

  paritydroplist = new QComboBox(groupBox);
  paritydroplist->setObjectName(QString::fromUtf8("paritydroplist"));
  paritydroplist->setMinimumSize(QSize(146, 0));
  paritydroplist->addItem(QCoreApplication::translate("PageTransceiver", "None", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageTransceiver", "Odd", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageTransceiver", "Even", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageTransceiver", "Mark", nullptr));
  paritydroplist->addItem(QCoreApplication::translate("PageTransceiver", "Space", nullptr));

  horizontalLayout_4->addWidget(paritydroplist);

  verticalLayout->addLayout(horizontalLayout_4);

  horizontalLayout_5 = new QHBoxLayout();
  horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));

  label_5 = new QLabel(groupBox);
  label_5->setObjectName(QString::fromUtf8("label_5"));
  label_5->setMinimumSize(QSize(80, 0));

  horizontalLayout_5->addWidget(label_5);

  stopbitsdroplist = new QComboBox(groupBox);
  stopbitsdroplist->setObjectName(QString::fromUtf8("stopbitsdroplist"));
  stopbitsdroplist->setMinimumSize(QSize(146, 0));
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

  verticalLayout_7->addWidget(groupBox);

  verticalGroupBox = new QGroupBox(frame);
  verticalGroupBox->setObjectName(QString::fromUtf8("verticalGroupBox"));

  verticalLayout_4 = new QVBoxLayout(verticalGroupBox);
  verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
  verticalLayout_4->setSpacing(10);

  horizontalLayout_11 = new QHBoxLayout();
  horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));

  receive_frames = new QLabel(verticalGroupBox);
  receive_frames->setObjectName(QString::fromUtf8("receive_frames"));

  horizontalLayout_11->addWidget(receive_frames);

  receive_frames_count = new QLabel(verticalGroupBox);
  receive_frames_count->setObjectName(QString::fromUtf8("receive_frames_count"));
  receive_frames_count->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #e74c3c;"));
  receive_frames_count->setText(QString::fromUtf8("0"));

  horizontalLayout_11->addWidget(receive_frames_count);

  verticalLayout_4->addLayout(horizontalLayout_11);

  horizontalLayout_12 = new QHBoxLayout();
  horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));

  receive_bytes = new QLabel(verticalGroupBox);
  receive_bytes->setObjectName(QString::fromUtf8("receive_bytes"));

  horizontalLayout_12->addWidget(receive_bytes);

  receive_bytes_count = new QLabel(verticalGroupBox);
  receive_bytes_count->setObjectName(QString::fromUtf8("receive_bytes_count"));
  receive_bytes_count->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #e74c3c;"));
  receive_bytes_count->setText(QString::fromUtf8("0"));

  horizontalLayout_12->addWidget(receive_bytes_count);

  verticalLayout_4->addLayout(horizontalLayout_12);

  horizontalLayout_13 = new QHBoxLayout();
  horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));

  send_frames = new QLabel(verticalGroupBox);
  send_frames->setObjectName(QString::fromUtf8("send_frames"));

  horizontalLayout_13->addWidget(send_frames);

  send_frames_count = new QLabel(verticalGroupBox);
  send_frames_count->setObjectName(QString::fromUtf8("send_frames_count"));
  send_frames_count->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #3498db;"));
  send_frames_count->setText(QString::fromUtf8("0"));

  horizontalLayout_13->addWidget(send_frames_count);

  verticalLayout_4->addLayout(horizontalLayout_13);

  horizontalLayout_14 = new QHBoxLayout();
  horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));

  send_bytes = new QLabel(verticalGroupBox);
  send_bytes->setObjectName(QString::fromUtf8("send_bytes"));

  horizontalLayout_14->addWidget(send_bytes);

  send_bytes_count = new QLabel(verticalGroupBox);
  send_bytes_count->setObjectName(QString::fromUtf8("send_bytes_count"));
  send_bytes_count->setStyleSheet(QString::fromUtf8("font-weight: bold; color: #3498db;"));
  send_bytes_count->setText(QString::fromUtf8("0"));

  horizontalLayout_14->addWidget(send_bytes_count);

  verticalLayout_4->addLayout(horizontalLayout_14);

  clearcount = new QPushButton(verticalGroupBox);
  clearcount->setObjectName(QString::fromUtf8("clearcount"));

  verticalLayout_4->addWidget(clearcount);

  verticalLayout_7->addWidget(verticalGroupBox);

  horizontalLayout_10->addWidget(frame);

  verticalLayout_3 = new QVBoxLayout();
  verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
  verticalLayout_3->setStretch(0, 1);
  verticalLayout_3->setStretch(1, 0);
  verticalLayout_3->setSpacing(15);

  receivegroup = new QGroupBox();
  receivegroup->setObjectName(QString::fromUtf8("receivegroup"));
  receivegroup->setMinimumSize(QSize(0, 320));

  verticalLayout_5 = new QVBoxLayout(receivegroup);
  verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
  verticalLayout_5->setSpacing(10);

  receiveedit = new CustomTextEdit(receivegroup);
  receiveedit->setObjectName(QString::fromUtf8("receiveedit"));
  receiveedit->setStyleSheet(QString::fromUtf8("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;"));

  verticalLayout_5->addWidget(receiveedit);

  horizontalLayout = new QHBoxLayout();
  horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  horizontalLayout->setSpacing(15);

  receivehexcheckbox = new QCheckBox(receivegroup);
  receivehexcheckbox->setObjectName(QString::fromUtf8("receivehexcheckbox"));

  horizontalLayout->addWidget(receivehexcheckbox);

  displaysendcheckbox = new QCheckBox(receivegroup);
  displaysendcheckbox->setObjectName(QString::fromUtf8("displaysendcheckbox"));

  horizontalLayout->addWidget(displaysendcheckbox);

  displaytimecheckbox = new QCheckBox(receivegroup);
  displaytimecheckbox->setObjectName(QString::fromUtf8("displaytimecheckbox"));

  horizontalLayout->addWidget(displaytimecheckbox);

  linebreakcheckbox = new QCheckBox(receivegroup);
  linebreakcheckbox->setObjectName(QString::fromUtf8("linebreakcheckbox"));

  horizontalLayout->addWidget(linebreakcheckbox);

  clearreceive = new QPushButton(receivegroup);
  clearreceive->setObjectName(QString::fromUtf8("clearreceive"));

  horizontalLayout->addWidget(clearreceive);

  verticalLayout_5->addLayout(horizontalLayout);

  verticalLayout_3->addWidget(receivegroup);

  sendgroup = new QGroupBox();
  sendgroup->setObjectName(QString::fromUtf8("sendgroup"));
  sendgroup->setMaximumSize(QSize(16777215, 230));

  verticalLayout_6 = new QVBoxLayout(sendgroup);
  verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
  verticalLayout_6->setSpacing(10);

  sendedit = new CustomTextEdit(sendgroup);
  sendedit->setObjectName(QString::fromUtf8("sendedit"));
  sendedit->setStyleSheet(QString::fromUtf8("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;"));

  verticalLayout_6->addWidget(sendedit);

  horizontalLayout_9 = new QHBoxLayout();
  horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
  horizontalLayout_9->setSpacing(10);

  sendbutton = new QPushButton(sendgroup);
  sendbutton->setObjectName(QString::fromUtf8("sendbutton"));
  sendbutton->setMinimumSize(QSize(130, 36));

  horizontalLayout_9->addWidget(sendbutton);

  verifybutton = new QPushButton(sendgroup);
  verifybutton->setObjectName(QString::fromUtf8("verifybutton"));
  verifybutton->setMinimumSize(QSize(130, 36));

  horizontalLayout_9->addWidget(verifybutton);

  clearsend = new QPushButton(sendgroup);
  clearsend->setObjectName(QString::fromUtf8("clearsend"));
  clearsend->setMinimumSize(QSize(130, 36));

  horizontalLayout_9->addWidget(clearsend);

  sendhexcheckbox = new QCheckBox(sendgroup);
  sendhexcheckbox->setObjectName(QString::fromUtf8("sendhexcheckbox"));

  horizontalLayout_9->addWidget(sendhexcheckbox);

  autosendcheckbox = new QCheckBox(sendgroup);
  autosendcheckbox->setObjectName(QString::fromUtf8("autosendcheckbox"));

  horizontalLayout_9->addWidget(autosendcheckbox);

  frame_3 = new QFrame(sendgroup);
  frame_3->setObjectName(QString::fromUtf8("frame_3"));
  frame_3->setMaximumSize(QSize(200, 50));

  horizontalLayout_8 = new QHBoxLayout(frame_3);
  horizontalLayout_8->setSpacing(5);
  horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
  horizontalLayout_8->setContentsMargins(0, 0, 0, 0);

  label_6 = new QLabel(frame_3);
  label_6->setObjectName(QString::fromUtf8("label_6"));

  horizontalLayout_8->addWidget(label_6);

  senddutyedit = new QLineEdit(frame_3);
  senddutyedit->setObjectName(QString::fromUtf8("senddutyedit"));
  senddutyedit->setMaximumSize(QSize(60, 16777215));
  senddutyedit->setText(QString::fromUtf8("1000"));
  senddutyedit->setAlignment(Qt::AlignCenter);

  horizontalLayout_8->addWidget(senddutyedit);

  label_7 = new QLabel(frame_3);
  label_7->setObjectName(QString::fromUtf8("label_7"));

  horizontalLayout_8->addWidget(label_7);

  horizontalLayout_9->addWidget(frame_3);

  verticalLayout_6->addLayout(horizontalLayout_9);

  verticalLayout_3->addWidget(sendgroup);

  horizontalLayout_10->addLayout(verticalLayout_3);

  QMetaObject::connectSlotsByName(this);
}

void PageTransceiver::retranslateUi()
{
  this->setWindowTitle(QCoreApplication::translate("PageTransceiver", "Serial Port Transceiver", nullptr));
  groupBox->setTitle(QCoreApplication::translate("PageTransceiver", "Serial Port Settings", nullptr));
  label->setText(QCoreApplication::translate("PageTransceiver", "Port", nullptr));
  label_2->setText(QCoreApplication::translate("PageTransceiver", "Baudrate", nullptr));
  label_3->setText(QCoreApplication::translate("PageTransceiver", "Data bits", nullptr));
  label_4->setText(QCoreApplication::translate("PageTransceiver", "Parity", nullptr));
  label_5->setText(QCoreApplication::translate("PageTransceiver", "Stop bits", nullptr));
  openbutton->setText(QCoreApplication::translate("PageTransceiver", "Open", nullptr));
  verticalGroupBox->setTitle(QCoreApplication::translate("PageTransceiver", "Statistics", nullptr));
  receive_frames->setText(QCoreApplication::translate("PageTransceiver", "Received frames", nullptr));
  receive_bytes->setText(QCoreApplication::translate("PageTransceiver", "Received bytes", nullptr));
  send_frames->setText(QCoreApplication::translate("PageTransceiver", "Send frames", nullptr));
  send_bytes->setText(QCoreApplication::translate("PageTransceiver", "Send bytes", nullptr));
  clearcount->setText(QCoreApplication::translate("PageTransceiver", "Clear count", nullptr));
  receivegroup->setTitle(QCoreApplication::translate("PageTransceiver", "Received Data", nullptr));
  receivehexcheckbox->setText(QCoreApplication::translate("PageTransceiver", "Hex", nullptr));
  displaysendcheckbox->setText(QCoreApplication::translate("PageTransceiver", "Display send", nullptr));
  displaytimecheckbox->setText(QCoreApplication::translate("PageTransceiver", "Display time", nullptr));
  linebreakcheckbox->setText(QCoreApplication::translate("PageTransceiver", "Frame line break", nullptr));
  clearreceive->setText(QCoreApplication::translate("PageTransceiver", "Clear Receive", nullptr));
  sendgroup->setTitle(QCoreApplication::translate("PageTransceiver", "Send Data", nullptr));
  sendbutton->setText(QCoreApplication::translate("PageTransceiver", "Send", nullptr));
  verifybutton->setText(QCoreApplication::translate("PageTransceiver", "Verification", nullptr));
  clearsend->setText(QCoreApplication::translate("PageTransceiver", "Clear Send", nullptr));
  sendhexcheckbox->setText(QCoreApplication::translate("PageTransceiver", "Hex", nullptr));
  autosendcheckbox->setText(QCoreApplication::translate("PageTransceiver", "Auto send", nullptr));
  label_6->setText(QCoreApplication::translate("PageTransceiver", "Send duty", nullptr));
  label_7->setText(QCoreApplication::translate("PageTransceiver", "ms", nullptr));
}

void PageTransceiver::on_openbutton_clicked()
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
    openbutton->setStyleSheet(
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
    disconnect(&serialPort, &QSerialPort::readyRead, this, &PageTransceiver::receiveData);
  }
  else
  {
    if(serialPort.open(QIODevice::ReadWrite))
    {
      isSerial_Open = true;
      openbutton->setText(tr("Close"));
      openbutton->setStyleSheet(
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
      connect(&serialPort, &QSerialPort::readyRead, this, &PageTransceiver::receiveData);
    }
    else
    {
      qDebug() << "串口无法打开, 错误原因:" << serialPort.errorString();
    }
  }
}

void PageTransceiver::on_sendbutton_clicked()
{
  sendData();
}

void PageTransceiver::on_verifybutton_clicked()
{
  if(sendhexcheckbox->isChecked())
  {
    PageVerifiyDialog pageVerifiyDialog;
    connect(&pageVerifiyDialog, &PageVerifiyDialog::selectedIndexChanged, this, &PageTransceiver::verifysend);

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

void PageTransceiver::on_clearcount_clicked()
{
  receive_frame_count = 0;
  receive_byte_count = 0;
  send_frame_count = 0;
  send_byte_count = 0;
  receive_frames_count->setText(QString::number(receive_frame_count));
  receive_bytes_count->setText(QString::number(receive_byte_count));
  send_frames_count->setText(QString::number(send_frame_count));
  send_bytes_count->setText(QString::number(send_byte_count));
}

void PageTransceiver::on_clearreceive_clicked()
{
  receiveedit->clear();
  frameBuffer.clear(); // Also clear the frame buffer
  frameProcessingTimer->stop(); // Stop the timer too
}

void PageTransceiver::on_clearsend_clicked()
{
  sendedit->clear();
}

void PageTransceiver::on_senddutyedit_textChanged(const QString &arg1)
{
  int interval = arg1.toInt();
  if(interval < 10)
  {
    interval = 10;
  }
  AutosendTimer->setInterval(interval);
}

void PageTransceiver::handleTimeout()
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
        openbutton->setText(tr("Open"));
        openbutton->setStyleSheet(
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

void PageTransceiver::autosend()
{
  if(autosendcheckbox->isChecked())
  {
    sendData();
  }
}

void PageTransceiver::sendData()
{
  QByteArray byteArray;
  if(!isSerial_Open)
  {
    qDebug() << "请先打开串口";
    return;
  }
  QString dataToSend = sendedit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(sendhexcheckbox->isChecked())
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
    if(displaysendcheckbox->isChecked())
    {
      receiveedit->moveCursor(QTextCursor::End);  // 移动光标至末尾
      if(receivehexcheckbox->isChecked())
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
      if(linebreakcheckbox->isChecked())
      {
        dataToSend = dataToSend + "\n"; // 插入换行符
      }
      dataToSend = "[发送]" + dataToSend;
      if(displaytimecheckbox->isChecked())
      {
        QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
        dataToSend = currentTime + dataToSend;  // 插入当前时间
      }
      //插入绿色字体
      receiveedit->setTextColor(QColor(0, 128, 0)); // 设置为绿色
      receiveedit->insertPlainText(dataToSend);  // 插入文本
    }
    qDebug() << "数据已成功写入, 字节数:" << bytesWritten;
    send_frame_count++;
    send_byte_count = send_byte_count + bytesWritten;
    send_frames_count->setText(QString::number(send_frame_count));
    send_bytes_count->setText(QString::number(send_byte_count));
  }
}

// Helper method to get the current frame delimiter based on settings
QByteArray PageTransceiver::getFrameDelimiter()
{
  // For now, return commonly used delimiters
  // In a more advanced implementation, this could be configurable from UI
  return QByteArray(); // Return empty to use default logic
}

void PageTransceiver::receiveData()
{
  QByteArray data = serialPort.readAll();
  if(data.isEmpty())
    return;

  int oldSize = frameBuffer.size();
  // Add received data to the frame buffer
  frameBuffer.append(data);

  // Process complete frames in the buffer based on delimiter
  bool hasProcessedFrame = true;
  while(hasProcessedFrame)
  {
    hasProcessedFrame = false;
    int frameEnd = -1;

    // Look for common frame delimiters in order of preference
    // First check for CRLF (carriage return + line feed)
    frameEnd = frameBuffer.indexOf("\r\n");
    if(frameEnd == -1)
    {
      // Then check for just LF (line feed)
      frameEnd = frameBuffer.indexOf('\n');
    }
    if(frameEnd == -1)
    {
      // Then check for just CR (carriage return)
      frameEnd = frameBuffer.indexOf('\r');
    }

    // If we found a delimiter
    if(frameEnd != -1)
    {
      // Handle CRLF case specifically
      int delimiterLength = 1;
      if(frameEnd + 1 < frameBuffer.size() && frameBuffer.at(frameEnd + 1) == '\n')
      {
        delimiterLength = 2; // CRLF case
      }

      // Extract the complete frame (including the delimiter)
      QByteArray completeFrame = frameBuffer.left(frameEnd + delimiterLength);
      // Remove the processed frame from the buffer
      frameBuffer = frameBuffer.mid(frameEnd + delimiterLength);

      // Process the complete frame
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

  // Optional: Handle case where buffer becomes too large (possible missing frame delimiter)
  if(frameBuffer.size() > 10240)    // 10KB threshold
  {
    // Process whatever we have as a frame if it gets too large
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
    frameProcessingTimer->stop(); // Stop timer since buffer is now empty
  }
}

void PageTransceiver::processCompleteFrame(const QByteArray &frame)
{
  if(frame.isEmpty())
    return;

  qDebug() << "完整帧已接收, 字节数:" << frame.size();
  receive_frame_count++;
  receive_byte_count += frame.size();
  receive_frames_count->setText(QString::number(receive_frame_count));
  receive_bytes_count->setText(QString::number(receive_byte_count));

  receiveedit->moveCursor(QTextCursor::End);  // 移动光标至末尾
  QString dataToDisplay;
  if(receivehexcheckbox->isChecked())
  {
    // 十六进制显示
    QString hexString;
    for(char byte : frame)
    {
      hexString += QString("%1 ").arg(static_cast<unsigned char>(byte), 2, 16, QChar('0')).toUpper();
    }
    dataToDisplay = hexString.trimmed(); // 去除末尾空格
  }
  else
  {
    dataToDisplay = QString::fromUtf8(frame);
  }
  dataToDisplay = dataToDisplay + "  ";
  if(linebreakcheckbox->isChecked())
  {
    dataToDisplay = dataToDisplay + "\n"; // 插入换行符
  }
  dataToDisplay = "[接收]" + dataToDisplay;
  if(displaytimecheckbox->isChecked())
  {
    QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    dataToDisplay = currentTime + dataToDisplay;  // 插入当前时间
  }
  //插入蓝色字体
  receiveedit->setTextColor(QColor(0, 0, 255)); // 设置为蓝色
  receiveedit->insertPlainText(dataToDisplay);  // 插入文本
}

void PageTransceiver::onFrameProcessingTimeout()
{
  // Process the remaining data in the buffer as a frame
  if(!frameBuffer.isEmpty())
  {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageTransceiver::processFrameBuffer()
{
  // Process all data in the buffer as a single frame
  if(!frameBuffer.isEmpty())
  {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageTransceiver::verifysend(int selectedIndex)
{
  PageVerifiyDialog::VERIFIY_TYPE type = (PageVerifiyDialog::VERIFIY_TYPE)selectedIndex;
  QString dataToSend = sendedit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(!sendhexcheckbox->isChecked())
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
        sendedit->setPlainText(dataToSend + " " + crcStr);
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
        sendedit->setPlainText(dataToSend + " " + crcStr);
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
        sendedit->setPlainText(dataToSend + " " + crcStr);
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
        sendedit->setPlainText(dataToSend + " " + sumStr);
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
        sendedit->setPlainText(dataToSend + " " + xorStr);
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
        sendedit->setPlainText(dataToSend + " " + crcStr);
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
        sendedit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC32_HIGH 校验码:" << crcStr;
        break;
      }
    default:
      qDebug() << "未知的校验类型:" << type;
      break;
  }
}
