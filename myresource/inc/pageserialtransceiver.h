#ifndef PAGESERIALTRANSCEIVER_H
#define PAGESERIALTRANSCEIVER_H

#include "qtinclude.h"

class PageSerialTransceiver : public QMainWindow
{
    Q_OBJECT

  public:
    PageSerialTransceiver(QWidget *parent = nullptr);
    ~PageSerialTransceiver();

  private slots:
    void on_openButton_clicked();
    void on_sendButton_clicked();
    void on_verifyButton_clicked();
    void on_clearcountButton_clicked();
    void on_clearReceiveButton_clicked();
    void on_clearSendButton_clicked();
    void on_sendDutyEdit_textChanged(const QString &arg1);

  private:
    void setupUi();
    void retranslateUi();

    // 核心数据成员
    QTimer * AutosendTimer;
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
    QByteArray frameBuffer;  // Buffer to accumulate received data
    QTimer * frameProcessingTimer;  // Timer to periodically process incomplete frames

    // 主窗口和布局
    QWidget * centralwidget;
    QHBoxLayout * mainLayout;

    // 配置区域
    QFrame * configFrame;
    QVBoxLayout * configFrameLayout;
    
    // 串口设置组
    QGroupBox * configGroupBox;
    QVBoxLayout * configLayout;
    QHBoxLayout * portLayout;
    QHBoxLayout * baudrateLayout;
    QHBoxLayout * databitsLayout;
    QHBoxLayout * parityLayout;
    QHBoxLayout * stopbitsLayout;
    QLabel * portLabel;
    QLabel * baudrateLabel;
    QLabel * databitsLabel;
    QLabel * parityLabel;
    QLabel * stopbitsLabel;
    QComboBox * portdroplist;
    QComboBox * baudratedroplist;
    QComboBox * databitsdroplist;
    QComboBox * paritydroplist;
    QComboBox * stopbitsdroplist;
    QPushButton * openButton;
    
    // 统计信息组
    QGroupBox * statsGroupBox;
    QVBoxLayout * statsLayout;
    QHBoxLayout * receiveFramesLayout;
    QHBoxLayout * receiveBytesLayout;
    QHBoxLayout * sendFramesLayout;
    QHBoxLayout * sendBytesLayout;
    QLabel * receiveFramesLabel;
    QLabel * receiveFramesCountLabel;
    QLabel * receiveBytesLabel;
    QLabel * receiveBytesCountLabel;
    QLabel * sendFramesLabel;
    QLabel * sendFramesCountLabel;
    QLabel * sendBytesLabel;
    QLabel * sendBytesCountLabel;
    QPushButton * clearcountButton;

    // 数据区域
    QVBoxLayout * dataLayout;
    
    // 接收区域
    QGroupBox * receiveGroupBox;
    QVBoxLayout * receiveLayout;
    CustomTextEdit * receiveEdit;
    QHBoxLayout * receiveOptionsLayout;
    QCheckBox * receiveHexCheckbox;
    QCheckBox * displaySendCheckbox;
    QCheckBox * displayTimeCheckbox;
    QCheckBox * lineBreakCheckbox;
    QPushButton * clearReceiveButton;
    
    // 发送区域
    QGroupBox * sendGroupBox;
    QVBoxLayout * sendLayout;
    CustomTextEdit * sendEdit;
    QHBoxLayout * sendOptionsLayout;
    QPushButton * sendButton;
    QPushButton * verifyButton;
    QPushButton * clearSendButton;
    QCheckBox * sendHexCheckbox;
    QCheckBox * autoSendCheckbox;
    QFrame * sendDutyFrame;
    QHBoxLayout * sendDutyLayout;
    QLabel * sendDutyLabel;
    CustomLineEdit * sendDutyEdit;
    QLabel * sendDutyUnitLabel;

    QByteArray getFrameDelimiter(); // Get the current frame delimiter based on settings
    void handleTimeout();
    void autosend();
    void sendData();
    void receiveData();
    void processFrameBuffer();
    void processCompleteFrame(const QByteArray &frame);
    void onFrameProcessingTimeout(); // Timer timeout handler
    void verifysend(int selectedIndex);
};
#endif
