#include "myresource.h"

PageSshClient::PageSshClient(QWidget *parent)
  : QWidget(parent)
  , session(nullptr)
  , channel(nullptr)
  , isConnected(false)
  , dataTimer(new QTimer(this))
  , m_usernameInput(nullptr)
  , m_passwordInput(nullptr)
  , m_currentAuthState(nullptr)
  , m_authStateInitialized(nullptr)
  , contextMenu(nullptr)
  , copyAction(nullptr)
  , pasteAction(nullptr)
  , clearAction(nullptr)
{
  setupUi();
  setupConnections();
}

PageSshClient::~PageSshClient()
{
  disconnectFromHost();

  // 释放临时存储
  if(m_usernameInput)
  {
    delete m_usernameInput;
    m_usernameInput = nullptr;
  }
  if(m_passwordInput)
  {
    delete m_passwordInput;
    m_passwordInput = nullptr;
  }
  if(m_currentAuthState)
  {
    delete m_currentAuthState;
    m_currentAuthState = nullptr;
  }
  if(m_authStateInitialized)
  {
    delete m_authStateInitialized;
    m_authStateInitialized = nullptr;
  }
}

void PageSshClient::setupUi()
{
  // 设置样式表，与串口收发器保持一致
  QString styleSheet = QString(
                         "QGroupBox {\n"
                         "    font-weight: bold;\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 6px;\n"
                         "    margin-top: 12px;\n"
                         "    padding-top: 12px;\n"
                         "    background-color: white;\n"
                         "}\n"
                         "QGroupBox::title {\n"
                         "    subcontrol-origin: margin;\n"
                         "    subcontrol-position: top center;\n"
                         "    padding: 0 8px;\n"
                         "    background-color: white;\n"
                         "    color: #2c3e50;\n"
                         "}\n"
                         "QPushButton {\n"
                         "    background-color: #3498db;\n"
                         "    border: none;\n"
                         "    border-radius: 4px;\n"
                         "    color: white;\n"
                         "    padding: 8px 16px;\n"
                         "    font-weight: bold;\n"
                         "    min-height: 20px;\n"
                         "}\n"
                         "QPushButton:hover {\n"
                         "    background-color: #2980b9;\n"
                         "}\n"
                         "QPushButton:pressed {\n"
                         "    background-color: #21618c;\n"
                         "}\n"
                         "QPushButton:disabled {\n"
                         "    background-color: #bdc3c7;\n"
                         "    color: #7f8c8d;\n"
                         "}\n"
                         "QComboBox {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 6px 12px;\n"
                         "    background-color: white;\n"
                         "    selection-background-color: #3498db;\n"
                         "    selection-color: white;\n"
                         "    min-width: 120px;\n"
                         "    color: black;\n"
                         "}\n"
                         "QComboBox::drop-down {\n"
                         "    subcontrol-origin: padding;\n"
                         "    subcontrol-position: top right;\n"
                         "    width: 20px;\n"
                         "    border-left: 1px solid #c0c6d0;\n"
                         "}\n"
                         "QComboBox::down-arrow {\n"
                         "    image: none;\n"
                         "    border-left: 4px solid transparent;\n"
                         "    border-right: 4px solid transparent;\n"
                         "    border-top: 4px solid #7f8c8d;\n"
                         "    width: 0;\n"
                         "    height: 0;\n"
                         "}\n"
                         "QTextEdit {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 8px;\n"
                         "    background-color: white;\n"
                         "    font-family: 'Consolas', 'Monaco', monospace;\n"
                         "}\n"
                         "QCheckBox {\n"
                         "    spacing: 6px;\n"
                         "}\n"
                         "QCheckBox::indicator {\n"
                         "    width: 16px;\n"
                         "    height: 16px;\n"
                         "}\n"
                         "QCheckBox::indicator:unchecked {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 2px;\n"
                         "    background-color: white;\n"
                         "}\n"
                         "QCheckBox::indicator:checked {\n"
                         "    border: 1px solid #3498db;\n"
                         "    border-radius: 2px;\n"
                         "    background-color: #3498db;\n"
                         "}\n"
                         "QLabel {\n"
                         "    color: #2c3e50;\n"
                         "    font-weight: 500;\n"
                         "}\n"
                         "QLineEdit {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 6px 8px;\n"
                         "    background-color: white;\n"
                         "}\n"
                         "QLineEdit:focus {\n"
                         "    border-color: #3498db;\n"
                         "}"
                       );
  this->setStyleSheet(styleSheet);

  // 主布局 - 水平分割
  mainLayout = new QGridLayout(this);

  // 左侧连接配置组
  connectionGroup = new QGroupBox(tr("SSH Connection"));
  connectionLayout = new QGridLayout(connectionGroup);

  hostLabel = new QLabel(tr("Host:"));
  portLabel = new QLabel(tr("Port:"));
  usernameLabel = new QLabel(tr("Username:"));
  passwordLabel = new QLabel(tr("Password:"));

  hostEdit = new QLineEdit("localhost");
  portEdit = new QLineEdit("22");
  usernameEdit = new QLineEdit();
  passwordEdit = new QLineEdit();
  passwordEdit->setEchoMode(QLineEdit::Password);

  connectButton = new QPushButton(tr("Connect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #27ae60;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #219653;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #1e874b;\n"
                                 "}"
                               ));

  // 连接配置布局 - 垂直排列
  connectionLayout->addWidget(hostLabel, 0, 0);
  connectionLayout->addWidget(hostEdit, 0, 1);
  connectionLayout->addWidget(portLabel, 1, 0);
  connectionLayout->addWidget(portEdit, 1, 1);
  connectionLayout->addWidget(usernameLabel, 2, 0);
  connectionLayout->addWidget(usernameEdit, 2, 1);
  connectionLayout->addWidget(passwordLabel, 3, 0);
  connectionLayout->addWidget(passwordEdit, 3, 1);
  connectionLayout->addWidget(connectButton, 4, 0, 1, 2);  // 跨两列

  // 右侧终端组
  terminalGroup = new QGroupBox(tr("SSH Terminal"));
  terminalLayout = new QVBoxLayout(terminalGroup);

  // 创建终端部件
  terminalWidget = new QTermWidget(0, this);
  terminalWidget->setShellProgram("/bin/bash");
  terminalWidget->setArgs(QStringList());
  terminalWidget->setColorScheme("BreezeModified");
  terminalWidget->setHistorySize(1000);
  terminalWidget->setTerminalFont(QFont("Consolas", 11));
  terminalWidget->setMonitorActivity(true);
  terminalWidget->setMonitorSilence(false);
  terminalWidget->setScrollBarPosition(QTermWidget::ScrollBarRight);
  terminalWidget->setTerminalSizeHint(false);
  terminalWidget->setBlinkingCursor(true);

  // 启用终端 teletype 模式，这样可以捕获用户输入
  terminalWidget->startTerminalTeletype();

  // 设置终端的最小大小，避免左侧设置区域变大
  terminalWidget->setMinimumWidth(600);

  terminalLayout->addWidget(terminalWidget);

  // 初始化右键菜单
  setupContextMenu();

  // 状态栏
  statusBar = new QStatusBar();
  statusLabel = new QLabel(tr("Disconnected"));
  statusBar->addWidget(statusLabel);

  // 添加到主布局 - 左右分布
  mainLayout->addWidget(connectionGroup, 0, 0);
  mainLayout->addWidget(terminalGroup, 0, 1);
  mainLayout->addWidget(statusBar, 1, 0, 1, 2);

  // 设置列比例 - 左侧较窄，右侧较宽
  mainLayout->setColumnStretch(0, 1);
  mainLayout->setColumnStretch(1, 3);
  mainLayout->setRowStretch(0, 1);
  mainLayout->setRowStretch(1, 0);
}

