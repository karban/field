// This file is part of Agros2D.
//
// Agros2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Agros2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Agros2D.  If not, see <http://www.gnu.org/licenses/>.
//
// hp-FEM group (http://hpfem.org/)
// University of Nevada, Reno (UNR) and University of West Bohemia, Pilsen
// Email: agros2d@googlegroups.com, home page: http://hpfem.org/agros2d/

#ifdef WITH_UNITY
#include <unity.h>
#endif

#include "util.h"

#include "scene.h"
#include "pythonlabagros.h"
#include "style/manhattanstyle.h"

#include "util/constants.h"

#define M_PI_2		1.57079632679489661923	/* pi/2 */

static QHash<SceneViewPost3DMode, QString> sceneViewPost3DModeList;
static QHash<PaletteType, QString> paletteTypeList;
static QHash<VectorType, QString> vectorTypeList;
static QHash<VectorCenter, QString> vectorCenterList;

QStringList sceneViewPost3DModeStringKeys() { return sceneViewPost3DModeList.values(); }
QString sceneViewPost3DModeToStringKey(SceneViewPost3DMode sceneViewPost3DMode) { return sceneViewPost3DModeList[sceneViewPost3DMode]; }
SceneViewPost3DMode sceneViewPost3DModeFromStringKey(const QString &sceneViewPost3DMode) { return sceneViewPost3DModeList.key(sceneViewPost3DMode); }

QStringList paletteTypeStringKeys() { return paletteTypeList.values(); }
QString paletteTypeToStringKey(PaletteType paletteType) { return paletteTypeList[paletteType]; }
PaletteType paletteTypeFromStringKey(const QString &paletteType) { return paletteTypeList.key(paletteType); }

QStringList vectorTypeStringKeys() { return vectorTypeList.values(); }
QString vectorTypeToStringKey(VectorType vectorType) { return vectorTypeList[vectorType]; }
VectorType vectorTypeFromStringKey(const QString &vectorType) { return vectorTypeList.key(vectorType); }

QStringList vectorCenterStringKeys() { return vectorCenterList.values(); }
QString vectorCenterToStringKey(VectorCenter vectorCenter) { return vectorCenterList[vectorCenter]; }
VectorCenter vectorCenterFromStringKey(const QString &vectorCenter) { return vectorCenterList.key(vectorCenter); }

void initLists()
{
    // post3d
    //sceneViewPost3DModeList.insert(SceneViewPost3DMode_Undefined, "");
    sceneViewPost3DModeList.insert(SceneViewPost3DMode_None, "none");
    sceneViewPost3DModeList.insert(SceneViewPost3DMode_ScalarView3D, "scalar");
    sceneViewPost3DModeList.insert(SceneViewPost3DMode_ParticleTracing, "particletracing");
    sceneViewPost3DModeList.insert(SceneViewPost3DMode_Model, "model");

    // PaletteType
    paletteTypeList.insert(Palette_Agros2D, "agros2d");
    paletteTypeList.insert(Palette_Jet, "jet");
    paletteTypeList.insert(Palette_Copper, "copper");
    paletteTypeList.insert(Palette_Hot, "hot");
    paletteTypeList.insert(Palette_Bone, "bone");
    paletteTypeList.insert(Palette_Pink, "pink");
    paletteTypeList.insert(Palette_Spring, "spring");
    paletteTypeList.insert(Palette_Summer, "summer");
    paletteTypeList.insert(Palette_Autumn, "autumn");
    paletteTypeList.insert(Palette_Winter, "winter");
    paletteTypeList.insert(Palette_HSV, "hsv");
    paletteTypeList.insert(Palette_BWAsc, "bw_ascending");
    paletteTypeList.insert(Palette_BWDesc, "bw_descending");

    // VectorType
    vectorTypeList.insert(VectorType_Arrow, "arrow");
    vectorTypeList.insert(VectorType_Cone, "cone");

    // VectorCenter
    vectorCenterList.insert(VectorCenter_Tail, "tail");
    vectorCenterList.insert(VectorCenter_Head, "head");
    vectorCenterList.insert(VectorCenter_Center, "center");
}

QString stringListToString(const QStringList &list)
{
    QString out;
    foreach (QString str, list)
        if (!str.isEmpty())
            out += str + ", ";

    if (out.length() > 0)
        out = out.left(out.length() - 2);

    return out;
}

