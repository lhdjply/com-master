#include "myresource.h"
#include "./ui_pagetransceiver.h"

PageTransceiver::PageTransceiver(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::PageTransceiver)
{
  ui->setupUi(this);

  receive_frame_count = 0;
  receive_byte_count = 0;
  send_frame_count = 0;
  send_byte_count = 0;
  ui->receive_frames_count->setText(QString::number(receive_frame_count));
  ui->receive_bytes_count->setText(QString::number(receive_byte_count));
  ui->send_frames_count->setText(QString::number(send_frame_count));
  ui->send_bytes_count->setText(QString::number(send_byte_count));

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
  delete ui;
}

void PageTransceiver::on_openbutton_clicked()
{
  const auto serialPortInfos = QSerialPortInfo::availablePorts();

  if(ui->portdroplist->count() == 0)
  {
    return;
  }
#ifdef Q_OS_WIN
  Serial_Name = serialPortInfos.at(ui->portdroplist->currentIndex()).portName();
#else
  Serial_Name = serialPortInfos.at(ui->portdroplist->currentIndex()).systemLocation();
#endif
  Current_BaudRate = ui->baudratedroplist->currentText().toInt();
  Current_DataBits = (QSerialPort::DataBits)ui->databitsdroplist->currentText().toInt();
  Current_Parity = ui->paritydroplist->currentIndex();
  Current_StopBits = ui->stopbitsdroplist->currentIndex();
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
    ui->openbutton->setText(tr("Open"));
    ui->openbutton->setStyleSheet(
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
      ui->openbutton->setText(tr("Close"));
      ui->openbutton->setStyleSheet(
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
  if(ui->sendhexcheckbox->isChecked())
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
  ui->receive_frames_count->setText(QString::number(receive_frame_count));
  ui->receive_bytes_count->setText(QString::number(receive_byte_count));
  ui->send_frames_count->setText(QString::number(send_frame_count));
  ui->send_bytes_count->setText(QString::number(send_byte_count));
}

void PageTransceiver::on_clearreceive_clicked()
{
  ui->receiveedit->clear();
  frameBuffer.clear(); // Also clear the frame buffer
  frameProcessingTimer->stop(); // Stop the timer too
}

void PageTransceiver::on_clearsend_clicked()
{
  ui->sendedit->clear();
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
    else if(ui->portdroplist->count() > 0)
    {
      lastSelectedPort = ui->portdroplist->currentText();
    }

    ui->portdroplist->clear();
    for(uint32_t i = 0; i < last_serial_num; i++)
    {
#ifdef Q_OS_WIN
      ui->portdroplist->addItem(serialPortInfos.at(i).portName());
#else
      ui->portdroplist->addItem(serialPortInfos.at(i).systemLocation());
#endif
    }

    // 尝试恢复之前选中的串口
    if(!lastSelectedPort.isEmpty())
    {
      int index = ui->portdroplist->findText(lastSelectedPort);
      if(index != -1)
      {
        ui->portdroplist->setCurrentIndex(index);
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
        ui->openbutton->setText(tr("Open"));
        ui->openbutton->setText(tr("Open"));
        ui->openbutton->setStyleSheet(
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
  if(ui->autosendcheckbox->isChecked())
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
  QString dataToSend = ui->sendedit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(ui->sendhexcheckbox->isChecked())
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
    if(ui->displaysendcheckbox->isChecked())
    {
      ui->receiveedit->moveCursor(QTextCursor::End);  // 移动光标至末尾
      if(ui->receivehexcheckbox->isChecked())
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
      if(ui->linebreakcheckbox->isChecked())
      {
        dataToSend = dataToSend + "\n"; // 插入换行符
      }
      dataToSend = "[发送]" + dataToSend;
      if(ui->displaytimecheckbox->isChecked())
      {
        QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
        dataToSend = currentTime + dataToSend;  // 插入当前时间
      }
      //插入绿色字体
      ui->receiveedit->setTextColor(QColor(0, 128, 0)); // 设置为绿色
      ui->receiveedit->insertPlainText(dataToSend);  // 插入文本
    }
    qDebug() << "数据已成功写入, 字节数:" << bytesWritten;
    send_frame_count++;
    send_byte_count = send_byte_count + bytesWritten;
    ui->send_frames_count->setText(QString::number(send_frame_count));
    ui->send_bytes_count->setText(QString::number(send_byte_count));
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
  while (hasProcessedFrame) {
    hasProcessedFrame = false;
    int frameEnd = -1;
    
    // Look for common frame delimiters in order of preference
    // First check for CRLF (carriage return + line feed)
    frameEnd = frameBuffer.indexOf("\r\n");
    if (frameEnd == -1) {
      // Then check for just LF (line feed)
      frameEnd = frameBuffer.indexOf('\n');
    }
    if (frameEnd == -1) {
      // Then check for just CR (carriage return)
      frameEnd = frameBuffer.indexOf('\r');
    }
    
    // If we found a delimiter
    if (frameEnd != -1) {
      // Handle CRLF case specifically
      int delimiterLength = 1;
      if (frameEnd + 1 < frameBuffer.size() && frameBuffer.at(frameEnd + 1) == '\n') {
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
  if (!frameBuffer.isEmpty()) {
    // Restart the timer to process remaining data if no delimiter appears soon
    frameProcessingTimer->stop();
    frameProcessingTimer->start(100); // Process after 100ms if no more data comes
  } else {
    // If buffer is empty, stop the timer
    frameProcessingTimer->stop();
  }
  
  // Optional: Handle case where buffer becomes too large (possible missing frame delimiter)
  if (frameBuffer.size() > 10240) { // 10KB threshold
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
  ui->receive_frames_count->setText(QString::number(receive_frame_count));
  ui->receive_bytes_count->setText(QString::number(receive_byte_count));

  ui->receiveedit->moveCursor(QTextCursor::End);  // 移动光标至末尾
  QString dataToDisplay;
  if(ui->receivehexcheckbox->isChecked())
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
  if(ui->linebreakcheckbox->isChecked())
  {
    dataToDisplay = dataToDisplay + "\n"; // 插入换行符
  }
  dataToDisplay = "[接收]" + dataToDisplay;
  if(ui->displaytimecheckbox->isChecked())
  {
    QString currentTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss]");
    dataToDisplay = currentTime + dataToDisplay;  // 插入当前时间
  }
  //插入蓝色字体
  ui->receiveedit->setTextColor(QColor(0, 0, 255)); // 设置为蓝色
  ui->receiveedit->insertPlainText(dataToDisplay);  // 插入文本
}

void PageTransceiver::onFrameProcessingTimeout()
{
  // Process the remaining data in the buffer as a frame
  if (!frameBuffer.isEmpty()) {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageTransceiver::processFrameBuffer()
{
  // Process all data in the buffer as a single frame
  if (!frameBuffer.isEmpty()) {
    processCompleteFrame(frameBuffer);
    frameBuffer.clear();
  }
}

void PageTransceiver::verifysend(int selectedIndex)
{
  PageVerifiyDialog::VERIFIY_TYPE type = (PageVerifiyDialog::VERIFIY_TYPE)selectedIndex;
  QString dataToSend = ui->sendedit->toPlainText();
  if(dataToSend.isEmpty())
  {
    qDebug() << "发送内容为空";
    return;
  }
  if(!ui->sendhexcheckbox->isChecked())
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
        ui->sendedit->setPlainText(dataToSend + " " + crcStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + crcStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + crcStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + sumStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + xorStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + crcStr);
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
        ui->sendedit->setPlainText(dataToSend + " " + crcStr);
        qDebug() << "CRC32_HIGH 校验码:" << crcStr;
        break;
      }
    default:
      qDebug() << "未知的校验类型:" << type;
      break;
  }
}
