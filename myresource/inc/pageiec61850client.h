#ifndef PAGEIEC61850CLINET_H
#define PAGEIEC61850CLINET_H

#include "qtinclude.h"

struct IEC61850DataPoint {
    QString objectReference;
    QString functionalConstraint;
    QVariant value;
    QDateTime timestamp;
    QString quality;
    QString dataType;
};

struct IEC61850DataSet {
    QString reference;
    QStringList dataPoints;
    bool isDeletable;
};

struct IEC61850Report {
    QString reference;
    QString dataSetReference;
    bool isBuffered;
    bool isEnabled;
    QString reportId;
};

QT_BEGIN_NAMESPACE
class QSplitter;
class QTreeWidget;
class QTextEdit;
class QStatusBar;
class QToolBar;
class QAction;
class QLabel;
class QProgressBar;
QT_END_NAMESPACE

class ConnectionDialog;

class PageIEC61850Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit PageIEC61850Client(QWidget *parent = nullptr);
    ~PageIEC61850Client();

    // Connection management
    bool connect(const QString& hostname, int port = 102);
    void disconnect();
    bool isConnected() const;

    // Data model discovery
    QStringList getLogicalDeviceList();
    QStringList getLogicalNodeList(const QString& deviceName);
    QStringList getDataDirectory(const QString& objectReference);
    QStringList getLogicalNodeDirectory(const QString& nodeReference);
    QStringList getAllDataDirectories(const QString& objectReference);
    QStringList getDataSetDirectory(const QString& dataSetReference, bool* isDeletable = nullptr);
    
    // Data access services
    QVariant readValue(const QString& objectReference, const QString& functionalConstraint = "MX");
    bool writeValue(const QString& objectReference, const QVariant& value, const QString& functionalConstraint = "MX");
    QString getFunctionalConstraint(const QString& objectReference);
    
    // Data set services
    bool createDataSet(const QString& dataSetReference, const QStringList& dataPoints);
    bool deleteDataSet(const QString& dataSetReference);
    QList<QVariant> readDataSetValues(const QString& dataSetReference);
    
    // Report services
    QList<IEC61850Report> getReports();
    bool enableReport(const QString& reportReference, bool enable = true);
    
    // Control services
    bool controlObject(const QString& objectReference, const QVariant& value, bool selectBeforeOperate = false);

    // DataModelTree functionality
    void buildDataModel();
    void clearDataModel();
    
    // DataValueWidget functionality
    void showObjectDetails(const QString& objectReference);
    void clearObjectDetails();
    
    // UI getters
    QTreeWidget* getDataModelTreeWidget() const { return m_dataModelTree; }
    QWidget* getDataValueWidget() const { return m_dataValueWidget; }

private slots:
    // DataModelTree slots
    void onItemDoubleClicked(QTreeWidgetItem* item, int column);
    void onContextMenuRequested(const QPoint& pos);
    
    // DataValueWidget slots
    void readValue();
    void writeValue();
    
    // MainWindow slots
    void performConnection(const QString& hostname, int port);
    void disconnectFromDevice();
    void showConnectionDialog();
    void refreshDataModel();
    void onConnectionStateChanged(bool connected);
    void onDataModelItemDoubleClicked(const QString& objectReference);
    void onDataModelLoadProgress(int value, const QString& message);

private:
    // IEC61850 Client private methods
    QVariant mmsValueToQVariant(MmsValue* mmsValue);
    MmsValue* qVariantToMmsValue(const QVariant& value, const QString& dataType);
    QString getDataType(const QString& objectReference);
    
    // DataModelTree private methods
    void setupDataModelTreeUI();
    void setupDataModelTreeContextMenu();
    QTreeWidgetItem* addLogicalDevice(const QString& deviceName);
    QTreeWidgetItem* addLogicalNode(QTreeWidgetItem* deviceItem, const QString& nodeName);
    QTreeWidgetItem* addDataObject(QTreeWidgetItem* parentItem, const QString& objectName, const QString& objectReference, int depth = 0);
    
    // DataValueWidget private methods
    void setupDataValueWidgetUI();
    
    protected:
    void showEvent(QShowEvent *event) override;

private:
    // PageTabContent private methods
    void setupUi();
    void setupToolBar();
    void setupStatusBar();
    void updateConnectionStatus(bool connected);
    void enableConnectionDependentUI(bool enabled);

    // Connection members
    IedConnection m_connection;
    bool m_isConnected;
    mutable QMutex m_mutex;
    
    // Cache for data model
    QStringList m_logicalDevices;
    QMap<QString, QStringList> m_logicalNodes;
    QMap<QString, QStringList> m_dataDirectories;
    
    // DataModelTree members
    QTreeWidget* m_dataModelTree;
    QTreeWidgetItem* m_rootItem;
    QMenu* m_contextMenu;
    QAction* m_readValueAction;
    QAction* m_writeValueAction;
    QAction* m_controlAction;
    QAction* m_createDataSetAction;
    
    QMap<QString, QTreeWidgetItem*> m_deviceItems;
    QMap<QString, QTreeWidgetItem*> m_nodeItems;
    QMap<QString, QTreeWidgetItem*> m_objectItems;
    
    // DataValueWidget members
    QWidget* m_dataValueWidget;
    QVBoxLayout* m_mainLayout;
    QGroupBox* m_objectInfoGroup;
    QFormLayout* m_infoLayout;
    QLabel* m_objectReferenceLabel;
    QLabel* m_dataTypeLabel;
    QLabel* m_valueLabel;
    
    QGroupBox* m_readWriteGroup;
    QPushButton* m_readButton;
    QPushButton* m_writeButton;
    
    QLineEdit* m_writeValueEdit;
    
    QTableWidget* m_valueTable;
    
    QString m_currentObjectReference;

    // UI Components
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    // Toolbar
    QToolBar *m_toolBar;
    
    // Actions
    QAction *m_connectAction;
    QAction *m_disconnectAction;
    QAction *m_refreshAction;
    
    // Status bar
    QStatusBar *m_statusBar;
    QLabel *m_connectionStatusLabel;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
};

#endif // PAGEIEC61850CLINET_H
