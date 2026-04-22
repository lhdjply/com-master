#include "myresource.h"
#include "appcolors.h"

PageFtpClient::PageFtpClient(QWidget *parent)
  : QWidget(parent)
  , socket(nullptr)
  , dataSocket(nullptr)
  , file(nullptr)
  , currentPath("/")
  , isConnected(false)
  , replyCode(0)
  , currentOperation(None)
  , totalBytes(0)
  , bytesTransferred(0)
{
  // 先初始化socket和dataSocket
  socket = new QTcpSocket(this);

  connect(socket, &QTcpSocket::connected, this, &PageFtpClient::onSocketConnected);
  connect(socket, &QTcpSocket::disconnected, this, &PageFtpClient::onSocketDisconnected);
  connect(socket, &QTcpSocket::readyRead, this, &PageFtpClient::onSocketReadyRead);
  connect(socket, &QTcpSocket::errorOccurred, this, &PageFtpClient::onSocketError);

  dataSocket = new QTcpSocket(this);

  connect(dataSocket, &QTcpSocket::connected, this, &PageFtpClient::onDataSocketConnected);
  connect(dataSocket, &QTcpSocket::disconnected, this, &PageFtpClient::onDataSocketDisconnected);
  connect(dataSocket, &QTcpSocket::readyRead, this, &PageFtpClient::onDataSocketReadyRead);
  connect(dataSocket, &QTcpSocket::errorOccurred, this, &PageFtpClient::onDataSocketError);
  connect(dataSocket, &QTcpSocket::bytesWritten, this, &PageFtpClient::onDataSocketBytesWritten);

  // 然后再调用setupUi()
  setupUi();
}

PageFtpClient::~PageFtpClient()
{
  if(socket)
  {
    socket->disconnectFromHost();
    socket->deleteLater();
  }
  if(dataSocket)
  {
    dataSocket->disconnectFromHost();
    dataSocket->deleteLater();
  }
  if(file)
  {
    delete file;
  }
}

