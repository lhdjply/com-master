#ifndef PAGESSHCLIENT_H
#define PAGESSHCLIENT_H

#include "qtinclude.h"
#include <libssh/libssh.h>

class PageSshClient : public QWidget
{
    Q_OBJECT

  public:
    explicit PageSshClient(QWidget *parent = nullptr);
    ~PageSshClient();

  private slots:
    void onConnectButtonClicked();
    void onDataReceived();
    void onTerminalDataReceived(const QByteArray &data);

  private:
    void setupUi();
    void setupConnections();
    void connectToHost();
    void disconnectFromHost();
    void authenticate();
    void authenticateWithCredentials();
    void authenticateWithTerminalCredentials(const QString &username, const QString &password);
    void startShell();
    void startInteractiveShell();
    void startInteractiveShellWithPasswordPrompt();
    void createRealShell();
    void writeToTerminal(const QString &text);
    void clearTerminal();
    bool verifyKnownHosts();

    // UI components
    QGridLayout * mainLayout;
    QGroupBox * connectionGroup;
    QGridLayout * connectionLayout;
    QLabel * hostLabel;
    QLabel * portLabel;
    QLabel * usernameLabel;
    QLabel * passwordLabel;
    QLineEdit * hostEdit;
    QLineEdit * portEdit;
    QLineEdit * usernameEdit;
    QLineEdit * passwordEdit;
    QPushButton * connectButton;

    QGroupBox * terminalGroup;
    QVBoxLayout * terminalLayout;
    QTermWidget * terminalWidget;

    QStatusBar * statusBar;
    QLabel * statusLabel;

    // SSH session
    ssh_session session;
    ssh_channel channel;
    bool isConnected;

    // Timer for checking data
    QTimer * dataTimer;

    // Temporary storage for terminal input
    QString * m_usernameInput;
    QString * m_passwordInput;

    // Authentication state tracking
    int * m_currentAuthState;
    bool * m_authStateInitialized;

    // Authentication state enum
    enum
    {
      WAITING_FOR_USERNAME = 0,
      WAITING_FOR_PASSWORD = 1
    };

    // 右键菜单相关
    QMenu * contextMenu;
    QAction * copyAction;
    QAction * pasteAction;
    QAction * clearAction;

    // 右键菜单槽函数
    void showTerminalContextMenu(const QPoint& pos);
    void setupContextMenu();
};

#endif