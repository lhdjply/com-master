#include "myresource.h"
#include "pagenetworktool.h"
#include "pageverifiydialog.h"

PageNetworkTool::PageNetworkTool(QWidget *parent)
    : QWidget(parent)
    , tcpServer(nullptr)
    , tcpSocket(nullptr)
    , udpSocket(nullptr)
    , tcpServerRunning(false)
    , tcpClientConnected(false)
    , udpServerRunning(false)
    , tcpAutoSendTimer(new QTimer(this))
    , tcpServerAutoSendTimer(new QTimer(this))
    , udpAutoSendTimer(new QTimer(this))
{
    setupUi();
    
    connect(tcpAutoSendTimer, &QTimer::timeout, this, &PageNetworkTool::tcpClientSendData);
    connect(tcpServerAutoSendTimer, &QTimer::timeout, this, &PageNetworkTool::tcpSendBroadcast);
    connect(udpAutoSendTimer, &QTimer::timeout, this, &PageNetworkTool::udpSendData);
    connect(protocolCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PageNetworkTool::updateProtocolSelection);
    
    // Initially show TCP Server
    updateProtocolSelection();
}

void PageNetworkTool::setupUi()
{
    // Apply styles similar to serial transceiver
    QString styleSheet = QString::fromUtf8(
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
                         "    color: white;\n"
                         "    border: none;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 8px 16px;\n"
                         "    font-weight: bold;\n"
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
                         "    padding: 6px 8px;\n"
                         "    background-color: white;\n"
                         "    selection-background-color: #3498db;\n"
                         "    selection-color: white;\n"
                         "    min-width: 80px;\n"
                         "    color: black;\n"
                         "}\n"
                         "QComboBox::drop-down {\n"
                         "    subcontrol-origin: padding;\n"
                         "    subcontrol-position: top right;\n"
                         "    width: 15px;\n"
                         "    border-left-width: 1px;\n"
                         "    border-left-color: #c0c6d0;\n"
                         "    border-left-style: solid;\n"
                         "    border-top-right-radius: 4px;\n"
                         "    border-bottom-right-radius: 4px;\n"
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
                         "QListWidget {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    background-color: white;\n"
                         "    padding: 4px;\n"
                         "}\n"
                         "QListWidget::item {\n"
                         "    padding: 4px;\n"
                         "    border-radius: 2px;\n"
                         "}\n"
                         "QListWidget::item:selected {\n"
                         "    background-color: #3498db;\n"
                         "    color: white;\n"
                         "}"
                       );
    this->setStyleSheet(styleSheet);
    
    mainLayout = new QGridLayout(this);
    mainLayout->setSpacing(15);
    
    // Create main horizontal layout for left/right panels
    QHBoxLayout *mainHorizontalLayout = new QHBoxLayout();
    mainHorizontalLayout->setSpacing(20);
    
    // Left panel - Configuration (similar to serial transceiver frame)
    leftFrame = new QFrame();
    leftFrame->setMinimumSize(QSize(280, 0));
    leftFrame->setMaximumSize(QSize(280, 16777215));
    
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftFrame);
    leftPanelLayout->setSpacing(15);
    
    // Protocol selection at top of left panel
    QHBoxLayout *protocolLayout = new QHBoxLayout();
    protocolLayout->setSpacing(10);
    
    protocolCombo = new QComboBox();
    protocolCombo->addItem(tr("TCP Server"), 0);
    protocolCombo->addItem(tr("TCP Client"), 1);
    protocolCombo->addItem(tr("UDP"), 2);
    protocolCombo->setMinimumSize(QSize(150, 0));
    
    protocolLayout->addWidget(new QLabel(tr("Protocol:")));
    protocolLayout->addWidget(protocolCombo);
    protocolLayout->addStretch();
    
    leftPanelLayout->addLayout(protocolLayout);
    
    // TCP Server configuration
    tcpServerGroup = new QGroupBox(tr("TCP Server Configuration"));
    QVBoxLayout *tcpServerLayout = new QVBoxLayout(tcpServerGroup);
    tcpServerLayout->setSpacing(12);
    
    tcpServerPortLabel = new QLabel(tr("Port:"));
    tcpServerPortLabel->setMinimumSize(QSize(60, 0));
    tcpServerPortEdit = new QLineEdit("8080");
    tcpServerPortEdit->setMinimumSize(QSize(146, 0));
    tcpStartButton = new QPushButton(tr("Start"));
    tcpStartButton->setMinimumSize(QSize(70, 36));
    tcpStartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; } QPushButton:disabled { background-color: #CCCCCC; color: #666666; }");
    tcpStopButton = new QPushButton(tr("Stop"));
    tcpStopButton->setMinimumSize(QSize(70, 36));
    tcpStopButton->setStyleSheet("QPushButton { background-color: #F44336; color: white; } QPushButton:disabled { background-color: #CCCCCC; color: #666666; }");
    tcpStopButton->setEnabled(false);
    tcpStatusLabel = new QLabel(tr("Status: Stopped"));
    
    QHBoxLayout *tcpServerPortLayout = new QHBoxLayout();
    tcpServerPortLayout->addWidget(tcpServerPortLabel);
    tcpServerPortLayout->addWidget(tcpServerPortEdit);
    tcpServerLayout->addLayout(tcpServerPortLayout);
    
    QHBoxLayout *tcpServerButtonLayout = new QHBoxLayout();
    tcpServerButtonLayout->addWidget(tcpStartButton);
    tcpServerButtonLayout->addWidget(tcpStopButton);
    tcpServerButtonLayout->addStretch();
    tcpServerLayout->addLayout(tcpServerButtonLayout);
    
    tcpServerLayout->addWidget(tcpStatusLabel);
    
    // TCP Client list
    tcpClientList = new QListWidget();
    tcpClientList->setMaximumHeight(60);
    
    // TCP Server send buttons - declare but don't create yet
    tcpSendBroadcastButton = nullptr;
    tcpSendToClientButton = nullptr;
    
    tcpServerLayout->addWidget(new QLabel(tr("Clients:")));
    tcpServerLayout->addWidget(tcpClientList);
    
    // TCP Client configuration
    tcpClientGroup = new QGroupBox(tr("TCP Client Configuration"));
    QVBoxLayout *tcpClientLayout = new QVBoxLayout(tcpClientGroup);
    tcpClientLayout->setSpacing(12);
    
    tcpClientIpLabel = new QLabel(tr("Server IP:"));
    tcpClientIpLabel->setMinimumSize(QSize(60, 0));
    tcpClientIpEdit = new QLineEdit("127.0.0.1");
    tcpClientIpEdit->setMinimumSize(QSize(146, 0));
    tcpClientPortLabel = new QLabel(tr("Port:"));
    tcpClientPortLabel->setMinimumSize(QSize(60, 0));
    tcpClientPortEdit = new QLineEdit("8080");
    tcpClientPortEdit->setMinimumSize(QSize(146, 0));
    tcpConnectButton = new QPushButton(tr("Connect"));
    tcpConnectButton->setMinimumSize(QSize(70, 36));
    tcpDisconnectButton = new QPushButton(tr("Disconnect"));
    tcpDisconnectButton->setMinimumSize(QSize(70, 36));
    tcpDisconnectButton->setEnabled(false);
    tcpClientStatusLabel = new QLabel(tr("Status: Disconnected"));
    
    QHBoxLayout *tcpClientIpLayout = new QHBoxLayout();
    tcpClientIpLayout->addWidget(tcpClientIpLabel);
    tcpClientIpLayout->addWidget(tcpClientIpEdit);
    tcpClientLayout->addLayout(tcpClientIpLayout);
    
    QHBoxLayout *tcpClientPortLayout = new QHBoxLayout();
    tcpClientPortLayout->addWidget(tcpClientPortLabel);
    tcpClientPortLayout->addWidget(tcpClientPortEdit);
    tcpClientLayout->addLayout(tcpClientPortLayout);
    
    QHBoxLayout *tcpClientButtonLayout = new QHBoxLayout();
    tcpClientButtonLayout->addWidget(tcpConnectButton);
    tcpClientButtonLayout->addWidget(tcpDisconnectButton);
    tcpClientButtonLayout->addStretch();
    tcpClientLayout->addLayout(tcpClientButtonLayout);
    
    tcpClientLayout->addWidget(tcpClientStatusLabel);
    
    // Declare TCP client send button
    tcpClientSendButton = nullptr;
    
    // UDP configuration
    udpGroup = new QGroupBox(tr("UDP Configuration"));
    QVBoxLayout *udpLayout = new QVBoxLayout(udpGroup);
    udpLayout->setSpacing(12);
    
    // UDP Local settings
    udpServerPortLabel = new QLabel(tr("Local Port:"));
    udpServerPortLabel->setMinimumSize(QSize(60, 0));
    udpServerPortEdit = new QLineEdit("8080");
    udpServerPortEdit->setMinimumSize(QSize(146, 0));
    udpStartButton = new QPushButton(tr("Start"));
    udpStartButton->setMinimumSize(QSize(70, 36));
    udpStartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; } QPushButton:disabled { background-color: #CCCCCC; color: #666666; }");
    udpStopButton = new QPushButton(tr("Stop"));
    udpStopButton->setMinimumSize(QSize(70, 36));
    udpStopButton->setStyleSheet("QPushButton { background-color: #F44336; color: white; } QPushButton:disabled { background-color: #CCCCCC; color: #666666; }");
    udpStopButton->setEnabled(false);
    udpStatusLabel = new QLabel(tr("Status: Stopped"));
    
    QHBoxLayout *udpServerPortLayout = new QHBoxLayout();
    udpServerPortLayout->addWidget(udpServerPortLabel);
    udpServerPortLayout->addWidget(udpServerPortEdit);
    udpLayout->addLayout(udpServerPortLayout);
    
    QHBoxLayout *udpServerButtonLayout = new QHBoxLayout();
    udpServerButtonLayout->addWidget(udpStartButton);
    udpServerButtonLayout->addWidget(udpStopButton);
    udpServerButtonLayout->addStretch();
    udpLayout->addLayout(udpServerButtonLayout);
    
    udpLayout->addWidget(udpStatusLabel);
    
    // UDP Remote settings
    udpTargetIpLabel = new QLabel(tr("Remote IP:"));
    udpTargetIpLabel->setMinimumSize(QSize(60, 0));
    udpTargetIpEdit = new QLineEdit("127.0.0.1");
    udpTargetIpEdit->setMinimumSize(QSize(146, 0));
    udpTargetPortLabel = new QLabel(tr("Port:"));
    udpTargetPortLabel->setMinimumSize(QSize(60, 0));
    udpTargetPortEdit = new QLineEdit("8080");
    udpTargetPortEdit->setMinimumSize(QSize(146, 0));
    
    QHBoxLayout *udpTargetIpLayout = new QHBoxLayout();
    udpTargetIpLayout->addWidget(udpTargetIpLabel);
    udpTargetIpLayout->addWidget(udpTargetIpEdit);
    udpLayout->addLayout(udpTargetIpLayout);
    
    QHBoxLayout *udpTargetPortLayout = new QHBoxLayout();
    udpTargetPortLayout->addWidget(udpTargetPortLabel);
    udpTargetPortLayout->addWidget(udpTargetPortEdit);
    udpLayout->addLayout(udpTargetPortLayout);
    
    // Declare UDP send button
    udpSendButton = nullptr;
    
    // UDP Recent endpoints
    udpClientList = new QListWidget();
    udpClientList->setMaximumHeight(60);
    udpLayout->addWidget(new QLabel(tr("Recent Endpoints:")));
    udpLayout->addWidget(udpClientList);
    
    // Add configuration groups to left panel
    leftPanelLayout->addWidget(tcpServerGroup);
    leftPanelLayout->addWidget(tcpClientGroup);
    leftPanelLayout->addWidget(udpGroup);
    
    // Add log window at bottom left
    QGroupBox *logGroup = new QGroupBox(tr("Log"));
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logLayout->setSpacing(5);
    
    receiveEdit = new QTextEdit();
    receiveEdit->setReadOnly(true);
    receiveEdit->setStyleSheet("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;");
    
    QHBoxLayout *logButtonLayout = new QHBoxLayout();
    logButtonLayout->setSpacing(10);
    clearButton = new QPushButton(tr("Clear"));
    clearButton->setMinimumSize(QSize(80, 30));
    saveButton = new QPushButton(tr("Save"));
    saveButton->setMinimumSize(QSize(80, 30));
    logButtonLayout->addWidget(clearButton);
    logButtonLayout->addWidget(saveButton);
    logButtonLayout->addStretch();
    
    logLayout->addWidget(receiveEdit);
    logLayout->addLayout(logButtonLayout);
    
    leftPanelLayout->addWidget(logGroup);
    
    // Right panel - Send and Receive
    QVBoxLayout *rightPanelLayout = new QVBoxLayout();
    rightPanelLayout->setSpacing(15);
    
    // Send data group
    sendGroup = new QGroupBox(tr("Send Data"));
    sendGroup->setMaximumSize(QSize(16777215, 230));
    sendLayout = new QVBoxLayout(sendGroup);
    
    // Initialize hex checkbox (similar to serial transceiver)
    sendHexCheckbox = new QCheckBox(tr("Hex"));
    
    sendEdit = new QTextEdit();
    sendEdit->setStyleSheet("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;");
    
    // Create send buttons layout
    QHBoxLayout *sendButtonLayout = new QHBoxLayout();
    sendButtonLayout->setSpacing(10);
    
    // Create all send buttons
    tcpSendBroadcastButton = new QPushButton(tr("Broadcast"));
    tcpSendBroadcastButton->setMinimumSize(QSize(80, 30));
    tcpSendToClientButton = new QPushButton(tr("Send to Selected"));
    tcpSendToClientButton->setMinimumSize(QSize(80, 30));
    tcpSendToClientButton->setEnabled(false);
    tcpClientSendButton = new QPushButton(tr("Send"));
    tcpClientSendButton->setMinimumSize(QSize(80, 30));
    tcpClientSendButton->setEnabled(false);
    udpSendButton = new QPushButton(tr("Send"));
    udpSendButton->setMinimumSize(QSize(80, 30));
    
    // TCP Client auto send controls
    tcpAutoSendCheck = new QCheckBox(tr("Auto Send"));
    
    // Auto send frame (similar to serial transceiver)
    autoSendFrame = new QFrame();
    autoSendFrame->setMaximumSize(QSize(200, 50));
    autoSendFrameLayout = new QHBoxLayout(autoSendFrame);
    autoSendFrameLayout->setSpacing(5);
    autoSendFrameLayout->setContentsMargins(0, 0, 0, 0);
    
    autoSendLabel = new QLabel(tr("Send duty:"));
    autoSendIntervalEdit = new QLineEdit("1000");
    autoSendIntervalEdit->setMaximumSize(QSize(60, 16777215));
    autoSendIntervalEdit->setAlignment(Qt::AlignCenter);
    autoSendIntervalEdit->setEnabled(false);
    autoSendMsLabel = new QLabel("ms");
    
    autoSendFrameLayout->addWidget(autoSendLabel);
    autoSendFrameLayout->addWidget(autoSendIntervalEdit);
    autoSendFrameLayout->addWidget(autoSendMsLabel);
    
    // TCP Server auto send controls
    tcpServerAutoSendCheck = new QCheckBox(tr("Auto Send"));
    
    // UDP auto send controls
    udpAutoSendCheck = new QCheckBox(tr("Auto Send"));
    
    // Create verify button
    verifyButton = new QPushButton(tr("Verification"));
    verifyButton->setMinimumSize(QSize(130, 36));
    
    // Add all buttons to layout
    sendButtonLayout->addWidget(tcpSendBroadcastButton);
    sendButtonLayout->addWidget(tcpSendToClientButton);
    sendButtonLayout->addWidget(tcpClientSendButton);
    sendButtonLayout->addWidget(udpSendButton);
    sendButtonLayout->addWidget(verifyButton);
    sendButtonLayout->addWidget(tcpAutoSendCheck);
    sendButtonLayout->addWidget(tcpServerAutoSendCheck);
    sendButtonLayout->addWidget(udpAutoSendCheck);
    sendButtonLayout->addWidget(sendHexCheckbox);
    sendButtonLayout->addWidget(autoSendFrame);
    sendButtonLayout->addStretch();
    
    QPushButton *clearSendButton = new QPushButton(tr("Clear Send"));
    clearSendButton->setMinimumSize(QSize(80, 30));
    sendButtonLayout->addWidget(clearSendButton);
    sendButtonLayout->addStretch();
    
    sendLayout->addWidget(sendEdit);
    sendLayout->addLayout(sendButtonLayout);
    
    // Receive data group
    receiveGroup = new QGroupBox(tr("Received Data"));
    receiveGroup->setMinimumSize(QSize(0, 320));
    receiveLayout = new QVBoxLayout(receiveGroup);
    receiveLayout->setSpacing(10);
    
    receiveDataEdit = new QTextEdit();
    receiveDataEdit->setReadOnly(true);
    receiveDataEdit->setStyleSheet("font-family: 'Consolas', 'Monaco', monospace; font-size: 12px;");
    
    // Receive options similar to serial transceiver - all in one layout
    QHBoxLayout *receiveOptionsLayout = new QHBoxLayout();
    receiveOptionsLayout->setSpacing(15);
    
    receiveHexCheckbox = new QCheckBox(tr("Hex"));
    displaySendCheckbox = new QCheckBox(tr("Display send"));
    displayTimeCheckbox = new QCheckBox(tr("Display time"));
    frameLineBreakCheckbox = new QCheckBox(tr("Frame line break"));
    QPushButton *clearReceiveButton = new QPushButton(tr("Clear Receive"));
    
    receiveOptionsLayout->addWidget(receiveHexCheckbox);
    receiveOptionsLayout->addWidget(displaySendCheckbox);
    receiveOptionsLayout->addWidget(displayTimeCheckbox);
    receiveOptionsLayout->addWidget(frameLineBreakCheckbox);
    receiveOptionsLayout->addWidget(clearReceiveButton);
    receiveOptionsLayout->addStretch();
    
    receiveLayout->addWidget(receiveDataEdit);
    receiveLayout->addLayout(receiveOptionsLayout);
    
    // Add receive and send groups to right panel (reversed order)
    rightPanelLayout->addWidget(receiveGroup);
    rightPanelLayout->addWidget(sendGroup);
    
    // Add left and right panels to main horizontal layout
    mainHorizontalLayout->addWidget(leftFrame);
    mainHorizontalLayout->addLayout(rightPanelLayout);
    
    // Add all groups to main layout
    mainLayout->addLayout(mainHorizontalLayout, 0, 0);
    
    // Initially hide all protocol groups
    tcpServerGroup->hide();
    tcpClientGroup->hide();
    udpGroup->hide();
    
    // Initially set protocol to TCP Server (index 0)
    protocolCombo->setCurrentIndex(0);
    
    // Use QTimer to ensure size adjustment after UI is fully constructed
    QTimer::singleShot(0, this, [this]() {
        updateProtocolSelection();
    });
    
    // Connect signals and slots
    // TCP Server
    connect(tcpStartButton, &QPushButton::clicked, this, &PageNetworkTool::startTcpServer);
    connect(tcpStopButton, &QPushButton::clicked, this, &PageNetworkTool::stopTcpServer);
    connect(tcpSendBroadcastButton, &QPushButton::clicked, this, &PageNetworkTool::tcpSendBroadcast);
    connect(tcpSendToClientButton, &QPushButton::clicked, this, &PageNetworkTool::tcpSendToClient);
    connect(tcpClientList, &QListWidget::itemSelectionChanged, [this]() {
        tcpSendToClientButton->setEnabled(tcpClientList->selectedItems().count() > 0);
    });
    
    // TCP Client
    connect(tcpConnectButton, &QPushButton::clicked, this, &PageNetworkTool::connectTcpClient);
    connect(tcpDisconnectButton, &QPushButton::clicked, this, &PageNetworkTool::disconnectTcpClient);
    connect(tcpClientSendButton, &QPushButton::clicked, this, &PageNetworkTool::tcpClientSendData);
    connect(verifyButton, &QPushButton::clicked, this, &PageNetworkTool::verifySendData);
    connect(tcpAutoSendCheck, &QCheckBox::toggled, [this](bool checked) {
        autoSendIntervalEdit->setEnabled(checked && protocolCombo->currentData().toInt() == 1);
        if (checked && tcpClientConnected) {
            bool ok;
            int interval = autoSendIntervalEdit->text().toInt(&ok);
            if (ok && interval > 0) {
                tcpAutoSendTimer->start(interval);
            }
        } else {
            tcpAutoSendTimer->stop();
        }
    });
    
    // TCP Server auto send
    connect(tcpServerAutoSendCheck, &QCheckBox::toggled, [this](bool checked) {
        autoSendIntervalEdit->setEnabled(checked && protocolCombo->currentData().toInt() == 0);
        if (checked && tcpServerRunning && !tcpClients.isEmpty()) {
            bool ok;
            int interval = autoSendIntervalEdit->text().toInt(&ok);
            if (ok && interval > 0) {
                tcpServerAutoSendTimer->start(interval);
            }
        } else {
            tcpServerAutoSendTimer->stop();
        }
    });
    
    // UDP auto send
    connect(udpAutoSendCheck, &QCheckBox::toggled, [this](bool checked) {
        autoSendIntervalEdit->setEnabled(checked && protocolCombo->currentData().toInt() == 2);
        if (checked && udpServerRunning) {
            bool ok;
            int interval = autoSendIntervalEdit->text().toInt(&ok);
            if (ok && interval > 0) {
                udpAutoSendTimer->start(interval);
            }
        } else {
            udpAutoSendTimer->stop();
        }
    });
    
    // Auto send interval text changed
    connect(autoSendIntervalEdit, &QLineEdit::textChanged, [this]() {
        int index = protocolCombo->currentData().toInt();
        bool ok;
        int interval = autoSendIntervalEdit->text().toInt(&ok);
        if (!ok || interval <= 0) return;
        
        if (index == 0 && tcpServerAutoSendCheck->isChecked() && tcpServerRunning && !tcpClients.isEmpty()) {
            tcpServerAutoSendTimer->setInterval(interval);
        } else if (index == 1 && tcpAutoSendCheck->isChecked() && tcpClientConnected) {
            tcpAutoSendTimer->setInterval(interval);
        } else if (index == 2 && udpAutoSendCheck->isChecked() && udpServerRunning) {
            udpAutoSendTimer->setInterval(interval);
        }
    });
    
    // UDP
    connect(udpStartButton, &QPushButton::clicked, this, &PageNetworkTool::startUdpServer);
    connect(udpStopButton, &QPushButton::clicked, this, &PageNetworkTool::stopUdpServer);
    connect(udpSendButton, &QPushButton::clicked, this, &PageNetworkTool::udpSendData);
    
    // Common
    connect(clearButton, &QPushButton::clicked, this, &PageNetworkTool::clearLog);
    connect(saveButton, &QPushButton::clicked, this, &PageNetworkTool::saveLog);
    connect(clearSendButton, &QPushButton::clicked, [this]() {
        sendEdit->clear();
    });
    connect(clearReceiveButton, &QPushButton::clicked, [this]() {
        receiveDataEdit->clear();
    });
    
    // Connect receive options
    connect(receiveHexCheckbox, &QCheckBox::toggled, [this](bool checked) {
        // Update display format based on hex setting
    });
    connect(displaySendCheckbox, &QCheckBox::toggled, [this](bool checked) {
        // Update send display setting
    });
    connect(displayTimeCheckbox, &QCheckBox::toggled, [this](bool checked) {
        // Update time display setting
    });
    connect(frameLineBreakCheckbox, &QCheckBox::toggled, [this](bool checked) {
        // Update frame line break setting
    });
}