QString getFileIcon(const QString &fileName)
{
  QString ext = fileName.toLower();
  int dotIndex = ext.lastIndexOf(".");
  if(dotIndex > 0)
  {
    ext = ext.mid(dotIndex + 1);
  }

  if(ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" || ext == "bmp" || ext == "svg" || ext == "webp")
  {
    return ":/icons/image.svg";
  }
  if(ext == "zip" || ext == "rar" || ext == "7z" || ext == "tar" || ext == "gz" || ext == "bz2")
  {
    return ":/icons/archive.svg";
  }
  return ":/icons/file.svg";
}

QString getFileType(const QString &fileName)
{
  int dotIndex = fileName.lastIndexOf(".");
  if(dotIndex > 0)
  {
    QString ext = fileName.mid(dotIndex + 1).toUpper();
    return ext + " " + QObject::tr("File");
  }
  return QObject::tr("File");
}

void PageFtpClient::setupUi()
{
  // 应用与其他界面一致的样式
  this->setStyleSheet(AppColors::getStyleSheet());

  // 主垂直布局
  mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(15);

  // 初始化未使用的成员变量
  connectionLayout = nullptr;
  fileOperationLayout = nullptr;

  // 创建主水平布局，分为左右两个面板
  QHBoxLayout *mainHorizontalLayout = new QHBoxLayout();
  mainHorizontalLayout->setSpacing(20);

  // 左侧面板 - 配置
  QFrame *leftFrame = new QFrame();
  leftFrame->setMinimumSize(QSize(280, 0));
  leftFrame->setMaximumSize(QSize(280, 16777215));

  QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftFrame);
  leftPanelLayout->setSpacing(15);

  // FTP 服务器配置
  QGroupBox *ftpConfigGroup = new QGroupBox(tr("FTP Server Configuration"));
  QVBoxLayout *ftpConfigLayout = new QVBoxLayout(ftpConfigGroup);
  ftpConfigLayout->setSpacing(12);

  QLabel *serverLabel = new QLabel(tr("Server:"));
  serverLabel->setMinimumSize(QSize(60, 0));
  serverEdit = new CustomLineEdit("localhost");
  serverEdit->setMinimumSize(QSize(146, 0));

  QLabel *portLabel = new QLabel(tr("Port:"));
  portLabel->setMinimumSize(QSize(60, 0));
  portEdit = new CustomLineEdit("21");
  portEdit->setMinimumSize(QSize(146, 0));

  QLabel *userLabel = new QLabel(tr("User:"));
  userLabel->setMinimumSize(QSize(60, 0));
  userEdit = new CustomLineEdit();
  userEdit->setMinimumSize(QSize(146, 0));

  QLabel *passLabel = new QLabel(tr("Password:"));
  passLabel->setMinimumSize(QSize(60, 0));
  passwordEdit = new CustomLineEdit();
  passwordEdit->setEchoMode(QLineEdit::Password);
  passwordEdit->setMinimumSize(QSize(146, 0));

  connectButton = new QPushButton(tr("Connect"));
  connectButton->setMinimumSize(QSize(70, 36));
  connectButton->setStyleSheet(AppColors::getStartButtonStyle());
  disconnectButton = new QPushButton(tr("Disconnect"));
  disconnectButton->setMinimumSize(QSize(70, 36));
  disconnectButton->setStyleSheet(AppColors::getStopButtonStyle());
  disconnectButton->setEnabled(false);
  QLabel *statusLabel = new QLabel(tr("Status: Disconnected"));

  QHBoxLayout *serverLayout = new QHBoxLayout();
  serverLayout->addWidget(serverLabel);
  serverLayout->addWidget(serverEdit);
  ftpConfigLayout->addLayout(serverLayout);

  QHBoxLayout *portLayout = new QHBoxLayout();
  portLayout->addWidget(portLabel);
  portLayout->addWidget(portEdit);
  ftpConfigLayout->addLayout(portLayout);

  QHBoxLayout *userLayout = new QHBoxLayout();
  userLayout->addWidget(userLabel);
  userLayout->addWidget(userEdit);
  ftpConfigLayout->addLayout(userLayout);

  QHBoxLayout *passLayout = new QHBoxLayout();
  passLayout->addWidget(passLabel);
  passLayout->addWidget(passwordEdit);
  ftpConfigLayout->addLayout(passLayout);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(connectButton);
  buttonLayout->addWidget(disconnectButton);
  buttonLayout->addStretch();
  ftpConfigLayout->addLayout(buttonLayout);

  ftpConfigLayout->addWidget(statusLabel);

  // 添加配置组到左侧面板
  leftPanelLayout->addWidget(ftpConfigGroup);

  // 左侧面板底部的日志窗口
  QGroupBox *logGroup = new QGroupBox(tr("Log"));
  QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
  logLayout->setSpacing(5);

  logText = new CustomTextEdit();
  logText->setReadOnly(true);
  logText->setStyleSheet("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;");

  QHBoxLayout *logButtonLayout = new QHBoxLayout();
  logButtonLayout->setSpacing(10);
  QPushButton *clearLogButton = new QPushButton(tr("Clear"));
  clearLogButton->setMinimumSize(QSize(80, 30));
  QPushButton *saveLogButton = new QPushButton(tr("Save"));
  saveLogButton->setMinimumSize(QSize(80, 30));
  logButtonLayout->addWidget(clearLogButton);
  logButtonLayout->addWidget(saveLogButton);
  logButtonLayout->addStretch();

  logLayout->addWidget(logText);
  logLayout->addLayout(logButtonLayout);

  leftPanelLayout->addWidget(logGroup);

  // 右侧面板 - 文件列表和操作
  QVBoxLayout *rightPanelLayout = new QVBoxLayout();
  rightPanelLayout->setSpacing(15);

  // 当前目录显示
  QHBoxLayout *currentPathLayout = new QHBoxLayout();
  currentPathLabel = new QLabel(tr("Current Path: /"));
  currentPathLayout->addWidget(currentPathLabel);
  currentPathLayout->addStretch();
  rightPanelLayout->addLayout(currentPathLayout);

  // 文件列表
  QGroupBox *fileListGroup = new QGroupBox(tr("File List"));
  QVBoxLayout *fileListLayout = new QVBoxLayout(fileListGroup);
  fileList = new QTreeWidget();
  fileList->setHeaderLabels({tr("Name"), tr("Size"), tr("Type"), tr("Date")});
  fileList->setColumnWidth(0, 200);
  fileList->setColumnWidth(1, 80);
  fileList->setColumnWidth(2, 80);
  fileList->setColumnWidth(3, 120);
  fileListLayout->addWidget(fileList);
  rightPanelLayout->addWidget(fileListGroup);

  // 操作按钮
  QGroupBox *actionGroup = new QGroupBox(tr("Actions"));
  QVBoxLayout *actionLayout = new QVBoxLayout(actionGroup);
  actionLayout->setSpacing(10);

  QHBoxLayout *actionButtonLayout = new QHBoxLayout();
  actionButtonLayout->setSpacing(10);

  downloadButton = new QPushButton(tr("Download"));
  downloadButton->setMinimumSize(QSize(80, 36));
  downloadButton->setEnabled(false);

  uploadButton = new QPushButton(tr("Upload"));
  uploadButton->setMinimumSize(QSize(80, 36));
  uploadButton->setEnabled(false);

  listButton = new QPushButton(tr("List"));
  listButton->setMinimumSize(QSize(80, 36));
  listButton->setEnabled(false);

  actionButtonLayout->addWidget(downloadButton);
  actionButtonLayout->addWidget(uploadButton);
  actionButtonLayout->addWidget(listButton);
  actionButtonLayout->addStretch();

  actionLayout->addLayout(actionButtonLayout);

  // 进度条
  progressBar = new QProgressBar();
  progressBar->setMinimum(0);
  progressBar->setMaximum(100);
  progressBar->setValue(0);
  progressBar->setVisible(false);

  actionLayout->addWidget(progressBar);

  rightPanelLayout->addWidget(actionGroup);

  // 添加左右面板到主水平布局
  mainHorizontalLayout->addWidget(leftFrame);
  mainHorizontalLayout->addLayout(rightPanelLayout);

  // 添加到主布局
  mainLayout->addLayout(mainHorizontalLayout);

  // 连接信号和槽
  connect(connectButton, &QPushButton::clicked, this, &PageFtpClient::connectToFtp);
  connect(disconnectButton, &QPushButton::clicked, this, &PageFtpClient::disconnectFromFtp);
  connect(downloadButton, &QPushButton::clicked, this, &PageFtpClient::downloadFile);
  connect(uploadButton, &QPushButton::clicked, this, &PageFtpClient::uploadFile);
  connect(listButton, &QPushButton::clicked, this, &PageFtpClient::listDirectory);
  connect(fileList, &QTreeWidget::itemDoubleClicked, this, &PageFtpClient::processItem);
  connect(clearLogButton, &QPushButton::clicked, [this]()
  {
    logText->clear();
  });
  connect(saveLogButton, &QPushButton::clicked, [this]()
  {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log"), "", tr("Text Files (*.txt)"));
    if(!fileName.isEmpty())
    {
      QFile file(fileName);
      if(file.open(QIODevice::WriteOnly | QIODevice::Text))
      {
        QTextStream out(&file);
        out << logText->toPlainText();
        logText->append(tr("Log saved to %1").arg(fileName));
      }
    }
  });
}