QString paletteTypeString(PaletteType paletteType)
{
    switch (paletteType)
    {
    case Palette_Agros2D:
        return QObject::tr("Agros2D");
    case Palette_Jet:
        return QObject::tr("Jet");
    case Palette_Copper:
        return QObject::tr("Copper");
    case Palette_Hot:
        return QObject::tr("Hot");
    case Palette_Cool:
        return QObject::tr("Cool");
    case Palette_Bone:
        return QObject::tr("Bone");
    case Palette_Pink:
        return QObject::tr("Pink");
    case Palette_Spring:
        return QObject::tr("Spring");
    case Palette_Summer:
        return QObject::tr("Summer");
    case Palette_Autumn:
        return QObject::tr("Autumn");
    case Palette_Winter:
        return QObject::tr("Winter");
    case Palette_HSV:
        return QObject::tr("HSV");
    case Palette_BWAsc:
        return QObject::tr("B/W ascending");
    case Palette_BWDesc:
        return QObject::tr("B/W descending");
    default:
        std::cerr << "Palette type '" + QString::number(paletteType).toStdString() + "' is not implemented. paletteTypeString(PaletteType paletteType)" << endl;
        throw;
    }
}

QString vectorTypeString(VectorType vectorType)
{
    switch (vectorType)
    {
    case VectorType_Arrow:
        return QObject::tr("Arrow");
    case VectorType_Cone:
        return QObject::tr("Cone");
    default:
        std::cerr << "Vector type '" + QString::number(vectorType).toStdString() + "' is not implemented. vectorTypeString(VectorType vectorType)" << endl;
        throw;
    }
}

QString vectorCenterString(VectorCenter vectorCenter)
{
    switch (vectorCenter)
    {
    case VectorCenter_Tail:
        return QObject::tr("Tail");
    case VectorCenter_Head:
        return QObject::tr("Head");
    case VectorCenter_Center:
        return QObject::tr("Center");
    default:
        std::cerr << "Vector center '" + QString::number(vectorCenter).toStdString() + "' is not implemented. vectorCenterString(VectorCenter vectorCenter)" << endl;
        throw;
    }
}

void setGUIStyle(const QString &styleName)
{
    QStyle *style = NULL;
    if (styleName == "Manhattan")
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
#else
        styleName = "Plastique";
#endif

        style = new ManhattanStyle(styleName);
    }
    else
    {
        // standard style
        style = QStyleFactory::create(styleName);
    }

    QApplication::setStyle(style);
    if (QApplication::desktopSettingsAware())
    {
        QApplication::setPalette(QApplication::palette());
    }
}