void PageSshClient::setupConnections()
{
  connect(connectButton, &QPushButton::clicked, this, &PageSshClient::onConnectButtonClicked);
  connect(terminalWidget, &QTermWidget::sendData, this, [this](const char * data, int len)
  {
    onTerminalDataReceived(QByteArray(data, len));
  });
  connect(dataTimer, &QTimer::timeout, this, &PageSshClient::onDataReceived);

  // 连接终端的右键菜单信号
  // 通过 QTermWidget 的内部 TerminalDisplay 来获取右键菜单信号
  Konsole::TerminalDisplay* terminalDisplay = terminalWidget->findChild<Konsole::TerminalDisplay *>();
  if(terminalDisplay)
  {
    connect(terminalDisplay, &Konsole::TerminalDisplay::configureRequest, this,
            &PageSshClient::showTerminalContextMenu);
  }
}

void PageSshClient::onConnectButtonClicked()
{
  if(isConnected)
  {
    disconnectFromHost();
  }
  else
  {
    connectToHost();
  }
}

void PageSshClient::onDataReceived()
{
  if(!isConnected)
  {
    return;
  }

  // 如果有SSH通道，从通道读取数据
  if(channel)
  {
    char data[1024];
    int nbytes = ssh_channel_read_nonblocking(channel, data, sizeof(data), 0);

    if(nbytes > 0)
    {
      QString text = QString::fromUtf8(data, nbytes);
      writeToTerminal(text);
    }

    // 检查通道是否已关闭
    if(ssh_channel_is_eof(channel))
    {
      disconnectFromHost();
    }
  }
}