void PageFtpClient::connectToFtp()
{
  if(isConnected)
  {
    logText->append(tr("Already connected"));
    return;
  }

  if(socket->state() == QTcpSocket::ConnectingState)
  {
    logText->append(tr("Connection in progress"));
    return;
  }

  if(socket->state() == QTcpSocket::ConnectedState)
  {
    socket->disconnectFromHost();
  }

  QString server = serverEdit->text();
  int port = portEdit->text().toInt();

  // 处理 localhost，自动替换为 127.0.0.1
  if(server.toLower() == "localhost")
  {
    server = "127.0.0.1";
  }

  // 禁用代理设置，避免代理类型错误
  socket->setProxy(QNetworkProxy::NoProxy);

  logText->append(tr("Connecting to %1:%2...").arg(server).arg(port));
  socket->connectToHost(server, port);
}

void PageFtpClient::disconnectFromFtp()
{
  if(!isConnected)
  {
    logText->append(tr("Not connected"));
    return;
  }

  sendCommand("QUIT");
  socket->disconnectFromHost();
}

void PageFtpClient::downloadFile()
{
  QTreeWidgetItem *currentItem = fileList->currentItem();
  if(!currentItem || currentItem->text(2) == tr("Directory"))
    return;

  QString fileName = currentItem->text(0);
  QString localFileName = QFileDialog::getSaveFileName(this, tr("Save File"), fileName);
  if(localFileName.isEmpty())
    return;

  file = new QFile(localFileName);
  if(!file->open(QIODevice::WriteOnly))
  {
    logText->append(tr("Error opening file for writing: %1").arg(file->errorString()));
    delete file;
    file = nullptr;
    return;
  }

  file->setProperty("ftpFileName", fileName);

  // 从列表项获取文件大小并解析
  QString sizeStr = currentItem->text(1);
  totalBytes = parseFileSize(sizeStr);

  if(dataSocket->state() != QTcpSocket::UnconnectedState)
  {
    dataSocket->abort();
  }

  bytesTransferred = 0;

  progressBar->setValue(0);
  progressBar->setVisible(true);

  currentOperation = Download;
  sendCommand("PASV");

  logText->append(tr("Downloading file: %1").arg(fileName));
}

