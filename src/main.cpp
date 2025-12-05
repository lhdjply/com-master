#include "myresource.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char * argv[])
{
  QApplication app(argc, argv);

  // Fusion style
  app.setStyle(QStyleFactory::create("Fusion"));
#ifdef Q_OS_WIN
  app.setWindowIcon(QIcon(":/icons/windows/ly-serial-tool.ico"));
#else
  app.setWindowIcon(QIcon(":/icons/linux/hicolor/scalable/apps/ly-serial-tool.svg"));
#endif

  // Translation setup
  QTranslator translator;
#if defined(TRANSLATION_RESOURCE_EMBEDDING)
  const QString qmDir = u":/i18n/"_s;
#elif defined(QM_FILE_INSTALL_ABSOLUTE_DIR)
  const QString qmDir = QT_STRINGIFY(QM_FILE_INSTALL_ABSOLUTE_DIR);
#else
  const QString qmDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
#endif
  if(translator.load(QLocale(), u"ly-serial-tool"_s, u"_"_s, qmDir))
  {
    QCoreApplication::installTranslator(&translator);
  }

  // Create and show main window
  PageMainWindow pagemain;
  pagemain.show();

  return app.exec();
}