void PageSshClient::onTerminalDataReceived(const QByteArray &data)
{
  if(!isConnected)
  {
    return;
  }

  QString input = QString::fromUtf8(data);

  // 如果还没有创建真正的SSH通道，处理用户名和密码输入
  if(!channel)
  {
    // 使用成员变量来跟踪状态，而不是静态变量
    if(!m_usernameInput)
    {
      m_usernameInput = new QString();
    }
    if(!m_passwordInput)
    {
      m_passwordInput = new QString();
    }

    // 使用成员变量来跟踪当前状态，避免静态变量在断开重连时不重置
    if(!m_currentAuthState)
    {
      m_currentAuthState = new int(WAITING_FOR_USERNAME);
    }
    if(!m_authStateInitialized)
    {
      m_authStateInitialized = new bool(false);
    }

    // 初始化状态
    if(!(*m_authStateInitialized))
    {
      if(!usernameEdit->text().isEmpty())
      {
        *m_currentAuthState = WAITING_FOR_PASSWORD;
      }
      *m_authStateInitialized = true;
    }

    if(input == "\r" || input == "\n")
    {
      // 用户按回车键
      if(*m_currentAuthState == WAITING_FOR_USERNAME)
      {
        // 用户输入了用户名
        writeToTerminal("\r\n");
        writeToTerminal(tr("Please enter password: "));
        *m_currentAuthState = WAITING_FOR_PASSWORD;
      }
      else if(*m_currentAuthState == WAITING_FOR_PASSWORD)
      {
        // 用户输入了密码
        writeToTerminal("\r\n");

        // 直接尝试认证，不将密码设置到左侧输入框
        authenticateWithTerminalCredentials(*m_usernameInput, *m_passwordInput);

        // 清空密码输入，但保留用户名以便重新输入密码
        m_passwordInput->clear();

        // 认证失败后状态会保持在 WAITING_FOR_PASSWORD，允许重新输入密码
      }
    }
    else if(input == "\x7f" || input == "\b")
    {
      // 退格键
      if(*m_currentAuthState == WAITING_FOR_USERNAME)
      {
        m_usernameInput->chop(1);
      }
      else if(*m_currentAuthState == WAITING_FOR_PASSWORD)
      {
        m_passwordInput->chop(1);
      }
      writeToTerminal("\b \b");
    }
    else
    {
      // 普通字符输入
      if(*m_currentAuthState == WAITING_FOR_USERNAME)
      {
        *m_usernameInput += input;
        writeToTerminal(input);
      }
      else if(*m_currentAuthState == WAITING_FOR_PASSWORD)
      {
        *m_passwordInput += input;
        writeToTerminal("*"); // 密码显示为星号
      }
    }
  }
  else
  {
    // 已经创建了SSH通道，直接发送数据
    ssh_channel_write(channel, data.constData(), data.length());
  }
}



