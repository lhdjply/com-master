#ifndef PAGESERIALTRANSCEIVER_H
#define PAGESERIALTRANSCEIVER_H

#include "qtinclude.h"

class CustomTextEdit : public QTextEdit
{
    Q_OBJECT

  public:
    explicit CustomTextEdit(QWidget *parent = nullptr);

  protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

class PageSerialTransceiver : public QMainWindow
{
    Q_OBJECT

  public:
    PageSerialTransceiver(QWidget *parent = nullptr);
    ~PageSerialTransceiver();

  private slots:
    void on_openbutton_clicked();
    void on_sendbutton_clicked();
    void on_verifybutton_clicked();
    void on_clearcount_clicked();
    void on_clearreceive_clicked();
    void on_clearsend_clicked();
    void on_senddutyedit_textChanged(const QString &arg1);

  private:
    void setupUi();
    void retranslateUi();

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

    QWidget * centralwidget;
    QHBoxLayout * horizontalLayout_10;
    QFrame * frame;
    QVBoxLayout * verticalLayout_7;
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
    QGroupBox * verticalGroupBox;
    QVBoxLayout * verticalLayout_4;
    QHBoxLayout * horizontalLayout_11;
    QHBoxLayout * horizontalLayout_12;
    QHBoxLayout * horizontalLayout_13;
    QHBoxLayout * horizontalLayout_14;
    QLabel * receive_frames;
    QLabel * receive_frames_count;
    QLabel * receive_bytes;
    QLabel * receive_bytes_count;
    QLabel * send_frames;
    QLabel * send_frames_count;
    QLabel * send_bytes;
    QLabel * send_bytes_count;
    QPushButton * clearcount;
    QVBoxLayout * verticalLayout_3;
    QGroupBox * receivegroup;
    QVBoxLayout * verticalLayout_5;
    CustomTextEdit * receiveedit;
    QHBoxLayout * horizontalLayout;
    QCheckBox * receivehexcheckbox;
    QCheckBox * displaysendcheckbox;
    QCheckBox * displaytimecheckbox;
    QCheckBox * linebreakcheckbox;
    QPushButton * clearreceive;
    QGroupBox * sendgroup;
    QVBoxLayout * verticalLayout_6;
    CustomTextEdit * sendedit;
    QHBoxLayout * horizontalLayout_9;
    QPushButton * sendbutton;
    QPushButton * verifybutton;
    QPushButton * clearsend;
    QCheckBox * sendhexcheckbox;
    QCheckBox * autosendcheckbox;
    QFrame * frame_3;
    QHBoxLayout * horizontalLayout_8;
    QLabel * label_6;
    QLineEdit * senddutyedit;
    QLabel * label_7;

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