void PageNetworkTool::updateProtocolSelection()
{
    int index = protocolCombo->currentData().toInt();
    
    tcpServerGroup->setVisible(index == 0);
    tcpClientGroup->setVisible(index == 1);
    udpGroup->setVisible(index == 2);
    
    // Update send buttons visibility
    tcpSendBroadcastButton->setVisible(index == 0);
    tcpSendToClientButton->setVisible(index == 0);
    tcpClientSendButton->setVisible(index == 1);
    udpSendButton->setVisible(index == 2);
    
    // Update auto send controls visibility
    bool showAutoSend = (index >= 0 && index <= 2);
    tcpAutoSendCheck->setVisible(index == 1);
    tcpServerAutoSendCheck->setVisible(index == 0);
    udpAutoSendCheck->setVisible(index == 2);
    autoSendFrame->setVisible(showAutoSend);
    
    // Update auto send interval edit enabled state
    if (showAutoSend) {
        bool enabled = false;
        if (index == 0) {
            enabled = tcpServerAutoSendCheck->isChecked();
        } else if (index == 1) {
            enabled = tcpAutoSendCheck->isChecked();
        } else if (index == 2) {
            enabled = udpAutoSendCheck->isChecked();
        }
        autoSendIntervalEdit->setEnabled(enabled);
    }
    
    // Adjust left frame size based on visible content
    adjustLeftFrameSize();
}