void PageFtpClient::uploadFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select File"));
  if(fileName.isEmpty())
    return;

  QFileInfo fileInfo(fileName);
  file = new QFile(fileName);
  if(!file->open(QIODevice::ReadOnly))
  {
    logText->append(tr("Error opening file for reading: %1").arg(file->errorString()));
    delete file;
    file = nullptr;
    return;
  }

  if(dataSocket->state() != QTcpSocket::UnconnectedState)
  {
    dataSocket->abort();
  }

  totalBytes = file->size();
  bytesTransferred = 0;

  progressBar->setValue(0);
  progressBar->setVisible(true);

  currentOperation = Upload;
  sendCommand("PASV");

  logText->append(tr("Uploading file: %1").arg(fileInfo.fileName()));
}

void PageFtpClient::listDirectory()
{
  if(dataSocket->state() != QTcpSocket::UnconnectedState)
  {
    dataSocket->abort();
  }

  currentOperation = List;
  sendCommand("PASV");

  logText->append(tr("Listing directory: %1").arg(currentPath));
}

void PageFtpClient::onSocketConnected()
{
  logText->append(tr("Connected to server"));
  // 连接成功后，服务器会发送欢迎消息
  // 我们需要等待欢迎消息，然后发送登录信息
}

void PageFtpClient::onSocketDisconnected()
{
  logText->append(tr("Disconnected from server"));
  // 重置连接状态
  resetConnectionState();

  fileList->clear();
}

void PageFtpClient::resetConnectionState()
{
  isConnected = false;
  connectButton->setEnabled(true);
  disconnectButton->setEnabled(false);
  downloadButton->setEnabled(false);
  uploadButton->setEnabled(false);
  listButton->setEnabled(false);
}

void PageFtpClient::onSocketError(QAbstractSocket::SocketError error)
{
  logText->append(tr("Socket error: %1").arg(socket->errorString()));
  // 重置连接状态
  resetConnectionState();
}

void PageFtpClient::onSocketReadyRead()
{
  QByteArray data = socket->readAll();
  QString response = QString(data);

  // 处理FTP响应
  handleFtpResponse(response);
}

void PageFtpClient::sendCommand(const QString &command)
{
  // 只记录重要命令的日志
  QStringList importantCommands = {"USER", "LIST", "RETR", "STOR", "QUIT"};
  QString cmd = command.section(' ', 0, 0);
  if(!command.startsWith("PASS") && importantCommands.contains(cmd))
  {
    logText->append(tr("Sending command: %1").arg(command));
  }
  socket->write((command + "\r\n").toUtf8());
}