void PageSshClient::connectToHost()
{
  QString host = hostEdit->text();
  int port = portEdit->text().toInt();
  QString username = usernameEdit->text();
  QString password = passwordEdit->text();

  if(host.isEmpty())
  {
    statusLabel->setText(tr("Please fill in host"));
    return;
  }

  // 创建SSH会话
  session = ssh_new();
  if(!session)
  {
    statusLabel->setText(tr("Failed to create SSH session"));
    return;
  }

  // 设置连接选项
  ssh_options_set(session, SSH_OPTIONS_HOST, host.toUtf8().constData());
  ssh_options_set(session, SSH_OPTIONS_PORT, &port);

  // 只有在提供了用户名时才设置用户名选项
  if(!username.isEmpty())
  {
    ssh_options_set(session, SSH_OPTIONS_USER, username.toUtf8().constData());
  }

  // 连接到服务器
  int rc = ssh_connect(session);
  if(rc != SSH_OK)
  {
    statusLabel->setText(tr("Connection failed: %1").arg(ssh_get_error(session)));
    ssh_free(session);
    session = nullptr;
    return;
  }

  // 验证主机密钥
  if(!verifyKnownHosts())
  {
    ssh_disconnect(session);
    ssh_free(session);
    session = nullptr;
    return;
  }

  // 检查是否提供了用户名和密码
  if(!username.isEmpty() && !password.isEmpty())
  {
    // 如果提供了用户名和密码，直接进行认证
    authenticateWithCredentials();
  }
  else
  {
    // 否则启动交互式终端，让用户在终端中输入
    // 重置认证状态以确保正确的提示
    if(m_currentAuthState)
    {
      *m_currentAuthState = WAITING_FOR_USERNAME;
    }
    if(m_authStateInitialized)
    {
      *m_authStateInitialized = false;
    }
    startInteractiveShell();
  }
}

void PageSshClient::disconnectFromHost()
{
  if(dataTimer->isActive())
  {
    dataTimer->stop();
  }

  if(channel)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    channel = nullptr;
  }

  if(session)
  {
    ssh_disconnect(session);
    ssh_free(session);
    session = nullptr;
  }

  isConnected = false;

  // 重置认证状态，确保断开连接后重新连接时状态正确
  if(m_usernameInput)
  {
    m_usernameInput->clear();
  }
  if(m_passwordInput)
  {
    m_passwordInput->clear();
  }
  if(m_currentAuthState)
  {
    *m_currentAuthState = WAITING_FOR_USERNAME;
  }
  if(m_authStateInitialized)
  {
    *m_authStateInitialized = false;
  }

  // 更新UI状态
  connectButton->setEnabled(true);
  connectButton->setText(tr("Connect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #27ae60;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #219653;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #1e874b;\n"
                                 "}"
                               ));
  statusLabel->setText(tr("Disconnected"));

  clearTerminal();
}

void PageSshClient::authenticate()
{
  QString password = passwordEdit->text();
  int rc;

  // 首先尝试"none"认证，查看服务器支持哪些认证方法
  rc = ssh_userauth_none(session, nullptr);

  // 获取支持的认证方法
  int method = ssh_userauth_list(session, nullptr);

  // 如果密码不为空，先尝试密码认证
  if(!password.isEmpty() && (method & SSH_AUTH_METHOD_PASSWORD))
  {
    rc = ssh_userauth_password(session, nullptr, password.toUtf8().constData());
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
  }

  // 如果没有提供密码，或者密码认证失败，尝试其他方法
  // 如果支持键盘交互式认证，我们直接创建shell，让用户在终端中输入
  if(method & SSH_AUTH_METHOD_INTERACTIVE)
  {
    createRealShell();
    return;
  }

  // 如果支持公钥认证，尝试使用默认公钥
  if(method & SSH_AUTH_METHOD_PUBLICKEY)
  {
    rc = ssh_userauth_publickey_auto(session, nullptr, nullptr);
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
  }

  // 如果所有认证方法都失败，我们仍然尝试创建shell
  // 这样用户可以在终端中手动完成认证
  createRealShell();
}

