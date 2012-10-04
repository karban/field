#include <QtGui/QApplication>

#include <QTranslator>
#include <QTextCodec>
#include <QDir>
#include <QString>

#include "util.h"
#include "mainwindow.h"

class ArgosApplication : public QApplication
{
public:
    ArgosApplication(int& argc, char ** argv) : QApplication(argc, argv) {}

    // reimplemented from QApplication so we can throw exceptions in slots
    virtual bool notify(QObject *receiver, QEvent *event)
    {
        try
        {
            return QApplication::notify(receiver, event);
        }
        catch (std::exception& e)
        {
            qCritical() << "Exception thrown: " << e.what();
            throw;
        }
        catch (AgrosException e)
        {
            qCritical() << "Exception thrown: " << e.what();
            throw;
        }
        catch (...)
        {
            qCritical() << "Unknown exception thrown";
            throw;
        }

        return false;
    }
};


int main(int argc, char *argv[])
{
    // start application
    QString str = QString("\n\n%1: Field").
            arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));

    QString location = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    QDir("/").mkpath(location);
    appendToFile(location, str);

    // register message handler
    // TODO: qInstallMsgHandler(logOutput);

    ArgosApplication a(argc, argv);
    // QApplication a(argc, argv);

#ifdef VERSION_BETA
    bool beta = true;
#else
    bool beta = false;
#endif

    a.setWindowIcon(icon("field"));
    a.setApplicationVersion(versionString(VERSION_MAJOR, VERSION_MINOR, VERSION_SUB, VERSION_GIT, VERSION_YEAR, VERSION_MONTH, VERSION_DAY, beta));
    a.setOrganizationName("hpfem.org");
    a.setOrganizationDomain("hpfem.org");
    a.setApplicationName("Field");

#ifdef Q_WS_MAC
    // don't show icons in menu
    a.setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    // parameters
    QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
    {
        if (args.contains( "--help") || args.contains("/help"))
        {
            cout << "field [fileName (*.fld; *.py) | -run fileName (*.py) | --help | --verbose]" << endl;
            exit(0);
            return 0;
        }
    }

    QSettings settings;

    // first run
    if (settings.value("General/GUIStyle").value<QString>().isEmpty())
    {
        QString styleName = "";
        QStringList styles = QStyleFactory::keys();

#ifdef Q_WS_X11
        // kde 3
        if (getenv("KDE_FULL_SESSION") != NULL)
            styleName = "Plastique";
        // kde 4
        if (getenv("KDE_SESSION_VERSION") != NULL)
        {
            if (styles.contains("Oxygen"))
                styleName = "Oxygen";
            else
                styleName = "Plastique";
        }
        // gtk+
        if (styleName == "")
            styleName = "GTK+";
#endif

#ifdef Q_WS_WIN
        if (styles.contains("WindowsVista"))
            styleName = "WindowsVista";
        else if (styles.contains("WindowsXP"))
            styleName = "WindowsXP";
        else
            styleName = "Windows";
#endif


#ifdef Q_WS_MAC
        styleName = "Aqua";
#endif

        settings.setValue("General/GUIStyle", styleName);
    }

    // setting gui style
    setGUIStyle(settings.value("General/GUIStyle").value<QString>());

    // language
    QString locale = settings.value("General/Language", QLocale::system().name()).value<QString>();
    setLanguage(locale);

    // init indicator (ubuntu - unity, windows - overlay icon, macosx - ???)
    Indicator::init();

    MainWindow w;
    w.show();

    return a.exec();
}