void PageNetworkTool::adjustLeftFrameSize()
{
    int minHeight = 200; // minimum height
    int protocolHeight = 40; // protocol selection height
    int configHeight = 0;
    int logHeight = 200; // log window height
    int spacing = 15; // spacing between sections
    
    // Calculate height based on visible configuration group
    if (tcpServerGroup->isVisible()) {
        configHeight = tcpServerGroup->sizeHint().height();
    } else if (tcpClientGroup->isVisible()) {
        configHeight = tcpClientGroup->sizeHint().height();
    } else if (udpGroup->isVisible()) {
        configHeight = udpGroup->sizeHint().height();
    }
    
    int totalHeight = protocolHeight + configHeight + spacing + logHeight + spacing;
    if (totalHeight < minHeight) totalHeight = minHeight;
    
    leftFrame->setMinimumHeight(totalHeight);
}

// TCP Server methods
void PageNetworkTool::startTcpServer()
{
    if (tcpServerRunning) return;
    
    tcpServer = new QTcpServer(this);
    
    quint16 port = tcpServerPortEdit->text().toUShort();
    
    if (tcpServer->listen(QHostAddress::Any, port)) {
        tcpServerRunning = true;
        tcpStartButton->setEnabled(false);
        tcpStopButton->setEnabled(true);
        tcpServerPortEdit->setEnabled(false);
        tcpStatusLabel->setText(tr("Status: Running on port %1").arg(tcpServer->serverPort()));
        logMessage(tr("TCP Server started on port %1").arg(tcpServer->serverPort()));
        
        connect(tcpServer, &QTcpServer::newConnection, this, &PageNetworkTool::newTcpClientConnected);
    } else {
        logMessage(tr("Failed to start server: %1").arg(tcpServer->errorString()));
        delete tcpServer;
        tcpServer = nullptr;
    }
}

