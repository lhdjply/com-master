#ifndef PAGETERMINAL_H
#define PAGETERMINAL_H

#include "qtinclude.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class PageTerminal;
}
QT_END_NAMESPACE

class PageTerminal : public QMainWindow
{
    Q_OBJECT

  public:
    PageTerminal(QWidget *parent = nullptr);
    ~PageTerminal();

  private slots:
    void on_openbutton_clicked();

  private:
    Ui::PageTerminal * ui;
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
