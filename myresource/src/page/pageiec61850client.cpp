#include "myresource.h"

PageIEC61850Client::PageIEC61850Client(QWidget *parent)
  : QMainWindow(parent)
{
  // Set size policy to expand
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  m_connection = IedConnection_create();
  setupDataModelTreeUI();
  setupDataValueWidgetUI();
  setupUi();
}

PageIEC61850Client::~PageIEC61850Client()
{
  if(m_isConnected)
  {
    disconnect();
  }
  if(m_connection)
  {
    IedConnection_destroy(m_connection);
  }
}

void PageIEC61850Client::setupUi()
{
  // Set window properties
  setWindowTitle(tr("IEC61850 Client"));
  setMinimumSize(800, 600);
  resize(1000, 700);

  // Apply the same style sheet as PageSerialTransceiver
  QString styleSheet = QString::fromUtf8(
                         "QMainWindow {\n"
                         "    background-color: #f5f7fa;\n"
                         "    color: #333333;\n"
                         "    font-family: 'Segoe UI', Arial, sans-serif;\n"
                         "}\n"
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
                         "QTreeWidget {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 8px;\n"
                         "    background-color: white;\n"
                         "    font-family: 'Consolas', 'Monaco', monospace;\n"
                         "}\n"
                         "QTableWidget {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    padding: 8px;\n"
                         "    background-color: white;\n"
                         "    font-family: 'Consolas', 'Monaco', monospace;\n"
                         "    gridline-color: #ecf0f1;\n"
                         "    alternate-background-color: #f8fafc;\n"
                         "}\n"
                         "QHeaderView::section {\n"
                         "    background-color: #ecf0f1;\n"
                         "    color: #2c3e50;\n"
                         "    padding: 6px;\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    font-weight: bold;\n"
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
                         "QToolBar {\n"
                         "    background-color: #f5f7fa;\n"
                         "    border: none;\n"
                         "    border-bottom: 1px solid #c0c6d0;\n"
                         "    spacing: 3px;\n"
                         "    padding: 4px;\n"
                         "}\n"
                         "QStatusBar {\n"
                         "    background-color: #f5f7fa;\n"
                         "    color: #2c3e50;\n"
                         "    border: none;\n"
                         "    border-top: 1px solid #c0c6d0;\n"
                         "}\n"
                         "QProgressBar {\n"
                         "    border: 1px solid #c0c6d0;\n"
                         "    border-radius: 4px;\n"
                         "    text-align: center;\n"
                         "    background-color: #ecf0f1;\n"
                         "    color: #2c3e50;\n"
                         "}\n"
                         "QProgressBar::chunk {\n"
                         "    background-color: #3498db;\n"
                         "    border-radius: 3px;\n"
                         "}"
                       );
  this->setStyleSheet(styleSheet);

  // Create central widget
  QWidget* centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Create main layout
  QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setSpacing(15);
  mainLayout->setContentsMargins(10, 10, 10, 10);

  // Create main splitter
  m_mainSplitter = new QSplitter(Qt::Horizontal, this);
  mainLayout->addWidget(m_mainSplitter);

  // Create left panel (Data Model)
  QWidget* leftPanel = new QWidget(this);
  leftPanel->setMinimumWidth(300);
  QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
  leftLayout->setSpacing(10);
  leftLayout->setContentsMargins(5, 5, 5, 5);

  // Create a group box for the data model tree
  QGroupBox* dataModelGroup = new QGroupBox(tr("Data Model"), leftPanel);
  QVBoxLayout* dataModelLayout = new QVBoxLayout(dataModelGroup);
  dataModelLayout->setSpacing(5);
  dataModelLayout->setContentsMargins(5, 5, 5, 5);
  dataModelLayout->addWidget(m_dataModelTree);

  leftLayout->addWidget(dataModelGroup);

  m_mainSplitter->addWidget(leftPanel);

  // Create right panel
  m_rightSplitter = new QSplitter(Qt::Vertical, this);
  m_mainSplitter->addWidget(m_rightSplitter);

  // Create data value widget
  m_rightSplitter->addWidget(m_dataValueWidget);

  // Set splitter sizes and stretch factors
  m_mainSplitter->setSizes({400, 800});
  m_mainSplitter->setStretchFactor(0, 1);
  m_mainSplitter->setStretchFactor(1, 2);
  m_rightSplitter->setSizes({400, 200});
  m_rightSplitter->setStretchFactor(0, 2);
  m_rightSplitter->setStretchFactor(1, 1);

  setupToolBar();
  setupStatusBar();
}

void PageIEC61850Client::setupToolBar()
{
  // Create actions
  m_connectAction = new QAction(tr("Connect"), this);
  m_connectAction->setToolTip(tr("Connect to IEC61850 device"));

  m_disconnectAction = new QAction(tr("Disconnect"), this);
  m_disconnectAction->setToolTip(tr("Disconnect from device"));
  m_disconnectAction->setEnabled(false);

  m_refreshAction = new QAction(tr("Refresh"), this);
  m_refreshAction->setToolTip(tr("Refresh data model"));
  m_refreshAction->setEnabled(false);

  // Create toolbar
  m_toolBar = addToolBar("Main");
  m_toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
  m_toolBar->setIconSize(QSize(24, 24));

  // Add actions to toolbar
  m_toolBar->addAction(m_connectAction);
  m_toolBar->addAction(m_disconnectAction);
  m_toolBar->addSeparator();
  m_toolBar->addAction(m_refreshAction);

  // Connect signals
  QObject::connect(m_connectAction, &QAction::triggered, this, &PageIEC61850Client::showConnectionDialog);
  QObject::connect(m_disconnectAction, &QAction::triggered, this, &PageIEC61850Client::disconnectFromDevice);
  QObject::connect(m_refreshAction, &QAction::triggered, this, &PageIEC61850Client::refreshDataModel);
}