void PageNetworkTool::stopTcpServer()
{
    if (!tcpServerRunning) return;
    
    for (QTcpSocket *client : tcpClients) {
        client->disconnectFromHost();
    }
    tcpClients.clear();
    updateTcpClientList();
    
    if (tcpServer) {
        tcpServer->close();
        delete tcpServer;
        tcpServer = nullptr;
    }
    
    tcpServerRunning = false;
    tcpServerAutoSendTimer->stop();
    tcpStartButton->setEnabled(true);
    tcpStopButton->setEnabled(false);
    tcpServerPortEdit->setEnabled(true);
    tcpStatusLabel->setText(tr("Status: Stopped"));
    logMessage(tr("TCP Server stopped"));
}

void PageNetworkTool::newTcpClientConnected()
{
    while (tcpServer->hasPendingConnections()) {
        QTcpSocket *client = tcpServer->nextPendingConnection();
        tcpClients.append(client);
        
        connect(client, &QTcpSocket::disconnected, this, &PageNetworkTool::tcpClientDisconnected);
        connect(client, &QTcpSocket::readyRead, this, &PageNetworkTool::readTcpClientData);
        
        updateTcpClientList();
        logMessage(tr("New TCP client connected: %1:%2").arg(formatAddress(client->peerAddress())).arg(client->peerPort()));
        
        // Start auto send if enabled and this is the first client
        if (tcpServerAutoSendCheck->isChecked() && tcpClients.size() == 1) {
            bool ok;
            int interval = autoSendIntervalEdit->text().toInt(&ok);
            if (ok && interval > 0) {
                tcpServerAutoSendTimer->start(interval);
            }
        }
    }
}

