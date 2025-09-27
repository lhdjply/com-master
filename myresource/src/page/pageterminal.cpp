#include "myresource.h"
#include "./ui_pageterminal.h"

PageTerminal::PageTerminal(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::PageTerminal)
{
  ui->setupUi(this);

  last_serial_num = 0;
  isSerial_Open = false;

  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &PageTerminal::handleTimeout);
  timer->start(1000);
}

PageTerminal::~PageTerminal()
{
  delete ui;
}

void PageTerminal::on_openbutton_clicked()
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
    serialPort.close();
    qDebug() << "串口已关闭";
  }
  else
  {
    if(serialPort.open(QIODevice::ReadWrite))
    {
      isSerial_Open = true;
      ui->openbutton->setText(tr("Close"));
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
        serialPort.close();
        qDebug() << "串口被拔除，已自动关闭";
      }
    }
  }
}