void PageIEC61850Client::setupStatusBar()
{
  m_statusBar = statusBar();

  m_connectionStatusLabel = new QLabel(tr("Disconnected"), this);
  m_statusBar->addPermanentWidget(m_connectionStatusLabel);

  // Progress bar for data model loading
  m_progressBar = new QProgressBar(this);
  m_progressBar->setRange(0, 100);
  m_progressBar->setVisible(false);
  m_progressBar->setMaximumWidth(200);
  m_statusBar->addPermanentWidget(m_progressBar);

  m_progressLabel = new QLabel("", this);
  m_progressLabel->setVisible(false);
  m_statusBar->addPermanentWidget(m_progressLabel);
}

bool PageIEC61850Client::connect(const QString& hostname, int port)
{
  QMutexLocker locker(&m_mutex);

  // If already connected, verify the connection is still valid
  if(m_isConnected)
  {
    // Try to get server state to verify connection is still alive
    IedConnectionState state = IedConnection_getState(m_connection);
    if(state == IED_STATE_CONNECTED)
    {
      return true; // Connection is still valid
    }
    else
    {
      // Connection is no longer valid, clean up and proceed with reconnection
      IedConnection_close(m_connection);
      m_isConnected = false;
      m_logicalDevices.clear();
      m_logicalNodes.clear();
      m_dataDirectories.clear();
    }
  }

  IedClientError error;

  // Try to connect
  IedConnection_connect(m_connection, &error, hostname.toStdString().c_str(), port);

  if(error != IED_ERROR_OK)
  {
    return false;
  }

  m_isConnected = true;

  // Get logical device list
  LinkedList logicalDevices = IedConnection_getLogicalDeviceList(m_connection, &error);
  if(error == IED_ERROR_OK)
  {
    LinkedList device = LinkedList_getNext(logicalDevices);
    while(device != nullptr)
    {
      QString deviceName = QString((char *)device->data);
      m_logicalDevices.append(deviceName);
      device = LinkedList_getNext(device);
    }
    LinkedList_destroy(logicalDevices);
  }

  // Pre-load logical nodes for better UX
  for(const QString& device : m_logicalDevices)
  {
    // Use getLogicalDeviceDirectory to get all logical nodes in the device
    LinkedList nodes = IedConnection_getLogicalDeviceDirectory(m_connection, &error, device.toStdString().c_str());
    if(error == IED_ERROR_OK)
    {
      QStringList nodeList;
      LinkedList node = LinkedList_getNext(nodes);
      while(node != nullptr)
      {
        nodeList.append(QString((char *)node->data));
        node = LinkedList_getNext(node);
      }
      m_logicalNodes[device] = nodeList;
      LinkedList_destroy(nodes);
    }
  }

  // Release the mutex before calling onConnectionStateChanged to avoid deadlock
  // since onConnectionStateChanged will call refreshDataModel() which calls isConnected()
  // that also tries to lock the mutex
  locker.unlock();

  onConnectionStateChanged(true);

  return true;
}

void PageIEC61850Client::disconnect()
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return;
  }

  IedConnection_close(m_connection);
  m_isConnected = false;

  // Clear cache
  m_logicalDevices.clear();
  m_logicalNodes.clear();
  m_dataDirectories.clear();

  onConnectionStateChanged(false);
}

bool PageIEC61850Client::isConnected() const
{
  QMutexLocker locker(&m_mutex);
  return m_isConnected;
}

QStringList PageIEC61850Client::getLogicalDeviceList()
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return QStringList();
  }

  return m_logicalDevices;
}

QStringList PageIEC61850Client::getLogicalNodeList(const QString& deviceName)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return QStringList();
  }

  // Check cache first
  if(m_logicalNodes.contains(deviceName))
  {
    return m_logicalNodes[deviceName];
  }

  IedClientError error;
  LinkedList logicalNodes = IedConnection_getLogicalDeviceDirectory(m_connection, &error,
                                                                    deviceName.toStdString().c_str());

  QStringList nodes;
  if(error == IED_ERROR_OK)
  {
    LinkedList node = LinkedList_getNext(logicalNodes);
    while(node != nullptr)
    {
      nodes.append(QString((char *)node->data));
      node = LinkedList_getNext(node);
    }
    LinkedList_destroy(logicalNodes);

    // Cache the result
    m_logicalNodes[deviceName] = nodes;
  }

  return nodes;
}

QStringList PageIEC61850Client::getDataDirectory(const QString& objectReference)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected || objectReference.isEmpty() || !m_connection)
  {
    return QStringList();
  }

  IedClientError error;
  LinkedList dataDirectory = IedConnection_getDataDirectory(m_connection, &error, objectReference.toStdString().c_str());

  if(error != IED_ERROR_OK || dataDirectory == nullptr)
  {
    if(dataDirectory != nullptr)
    {
      LinkedList_destroy(dataDirectory);
    }
    return QStringList();
  }

  QStringList directory;
  LinkedList entry = dataDirectory;
  int entryCount = 0;

  while(entry != nullptr && entryCount < 1000)    // Limit to prevent infinite loops
  {
    if(entry->data != nullptr)
    {
      QString entryStr = QString::fromUtf8((char *)entry->data);
      directory.append(entryStr);
    }
    entry = entry->next;
    entryCount++;
  }

  LinkedList_destroy(dataDirectory);
  return directory;
}

QStringList PageIEC61850Client::getLogicalNodeDirectory(const QString& nodeReference)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected || nodeReference.isEmpty() || !m_connection)
  {
    return QStringList();
  }

  IedClientError error;
  LinkedList nodeDirectory = IedConnection_getLogicalNodeDirectory(m_connection, &error,
                                                                   nodeReference.toStdString().c_str(),
                                                                   ACSI_CLASS_DATA_OBJECT);

  if(error != IED_ERROR_OK || nodeDirectory == nullptr)
  {
    if(nodeDirectory != nullptr)
    {
      LinkedList_destroy(nodeDirectory);
    }
    return QStringList();
  }

  QStringList directory;
  LinkedList entry = nodeDirectory;
  int entryCount = 0;

  while(entry != nullptr && entryCount < 1000)    // Limit to prevent infinite loops
  {
    if(entry->data != nullptr)
    {
      QString entryStr = QString::fromUtf8((char *)entry->data);
      directory.append(entryStr);
    }
    entry = entry->next;
    entryCount++;
  }

  LinkedList_destroy(nodeDirectory);
  return directory;
}

