#include "myresource.h"

PageConnectionDialog::PageConnectionDialog(QWidget *parent)
  : QDialog(parent)
{
  setupUi();
  connectSignals();

  setWindowTitle(tr("Connect to IEC61850 Device"));
  setModal(true);
  resize(400, 350);
}

PageConnectionDialog::~PageConnectionDialog()
{
}

void PageConnectionDialog::setupUi()
{
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Connection settings group
  QGroupBox *connectionGroup = new QGroupBox(tr("Connection Settings"), this);
  QFormLayout *connectionLayout = new QFormLayout(connectionGroup);

  m_hostnameEdit = new QLineEdit("192.168.1.100", this);
  connectionLayout->addRow(tr("Hostname:"), m_hostnameEdit);

  m_portSpinBox = new QSpinBox(this);
  m_portSpinBox->setRange(1, 65535);
  m_portSpinBox->setValue(102);
  connectionLayout->addRow(tr("Port:"), m_portSpinBox);

  m_timeoutSpinBox = new QSpinBox(this);
  m_timeoutSpinBox->setRange(1000, 60000);
  m_timeoutSpinBox->setValue(5000);
  m_timeoutSpinBox->setSuffix(" ms");
  connectionLayout->addRow(tr("Timeout:"), m_timeoutSpinBox);

  mainLayout->addWidget(connectionGroup);

  // Authentication group
  m_authGroupBox = new QGroupBox(tr("Authentication"), this);
  QFormLayout *authLayout = new QFormLayout(m_authGroupBox);

  m_authCheckBox = new QCheckBox(tr("Use Authentication"), this);
  authLayout->addRow(m_authCheckBox);

  m_usernameEdit = new QLineEdit(this);
  m_usernameEdit->setEnabled(false);
  authLayout->addRow(tr("Username:"), m_usernameEdit);

  m_passwordEdit = new QLineEdit(this);
  m_passwordEdit->setEchoMode(QLineEdit::Password);
  m_passwordEdit->setEnabled(false);
  authLayout->addRow(tr("Password:"), m_passwordEdit);

  mainLayout->addWidget(m_authGroupBox);

  // Progress section
  m_progressLabel = new QLabel(tr("Ready to connect"), this);
  m_progressLabel->setVisible(false);
  mainLayout->addWidget(m_progressLabel);

  m_progressBar = new QProgressBar(this);
  m_progressBar->setRange(0, 100);
  m_progressBar->setVisible(false);
  mainLayout->addWidget(m_progressBar);

  // Buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();

  m_connectButton = new QPushButton(tr("Connect"), this);
  m_connectButton->setDefault(true);
  buttonLayout->addWidget(m_connectButton);

  m_cancelButton = new QPushButton(tr("Cancel"), this);
  buttonLayout->addWidget(m_cancelButton);

  mainLayout->addLayout(buttonLayout);
}

void PageConnectionDialog::connectSignals()
{
  connect(m_connectButton, &QPushButton::clicked, this, &PageConnectionDialog::onConnectClicked);
  connect(m_cancelButton, &QPushButton::clicked, this, &PageConnectionDialog::onCancelClicked);
  connect(m_authCheckBox, &QCheckBox::toggled, this, &PageConnectionDialog::onAuthenticationToggled);
}

void PageConnectionDialog::onConnectClicked()
{
  accept();
}

void PageConnectionDialog::onCancelClicked()
{
  reject();
}

void PageConnectionDialog::onAuthenticationToggled(bool enabled)
{
  m_authGroupBox->setEnabled(enabled);
}

QString PageConnectionDialog::getHostname() const
{
  return m_hostnameEdit->text();
}

int PageConnectionDialog::getPort() const
{
  return m_portSpinBox->value();
}

QString PageConnectionDialog::getUsername() const
{
  return m_usernameEdit->text();
}

QString PageConnectionDialog::getPassword() const
{
  return m_passwordEdit->text();
}

bool PageConnectionDialog::getUseAuthentication() const
{
  return m_authCheckBox->isChecked();
}

int PageConnectionDialog::getTimeout() const
{
  return m_timeoutSpinBox->value();
}