void PageSshClient::authenticateWithCredentials()
{
  QString username = usernameEdit->text();
  QString password = passwordEdit->text();
  int rc;

  // 连接成功
  isConnected = true;

  // 更新UI状态
  connectButton->setEnabled(true);
  connectButton->setText(tr("Disconnect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #e74c3c;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #c0392b;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #a93226;\n"
                                 "}"
                               ));
  statusLabel->setText(tr("Connected to %1").arg(hostEdit->text()));

  // 启动数据接收定时器
  dataTimer->start(100); // 每100ms检查一次数据

  // 首先尝试"none"认证，查看服务器支持哪些认证方法
  rc = ssh_userauth_none(session, nullptr);

  // 获取支持的认证方法
  int method = ssh_userauth_list(session, nullptr);

  // 尝试密码认证
  if(method & SSH_AUTH_METHOD_PASSWORD)
  {
    rc = ssh_userauth_password(session, nullptr, password.toUtf8().constData());
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
    else
    {
      writeToTerminal(tr("Password authentication failed\r\n"));
    }
  }

  // 尝试键盘交互式认证
  if(method & SSH_AUTH_METHOD_INTERACTIVE)
  {
    rc = ssh_userauth_kbdint(session, nullptr, nullptr);
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
  }

  // 尝试公钥认证
  if(method & SSH_AUTH_METHOD_PUBLICKEY)
  {
    rc = ssh_userauth_publickey_auto(session, nullptr, nullptr);
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
  }
}

void PageSshClient::authenticateWithTerminalCredentials(const QString &username, const QString &password)
{
  int rc;

  // 首先尝试"none"认证，查看服务器支持哪些认证方法
  rc = ssh_userauth_none(session, nullptr);

  // 获取支持的认证方法
  int method = ssh_userauth_list(session, nullptr);

  // 尝试密码认证
  if(method & SSH_AUTH_METHOD_PASSWORD)
  {
    rc = ssh_userauth_password(session, nullptr, password.toUtf8().constData());
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
    else
    {
      writeToTerminal(tr("Password authentication failed\r\n"));
      writeToTerminal(tr("Please enter password: "));
      // 重置状态以允许重新输入密码
      if(m_currentAuthState)
      {
        *m_currentAuthState = WAITING_FOR_PASSWORD;
      }
      return; // 返回而不是继续尝试其他认证方法，让用户重新输入密码
    }
  }

  // 尝试键盘交互式认证
  if(method & SSH_AUTH_METHOD_INTERACTIVE)
  {
    writeToTerminal(tr("Trying keyboard-interactive authentication...\r\n"));
    rc = ssh_userauth_kbdint(session, nullptr, nullptr);
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
    else
    {
      writeToTerminal(tr("Please enter password: "));
      // 重置状态以允许重新输入密码
      if(m_currentAuthState)
      {
        *m_currentAuthState = WAITING_FOR_PASSWORD;
      }
      return;
    }
  }

  // 尝试公钥认证
  if(method & SSH_AUTH_METHOD_PUBLICKEY)
  {
    writeToTerminal(tr("Trying public key authentication...\r\n"));
    rc = ssh_userauth_publickey_auto(session, nullptr, nullptr);
    if(rc == SSH_AUTH_SUCCESS)
    {
      createRealShell();
      return;
    }
  }

  // 如果所有认证方法都失败
  writeToTerminal(tr("Please enter password: "));
  // 重置状态以允许重新输入密码
  if(m_currentAuthState)
  {
    *m_currentAuthState = WAITING_FOR_PASSWORD;
  }
}