QStringList PageIEC61850Client::getAllDataDirectories(const QString& objectReference)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected || objectReference.isEmpty() || !m_connection)
  {
    return QStringList();
  }

  QStringList allResults;

  // Try all functional constraints to get all possible data
  FunctionalConstraint fcs[] =
  {
    IEC61850_FC_ST, IEC61850_FC_MX, IEC61850_FC_SP, IEC61850_FC_SV,
    IEC61850_FC_CF, IEC61850_FC_DC, IEC61850_FC_SG, IEC61850_FC_SE,
    IEC61850_FC_SR, IEC61850_FC_OR, IEC61850_FC_BL, IEC61850_FC_EX,
    IEC61850_FC_CO, IEC61850_FC_US, IEC61850_FC_MS, IEC61850_FC_RP,
    IEC61850_FC_BR, IEC61850_FC_LG, IEC61850_FC_GO
  };

  int numFcs = sizeof(fcs) / sizeof(FunctionalConstraint);

  for(int i = 0; i < numFcs; i++)
  {
    IedClientError error;
    LinkedList dataDirectory = IedConnection_getDataDirectoryByFC(m_connection, &error,
                                                                  objectReference.toStdString().c_str(),
                                                                  fcs[i]);

    if(error == IED_ERROR_OK && dataDirectory != nullptr)
    {
      LinkedList entry = dataDirectory;
      int entryCount = 0;

      while(entry != nullptr && entryCount < 1000)
      {
        if(entry->data != nullptr)
        {
          QString entryStr = QString::fromUtf8((char *)entry->data);
          // Add FC prefix to distinguish between different FC data
          QString fcString = FunctionalConstraint_toString(fcs[i]);
          QString prefixedEntry = entryStr + "$" + fcString;
          if(!allResults.contains(prefixedEntry))
          {
            allResults.append(prefixedEntry);
          }
        }
        entry = entry->next;
        entryCount++;
      }

      LinkedList_destroy(dataDirectory);
    }
  }

  return allResults;
}

QVariant PageIEC61850Client::readValue(const QString& objectReference, const QString& functionalConstraint)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return QVariant();
  }

  IedClientError error;
  FunctionalConstraint fc = FunctionalConstraint_fromString(functionalConstraint.toStdString().c_str());
  MmsValue* mmsValue = IedConnection_readObject(m_connection, &error,
                                                objectReference.toStdString().c_str(), fc);

  if(error != IED_ERROR_OK)
  {
    return QVariant();
  }

  QVariant result = mmsValueToQVariant(mmsValue);
  MmsValue_delete(mmsValue);

  return result;
}

bool PageIEC61850Client::writeValue(const QString& objectReference, const QVariant& value,
                                    const QString& functionalConstraint)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return false;
  }

  QString dataType = getDataType(objectReference);
  MmsValue* mmsValue = qVariantToMmsValue(value, dataType);

  if(!mmsValue)
  {
    return false;
  }

  IedClientError error;
  FunctionalConstraint fc = FunctionalConstraint_fromString(functionalConstraint.toStdString().c_str());

  // Check if FC conversion was successful
  if(fc == IEC61850_FC_NONE)
  {
    MmsValue_delete(mmsValue);
    return false;
  }

  IedConnection_writeObject(m_connection, &error,
                            objectReference.toStdString().c_str(),
                            fc,
                            mmsValue);

  MmsValue_delete(mmsValue);

  if(error != IED_ERROR_OK)
  {
    return false;
  }

  return true;
}

QVariant PageIEC61850Client::mmsValueToQVariant(MmsValue* mmsValue)
{
  if(!mmsValue)
  {
    return QVariant();
  }

  MmsType type = MmsValue_getType(mmsValue);

  switch(type)
  {
    case MMS_BOOLEAN:
      return QVariant(MmsValue_getBoolean(mmsValue));

    case MMS_INTEGER:
      return QVariant(static_cast<qint64>(MmsValue_toInt64(mmsValue)));

    case MMS_UNSIGNED:
      return QVariant(static_cast<quint64>(MmsValue_toUint32(mmsValue)));

    case MMS_FLOAT:
      return QVariant(MmsValue_toFloat(mmsValue));

    case MMS_VISIBLE_STRING:
      return QVariant(QString(MmsValue_toString(mmsValue)));

    case MMS_STRING:
      return QVariant(QString(MmsValue_toString(mmsValue)));

    case MMS_OCTET_STRING:
      {
        int size = MmsValue_getOctetStringSize(mmsValue);
        uint8_t * buffer = MmsValue_getOctetStringBuffer(mmsValue);
        QByteArray data((char *)buffer, size);
        return data;
      }

    case MMS_UTC_TIME:
      {
        QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(MmsValue_getUtcTimeInMs(mmsValue));
        return QVariant(timestamp);
      }

    case MMS_BIT_STRING:
      return QVariant(static_cast<int>(MmsValue_getBitStringAsInteger(mmsValue)));

    case MMS_ARRAY:
    case MMS_STRUCTURE:
      // Handle complex types - for now return as string representation
      return QVariant(QString("Complex type (size: %1)").arg(MmsValue_getArraySize(mmsValue)));

    case MMS_DATA_ACCESS_ERROR:
      // Handle data access errors - return error information
      return QVariant(QString("Access Error"));

    default:
      return QVariant(QString("Unsupported MMS type: %1").arg(type));
  }
}

