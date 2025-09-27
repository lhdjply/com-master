#ifndef PAGETRANSCEIVER_H
#define PAGETRANSCEIVER_H

#include "qtinclude.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class PageTransceiver;
}
QT_END_NAMESPACE

class PageTransceiver : public QMainWindow
{
    Q_OBJECT

  public:
    PageTransceiver(QWidget *parent = nullptr);
    ~PageTransceiver();

  private slots:
    void on_openbutton_clicked();

  private:
    Ui::PageTransceiver * ui;
    uint32_t last_serial_num;
    bool isSerial_Open;
    QString Serial_Name;
    uint32_t Current_BaudRate;
    QSerialPort::DataBits Current_DataBits;
    uint8_t Current_Parity;
    uint8_t Current_StopBits;
    QSerialPort serialPort;
    void handleTimeout();
};
#endif