void PageNetworkTool::tcpClientDisconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        logMessage(tr("TCP client disconnected: %1:%2").arg(formatAddress(client->peerAddress())).arg(client->peerPort()));
        tcpClients.removeOne(client);
        client->deleteLater();
        updateTcpClientList();
        
        // Stop auto send if no more clients
        if (tcpClients.isEmpty()) {
            tcpServerAutoSendTimer->stop();
        }
    }
}

void PageNetworkTool::readTcpClientData()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        QByteArray data = client->readAll();
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString message;
        
        // Check receive hex setting instead of send hex setting
        bool isReceiveHex = receiveHexCheckbox && receiveHexCheckbox->isChecked();
        if (isReceiveHex) {
            message = data.toHex(' ');
            // Add space between frames if frame line break is not enabled
            if (!frameLineBreakCheckbox->isChecked() && !message.isEmpty()) {
                message += ' ';
            }
        } else {
            message = QString::fromUtf8(data);
        }
        
        appendReceiveData(message);
    }
}

void PageNetworkTool::tcpSendBroadcast()
{
    QString data = sendEdit->toPlainText();
    if (data.isEmpty()) return;
    
    QByteArray sendData = prepareSendData(data, sendHexCheckbox->isChecked());
    if (sendData.isEmpty()) return;
    
    int sentCount = 0;
    for (QTcpSocket *client : tcpClients) {
        if (client->write(sendData) > 0) {
            sentCount++;
        }
    }
    
    logMessage(tr("TCP Broadcast sent to %1 clients").arg(sentCount));
}