MmsValue * PageIEC61850Client::qVariantToMmsValue(const QVariant& value, const QString& dataType)
{
  // Use dataType to determine the correct conversion
  if(dataType == "FLOAT")
  {
    return MmsValue_newFloat(value.toFloat());
  }
  else if(dataType == "INT")
  {
    return MmsValue_newInteger(value.toInt());
  }
  else if(dataType == "BOOLEAN")
  {
    return MmsValue_newBoolean(value.toBool());
  }
  else if(dataType == "STRING")
  {
    return MmsValue_newVisibleString(value.toString().toStdString().c_str());
  }
  else if(dataType == "OCTET_STRING")
  {
    QByteArray data = value.toByteArray();
    MmsValue* octetString = MmsValue_newOctetString(data.size(), data.size());
    uint8_t * buffer = MmsValue_getOctetStringBuffer(octetString);
    memcpy(buffer, data.data(), data.size());
    return octetString;
  }

  // Fallback: try to infer from QVariant type
  switch(value.metaType().id())
  {
    case QMetaType::Bool:
      return MmsValue_newBoolean(value.toBool());
    case QMetaType::Int:
      return MmsValue_newInteger(value.toInt());
    case QMetaType::UInt:
      return MmsValue_newUnsigned(value.toUInt());
    case QMetaType::Double:
      return MmsValue_newDouble(value.toDouble());
    case QMetaType::Float:
      return MmsValue_newFloat(value.toFloat());
    case QMetaType::QString:
      return MmsValue_newVisibleString(value.toString().toStdString().c_str());
    case QMetaType::QByteArray:
      {
        QByteArray data = value.toByteArray();
        MmsValue* octetString = MmsValue_newOctetString(data.size(), data.size());
        uint8_t * buffer = MmsValue_getOctetStringBuffer(octetString);
        memcpy(buffer, data.data(), data.size());
        return octetString;
      }
    default:
      return MmsValue_newVisibleString(value.toString().toStdString().c_str());
  }
}

QString PageIEC61850Client::getFunctionalConstraint(const QString& objectReference)
{
  if(!m_isConnected || objectReference.isEmpty() || !m_connection)
  {
    return "MX"; // Default fallback
  }

  // First try with priority FCs
  for(int i = IEC61850_FC_ST; i < IEC61850_FC_GO; i++)
  {
    // Test if we can actually read the object with this FC
    IedClientError error;
    MmsValue* testValue = IedConnection_readObject(m_connection, &error,
                                                   objectReference.toStdString().c_str(),
                                                   static_cast<FunctionalConstraint>(i));

    if(error == IED_ERROR_OK && testValue != nullptr)
    {
      // Check if the value is a data access error
      if(MmsValue_getType(testValue) != MMS_DATA_ACCESS_ERROR)
      {
        MmsValue_delete(testValue);
        // Found a readable FC for this object reference
        return QString(FunctionalConstraint_toString(static_cast<FunctionalConstraint>(i)));
      }
    }

    if(testValue != nullptr)
    {
      MmsValue_delete(testValue);
    }
  }

  // If no FC found, return default
  return "MX";
}

QString PageIEC61850Client::getDataType(const QString& objectReference)
{
  // First try to determine type from object reference pattern
  if(objectReference.contains(".AnIn"))
  {
    return "FLOAT";
  }
  else if(objectReference.contains(".SPCS"))
  {
    return "BOOLEAN";
  }
  else if(objectReference.contains(".NamPlt"))
  {
    return "STRING";
  }
  else if(objectReference.contains(".setMag"))
  {
    // setMag.f indicates float, setMag.i indicates integer
    if(objectReference.endsWith(".f"))
    {
      return "FLOAT";
    }
    else if(objectReference.endsWith(".i"))
    {
      return "INT";
    }
    else if(objectReference.endsWith(".b"))
    {
      return "BOOLEAN";
    }
    return "FLOAT";
  }
  else if(objectReference.contains(".mag"))
  {
    if(objectReference.endsWith(".f"))
    {
      return "FLOAT";
    }
    else if(objectReference.endsWith(".i"))
    {
      return "INT";
    }
    return "FLOAT";
  }
  else if(objectReference.contains(".stVal"))
  {
    return "BOOLEAN";
  }

  // If pattern matching fails, try to read the value to determine type
  if(m_isConnected && m_connection)
  {
    // Try to read with different FCs to determine the actual type
    for(int i = IEC61850_FC_SP; i <= IEC61850_FC_CO; i++)
    {
      IedClientError error;
      MmsValue* testValue = IedConnection_readObject(m_connection, &error,
                                                     objectReference.toStdString().c_str(),
                                                     static_cast<FunctionalConstraint>(i));

      if(error == IED_ERROR_OK && testValue != nullptr)
      {
        MmsType type = MmsValue_getType(testValue);
        MmsValue_delete(testValue);

        switch(type)
        {
          case MMS_BOOLEAN:
            return "BOOLEAN";
          case MMS_INTEGER:
          case MMS_UNSIGNED:
            return "INT";
          case MMS_FLOAT:
            return "FLOAT";
          case MMS_VISIBLE_STRING:
          case MMS_STRING:
            return "STRING";
          case MMS_OCTET_STRING:
            return "OCTET_STRING";
          default:
            break;
        }
      }
    }
  }

  return "UNKNOWN";
}

bool PageIEC61850Client::createDataSet(const QString& dataSetReference, const QStringList& dataPoints)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return false;
  }

  // Mock create data set for testing
  return true;
}

bool PageIEC61850Client::deleteDataSet(const QString& dataSetReference)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return false;
  }

  // Mock delete data set for testing
  return true;
}

QStringList PageIEC61850Client::getDataSetDirectory(const QString& dataSetReference, bool* isDeletable)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return QStringList();
  }

  // Mock data set directory for testing
  QStringList result;
  if(dataSetReference.contains("dataset01"))
  {
    result = {"LD0/MMXU1.TotW.mag.f", "LD0/MMXU1.TotVAr.mag.f", "LD0/MMXU1.Hz.mag.f"};
  }
  else if(dataSetReference.contains("dataset02"))
  {
    result = {"LD0/GGIO1.SPCSO1.stVal", "LD0/GGIO1.AnIn1.mag.f", "LD0/GGIO1.AnIn2.mag.f"};
  }

  if(isDeletable)
  {
    *isDeletable = true; // Mock: all data sets are deletable
  }

  return result;
}

