#ifndef PAGEMAIN_H
#define PAGEMAIN_H

#include "qtinclude.h"
#include "pagetabcontent.h"

class PageMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit PageMainWindow(QWidget *parent = nullptr);

  private:
    void setupUi();
    void addNewTab();
    void closeTab(int index);
    void opentoolpage(const QString &buttonName);
    int findTabWithContent(PageTabContent *content);

    // UI components
    QWidget * centralWidget;
    QGridLayout * gridLayout;
    QTabWidget * tabWidget;
    QWidget * tabBarWidget;
    QHBoxLayout * tabBarLayout;
    QPushButton * addTabButton;
};

#endif