void PageSshClient::startShell()
{
  // 连接成功
  isConnected = true;

  // 更新UI状态
  connectButton->setEnabled(true);
  connectButton->setText(tr("Disconnect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #e74c3c;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #c0392b;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #a93226;\n"
                                 "}"
                               ));
  statusLabel->setText(tr("Connected to %1").arg(hostEdit->text()));

  // 启动数据接收定时器
  dataTimer->start(100); // 每100ms检查一次数据

  // 重置认证状态
  if(!m_currentAuthState)
  {
    m_currentAuthState = new int(WAITING_FOR_USERNAME);
  }
  if(!m_authStateInitialized)
  {
    m_authStateInitialized = new bool(false);
  }
  *m_currentAuthState = WAITING_FOR_USERNAME;
  *m_authStateInitialized = false;

  // 创建一个特殊的交互式终端，用于处理SSH认证
  // 如果没有设置用户名，在终端中显示提示
  if(usernameEdit->text().isEmpty())
  {
    writeToTerminal(tr("Please enter username: "));
  }
  else if(passwordEdit->text().isEmpty())
  {
    *m_currentAuthState = WAITING_FOR_PASSWORD;
    *m_authStateInitialized = true;
    writeToTerminal(tr("Please enter password: "));
  }
  else
  {
    // 如果已经提供了用户名和密码，尝试创建真正的SSH shell
    createRealShell();
  }
}

void PageSshClient::startInteractiveShell()
{
  // 连接成功
  isConnected = true;

  // 更新UI状态
  connectButton->setEnabled(true);
  connectButton->setText(tr("Disconnect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #e74c3c;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #c0392b;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #a93226;\n"
                                 "}"
                               ));
  statusLabel->setText(tr("Connected to %1").arg(hostEdit->text()));

  // 启动数据接收定时器
  dataTimer->start(100); // 每100ms检查一次数据

  // 重置认证状态
  if(!m_currentAuthState)
  {
    m_currentAuthState = new int(WAITING_FOR_USERNAME);
  }
  if(!m_authStateInitialized)
  {
    m_authStateInitialized = new bool(false);
  }
  *m_currentAuthState = WAITING_FOR_USERNAME;
  *m_authStateInitialized = false;

  // 如果没有设置用户名，在终端中显示提示
  if(usernameEdit->text().isEmpty())
  {
    writeToTerminal(tr("Please enter username: "));
  }
  else
  {
    // 如果已经设置了用户名，直接提示输入密码
    *m_currentAuthState = WAITING_FOR_PASSWORD;
    *m_authStateInitialized = true;
    writeToTerminal(tr("Please enter password: "));
  }
}

void PageSshClient::startInteractiveShellWithPasswordPrompt()
{
  // 连接成功
  isConnected = true;

  // 更新UI状态
  connectButton->setEnabled(true);
  connectButton->setText(tr("Disconnect"));
  connectButton->setStyleSheet(QString::fromUtf8(
                                 "QPushButton {\n"
                                 "    background-color: #e74c3c;\n"
                                 "    padding: 10px;\n"
                                 "}\n"
                                 "QPushButton:hover {\n"
                                 "    background-color: #c0392b;\n"
                                 "}\n"
                                 "QPushButton:pressed {\n"
                                 "    background-color: #a93226;\n"
                                 "}"
                               ));
  statusLabel->setText(tr("Connected to %1").arg(hostEdit->text()));

  // 启动数据接收定时器
  dataTimer->start(100); // 每100ms检查一次数据

  // 重置认证状态
  if(!m_currentAuthState)
  {
    m_currentAuthState = new int(WAITING_FOR_USERNAME);
  }
  if(!m_authStateInitialized)
  {
    m_authStateInitialized = new bool(false);
  }
  *m_currentAuthState = WAITING_FOR_PASSWORD;
  *m_authStateInitialized = true;

  writeToTerminal(tr("Please enter password: "));
}

void PageSshClient::createRealShell()
{
  // 创建通道
  channel = ssh_channel_new(session);
  if(!channel)
  {
    // 不显示错误，而是尝试其他方式
    return;
  }

  // 打开会话通道
  int rc = ssh_channel_open_session(channel);
  if(rc != SSH_OK)
  {
    // 如果打开会话失败，释放通道
    ssh_channel_free(channel);
    channel = nullptr;
    return;
  }

  // 请求pty
  rc = ssh_channel_request_pty(channel);
  if(rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    channel = nullptr;
    return;
  }

  // 启动Shell
  rc = ssh_channel_request_shell(channel);
  if(rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    channel = nullptr;
    return;
  }
}