void PageNetworkTool::tcpSendToClient()
{
    QList<QListWidgetItem*> selectedItems = tcpClientList->selectedItems();
    if (selectedItems.isEmpty()) return;
    
    QString data = sendEdit->toPlainText();
    if (data.isEmpty()) return;
    
    QByteArray sendData = prepareSendData(data, sendHexCheckbox->isChecked());
    if (sendData.isEmpty()) return;
    
    int sentCount = 0;
    for (QListWidgetItem *item : selectedItems) {
        int index = tcpClientList->row(item);
        if (index >= 0 && index < tcpClients.size()) {
            QTcpSocket *client = tcpClients.at(index);
            if (client->write(sendData) > 0) {
                sentCount++;
            }
        }
    }
    
    logMessage(tr("TCP Data sent to %1 selected clients").arg(sentCount));
}

// TCP Client methods
void PageNetworkTool::connectTcpClient()
{
    if (tcpClientConnected) return;
    
    tcpSocket = new QTcpSocket(this);
    
    QString serverIp = tcpClientIpEdit->text();
    quint16 serverPort = tcpClientPortEdit->text().toUShort();
    
    connect(tcpSocket, &QTcpSocket::connected, this, &PageNetworkTool::tcpSocketConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &PageNetworkTool::tcpSocketDisconnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &PageNetworkTool::tcpSocketReadyRead);
    connect(tcpSocket, &QAbstractSocket::errorOccurred,
            this, &PageNetworkTool::tcpSocketError);
    
    logMessage(tr("Connecting to TCP server %1:%2...").arg(serverIp).arg(serverPort));
    tcpSocket->connectToHost(serverIp, serverPort);
}

void PageNetworkTool::disconnectTcpClient()
{
    if (!tcpClientConnected) return;
    
    tcpSocket->disconnectFromHost();
}

void PageNetworkTool::tcpSocketConnected()
{
    tcpClientConnected = true;
    updateConnectionStatus();
    logMessage(tr("Connected to TCP server"));
    
    if (tcpAutoSendCheck->isChecked()) {
        bool ok;
        int interval = autoSendIntervalEdit->text().toInt(&ok);
        if (ok && interval > 0) {
            tcpAutoSendTimer->start(interval);
        }
    }
}

void PageNetworkTool::tcpSocketDisconnected()
{
    tcpClientConnected = false;
    tcpAutoSendTimer->stop();
    updateConnectionStatus();
    logMessage(tr("Disconnected from TCP server"));
    
    if (tcpSocket) {
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
}

void PageNetworkTool::tcpSocketReadyRead()
{
    QByteArray data = tcpSocket->readAll();
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString message;
    
    // Check receive hex setting instead of send hex setting
    bool isReceiveHex = receiveHexCheckbox && receiveHexCheckbox->isChecked();
    if (isReceiveHex) {
        message = data.toHex(' ');
        // Add space between frames if frame line break is not enabled
        if (!frameLineBreakCheckbox->isChecked() && !message.isEmpty()) {
            message += ' ';
        }
    } else {
        message = QString::fromUtf8(data);
    }
    
    appendReceiveData(message);
}

void PageNetworkTool::tcpSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    logMessage(tr("TCP Socket error: %1").arg(tcpSocket->errorString()));
    
    if (tcpClientConnected) {
        tcpClientConnected = false;
        tcpAutoSendTimer->stop();
        updateConnectionStatus();
    }
}

void PageNetworkTool::tcpClientSendData()
{
    if (!tcpClientConnected) return;
    
    QString data = sendEdit->toPlainText();
    if (data.isEmpty()) return;
    
    QByteArray sendData = prepareSendData(data, sendHexCheckbox->isChecked());
    if (sendData.isEmpty()) return;
    
    qint64 bytesWritten = tcpSocket->write(sendData);
    if (bytesWritten > 0) {
        logMessage(tr("TCP Sent %1 bytes").arg(bytesWritten));
    } else {
        logMessage(tr("Failed to send TCP data"));
    }
}

// UDP methods
void PageNetworkTool::startUdpServer()
{
    if (udpServerRunning) return;
    
    udpSocket = new QUdpSocket(this);
    
    quint16 port = udpServerPortEdit->text().toUShort();
    
    if (udpSocket->bind(QHostAddress::Any, port)) {
        udpServerRunning = true;
        udpStartButton->setEnabled(false);
        udpStopButton->setEnabled(true);
        udpServerPortEdit->setEnabled(false);
        udpStatusLabel->setText(tr("Status: Listening on port %1").arg(udpSocket->localPort()));
        logMessage(tr("UDP started on port %1").arg(udpSocket->localPort()));
        
        connect(udpSocket, &QUdpSocket::readyRead, this, &PageNetworkTool::readPendingDatagrams);
        
        // Start auto send if enabled
        if (udpAutoSendCheck->isChecked()) {
            bool ok;
            int interval = autoSendIntervalEdit->text().toInt(&ok);
            if (ok && interval > 0) {
                udpAutoSendTimer->start(interval);
            }
        }
    } else {
        logMessage(tr("Failed to start UDP: %1").arg(udpSocket->errorString()));
        delete udpSocket;
        udpSocket = nullptr;
    }
}

void PageNetworkTool::stopUdpServer()
{
    if (!udpServerRunning) return;
    
    if (udpSocket) {
        udpSocket->close();
        delete udpSocket;
        udpSocket = nullptr;
    }
    
    udpServerRunning = false;
    udpAutoSendTimer->stop();
    udpStartButton->setEnabled(true);
    udpStopButton->setEnabled(false);
    udpServerPortEdit->setEnabled(true);
    udpStatusLabel->setText(tr("Status: Stopped"));
    logMessage(tr("UDP stopped"));
    
    udpClients.clear();
    updateUdpClientList();
}

