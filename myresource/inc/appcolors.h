#ifndef APPCOLORS_H
#define APPCOLORS_H

#include <QString>

namespace AppColors
{

namespace Primary
{
const QString BLUE = "#3498db";
const QString BLUE_HOVER = "#2980b9";
const QString BLUE_PRESSED = "#21618c";
}

namespace Success
{
const QString GREEN = "#27ae60";
const QString GREEN_HOVER = "#219653";
const QString GREEN_PRESSED = "#1e874b";
const QString GREEN_START = "#4CAF50";
}

namespace Danger
{
const QString RED = "#e74c3c";
const QString RED_HOVER = "#c0392b";
const QString RED_PRESSED = "#a93226";
const QString RED_STOP = "#F44336";
}

namespace Neutral
{
const QString BG_LIGHT = "#f5f7fa";
const QString BG_ALTERNATE = "#f8fafc";
const QString BG_HEADER = "#ecf0f1";
const QString BG_WHITE = "white";
const QString BG_DISABLED = "#bdc3c7";
const QString BG_DIALOG_START = "#f6f9fc";
const QString BG_DIALOG_END = "#eef2f7";
const QString BG_CANCEL = "#6c757d";
const QString BG_CANCEL_HOVER = "#5a6268";
const QString BORDER_DIALOG = "#d1d9e6";
const QString TEXT_DIALOG_TITLE = "#4a6ee0";
const QString BORDER_FOCUS = "#8ba3f0";
}

namespace Text
{
const QString PRIMARY = "#2c3e50";
const QString SECONDARY = "#333333";
const QString DISABLED = "#7f8c8d";
const QString WHITE = "white";
const QString BLACK = "black";
}

namespace Border
{
const QString DEFAULT = "#c0c6d0";
}

namespace Disabled
{
const QString BG = "#CCCCCC";
const QString TEXT = "#666666";
}

inline QString getStyleSheet()
{
  return QString(
           "QMainWindow {"
           "    background-color: %1;"
           "    color: %2;"
           "    font-family: 'Segoe UI', Arial, sans-serif;"
           "}"
           "QGroupBox {"
           "    font-weight: bold;"
           "    border: 1px solid %3;"
           "    border-radius: 6px;"
           "    margin-top: 12px;"
           "    padding-top: 12px;"
           "    background-color: %4;"
           "}"
           "QGroupBox::title {"
           "    subcontrol-origin: margin;"
           "    subcontrol-position: top center;"
           "    padding: 0 8px;"
           "    background-color: %4;"
           "    color: %5;"
           "}"
           "QPushButton {"
           "    background-color: %6;"
           "    border: none;"
           "    border-radius: 4px;"
           "    color: %7;"
           "    padding: 8px 16px;"
           "    font-weight: bold;"
           "    min-height: 20px;"
           "}"
           "QPushButton:hover {"
           "    background-color: %8;"
           "}"
           "QPushButton:pressed {"
           "    background-color: %9;"
           "}"
           "QPushButton:disabled {"
           "    background-color: %10;"
           "    color: %11;"
           "}"
           "QComboBox {"
           "    border: 1px solid %3;"
           "    border-radius: 4px;"
           "    padding: 6px 12px;"
           "    background-color: %4;"
           "    selection-background-color: %6;"
           "    selection-color: %7;"
           "    min-width: 120px;"
           "    color: %12;"
           "}"
           "QComboBox::drop-down {"
           "    subcontrol-origin: padding;"
           "    subcontrol-position: top right;"
           "    width: 20px;"
           "    border-left: 1px solid %3;"
           "}"
           "QComboBox::down-arrow {"
           "    image: none;"
           "    border-left: 4px solid transparent;"
           "    border-right: 4px solid transparent;"
           "    border-top: 4px solid %11;"
           "    width: 0;"
           "    height: 0;"
           "}"
           "QTextEdit {"
           "    border: 1px solid %3;"
           "    border-radius: 4px;"
           "    padding: 8px;"
           "    background-color: %4;"
           "    font-family: 'Consolas', 'Monaco', monospace;"
           "}"
           "QCheckBox {"
           "    spacing: 6px;"
           "}"
           "QCheckBox::indicator {"
           "    width: 16px;"
           "    height: 16px;"
           "}"
           "QCheckBox::indicator:unchecked {"
           "    border: 1px solid %3;"
           "    border-radius: 2px;"
           "    background-color: %4;"
           "}"
           "QCheckBox::indicator:checked {"
           "    border: 1px solid %6;"
           "    border-radius: 2px;"
           "    background-color: %6;"
           "}"
           "QLabel {"
           "    color: %5;"
           "    font-weight: 500;"
           "}"
           "QLineEdit {"
           "    border: 1px solid %3;"
           "    border-radius: 4px;"
           "    padding: 6px 8px;"
           "    background-color: %4;"
           "}"
           "QLineEdit:focus {"
           "    border-color: %6;"
           "}"
           "QListWidget {"
           "    border: 1px solid %3;"
           "    border-radius: 4px;"
           "    background-color: %4;"
           "}"
         ).arg(Neutral::BG_LIGHT)
         .arg(Text::SECONDARY)
         .arg(Border::DEFAULT)
         .arg(Neutral::BG_WHITE)
         .arg(Text::PRIMARY)
         .arg(Primary::BLUE)
         .arg(Text::WHITE)
         .arg(Primary::BLUE_HOVER)
         .arg(Primary::BLUE_PRESSED)
         .arg(Neutral::BG_DISABLED)
         .arg(Text::DISABLED)
         .arg(Text::BLACK);
}

inline QString getSuccessButtonStyle()
{
  return QString(
           "QPushButton {"
           "    background-color: %1;"
           "    padding: 10px;"
           "}"
           "QPushButton:hover {"
           "    background-color: %2;"
           "}"
           "QPushButton:pressed {"
           "    background-color: %3;"
           "}"
         ).arg(Success::GREEN)
         .arg(Success::GREEN_HOVER)
         .arg(Success::GREEN_PRESSED);
}

inline QString getDangerButtonStyle()
{
  return QString(
           "QPushButton {"
           "    background-color: %1;"
           "    padding: 10px;"
           "}"
           "QPushButton:hover {"
           "    background-color: %2;"
           "}"
           "QPushButton:pressed {"
           "    background-color: %3;"
           "}"
         ).arg(Danger::RED)
         .arg(Danger::RED_HOVER)
         .arg(Danger::RED_PRESSED);
}

inline QString getStartButtonStyle()
{
  return QString(
           "QPushButton { background-color: %1; color: %2; } "
           "QPushButton:disabled { background-color: %3; color: %4; }"
         ).arg(Success::GREEN_START)
         .arg(Text::WHITE)
         .arg(Disabled::BG)
         .arg(Disabled::TEXT);
}

inline QString getStopButtonStyle()
{
  return QString(
           "QPushButton { background-color: %1; color: %2; } "
           "QPushButton:disabled { background-color: %3; color: %4; }"
         ).arg(Danger::RED_STOP)
         .arg(Text::WHITE)
         .arg(Disabled::BG)
         .arg(Disabled::TEXT);
}

inline QString getReceiveLabelStyle()
{
  return QString("font-weight: bold; color: %1;").arg(Danger::RED);
}

inline QString getSendLabelStyle()
{
  return QString("font-weight: bold; color: %1;").arg(Primary::BLUE);
}

inline QString getConnectionConnectedStyle()
{
  return QString("font-weight: bold; color: %1;").arg(Success::GREEN);
}

inline QString getConnectionDisconnectedStyle()
{
  return QString("font-weight: bold; color: %1;").arg(Danger::RED);
}

inline QString getVersionLabelStyle()
{
  return QString("QLabel { color: %1; font-size: 14px; }").arg(Text::DISABLED);
}

inline QString getInfoLabelStyle()
{
  return QString("font-weight: bold; color: %1;").arg(Text::PRIMARY);
}

inline QString getDialogStyleSheet()
{
  return QString(
           "/* 主窗口样式 */\n"
           "QDialog {\n"
           "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
           "                                stop: 0 %1, stop: 1 %2);\n"
           "    font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;\n"
           "}\n"
           "/* 按钮盒样式 */\n"
           "QDialogButtonBox {\n"
           "    background: transparent;\n"
           "}\n"
           "\n"
           "QDialogButtonBox QPushButton {\n"
           "    background-color: %3;\n"
           "    border: none;\n"
           "    border-radius: 6px;\n"
           "    color: %4;\n"
           "    padding: 8px 16px;\n"
           "    font-weight: 500;\n"
           "    min-width: 80px;\n"
           "}\n"
           "\n"
           "QDialogButtonBox QPushButton:hover {\n"
           "    background-color: %5;\n"
           "}\n"
           "\n"
           "QDialogButtonBox QPushButton:pressed {\n"
           "    background-color: %6;\n"
           "}\n"
           "\n"
           "QDialogButtonBox QPushButton:focus {\n"
           "    outline: none;\n"
           "    border: 2px solid %7;\n"
           "}\n"
           "\n"
           "/* 取消按钮样式 */\n"
           "QDialogButtonBox QPushButton[text=\"Cancel\"] {\n"
           "    background-color: %8;\n"
           "}\n"
           "\n"
           "QDialogButtonBox QPushButton[text=\"Cancel\"]:hover {\n"
           "    background-color: %9;\n"
           "}\n"
           "\n"
           "/* 组框样式 */\n"
           "QGroupBox {\n"
           "    background-color: %10;\n"
           "    border: 1px solid %11;\n"
           "    border-radius: 10px;\n"
           "    margin-top: 10px;\n"
           "    padding-top: 10px;\n"
           "    font-weight: bold;\n"
           "    color: %12;\n"
           "}\n"
           "\n"
           "QGroupBox::title {\n"
           "    subcontrol-origin: margin;\n"
           "    subcontrol-position: top center;\n"
           "    padding: 0 8px;\n"
           "    background-color: %10;\n"
           "    color: %13;\n"
           "    font-weight: 600;\n"
           "}\n"
           "\n"
           "QRadioButton {\n"
           "    spacing: 6px;\n"
           "}\n"
           "QRadioButton::indicator {\n"
           "    width: 16px;\n"
           "    height: 16px;\n"
           "}\n"
           "QRadioButton::indicator:unchecked {\n"
           "    border: 1px solid %14;\n"
           "    border-radius: 2px;\n"
           "    background-color: %10;\n"
           "}\n"
           "QRadioButton::indicator:checked {\n"
           "    border: 1px solid %3;\n"
           "    border-radius: 2px;\n"
           "    background-color: %3;\n"
           "}"
         ).arg(Neutral::BG_DIALOG_START)
         .arg(Neutral::BG_DIALOG_END)
         .arg(Primary::BLUE)
         .arg(Text::WHITE)
         .arg(Primary::BLUE_HOVER)
         .arg(Primary::BLUE_PRESSED)
         .arg(Neutral::BORDER_FOCUS)
         .arg(Neutral::BG_CANCEL)
         .arg(Neutral::BG_CANCEL_HOVER)
         .arg(Neutral::BG_WHITE)
         .arg(Neutral::BORDER_DIALOG)
         .arg(Text::PRIMARY)
         .arg(Neutral::TEXT_DIALOG_TITLE)
         .arg(Border::DEFAULT);
}

}

#endif