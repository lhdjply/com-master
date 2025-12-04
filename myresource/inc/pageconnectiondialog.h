#ifndef PAGECONNECTIONDIALOG_H
#define PAGECONNECTIONDIALOG_H

#include "qtinclude.h"

class PageConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PageConnectionDialog(QWidget *parent = nullptr);
    ~PageConnectionDialog();

    QString getHostname() const;
    int getPort() const;
    QString getUsername() const;
    QString getPassword() const;
    bool getUseAuthentication() const;
    int getTimeout() const;

private slots:
    void onConnectClicked();
    void onCancelClicked();
    void onAuthenticationToggled(bool enabled);

private:
    void setupUi();
    void connectSignals();

    // Connection parameters
    QLineEdit *m_hostnameEdit;
    QSpinBox *m_portSpinBox;
    QCheckBox *m_authCheckBox;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QSpinBox *m_timeoutSpinBox;
    
    // Buttons
    QPushButton *m_connectButton;
    QPushButton *m_cancelButton;
    
    // Progress
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    
    // Authentication group
    QGroupBox *m_authGroupBox;
};

#endif // PAGECONNECTIONDIALOG_H