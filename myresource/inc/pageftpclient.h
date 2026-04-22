#ifndef PAGEFTPCLIENT_H
#define PAGEFTPCLIENT_H

#include "qtinclude.h"

class PageFtpClient : public QWidget
{
    Q_OBJECT

  public:
    PageFtpClient(QWidget *parent = nullptr);
    ~PageFtpClient();

  private slots:
    void connectToFtp();
    void disconnectFromFtp();
    void downloadFile();
    void uploadFile();
    void listDirectory();
    void onSocketReadyRead();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void processItem(QTreeWidgetItem *item, int column);
    void onDataSocketConnected();
    void onDataSocketDisconnected();
    void onDataSocketReadyRead();
    void onDataSocketError(QAbstractSocket::SocketError error);
    void onDataSocketBytesWritten(qint64 bytes);
    void resetConnectionState();

  private:
    void setupUi();
    void sendCommand(const QString &command);
    void parseFtpList(const QByteArray &data);
    void handleFtpResponse(const QString &response);
    QHostAddress parsePassiveModeAddress(const QString &response);
    quint16 parsePassiveModePort(const QString &response);
    void uploadFileData();
    qint64 parseFileSize(const QString &sizeStr);

    QVBoxLayout * mainLayout;
    QHBoxLayout * connectionLayout;
    CustomLineEdit * serverEdit;
    CustomLineEdit * portEdit;
    CustomLineEdit * userEdit;
    CustomLineEdit * passwordEdit;
    QPushButton * connectButton;
    QPushButton * disconnectButton;

    QTreeWidget * fileList;
    CustomTextEdit * logText;
    QLabel * currentPathLabel;

    QHBoxLayout * fileOperationLayout;
    QPushButton * downloadButton;
    QPushButton * uploadButton;
    QPushButton * listButton;
    QProgressBar * progressBar;

    QTcpSocket * socket;
    QTcpSocket * dataSocket;
    QString currentPath;
    QFile * file;
    QByteArray listData;
    bool isConnected;
    int replyCode;
    QString replyMessage;
    enum FtpOperation { None, List, Download, Upload } currentOperation;

    qint64 totalBytes;
    qint64 bytesTransferred;

    // FTP响应代码枚举
    enum FtpResponseCode
    {
      FTP_RESP_SERVICE_READY = 220, // 服务就绪
      FTP_RESP_NEED_PASSWORD = 331, // 需要密码
      FTP_RESP_LOGIN_SUCCESS = 230, // 登录成功
      FTP_RESP_ENTERING_PASSIVE_MODE = 227, // 进入被动模式
      FTP_RESP_DATA_CONNECTION_OPEN = 150, // 打开数据连接
      FTP_RESP_DATA_CONNECTION_CLOSED = 226, // 关闭数据连接
      FTP_RESP_FILE_OPERATION_SUCCESS = 250, // 文件操作成功
      FTP_RESP_LOGIN_FAILED = 530  // 登录失败
    };
};

#endif // PAGEFTPCLIENT_H