QList<QVariant> PageIEC61850Client::readDataSetValues(const QString& dataSetReference)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return QList<QVariant>();
  }

  // Mock data set values for testing
  QList<QVariant> result;
  if(dataSetReference.contains("dataset01"))
  {
    result = {1234.56f, 567.89f, 50.01f};
  }
  else if(dataSetReference.contains("dataset02"))
  {
    result = {true, 100.0f, 200.0f};
  }

  return result;
}

bool PageIEC61850Client::controlObject(const QString& objectReference, const QVariant& value, bool selectBeforeOperate)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return false;
  }

  // Mock control for testing
  QString operation = selectBeforeOperate ? "select-before-operate" : "direct";
  return true;
}

QList<IEC61850Report> PageIEC61850Client::getReports()
{
  QMutexLocker locker(&m_mutex);

  QList<IEC61850Report> reports;

  if(!m_isConnected)
  {
    return reports;
  }

  // This is a simplified implementation
  // In a real application, you would query the device for available reports

  // Add some sample reports
  IEC61850Report report1;
  report1.reference = "LD0/LLN0.urcb01";
  report1.dataSetReference = "LD0/LLN0.dataset01";
  report1.isBuffered = false;
  report1.isEnabled = false;
  report1.reportId = "urcb01";
  reports.append(report1);

  IEC61850Report report2;
  report2.reference = "LD0/LLN0.brcb01";
  report2.dataSetReference = "LD0/LLN0.dataset02";
  report2.isBuffered = true;
  report2.isEnabled = false;
  report2.reportId = "brcb01";
  reports.append(report2);

  return reports;
}

bool PageIEC61850Client::enableReport(const QString& reportReference, bool enable)
{
  QMutexLocker locker(&m_mutex);

  if(!m_isConnected)
  {
    return false;
  }
  return true;
}

void PageIEC61850Client::clearObjectDetails()
{
  m_currentObjectReference.clear();
  m_objectReferenceLabel->setText("N/A");
  m_dataTypeLabel->setText("N/A");
  m_valueLabel->setText("N/A");
  m_valueTable->setRowCount(0);
}

// DataModelTree implementation
void PageIEC61850Client::setupDataModelTreeUI()
{
  m_dataModelTree = new QTreeWidget();
  m_dataModelTree->setHeaderHidden(true);
  m_dataModelTree->setSelectionMode(QAbstractItemView::SingleSelection);
  m_dataModelTree->setContextMenuPolicy(Qt::CustomContextMenu);
  m_dataModelTree->setStyleSheet("QTreeWidget { font-family: 'Consolas', 'Monaco', monospace; font-size: 12px; }");

  // Set column
  m_dataModelTree->setColumnCount(1);

  // Create root item
  m_rootItem = new QTreeWidgetItem(m_dataModelTree);
  m_rootItem->setText(0, tr("IEC61850 Device"));
  m_rootItem->setIcon(0, m_dataModelTree->style()->standardIcon(QStyle::SP_ComputerIcon));
  m_dataModelTree->addTopLevelItem(m_rootItem);

  // Expand root item
  m_rootItem->setExpanded(true);

  QObject::connect(m_dataModelTree, &QTreeWidget::itemDoubleClicked, this, &PageIEC61850Client::onItemDoubleClicked);
  QObject::connect(m_dataModelTree, &QWidget::customContextMenuRequested, this,
                   &PageIEC61850Client::onContextMenuRequested);
  setupDataModelTreeContextMenu();
}

void PageIEC61850Client::setupDataModelTreeContextMenu()
{
  m_contextMenu = new QMenu(m_dataModelTree);

  m_readValueAction = m_contextMenu->addAction(tr("Read Value"));

  m_writeValueAction = m_contextMenu->addAction(tr("Write Value"));

  m_controlAction = m_contextMenu->addAction(tr("Control Object"));

  m_contextMenu->addSeparator();

  m_createDataSetAction = m_contextMenu->addAction(tr("Create Data Set"));

  m_contextMenu->addSeparator();

  m_refreshAction = m_contextMenu->addAction(tr("Refresh"));
}