void PageSshClient::writeToTerminal(const QString &text)
{
  if(terminalWidget)
  {
    // 将接收到的数据显示在终端上
    // 使用QTermWidget的displayData方法直接写入数据
    terminalWidget->displayData(text.toUtf8());
  }
}

void PageSshClient::clearTerminal()
{
  if(terminalWidget)
  {
    // 清空终端内容
    terminalWidget->clear();
  }
}

bool PageSshClient::verifyKnownHosts()
{
  ssh_key pubkey;
  int rc;
  unsigned char * hash = nullptr;
  size_t hlen;
  char * hexa;
  char buf[10];
  int state;

  rc = ssh_get_server_publickey(session, &pubkey);
  if(rc < 0)
  {
    return false;
  }

  rc = ssh_get_publickey_hash(pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
  ssh_key_free(pubkey);
  if(rc < 0)
  {
    return false;
  }

  state = ssh_session_is_known_server(session);

  switch(state)
  {
    case SSH_KNOWN_HOSTS_OK:
      break; /* ok */
    case SSH_KNOWN_HOSTS_CHANGED:
      statusLabel->setText(tr("Host key for server changed: it is now"));
      ssh_clean_pubkey_hash(&hash);
      return false;
    case SSH_KNOWN_HOSTS_OTHER:
      statusLabel->setText(tr("The host key for this server was not found but an other type of key exists."));
      ssh_clean_pubkey_hash(&hash);
      return false;
    case SSH_KNOWN_HOSTS_UNKNOWN:
      hexa = ssh_get_hexa(hash, hlen);
      statusLabel->setText(tr("The server is unknown. Do you trust the host key?"));
      ssh_string_free_char(hexa);
      ssh_clean_pubkey_hash(&hash);
      // 在实际应用中，这里应该询问用户是否信任主机密钥
      // 为了演示，我们直接返回true
      return true;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
      statusLabel->setText(tr("Could not find known host file."));
      ssh_clean_pubkey_hash(&hash);
      return false;
    case SSH_KNOWN_HOSTS_ERROR:
      statusLabel->setText(tr("Error: %1").arg(ssh_get_error(session)));
      ssh_clean_pubkey_hash(&hash);
      return false;
  }

  ssh_clean_pubkey_hash(&hash);
  return true;
}

void PageSshClient::setupContextMenu()
{
  contextMenu = new QMenu(this);

  copyAction = new QAction(tr("Copy"), this);
  pasteAction = new QAction(tr("Paste"), this);
  clearAction = new QAction(tr("Clear"), this);

  // 设置图标
  copyAction->setIcon(QIcon::fromTheme("edit-copy", style()->standardIcon(QStyle::SP_FileIcon)));
  pasteAction->setIcon(QIcon::fromTheme("edit-paste", style()->standardIcon(QStyle::SP_FileIcon)));
  clearAction->setIcon(QIcon::fromTheme("edit-clear", style()->standardIcon(QStyle::SP_TrashIcon)));

  // 设置快捷键
  copyAction->setShortcut(QKeySequence::Copy);
  pasteAction->setShortcut(QKeySequence::Paste);

  // 添加到菜单
  contextMenu->addAction(copyAction);
  contextMenu->addAction(pasteAction);
  contextMenu->addSeparator();
  contextMenu->addAction(clearAction);

  // 连接信号
  connect(copyAction, &QAction::triggered, terminalWidget, &QTermWidget::copyClipboard);
  connect(pasteAction, &QAction::triggered, terminalWidget, &QTermWidget::pasteClipboard);
  connect(clearAction, &QAction::triggered, terminalWidget, &QTermWidget::clear);
}

void PageSshClient::showTerminalContextMenu(const QPoint& pos)
{
  // 获取全局坐标
  QPoint globalPos = terminalWidget->mapToGlobal(pos);

  // 显示右键菜单
  contextMenu->popup(globalPos);
}