void PageNetworkTool::readPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray data = datagram.data();
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        QString message;
        
        // Check receive hex setting instead of send hex setting
        bool isReceiveHex = receiveHexCheckbox && receiveHexCheckbox->isChecked();
        if (isReceiveHex) {
            message = data.toHex(' ');
            // Add space between frames if frame line break is not enabled
            if (!frameLineBreakCheckbox->isChecked() && !message.isEmpty()) {
                message += ' ';
            }
        } else {
            message = QString::fromUtf8(data);
        }
        
        appendReceiveData(message);
        
        // Update client list
        bool found = false;
        for (int i = 0; i < udpClients.size(); ++i) {
            if (udpClients[i].address == datagram.senderAddress() && udpClients[i].port == datagram.senderPort()) {
                udpClients[i].lastSeen = QDateTime::currentDateTime();
                found = true;
                break;
            }
        }
        
        if (!found) {
            UdpClientInfo client;
            client.address = datagram.senderAddress();
            client.port = datagram.senderPort();
            client.lastSeen = QDateTime::currentDateTime();
            udpClients.append(client);
            updateUdpClientList();
        }
    }
}

void PageNetworkTool::udpSendData()
{
    if (!udpServerRunning) {
        logMessage(tr("Please start UDP first"));
        return;
    }
    
    QString data = sendEdit->toPlainText();
    if (data.isEmpty()) return;
    
    QByteArray sendData = prepareSendData(data, sendHexCheckbox->isChecked());
    if (sendData.isEmpty()) return;
    
    QString targetIp = udpTargetIpEdit->text();
    quint16 targetPort = udpTargetPortEdit->text().toUShort();
    
    QHostAddress targetAddress(targetIp);
    
    qint64 bytesWritten = udpSocket->writeDatagram(sendData, targetAddress, targetPort);
    if (bytesWritten > 0) {
        logMessage(tr("UDP Sent %1 bytes to %2:%3").arg(bytesWritten).arg(targetIp).arg(targetPort));
    } else {
        logMessage(tr("Failed to send UDP data"));
    }
}

// Common methods
void PageNetworkTool::clearLog()
{
    receiveEdit->clear();
}

void PageNetworkTool::clearReceiveData()
{
    receiveDataEdit->clear();
}

void PageNetworkTool::saveLog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Log"), "", tr("Text Files (*.txt)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << receiveEdit->toPlainText();
            logMessage(tr("Log saved to %1").arg(fileName));
        }
    }
}

void PageNetworkTool::logMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    receiveEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
    
    // Auto-scroll to bottom
    QTextCursor cursor = receiveEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    receiveEdit->setTextCursor(cursor);
}

void PageNetworkTool::appendReceiveData(const QString &message)
{
    QString displayMessage = message;
    
    // Add timestamp if enabled
    if (displayTimeCheckbox && displayTimeCheckbox->isChecked()) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
        displayMessage = QString("[%1] %2").arg(timestamp).arg(displayMessage);
    }
    
    // Add frame line break only if explicitly enabled
    if (frameLineBreakCheckbox && frameLineBreakCheckbox->isChecked()) {
        displayMessage += "\r\n";
    }
    
    // Use insertPlainText to avoid automatic newlines
    QTextCursor cursor = receiveDataEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(displayMessage);
    receiveDataEdit->setTextCursor(cursor);
    
    // Auto-scroll to bottom
    receiveDataEdit->ensureCursorVisible();
}

void PageNetworkTool::updateTcpClientList()
{
    tcpClientList->clear();
    for (int i = 0; i < tcpClients.size(); ++i) {
        QTcpSocket *client = tcpClients.at(i);
        QString clientInfo = tr("%1:%2").arg(formatAddress(client->peerAddress())).arg(client->peerPort());
        tcpClientList->addItem(clientInfo);
    }
}

void PageNetworkTool::updateUdpClientList()
{
    udpClientList->clear();
    for (int i = 0; i < udpClients.size(); ++i) {
        const UdpClientInfo &client = udpClients.at(i);
        QString clientInfo = tr("%1:%2 (Last: %3)")
                            .arg(formatAddress(client.address))
                            .arg(client.port)
                            .arg(client.lastSeen.toString("hh:mm:ss"));
        udpClientList->addItem(clientInfo);
    }
}

QString PageNetworkTool::formatAddress(const QHostAddress &address)
{
    QString addressStr = address.toString();
    
    if (address.protocol() == QAbstractSocket::IPv6Protocol && addressStr.startsWith("::ffff:")) {
        addressStr = addressStr.mid(7);
    }
    
    return addressStr;
}

void PageNetworkTool::updateConnectionStatus()
{
    if (tcpClientConnected) {
        tcpClientStatusLabel->setText(tr("Status: Connected to %1:%2")
                                    .arg(formatAddress(tcpSocket->peerAddress()))
                                    .arg(tcpSocket->peerPort()));
        tcpConnectButton->setEnabled(false);
        tcpDisconnectButton->setEnabled(true);
        tcpClientSendButton->setEnabled(true);
        tcpClientIpEdit->setEnabled(false);
        tcpClientPortEdit->setEnabled(false);
    } else {
        tcpClientStatusLabel->setText(tr("Status: Disconnected"));
        tcpConnectButton->setEnabled(true);
        tcpDisconnectButton->setEnabled(false);
        tcpClientSendButton->setEnabled(false);
        tcpClientIpEdit->setEnabled(true);
        tcpClientPortEdit->setEnabled(true);
    }
}

QByteArray PageNetworkTool::prepareSendData(const QString &text, bool isHex)
{
    QByteArray data;
    
    if (isHex) {
        QStringList hexValues = text.simplified().split(' ');
        for (const QString &hex : hexValues) {
            bool ok;
            int value = hex.toInt(&ok, 16);
            if (!ok) {
                logMessage(tr("Invalid hex data: %1").arg(hex));
                return QByteArray();
            }
            data.append(static_cast<char>(value));
        }
    } else {
        data = text.toUtf8();
    }
    
    return data;
}

void PageNetworkTool::verifySendData()
{
    if(sendHexCheckbox->isChecked())
    {
        PageVerifiyDialog pageVerifiyDialog;
        connect(&pageVerifiyDialog, &PageVerifiyDialog::selectedIndexChanged, this, &PageNetworkTool::verifysend);

        // 设置对话框为模态
        pageVerifiyDialog.setModal(true);

        // 计算对话框在父窗口中间的位置
        QPoint parentCenter = this->window()->geometry().center();
        QSize dialogSize = pageVerifiyDialog.size();

        // 如果对话框还没有显示过，size()可能返回无效值，所以先调整大小
        pageVerifiyDialog.adjustSize();
        dialogSize = pageVerifiyDialog.size();

        // 计算位置并移动对话框
        QPoint dialogPos = parentCenter - QPoint(dialogSize.width() / 2, dialogSize.height() / 2);
        pageVerifiyDialog.move(dialogPos);

        pageVerifiyDialog.exec();
    }
}

