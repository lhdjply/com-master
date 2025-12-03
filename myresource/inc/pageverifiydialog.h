#ifndef PAGEVERIFIYDIALOG_H
#define PAGEVERIFIYDIALOG_H

#include "qtinclude.h"

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
    void setupUi();
    void retranslateUi();

    QVBoxLayout * verticalLayout_2;
    QGroupBox * groupBox;
    QVBoxLayout * verticalLayout;
    QRadioButton * radioButton_1;
    QRadioButton * radioButton_2;
    QRadioButton * radioButton_3;
    QRadioButton * radioButton_4;
    QRadioButton * radioButton_5;
    QRadioButton * radioButton_6;
    QRadioButton * radioButton_7;
    QDialogButtonBox * selectbuttonbox;
};

#endif // PAGEVERIFIYDIALOG_H