void PageFtpClient::handleFtpResponse(const QString &response)
{
  // 解析FTP响应代码
  QStringList lines = response.split("\r\n", Qt::SkipEmptyParts);
  for(const QString &line : lines)
  {
    if(line.isEmpty())
      continue;

    // 提取响应代码
    if(line.length() >= 3)
    {
      bool ok;
      int code = line.left(3).toInt(&ok);
      if(ok)
      {
        replyCode = code;
        replyMessage = line.mid(4).trimmed();

        // 处理不同的响应代码
        switch(code)
        {
          case FTP_RESP_SERVICE_READY: // 服务就绪
            // 发送登录信息
            sendCommand("USER " + userEdit->text());
            break;
          case FTP_RESP_NEED_PASSWORD: // 需要密码
            sendCommand("PASS " + passwordEdit->text());
            break;
          case FTP_RESP_LOGIN_SUCCESS: // 登录成功
            logText->append(tr("Login successful"));
            // 启用按钮
            isConnected = true;
            connectButton->setEnabled(false);
            disconnectButton->setEnabled(true);
            downloadButton->setEnabled(true);
            uploadButton->setEnabled(true);
            listButton->setEnabled(true);
            // 列出根目录
            listDirectory();
            break;
          case FTP_RESP_ENTERING_PASSIVE_MODE: // 进入被动模式
            {
              // 解析被动模式地址和端口
              QHostAddress address = parsePassiveModeAddress(line);
              quint16 port = parsePassiveModePort(line);

              if(address.isNull() || port == 0)
              {
                logText->append(tr("Error parsing passive mode response"));
                break;
              }

              // 连接数据套接字
              dataSocket->setProxy(QNetworkProxy::NoProxy);
              dataSocket->connectToHost(address, port);

              break;
            }
          case FTP_RESP_DATA_CONNECTION_OPEN: // 文件状态正常，打开数据连接
            // 在开始接收数据时清空列表，避免提前清空导致空白
            if(currentOperation == List)
            {
              fileList->clear();
              listData.clear();
            }
            break;
          case FTP_RESP_DATA_CONNECTION_CLOSED: // 关闭数据连接
            if(file)
            {
              file->close();
              delete file;
              file = nullptr;
            }
            // 检查是否需要刷新目录（用于上传和下载操作）
            if(this->property("needRefresh").toBool())
            {
              logText->append(tr("File transfer completed"));
              progressBar->setValue(100);

              // 使用定时器检查数据套接字状态，一旦断开就立即刷新
              // 避免固定延迟对小文件造成卡顿
              QTimer *refreshTimer = new QTimer(this);
              connect(refreshTimer, &QTimer::timeout, this, [this, refreshTimer]()
              {
                if(dataSocket->state() == QTcpSocket::UnconnectedState)
                {
                  // 数据套接字已断开，可以刷新目录
                  refreshTimer->stop();
                  refreshTimer->deleteLater();
                  listDirectory();
                  progressBar->setVisible(false);
                  this->setProperty("needRefresh", false);
                }
              });
              refreshTimer->start(50); // 每50毫秒检查一次
            }
            break;
          case FTP_RESP_FILE_OPERATION_SUCCESS: // 请求的文件操作成功
            logText->append(tr("File operation successful"));
            break;
          case FTP_RESP_LOGIN_FAILED: // 登录失败
            logText->append(tr("Login failed"));
            // 重置连接状态
            resetConnectionState();
            // 断开套接字连接，以便可以重新连接
            socket->disconnectFromHost();
            break;
          default:
            logText->append(tr("Response code: %1, message: %2").arg(code).arg(replyMessage));
            break;
        }
      }
    }
  }
}