void PageNetworkTool::verifysend(int selectedIndex)
{
    PageVerifiyDialog::VERIFIY_TYPE type = (PageVerifiyDialog::VERIFIY_TYPE)selectedIndex;
    QString dataToSend = sendEdit->toPlainText();
    if(dataToSend.isEmpty())
    {
        qDebug() << "发送内容为空";
        return;
    }
    if(!sendHexCheckbox->isChecked())
    {
        qDebug() << "字符无法校验";
        return;
    }

    QByteArray byteArray;
    // 十六进制解析
    QString hexInput = dataToSend.trimmed();
    if(!hexInput.contains(' ') && hexInput.length() > 2)
    {
        QString spacedHex;
        for(int i = 0; i < hexInput.length(); i += 2)
        {
            if(i + 2 <= hexInput.length())
                spacedHex += hexInput.mid(i, 2) + " ";
            else
                spacedHex += hexInput.mid(i);
        }
        hexInput = spacedHex.trimmed();
    }
    QStringList hexValues = hexInput.split(' ', Qt::SkipEmptyParts);
    bool ok;
    for(const QString &hex : hexValues)
    {
        int byte = hex.toInt(&ok, 16);
        if(ok && byte >= 0 && byte <= 255)
        {
            byteArray.append(static_cast<char>(byte));
        }
        else
        {
            qDebug() << "无效的十六进制数:" << hex;
            return;
        }
    }

    switch(type)
    {
        case PageVerifiyDialog::CRC16_LOW:
        {
            // CRC16-IBM (Modbus)算法
            quint16 crc = 0xFFFF;
            for(auto b : byteArray)
            {
                crc ^= static_cast<quint8>(b);
                for(int i = 0; i < 8; ++i)
                {
                    if(crc & 0x0001)
                        crc = (crc >> 1) ^ 0xA001;
                    else
                        crc >>= 1;
                }
            }
            // 低字节在前
            QString crcStr = QString("%1 %2")
                             .arg(crc & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 8) & 0xFF, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + crcStr);
            qDebug() << "CRC16_LOW 校验码:" << crcStr;
            break;
        }
        case PageVerifiyDialog::CRC16_HIGH:
        {
            // CRC16-IBM (Modbus)算法
            quint16 crc = 0xFFFF;
            for(auto b : byteArray)
            {
                crc ^= static_cast<quint8>(b);
                for(int i = 0; i < 8; ++i)
                {
                    if(crc & 0x0001)
                        crc = (crc >> 1) ^ 0xA001;
                    else
                        crc >>= 1;
                }
            }
            // 高字节在前
            QString crcStr = QString("%1 %2")
                             .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                             .arg(crc & 0xFF, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + crcStr);
            qDebug() << "CRC16_HIGH 校验码:" << crcStr;
            break;
        }
        case PageVerifiyDialog::CRC8:
        {
            quint8 crc = 0;
            for(auto b : byteArray)
            {
                crc ^= static_cast<quint8>(b);
                for(int i = 0; i < 8; ++i)
                {
                    if(crc & 0x80)
                        crc = (crc << 1) ^ 0x07;
                    else
                        crc <<= 1;
                }
            }
            QString crcStr = QString("%1").arg(crc, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + crcStr);
            qDebug() << "CRC8 校验码:" << crcStr;
            break;
        }
        case PageVerifiyDialog::TOTAL_SUM:
        {
            quint8 sum = 0;
            for(auto b : byteArray)
            {
                sum += static_cast<quint8>(b);
            }
            QString sumStr = QString("%1").arg(sum, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + sumStr);
            qDebug() << "TOTAL_SUM 校验码:" << sumStr;
            break;
        }
        case PageVerifiyDialog::SUM_XOR:
        {
            quint8 xorVal = 0;
            for(auto b : byteArray)
            {
                xorVal ^= static_cast<quint8>(b);
            }
            QString xorStr = QString("%1").arg(xorVal, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + xorStr);
            qDebug() << "SUM_XOR 校验码:" << xorStr;
            break;
        }
        case PageVerifiyDialog::CRC32_LOW:
        {
            quint32 crc = 0xFFFFFFFF;
            for(auto b : byteArray)
            {
                crc ^= static_cast<quint8>(b);
                for(int i = 0; i < 8; ++i)
                {
                    if(crc & 0x00000001)
                        crc = (crc >> 1) ^ 0xEDB88320;
                    else
                        crc >>= 1;
                }
            }
            crc ^= 0xFFFFFFFF;
            QString crcStr = QString("%1 %2 %3 %4")
                             .arg(crc & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 16) & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 24) & 0xFF, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + crcStr);
            qDebug() << "CRC32_LOW 校验码:" << crcStr;
            break;
        }
        case PageVerifiyDialog::CRC32_HIGH:
        {
            quint32 crc = 0xFFFFFFFF;
            for(auto b : byteArray)
            {
                crc ^= static_cast<quint8>(b);
                for(int i = 0; i < 8; ++i)
                {
                    if(crc & 0x00000001)
                        crc = (crc >> 1) ^ 0xEDB88320;
                    else
                        crc >>= 1;
                }
            }
            crc ^= 0xFFFFFFFF;
            QString crcStr = QString("%1 %2 %3 %4")
                             .arg((crc >> 24) & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 16) & 0xFF, 2, 16, QChar('0'))
                             .arg((crc >> 8) & 0xFF, 2, 16, QChar('0'))
                             .arg(crc & 0xFF, 2, 16, QChar('0')).toUpper();
            sendEdit->setPlainText(dataToSend + " " + crcStr);
            qDebug() << "CRC32_HIGH 校验码:" << crcStr;
            break;
        }
    }
}