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
    void on_sendbutton_clicked();
    void on_verifybutton_clicked();
    void on_clearcount_clicked();
    void on_clearreceive_clicked();
    void on_clearsend_clicked();
    void on_senddutyedit_textChanged(const QString &arg1);

  private:
    QTimer * AutosendTimer;
    Ui::PageTransceiver * ui;
    uint32_t last_serial_num;
    uint32_t receive_frame_count;
    uint32_t receive_byte_count;
    uint32_t send_frame_count;
    uint32_t send_byte_count;
    bool isSerial_Open;
    QString Serial_Name;
    uint32_t Current_BaudRate;
    QSerialPort::DataBits Current_DataBits;
    uint8_t Current_Parity;
    uint8_t Current_StopBits;
    QSerialPort serialPort;
    void handleTimeout();
    void autosend();
    void sendData();
    void receiveData();
    void verifysend(int selectedIndex);
};
#endif