void PageFtpClient::parseFtpList(const QByteArray &data)
{
  fileList->clear();

  if(currentPath != "/")
  {
    QTreeWidgetItem *parentItem = new QTreeWidgetItem();
    parentItem->setText(0, "..");
    parentItem->setText(1, "-");
    parentItem->setText(2, tr("Directory"));
    parentItem->setText(3, "");
    parentItem->setIcon(0, QIcon(":/icons/folder.svg"));
    fileList->addTopLevelItem(parentItem);
  }

  QList<QTreeWidgetItem *> directories;
  QList<QTreeWidgetItem *> files;

  QStringList lines = QString(data).split("\n");
  foreach(QString line, lines)
  {
    line = line.trimmed();
    if(line.isEmpty())
      continue;

    QStringList parts = line.split(QRegularExpression("\\s+"));
    if(parts.size() < 9)
      continue;

    QString permissions = parts[0];
    QString name = parts[8];

    QString symlinkTarget = "";
    if(permissions.startsWith("l"))
    {
      // 对于符号链接，将整个剩余部分作为 name
      for(int i = 9; i < parts.size(); ++i)
      {
        name += " " + parts[i];
      }
      int arrowIndex = name.indexOf(" -> ");
      if(arrowIndex > 0)
      {
        symlinkTarget = name.mid(arrowIndex + 4);
        name = name.left(arrowIndex);
      }
    }
    else
    {
      for(int i = 9; i < parts.size(); ++i)
      {
        name += " " + parts[i];
      }
    }

    if(name == ".")
      continue;

    bool isDir = permissions.startsWith('d');
    bool isSymlink = permissions.startsWith('l');

    // For symlinks, try to determine if it points to a directory or file
    if(isSymlink)
    {
      // First, check if the target name looks like a directory (no extension)
      QFileInfo targetInfo(symlinkTarget);
      QString targetExt = targetInfo.suffix().toLower();

      // If no extension, assume it's a directory
      if(targetExt.isEmpty())
      {
        isDir = true;
      }
    }

    QString size = (isDir || isSymlink) ? "-" : parts[4];
    QString date = parts[5] + " " + parts[6] + " " + parts[7];

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setText(1, size);
    item->setText(2, isDir ? tr("Directory") : getFileType(name));
    item->setText(3, date);
    if(!symlinkTarget.isEmpty())
    {
      item->setData(0, Qt::UserRole, symlinkTarget);
    }

    if(isSymlink)
    {
      // Use dedicated symlink icons
      if(isDir)
      {
        // Use folder_symlink.svg for directory symlinks
        item->setIcon(0, QIcon(":/icons/folder_symlink.svg"));
        directories.append(item);
      }
      else
      {
        // For file symlinks, use the appropriate file_symlink icon
        QString baseIconPath = getFileIcon(name);
        QString symlinkIconPath;

        if(baseIconPath == ":/icons/image.svg")
        {
          symlinkIconPath = ":/icons/image_symlink.svg";
        }
        else if(baseIconPath == ":/icons/archive.svg")
        {
          symlinkIconPath = ":/icons/archive_symlink.svg";
        }
        else
        {
          symlinkIconPath = ":/icons/file_symlink.svg";
        }

        item->setIcon(0, QIcon(symlinkIconPath));
        files.append(item);
      }
    }
    else if(isDir)
    {
      item->setIcon(0, QIcon(":/icons/folder.svg"));
      directories.append(item);
    }
    else
    {
      item->setIcon(0, QIcon(getFileIcon(name)));
      files.append(item);
    }
  }

  std::sort(directories.begin(), directories.end(), [](QTreeWidgetItem * a, QTreeWidgetItem * b)
  {
    return a->text(0).compare(b->text(0), Qt::CaseInsensitive) < 0;
  });

  std::sort(files.begin(), files.end(), [](QTreeWidgetItem * a, QTreeWidgetItem * b)
  {
    return a->text(0).compare(b->text(0), Qt::CaseInsensitive) < 0;
  });

  foreach(QTreeWidgetItem *item, directories)
  {
    fileList->addTopLevelItem(item);
  }
  foreach(QTreeWidgetItem *item, files)
  {
    fileList->addTopLevelItem(item);
  }
}

