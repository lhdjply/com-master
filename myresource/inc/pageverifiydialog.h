#ifndef PAGEVERIFIYDIALOG_H
#define PAGEVERIFIYDIALOG_H

#include "qtinclude.h"

namespace Ui
{
class PageVerifiyDialog;
}

class PageVerifiyDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit PageVerifiyDialog(QWidget *parent = nullptr);
    ~PageVerifiyDialog();

    enum VERIFIY_TYPE
    {
      CRC16_LOW = 0,
      CRC16_HIGH,
      CRC8,
      TOTAL_SUM,
      SUM_XOR,
      CRC32_LOW,
      CRC32_HIGH,
    };
    Q_ENUM(VERIFIY_TYPE)

  signals:
    void selectedIndexChanged(int selectedIndex);

  private slots:
    void on_selectbuttonbox_accepted();

  private:
    Ui::PageVerifiyDialog * ui;
};

#endif // PAGEVERIFIYDIALOG_H