void PageIEC61850Client::buildDataModel()
{
  if(!isConnected())
  {
    return;
  }

  clearDataModel();

  onDataModelLoadProgress(5, "正在获取逻辑设备列表...");

  // Get logical devices
  QStringList logicalDevices = getLogicalDeviceList();

  // Sort logical devices alphabetically
  logicalDevices.sort();

  onDataModelLoadProgress(5, QString("找到 %1 个逻辑设备").arg(logicalDevices.size()));

  // Calculate total nodes across all devices for better progress calculation
  int totalNodes = 0;
  QList<QStringList> allDeviceNodes;
  for(const QString& device : logicalDevices)
  {
    QStringList logicalNodes = getLogicalNodeList(device);

    // Sort logical nodes alphabetically
    logicalNodes.sort();

    allDeviceNodes.append(logicalNodes);
    totalNodes += logicalNodes.size();
  }

  int processedNodes = 0;
  int deviceCount = logicalDevices.size();
  for(int deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
  {
    const QString& device = logicalDevices[deviceIndex];
    QStringList logicalNodes = allDeviceNodes[deviceIndex];

    int deviceStartProgress = 10 + (deviceIndex * 80 / deviceCount);
    onDataModelLoadProgress(deviceStartProgress,
                            QString("正在加载设备 %1 (%2/%3)").arg(device).arg(deviceIndex + 1).arg(deviceCount));

    QTreeWidgetItem* deviceItem = addLogicalDevice(device);

    onDataModelLoadProgress(deviceStartProgress + 2,
                            QString("设备 %1 包含 %2 个逻辑节点").arg(device).arg(logicalNodes.size()));

    int nodeCount = logicalNodes.size();
    for(int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    {
      const QString& node = logicalNodes[nodeIndex];
      QTreeWidgetItem* nodeItem = addLogicalNode(deviceItem, node);

      // Get data objects for this logical node
      QString nodeReference = device + "/" + node;
      QStringList dataObjects;

      // Calculate progress based on total nodes processed
      int nodeProgress = deviceStartProgress + 5 + (processedNodes * 65 / totalNodes);
      onDataModelLoadProgress(nodeProgress,
                              QString("正在处理节点 %1.%2 (%3/%4)").arg(device).arg(node).arg(processedNodes + 1).arg(totalNodes));

      // Use the proper IEC61850 API to get logical node directory
      dataObjects = getLogicalNodeDirectory(nodeReference);

      // If that fails, try the old method as fallback
      if(dataObjects.isEmpty())
      {
        dataObjects = getDataDirectory(nodeReference);
      }

      // If still empty, try with all functional constraints
      if(dataObjects.isEmpty())
      {
        dataObjects = getAllDataDirectories(nodeReference);
      }

      // Sort data objects numerically (remove FC prefix for comparison)
      QStringList sortedDataObjects = dataObjects;

      std::sort(sortedDataObjects.begin(), sortedDataObjects.end(), [](const QString & a, const QString & b)
      {
        // Remove FC prefix ($MX, $ST, etc.) for comparison
        QString nameA = a.section("$", 0, 0);
        QString nameB = b.section("$", 0, 0);

        // Extract all numeric parts for comparison
        QRegularExpression re("(\\d+)");
        QRegularExpressionMatchIterator matchAIt = re.globalMatch(nameA);
        QRegularExpressionMatchIterator matchBIt = re.globalMatch(nameB);

        // Compare prefix before first number
        int posA = 0, posB = 0;
        if(matchAIt.hasNext())
        {
          posA = matchAIt.peekNext().capturedStart();
        }
        if(matchBIt.hasNext())
        {
          posB = matchBIt.peekNext().capturedStart();
        }

        QString prefixA = nameA.left(posA);
        QString prefixB = nameB.left(posB);

        // If one has prefix and the other doesn't, use full string comparison
        if(prefixA.isEmpty() && !prefixB.isEmpty())
        {
          return nameA < nameB;
        }
        if(!prefixA.isEmpty() && prefixB.isEmpty())
        {
          return nameA < nameB;
        }

        if(prefixA != prefixB)
        {
          return prefixA < prefixB;
        }

        // Compare numbers one by one
        while(matchAIt.hasNext() && matchBIt.hasNext())
        {
          int numA = matchAIt.next().captured(1).toInt();
          int numB = matchBIt.next().captured(1).toInt();

          if(numA != numB)
          {
            return numA < numB;
          }
        }

        // Fallback to string comparison if no numbers or numbers are equal
        return nameA < nameB;
      });

      int objectCount = sortedDataObjects.size();
      for(int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
      {
        const QString& object = sortedDataObjects[objectIndex];
        QString objectReference = nodeReference + "." + object;
        addDataObject(nodeItem, object, objectReference, 0);

        // Update progress for every 10 objects or the last one
        if(objectIndex % 10 == 0 || objectIndex == objectCount - 1)
        {
          int objectProgress = nodeProgress + 5 + (objectIndex * 10 / objectCount);
          onDataModelLoadProgress(objectProgress,
                                  QString("节点 %1.%2: 加载数据对象 %3/%4").arg(device).arg(node).arg(objectIndex + 1).arg(objectCount));
        }
      }

      processedNodes++;
      // Don't auto-expand nodes to improve performance
      nodeItem->setExpanded(false);
    }

    // Only expand devices, not nodes
    deviceItem->setExpanded(true);
  }

  m_rootItem->setExpanded(true);
  onDataModelLoadProgress(100, "数据模型加载完成");
}

void PageIEC61850Client::clearDataModel()
{
  m_dataModelTree->clear();
  m_deviceItems.clear();
  m_nodeItems.clear();
  m_objectItems.clear();

  // Recreate root item
  m_rootItem = new QTreeWidgetItem(m_dataModelTree);
  m_rootItem->setText(0, tr("IEC61850 Device"));
  m_rootItem->setIcon(0, m_dataModelTree->style()->standardIcon(QStyle::SP_ComputerIcon));
  m_dataModelTree->addTopLevelItem(m_rootItem);
}

QTreeWidgetItem * PageIEC61850Client::addLogicalDevice(const QString& deviceName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(m_rootItem);
  item->setText(0, deviceName);
  item->setIcon(0, m_dataModelTree->style()->standardIcon(QStyle::SP_DirIcon));
  item->setData(0, Qt::UserRole, deviceName);

  m_deviceItems[deviceName] = item;
  return item;
}

QTreeWidgetItem * PageIEC61850Client::addLogicalNode(QTreeWidgetItem* deviceItem, const QString& nodeName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(deviceItem);
  item->setText(0, nodeName);
  item->setIcon(0, m_dataModelTree->style()->standardIcon(QStyle::SP_FileDialogDetailedView));
  item->setData(0, Qt::UserRole, deviceItem->data(0, Qt::UserRole).toString() + "/" + nodeName);

  m_nodeItems[nodeName] = item;
  return item;
}

QTreeWidgetItem * PageIEC61850Client::addDataObject(QTreeWidgetItem* parentItem, const QString& objectName,
                                                    const QString& objectReference, int depth)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
  item->setText(0, objectName);
  item->setIcon(0, m_dataModelTree->style()->standardIcon(QStyle::SP_FileIcon));
  item->setData(0, Qt::UserRole, objectReference);

  m_objectItems[objectReference] = item;

  // Try to get actual sub-data objects
  if(isConnected() && depth < 3)
  {
    QStringList subObjects;

    // First try to get the actual data directory
    subObjects = getDataDirectory(objectReference);

    // If that fails, try with all functional constraints
    if(subObjects.isEmpty())
    {
      subObjects = getAllDataDirectories(objectReference);
    }

    // Sort sub-objects numerically (remove FC prefix for comparison)
    QStringList sortedSubObjects = subObjects;
    std::sort(sortedSubObjects.begin(), sortedSubObjects.end(), [](const QString & a, const QString & b)
    {
      // Remove FC prefix ($MX, $ST, etc.) for comparison
      QString nameA = a.section("$", 0, 0);
      QString nameB = b.section("$", 0, 0);

      // Extract numeric parts for comparison
      QRegularExpression re("(\\d+)");
      QRegularExpressionMatch matchA = re.match(nameA);
      QRegularExpressionMatch matchB = re.match(nameB);

      if(matchA.hasMatch() && matchB.hasMatch())
      {
        int numA = matchA.captured(1).toInt();
        int numB = matchB.captured(1).toInt();
        if(numA != numB)
        {
          return numA < numB;
        }
      }

      // Fallback to string comparison if no numbers or numbers are equal
      return nameA < nameB;
    });

    for(const QString& subObject : sortedSubObjects)
    {
      QString subObjectReference = objectReference + "." + subObject;
      addDataObject(item, subObject, subObjectReference, depth + 1);
    }
  }

  return item;
}

void PageIEC61850Client::onItemDoubleClicked(QTreeWidgetItem* item, int column)
{
  Q_UNUSED(column)

  if(!item || item == m_rootItem)
  {
    return;
  }

  QString objectReference = item->data(0, Qt::UserRole).toString();
  if(!objectReference.isEmpty())
  {
    onDataModelItemDoubleClicked(objectReference);
  }
}

void PageIEC61850Client::onContextMenuRequested(const QPoint& pos)
{
  QTreeWidgetItem* item = m_dataModelTree->itemAt(pos);
  if(!item || item == m_rootItem)
  {
    return;
  }

  // Enable/disable actions based on the selected item
  QString objectReference = item->data(0, Qt::UserRole).toString();
  bool isDataObject = m_objectItems.contains(objectReference);

  m_readValueAction->setEnabled(isDataObject);
  m_writeValueAction->setEnabled(isDataObject);
  m_controlAction->setEnabled(isDataObject && objectReference.contains("SPCS"));
  m_createDataSetAction->setEnabled(!objectReference.isEmpty());
  m_refreshAction->setEnabled(true);

  m_contextMenu->exec(m_dataModelTree->mapToGlobal(pos));
}

// DataValueWidget implementation
void PageIEC61850Client::setupDataValueWidgetUI()
{
  m_dataValueWidget = new QWidget();
  m_mainLayout = new QVBoxLayout(m_dataValueWidget);
  m_mainLayout->setSpacing(10);
  m_mainLayout->setContentsMargins(5, 5, 5, 5);

  // Object information group
  m_objectInfoGroup = new QGroupBox(tr("Object Information"), m_dataValueWidget);
  m_infoLayout = new QFormLayout(m_objectInfoGroup);
  m_infoLayout->setSpacing(10);
  m_infoLayout->setContentsMargins(10, 10, 10, 10);

  m_objectReferenceLabel = new QLabel("N/A", m_dataValueWidget);
  m_objectReferenceLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
  m_infoLayout->addRow(tr("Object Reference:"), m_objectReferenceLabel);

  m_dataTypeLabel = new QLabel("N/A", m_dataValueWidget);
  m_dataTypeLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
  m_infoLayout->addRow(tr("Data Type:"), m_dataTypeLabel);

  m_valueLabel = new QLabel("N/A", m_dataValueWidget);
  m_valueLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");
  m_infoLayout->addRow(tr("Current Value:"), m_valueLabel);

  m_mainLayout->addWidget(m_objectInfoGroup);

  // Read/Write group
  m_readWriteGroup = new QGroupBox(tr("Operations"), m_dataValueWidget);
  QVBoxLayout* readWriteLayout = new QVBoxLayout(m_readWriteGroup);
  readWriteLayout->setSpacing(10);
  readWriteLayout->setContentsMargins(10, 10, 10, 10);

  // Write value input
  QHBoxLayout* writeLayout = new QHBoxLayout();
  writeLayout->setSpacing(10);
  m_writeValueEdit = new QLineEdit(m_dataValueWidget);
  m_writeValueEdit->setPlaceholderText(tr("Enter value to write..."));
  writeLayout->addWidget(m_writeValueEdit);

  m_writeButton = new QPushButton(tr("Write Value"), m_dataValueWidget);
  m_writeButton->setMinimumSize(130, 36);
  writeLayout->addWidget(m_writeButton);

  readWriteLayout->addLayout(writeLayout);

  // Read button
  m_readButton = new QPushButton(tr("Read Value"), m_dataValueWidget);
  m_readButton->setMinimumSize(130, 36);
  readWriteLayout->addWidget(m_readButton);

  m_mainLayout->addWidget(m_readWriteGroup);

  // Value history table
  m_valueTable = new QTableWidget(m_dataValueWidget);
  m_valueTable->setColumnCount(3);
  m_valueTable->setHorizontalHeaderLabels({tr("Timestamp"), tr("Value"), tr("Quality")});
  m_valueTable->horizontalHeader()->setStretchLastSection(true);
  m_valueTable->setAlternatingRowColors(true);
  m_valueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_valueTable->setStyleSheet("QTableWidget { font-family: 'Consolas', 'Monaco', monospace; font-size: 12px; }");

  m_mainLayout->addWidget(m_valueTable);

  // Set stretch factors
  m_mainLayout->setStretch(2, 1);

  QObject::connect(m_readButton, &QPushButton::clicked, this, [this]()
  {
    readValue();
  });
  QObject::connect(m_writeButton, &QPushButton::clicked, this, [this]()
  {
    writeValue();
  });
}

void PageIEC61850Client::showObjectDetails(const QString& objectReference)
{
  m_currentObjectReference = objectReference;
  m_objectReferenceLabel->setText(objectReference);

  // Determine data type based on object reference
  QString dataType = "UNKNOWN";
  if(objectReference.contains(".AnIn"))
  {
    dataType = "FLOAT";
  }
  else if(objectReference.contains(".SPCS"))
  {
    dataType = "BOOLEAN";
  }
  else if(objectReference.contains(".NamPlt"))
  {
    dataType = "STRING";
  }
  m_dataTypeLabel->setText(dataType);

  // Read current value
  readValue();

  // Clear history table
  m_valueTable->setRowCount(0);
}

void PageIEC61850Client::readValue()
{
  if(m_currentObjectReference.isEmpty())
  {
    return;
  }

  // Get functional constraint using the third-party API
  QString fc = getFunctionalConstraint(m_currentObjectReference);
  QVariant value = PageIEC61850Client::readValue(m_currentObjectReference, fc);

  if(value.isValid())
  {
    m_valueLabel->setText(value.toString());

    // Add to history table
    int row = m_valueTable->rowCount();
    m_valueTable->insertRow(row);
    m_valueTable->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
    m_valueTable->setItem(row, 1, new QTableWidgetItem(value.toString()));
    m_valueTable->setItem(row, 2, new QTableWidgetItem(tr("Good")));

    // Auto-scroll to bottom
    m_valueTable->scrollToBottom();

  }
  else
  {
    m_valueLabel->setText(tr("Read Error"));
  }
}

void PageIEC61850Client::writeValue()
{
  if(m_currentObjectReference.isEmpty())
  {
    return;
  }

  QString valueStr = m_writeValueEdit->text().trimmed();
  if(valueStr.isEmpty())
  {
    return;
  }

  // Convert string to appropriate type
  QVariant value;
  QString dataType = m_dataTypeLabel->text();

  if(dataType == "FLOAT")
  {
    value = valueStr.toFloat();
  }
  else if(dataType == "BOOLEAN")
  {
    value = (valueStr.toLower() == "true" || valueStr == "1");
  }
  else
  {
    value = valueStr;
  }

  // Get the correct functional constraint for this object
  QString fc = getFunctionalConstraint(m_currentObjectReference);

  if(PageIEC61850Client::writeValue(m_currentObjectReference, value, fc))
  {
    m_writeValueEdit->clear();
    // Read back the value to confirm
    readValue();
  }
  else
  {
    QMessageBox::warning(this,
                         tr("Write Failed"),
                         tr("Failed to write value to %1\n\n"
                            "Object Reference: %2\n"
                            "Functional Constraint: %3\n"
                            "Data Type: %4\n\n"
                            "Possible reasons:\n"
                            "• The object may not support write operations\n"
                            "• Wrong functional constraint (e.g., using MX instead of SP/CO)\n"
                            "• Device may be in a state that prevents writing\n"
                            "• Access control restrictions")
                         .arg(m_currentObjectReference)
                         .arg(m_currentObjectReference)
                         .arg(fc)
                         .arg(dataType));
  }
}

void PageIEC61850Client::showConnectionDialog()
{
  PageConnectionDialog dialog(this);

  if(dialog.exec() == QDialog::Accepted)
  {
    QString hostname = dialog.getHostname();
    int port = dialog.getPort();
    performConnection(hostname, port);
  }
}

void PageIEC61850Client::performConnection(const QString& hostname, int port)
{
  if(connect(hostname, port))
  {
    onConnectionStateChanged(true);
  }
  else
  {
    QMessageBox::critical(this,
                         tr("Connection Failed"),
                         tr("Failed to connect to %1:%2\n\n"
                            "Error: Connection Rejected\n\n"
                            "Possible reasons:\n"
                            "• No IEC61850 server is running on this port\n"
                            "• The port is being used by another application\n"
                            "• Firewall is blocking the connection\n\n"
                            "Troubleshooting steps:\n"
                            "1. Check if an IEC61850 server is running on the target device\n"
                            "2. Verify the port number (default is 102 for MMS)\n"
                            "3. Check network connectivity: ping %1\n"
                            "4. Check firewall settings\n"
                            "5. Try using a different port if the server is configured differently")
                         .arg(hostname).arg(port));
  }
}

void PageIEC61850Client::disconnectFromDevice()
{
  if(isConnected())
  {
    disconnect();
    onConnectionStateChanged(false);
  }
}

void PageIEC61850Client::refreshDataModel()
{
  if(isConnected())
  {
    buildDataModel();
  }
}

void PageIEC61850Client::onConnectionStateChanged(bool connected)
{
  updateConnectionStatus(connected);
  enableConnectionDependentUI(connected);

  if(connected)
  {
    refreshDataModel();
  }
  else
  {
    clearDataModel();
    clearObjectDetails();
  }
}

void PageIEC61850Client::onDataModelItemDoubleClicked(const QString& objectReference)
{
  if(!isConnected())
  {
    return;
  }

  if(!objectReference.isEmpty())
  {
    showObjectDetails(objectReference);
  }
}

void PageIEC61850Client::updateConnectionStatus(bool connected)
{
  if(connected)
  {
    m_connectionStatusLabel->setText(tr("Connected"));
    m_connectionStatusLabel->setStyleSheet("font-weight: bold; color: #27ae60;");
  }
  else
  {
    m_connectionStatusLabel->setText(tr("Disconnected"));
    m_connectionStatusLabel->setStyleSheet("font-weight: bold; color: #e74c3c;");
  }
}

void PageIEC61850Client::enableConnectionDependentUI(bool enabled)
{
  m_disconnectAction->setEnabled(enabled);
  m_refreshAction->setEnabled(enabled);
  m_dataModelTree->setEnabled(enabled);
  m_dataValueWidget->setEnabled(enabled);
}

void PageIEC61850Client::onDataModelLoadProgress(int value, const QString& message)
{
  if(m_progressBar)
  {
    m_progressBar->setValue(value);

    // Show progress bar when loading starts
    if(value > 0 && value < 100)
    {
      m_progressBar->setVisible(true);
    }

    // Hide progress bar when loading is complete
    if(value == 100)
    {
      m_progressBar->setVisible(false);
    }
  }
}

void PageIEC61850Client::showEvent(QShowEvent *event)
{
  QWidget::showEvent(event);

  // Force the widget to expand
  if(parentWidget())
  {
    resize(parentWidget()->size());
  }

  // Ensure splitters have proper sizes
  if(m_mainSplitter)
  {
    m_mainSplitter->setSizes({400, 800});
  }
  if(m_rightSplitter)
  {
    m_rightSplitter->setSizes({400, 200});
  }
}