void PageFtpClient::processItem(QTreeWidgetItem *item, int column)
{
  Q_UNUSED(column)

  if(!item)
    return;

  QString name = item->text(0);
  if(item->text(2) == tr("Directory"))
  {
    if(name == ".")
      return;

    if(name == "..")
    {
      // 返回到上一级目录
      int lastSlash = currentPath.lastIndexOf("/");
      if(lastSlash > 0)
      {
        currentPath = currentPath.left(lastSlash);
      }
      else
      {
        currentPath = "/";
      }
    }
    else
    {
      // 检查是否是符号链接
      QString symlinkTarget = item->data(0, Qt::UserRole).toString();
      if(!symlinkTarget.isEmpty())
      {
        // 处理符号链接路径
        if(symlinkTarget.startsWith("/"))
        {
          // 绝对路径
          currentPath = symlinkTarget;
        }
        else
        {
          // 相对路径
          if(currentPath == "/")
          {
            currentPath = "/" + symlinkTarget;
          }
          else
          {
            currentPath = currentPath + "/" + symlinkTarget;
          }
        }
      }
      else
      {
        // 普通目录
        if(currentPath == "/")
        {
          currentPath += name;
        }
        else
        {
          currentPath += "/" + name;
        }
      }
    }

    // 更新当前路径显示
    currentPathLabel->setText(tr("Current Path: %1").arg(currentPath));

    listDirectory();
  }
}

void PageFtpClient::onDataSocketConnected()
{
  // 根据当前操作发送相应的命令
  switch(currentOperation)
  {
    case List:
      sendCommand("LIST " + currentPath);
      break;
    case Download:
      // 使用存储的文件名
      if(file && file->property("ftpFileName").isValid())
      {
        QString fileName = file->property("ftpFileName").toString();
        QString fullPath = currentPath + "/" + fileName;
        // 处理路径中的双斜杠
        fullPath.replace("//", "/");
        sendCommand("RETR " + fullPath);
      }
      else if(fileList->currentItem())
      {
        // 备用方案：从文件列表中获取当前选中的文件名
        QString fileName = fileList->currentItem()->text(0);
        QString fullPath = currentPath + "/" + fileName;
        // 处理路径中的双斜杠
        fullPath.replace("//", "/");
        sendCommand("RETR " + fullPath);
      }
      break;
    case Upload:
      if(file)
      {
        QFileInfo fileInfo(file->fileName());
        QString fullPath = currentPath + "/" + fileInfo.fileName();
        // 处理路径中的双斜杠
        fullPath.replace("//", "/");
        sendCommand("STOR " + fullPath);

        // 开始上传文件数据
        uploadFileData();
      }
      break;
    case None:
      break;
  }
}

void PageFtpClient::onDataSocketDisconnected()
{
  // 处理数据传输完成
  if(currentOperation == List)
  {
    // 解析目录列表
    parseFtpList(listData);
  }
  else if(currentOperation == Download || currentOperation == Upload)
  {
    if(file)
    {
      file->close();
      delete file;
      file = nullptr;
    }
    // 保存当前操作类型，用于在收到226响应后刷新目录
    FtpOperation savedOperation = currentOperation;
    currentOperation = None;

    // 设置一个标志，表示需要刷新目录
    this->setProperty("needRefresh", true);
    this->setProperty("savedOperation", static_cast<int>(savedOperation));
  }
  else
  {
    currentOperation = None;
  }
}

void PageFtpClient::onDataSocketReadyRead()
{
  QByteArray data = dataSocket->readAll();

  switch(currentOperation)
  {
    case List:
      listData.append(data);
      break;
    case Download:
      if(file)
      {
        file->write(data);
        // 更新下载进度
        bytesTransferred += data.size();
        if(totalBytes > 0)
        {
          // 计算百分比进度
          int progress = static_cast<int>((static_cast<double>(bytesTransferred) / totalBytes) * 100);
          progressBar->setRange(0, 100);
          progressBar->setValue(progress);
        }
        else
        {
          // 如果无法获取文件大小，使用不确定进度模式
          progressBar->setRange(0, 0);
          progressBar->setValue(0);
        }
      }
      break;
    case Upload:
      // 上传时，数据套接字可读表示服务器的响应
      break;
    case None:
      break;
  }
}

