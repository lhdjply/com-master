#ifndef PAGENETWORKTOOL_H
#define PAGENETWORKTOOL_H

#include "qtinclude.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkDatagram>
#include <QAbstractSocket>
#include <QFileDialog>

class PageNetworkTool : public QWidget
{
    Q_OBJECT

  public:
    explicit PageNetworkTool(QWidget *parent = nullptr);

  private slots:
    // TCP Server slots
    void startTcpServer();
    void stopTcpServer();
    void tcpSendBroadcast();
    void tcpSendToClient();
    void newTcpClientConnected();
    void tcpClientDisconnected();
    void readTcpClientData();

    // TCP Client slots
    void connectTcpClient();
    void disconnectTcpClient();
    void tcpClientSendData();
    void tcpSocketConnected();
    void tcpSocketDisconnected();
    void tcpSocketReadyRead();
    void tcpSocketError(QAbstractSocket::SocketError error);

    // UDP Server slots
    void startUdpServer();
    void stopUdpServer();
    void udpSendData();
    void readPendingDatagrams();

    // Common slots
    void clearLog();
    void saveLog();
    void clearReceiveData();
    void appendReceiveData(const QString &message);
    void updateProtocolSelection();
    void adjustLeftFrameSize();
    void verifySendData();
    void verifysend(int index);

  private:
    void setupUi();
    void logMessage(const QString &message);
    void updateTcpClientList();
    void updateUdpClientList();
    void updateConnectionStatus();
    QByteArray prepareSendData(const QString &text, bool isHex);
    QString formatAddress(const QHostAddress &address);

    // UI components
    QGridLayout * mainLayout;

    // Protocol selection
    QGroupBox * protocolGroup;
    QHBoxLayout * protocolLayout;
    QComboBox * protocolCombo;

    // TCP Server UI
    QGroupBox * tcpServerGroup;
    QVBoxLayout * tcpServerLayout;
    QLabel * tcpServerIpLabel;
    CustomLineEdit * tcpServerIpEdit;
    QLabel * tcpServerPortLabel;
    CustomLineEdit * tcpServerPortEdit;
    QPushButton * tcpStartButton;
    QPushButton * tcpStopButton;
    QLabel * tcpStatusLabel;
    QListWidget * tcpClientList;
    QPushButton * tcpSendBroadcastButton;
    QPushButton * tcpSendToClientButton;

    // TCP Client UI
    QGroupBox * tcpClientGroup;
    QVBoxLayout * tcpClientLayout;
    QLabel * tcpClientIpLabel;
    CustomLineEdit * tcpClientIpEdit;
    QLabel * tcpClientPortLabel;
    CustomLineEdit * tcpClientPortEdit;
    QPushButton * tcpConnectButton;
    QPushButton * tcpDisconnectButton;
    QLabel * tcpClientStatusLabel;
    QPushButton * tcpClientSendButton;
    QCheckBox * tcpAutoSendCheck;
    CustomLineEdit * tcpAutoSendIntervalEdit;
    QLabel * tcpIntervalLabel;
    QLabel * tcpMsLabel;
    QCheckBox * tcpServerAutoSendCheck;
    CustomLineEdit * tcpServerAutoSendIntervalEdit;
    QLabel * tcpServerIntervalLabel;
    QLabel * tcpServerMsLabel;
    QCheckBox * udpAutoSendCheck;
    CustomLineEdit * udpAutoSendIntervalEdit;
    QLabel * udpIntervalLabel;
    QLabel * udpMsLabel;
    QFrame * autoSendFrame;
    QHBoxLayout * autoSendFrameLayout;
    QLabel * autoSendLabel;
    CustomLineEdit * autoSendIntervalEdit;
    QLabel * autoSendMsLabel;
    QPushButton * verifyButton;

    // UDP UI (Server and Client combined)
    QGroupBox * udpGroup;
    QVBoxLayout * udpLayout;
    QLabel * udpServerIpLabel;
    CustomLineEdit * udpServerIpEdit;
    QLabel * udpServerPortLabel;
    CustomLineEdit * udpServerPortEdit;
    QPushButton * udpStartButton;
    QPushButton * udpStopButton;
    QLabel * udpStatusLabel;
    QLabel * udpTargetIpLabel;
    CustomLineEdit * udpTargetIpEdit;
    QLabel * udpTargetPortLabel;
    CustomLineEdit * udpTargetPortEdit;
    QPushButton * udpSendButton;
    QListWidget * udpClientList;

    // Common UI
    QGroupBox * sendGroup;
    QVBoxLayout * sendLayout;
    CustomTextEdit * sendEdit;
    QCheckBox * sendHexCheckbox;

    QGroupBox * receiveGroup;
    QVBoxLayout * receiveLayout;
    CustomTextEdit * logText;
    CustomTextEdit * receiveDataEdit;
    QCheckBox * receiveHexCheckbox;
    QCheckBox * displaySendCheckbox;
    QCheckBox * displayTimeCheckbox;
    QCheckBox * frameLineBreakCheckbox;
    QHBoxLayout * receiveButtonLayout;
    QPushButton * clearButton;
    QPushButton * saveButton;

    // Network components
    QTcpServer * tcpServer;
    QList<QTcpSocket *> tcpClients;
    QTcpSocket * tcpSocket;
    QUdpSocket * udpSocket;

    // UI frame reference
    QFrame * leftFrame;

    // UDP client tracking
    struct UdpClientInfo
    {
      QHostAddress address;
      quint16 port;
      QDateTime lastSeen;
    };
    QList<UdpClientInfo> udpClients;

    // Status
    bool tcpServerRunning;
    bool tcpClientConnected;
    bool udpServerRunning;
    QTimer * tcpAutoSendTimer;
    QTimer * tcpServerAutoSendTimer;
    QTimer * udpAutoSendTimer;
};

#endif
