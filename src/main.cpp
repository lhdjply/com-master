#include "myresource.h"

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);

  // Fusion style
  app.setStyle(QStyleFactory::create("Fusion"));
  app.setWindowIcon(QIcon(":/icons/hicolor/scalable/apps/ly-serial-tool.svg"));

  // Translation setup
  QTranslator translator;
  const QStringList uiLanguages = QLocale::system().uiLanguages();
  for(const QString &locale : uiLanguages)
  {
    const QString baseName = "ly-serial-tool_" + QLocale(locale).name();
    if(translator.load(":/i18n/" + baseName))
    {
      app.installTranslator(&translator);
      break;
    }
  }

  // Create and show main window
  PageMainWindow pagemain;
  pagemain.show();

  return app.exec();
}