void setLanguage(const QString &locale)
{
    // non latin-1 chars
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    QTranslator *qtTranslator = new QTranslator();
    QTranslator *appTranslator = new QTranslator();

    QString country = locale.section('_',0,0);
    if (QFile::exists(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qt_" + country + ".qm"))
        qtTranslator->load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qt_" + country + ".qm");
    else if (QFile::exists(datadir() + LANGUAGEROOT + "/qt_" + country + ".qm"))
        qtTranslator->load(datadir() + LANGUAGEROOT + "/qt_" + country + ".qm");
    else
        qDebug() << "Qt language file not found.";

    if (QFile::exists(datadir() + LANGUAGEROOT + QDir::separator() + locale + ".qm"))
        appTranslator->load(datadir() + LANGUAGEROOT + QDir::separator() + locale + ".qm");
    else if (QFile::exists(datadir() + LANGUAGEROOT + "/en_US.qm"))
        appTranslator->load(datadir() + LANGUAGEROOT + "/en_US.qm");
    else
        qDebug() << "Language file not found.";

    QApplication::installTranslator(qtTranslator);
    QApplication::installTranslator(appTranslator);
}

QStringList availableLanguages()
{
    QDir dir;
    dir.setPath(datadir() + LANGUAGEROOT);

    // add all translations
    QStringList filters;
    filters << "*.qm";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    // remove extension
    QStringList list = dir.entryList();
    list.replaceInStrings(".qm", "");

    // remove system translations
    foreach (QString str, list)
        if (str.startsWith("qt_"))
            list.removeOne(str);

    return list;
}

QIcon icon(const QString &name)
{
    QString fileName;

#ifdef Q_WS_WIN
    if (QFile::exists(":/" + name + "-windows.png")) return QIcon(":/" + name + "-windows.png");
#endif

#ifdef Q_WS_X11
#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    return QIcon::fromTheme(name, QIcon(":/" + name + ".png"));
#endif

    QDir dir;

    QString style = "";
    QStringList styles = QStyleFactory::keys();

    // kde 3
    if (getenv("KDE_FULL_SESSION") != NULL)
    {}
    // kde 4
    if (getenv("KDE_SESSION_VERSION") != NULL)
    {
        // oxygen
        fileName = "/usr/share/icons/oxygen/22x22/actions/" + name;
        if (QFile::exists(fileName + ".svg")) return QIcon(fileName + ".svg");
        if (QFile::exists(fileName + ".png")) return QIcon(fileName + ".png");
    }
    // gtk+
    if (style == "")
    {
        // humanity (disabled - corrupted svg reader - Qt 4.6 has new method QIcon::fromTheme)
        // fileName = "/usr/share/icons/Humanity/actions/24/" + name;
        // if (QFile::exists(fileName + ".svg")) return QIcon(fileName + ".svg");
        // if (QFile::exists(fileName + ".png")) return QIcon(fileName + ".png");
    }
#endif

    if (QFile::exists(":/" + name + ".svg")) return QIcon(":/" + name + ".svg");
    if (QFile::exists(":/" + name + ".png")) return QIcon(":/" + name + ".png");

    return QIcon();
}

QString datadir()
{
    // windows and local installation
    if (QFile::exists(QApplication::applicationDirPath() + "/functions.py"))
        return QApplication::applicationDirPath();

    // linux
    if (QFile::exists(QApplication::applicationDirPath() + "/../share/field/functions.py"))
        return QApplication::applicationDirPath() + "/../share/field";

    qCritical() << "Datadir not found.";
    exit(1);
}

QString tempProblemDir()
{
    QDir(QDir::temp().absolutePath()).mkpath("field/" + QString::number(QApplication::applicationPid()));

    return QString("%1/field/%2").arg(QDir::temp().absolutePath()).arg(QApplication::applicationPid());
}

QString tempProblemFileName()
{
    return tempProblemDir() + "/temp";
}

QTime milisecondsToTime(int ms)
{
    // store the current ms remaining
    int tmp_ms = ms;

    // the amount of days left
    int days = floorf(tmp_ms/86400000);
    // adjust tmp_ms to leave remaining hours, minutes, seconds
    tmp_ms = tmp_ms - (days * 86400000);

    // calculate the amount of hours remaining
    int hours = floorf(tmp_ms/3600000);
    // adjust tmp_ms to leave the remaining minutes and seconds
    tmp_ms = tmp_ms - (hours * 3600000);

    // the amount of minutes remaining
    int mins = floorf(tmp_ms/60000);
    //adjust tmp_ms to leave only the remaining seconds
    tmp_ms = tmp_ms - (mins * 60000);

    // seconds remaining
    int secs = floorf(tmp_ms/1000);

    // milliseconds remaining
    tmp_ms = tmp_ms - (secs * 1000);

    return QTime(hours, mins, secs, tmp_ms);
}

bool removeDirectory(const QDir &dir)
{
    bool error = false;

    if (dir.exists())
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
        int count = entries.size();
        for (int idx = 0; idx < count; idx++)
        {
            QFileInfo entryInfo = entries[idx];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                error = removeDirectory(QDir(path));
            }
            else
            {
                QFile file(path);
                if (!file.remove())
                {
                    error = true;
                    break;
                }
            }
        }
        if (!dir.rmdir(dir.absolutePath()))
            error = true;
    }

    return error;
}

void msleep(unsigned long msecs)
{
    QWaitCondition w;
    QMutex sleepMutex;
    sleepMutex.lock();
    w.wait(&sleepMutex, msecs);
    sleepMutex.unlock();
}

void appendToFile(const QString &fileName, const QString &str)
{
    QFile file(fileName);

    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream outFile(&file);
        outFile << str << endl;

        file.close();
    }
}

void showPage(const QString &str)
{
    if (str.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(datadir() + "/resources/help/index.html"));
    else
        QDesktopServices::openUrl(QUrl::fromLocalFile(datadir() + "/resources/help/" + str));
}


QString readFileContent(const QString &fileName)
{
    QString content;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        content = stream.readAll();
        file.close();
        return content;
    }
    return NULL;
}

void writeStringContent(const QString &fileName, QString content)
{
    writeStringContent(fileName, &content);
}

void writeStringContent(const QString &fileName, QString *content)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << *content;

        file.waitForBytesWritten(0);
        file.close();
    }
}

QByteArray readFileContentByteArray(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray content = file.readAll();
        file.close();
        return content;
    }
    return NULL;
}

void writeStringContentByteArray(const QString &fileName, QByteArray content)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(content);

        file.waitForBytesWritten(0);
        file.close();
    }
}

QString unitToHTML(const QString &str)
{
    // dirty hack
    QString out = str;

    out.replace("-2", "<sup>&#8722;2</sup>");
    out.replace("-3", "<sup>&#8722;3</sup>");
    out.replace("2", "<sup>2</sup>");
    out.replace("3", "<sup>3</sup>");

    return out;
}