void PageFtpClient::onDataSocketError(QAbstractSocket::SocketError error)
{
  // 只有当错误不是远程主机关闭连接时，才记录错误并重置操作状态
  // 因为FTP服务器在传输完成后会正常关闭数据连接
  if(error != QAbstractSocket::RemoteHostClosedError)
  {
    logText->append(tr("Data socket error: %1").arg(dataSocket->errorString()));
    currentOperation = None;
  }
}

QHostAddress PageFtpClient::parsePassiveModeAddress(const QString &response)
{
  // 解析被动模式响应，格式如：227 Entering Passive Mode (192,168,1,55,111,242).
  QRegularExpression regex("\\((\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)\\)");
  QRegularExpressionMatch match = regex.match(response);

  if(match.hasMatch())
  {
    QString ip = QString("%1.%2.%3.%4").arg(match.captured(1)).arg(match.captured(2)).arg(match.captured(3)).arg(
                   match.captured(4));
    return QHostAddress(ip);
  }

  return QHostAddress();
}

quint16 PageFtpClient::parsePassiveModePort(const QString &response)
{
  // 解析被动模式响应，格式如：227 Entering Passive Mode (192,168,1,55,111,242).
  QRegularExpression regex("\\((\\d+),(\\d+),(\\d+),(\\d+),(\\d+),(\\d+)\\)");
  QRegularExpressionMatch match = regex.match(response);

  if(match.hasMatch())
  {
    int high = match.captured(5).toInt();
    int low = match.captured(6).toInt();
    return (high << 8) | low;
  }

  return 0;
}

void PageFtpClient::uploadFileData()
{
  if(!file || !dataSocket || !dataSocket->isOpen())
    return;

  // 读取文件数据并写入到数据套接字（异步方式）
  QByteArray chunk = file->read(65536); // 64KB chunks
  if(!chunk.isEmpty())
  {
    dataSocket->write(chunk);
  }
  else
  {
    // 文件已全部上传完成，关闭数据连接
    dataSocket->disconnectFromHost();
  }
}

void PageFtpClient::onDataSocketBytesWritten(qint64 bytes)
{
  if(currentOperation == Upload)
  {
    bytesTransferred += bytes;
    int progress = totalBytes > 0 ? (int)((bytesTransferred * 100) / totalBytes) : 0;
    progressBar->setValue(progress);

    if(file && dataSocket && dataSocket->isOpen())
    {
      // 继续读取并写入文件数据
      QByteArray chunk = file->read(65536); // 64KB chunks
      if(!chunk.isEmpty())
      {
        dataSocket->write(chunk);
      }
      else
      {
        // 文件已全部上传完成，关闭数据连接
        dataSocket->disconnectFromHost();
      }
    }
  }

}

// 解析文件大小字符串（如"1.2MB"、"1024B"等）
qint64 PageFtpClient::parseFileSize(const QString &sizeStr)
{
  QString str = sizeStr.trimmed();
  if(str.isEmpty())
    return 0;

  // 提取数字部分
  QString numStr;
  for(int i = 0; i < str.length(); i++)
  {
    if(str[i].isDigit() || str[i] == '.')
      numStr += str[i];
    else
      break;
  }

  double num = numStr.toDouble();
  QString unit = str.mid(numStr.length()).trimmed().toUpper();

  // 转换为字节
  if(unit == "KB" || unit == "K")
    return static_cast<qint64>(num * 1024);
  else if(unit == "MB" || unit == "M")
    return static_cast<qint64>(num * 1024 * 1024);
  else if(unit == "GB" || unit == "G")
    return static_cast<qint64>(num * 1024 * 1024 * 1024);
  else if(unit == "TB" || unit == "T")
    return static_cast<qint64>(num * 1024 * 1024 * 1024 * 1024);
  else
    return static_cast<qint64>(num); // 默认字节
}
