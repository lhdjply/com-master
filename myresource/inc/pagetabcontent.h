#ifndef PAGETABCONTENT_H
#define PAGETABCONTENT_H

#include "qtinclude.h"

class PageTabContent : public QWidget
{
    Q_OBJECT
  public:
    explicit PageTabContent(QWidget *parent = nullptr);

    // 添加信号，用于通知按钮点击
  signals:
    void buttonClicked(const QString &buttonName);

  private:
    QPushButton * transceiverButton;
    QPushButton * terminalButton;

    void setupUi();
};

#endif
