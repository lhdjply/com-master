#ifndef PAGETERMINAL_H
#define PAGETERMINAL_H

#include "qtinclude.h"

class PageTerminal : public QMainWindow
{
    Q_OBJECT

  public:
    PageTerminal(QWidget *parent = nullptr);
    ~PageTerminal();

  private slots:
    void on_openbutton_clicked();
    void readSerialData();

  private:
    void setupUi();
    void retranslateUi();

    QWidget * centralwidget;
    QHBoxLayout * horizontalLayout;
    QGroupBox * groupBox;
    QVBoxLayout * verticalLayout;
    QHBoxLayout * horizontalLayout_1;
    QHBoxLayout * horizontalLayout_2;
    QHBoxLayout * horizontalLayout_3;
    QHBoxLayout * horizontalLayout_4;
    QHBoxLayout * horizontalLayout_5;
    QLabel * label;
    QLabel * label_2;
    QLabel * label_3;
    QLabel * label_4;
    QLabel * label_5;
    QComboBox * portdroplist;
    QComboBox * baudratedroplist;
    QComboBox * databitsdroplist;
    QComboBox * paritydroplist;
    QComboBox * stopbitsdroplist;
    QPushButton * openbutton;
    QTermWidget * terminal;

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
