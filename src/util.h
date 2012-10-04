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

#ifndef UTIL_H
#define UTIL_H

#include <QtCore>
#include <QtGui>
#include <QtNetwork>
#include <QDomDocument>
#include <QtWebKit>
#include <QtXmlPatterns>
#include <QtPlugin>

#include <typeinfo>
#include <iostream>
#include <cmath>
#include <limits>
#include <vector>

#include <locale.h>
#include <stdlib.h>

#include "value.h"
#include "util/point.h"

#include "indicators/indicators.h"

// zero
#define EPS_ZERO 1e-10

// physical constants
#define EPS0 8.854e-12
#define MU0 4*M_PI*1e-7
#define SIGMA0 5.670373e-8
#define PRESSURE_MIN_AIR 20e-6
#define GRAVITATIONAL_ACCELERATION 9.81

#define deg2rad(degrees) (degrees*M_PI/180.0)
#define rad2deg(radians) (radians*180.0/M_PI)

using namespace std;

// sign function
inline int sign(double arg)
{
    if (arg > 0)
        return 1;
    else if (arg < 0)
        return -1;
    else
        return 0;
}

// show page
void showPage(const QString &str = "");

// set gui style
void setGUIStyle(const QString &styleName);

// set language
void setLanguage(const QString &locale);

// get available languages
QStringList availableLanguages();

// get icon with respect to actual theme
QIcon icon(const QString &name);

// get datadir
QString datadir();

// get external js functions
QString externalFunctions();

// get temp dir
QString tempProblemDir();

// get temp filename
QString tempProblemFileName();

// convert time in ms to QTime
QTime milisecondsToTime(int ms);

// remove directory content
bool removeDirectory(const QDir &dir);

// sleep function
void msleep(unsigned long msecs);

// read file content
QByteArray readFileContentByteArray(const QString &fileName);
QString readFileContent(const QString &fileName);

// write content into the file
void writeStringContent(const QString &fileName, QString content);
void writeStringContent(const QString &fileName, QString *content);
void writeStringContentByteArray(const QString &fileName, QByteArray content);

// append to the file
void appendToFile(const QString &fileName, const QString &str);

// join version
inline QString versionString(int major, int minor, int sub, int git, int year, int month, int day, bool beta)
{
    return QString("%1.%2.%3.%4 %5 (%6-%7-%8)")
            .arg(major)
            .arg(minor)
            .arg(sub)
            .arg(git)
            .arg(beta ? "beta" : "")
            .arg(year)
            .arg(QString("0%1").arg(month).right(2))
            .arg(QString("0%1").arg(day).right(2));
}

// dirty html unit replace
QString unitToHTML(const QString &str);

class AgrosException
{
public:
    AgrosException(const QString &what)
        : m_what(what)
    {
    }

    inline QString what() { return m_what; }
    inline QString toString(){ return m_what; }

protected:
    QString m_what;
};

struct ScriptResult
{
    ScriptResult()
    {
        text = "";
        traceback = "";
        isError = false;
        this->line = -1;
    }

    ScriptResult(const QString &text, const QString &traceback, bool isError = false, int line = -1)
    {
        this->text = text;
        this->traceback = traceback;
        this->isError = isError;
        this->line = line;
    }

    QString text;
    QString traceback;
    bool isError;
    int line;
};

struct ExpressionResult
{
    ExpressionResult()
    {
        this->error = "";
        this->traceback = "";
        this->value = 0.0;
    }

    ExpressionResult(double value, const QString &traceback, const QString &error)
    {
        this->error = error;
        this->traceback = traceback;
        this->value = value;
    }

    QString error;
    QString traceback;
    double value;
};

enum ErrorResultType
{
    ErrorResultType_None,
    ErrorResultType_Information,
    ErrorResultType_Warning,
    ErrorResultType_Critical
};

class ErrorResult
{
public:
    inline ErrorResultType type() { return m_type; }
    inline QString message() { return m_message; }

    inline ErrorResult()
    {
        m_type = ErrorResultType_None;
        m_message = "";
    }

    inline ErrorResult(ErrorResultType type, QString message)
    {
        m_type = type;
        m_message = message;
    }

    inline bool isError() { return (m_type != ErrorResultType_None); }

    void showDialog()
    {
        switch (m_type)
        {
        case ErrorResultType_None:
            return;
        case ErrorResultType_Information:
            QMessageBox::information(QApplication::activeWindow(), QObject::tr("Information"), m_message);
            break;
        case ErrorResultType_Warning:
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"), m_message);
            break;
        case ErrorResultType_Critical:
            QMessageBox::critical(QApplication::activeWindow(), QObject::tr("Critical"), m_message);
            break;
        }
    }

private:
    ErrorResultType m_type;
    QString m_message;
};

enum SceneGeometryMode
{
    SceneGeometryMode_OperateOnNodes
};

enum MouseSceneMode
{
    MouseSceneMode_Nothing,
    MouseSceneMode_Pan,
    MouseSceneMode_Rotate,
    MouseSceneMode_Move,
    MouseSceneMode_Add
};

enum SceneModePostprocessor
{
    SceneModePostprocessor_LocalValue,
    SceneModePostprocessor_SurfaceIntegral,
    SceneModePostprocessor_VolumeIntegral
};

enum PaletteType
{
    Palette_Agros2D,
    Palette_Jet,
    Palette_Copper,
    Palette_Hot,
    Palette_Cool,
    Palette_Bone,
    Palette_Pink,
    Palette_Spring,
    Palette_Summer,
    Palette_Autumn,
    Palette_Winter,
    Palette_HSV,
    Palette_BWAsc,
    Palette_BWDesc
};

enum SceneViewPost3DMode
{
    //SceneViewPost3DMode_Undefined,
    SceneViewPost3DMode_None,
    SceneViewPost3DMode_ScalarView3D,
    SceneViewPost3DMode_Model,
    SceneViewPost3DMode_ParticleTracing
};

enum SceneTransformMode
{
    SceneTransformMode_Translate,
    SceneTransformMode_Rotate,
    SceneTransformMode_Scale
};

enum VectorType
{
    VectorType_Arrow,
    VectorType_Cone
};

enum VectorCenter
{
    VectorCenter_Tail,
    VectorCenter_Head,
    VectorCenter_Center
};

QString stringListToString(const QStringList &list);

// keys
void initLists();

// scene view 3d mode
QStringList sceneViewPost3DModeStringKeys();
QString sceneViewPost3DModeToStringKey(SceneViewPost3DMode sceneViewPost3DMode);
SceneViewPost3DMode sceneViewPost3DModeFromStringKey(const QString &sceneViewPost3DMode);

// palette type
QStringList paletteTypeStringKeys();
QString paletteTypeToStringKey(PaletteType paletteType);
PaletteType paletteTypeFromStringKey(const QString &paletteType);
QString paletteTypeString(PaletteType paletteType);

// vector type
QString vectorTypeString(VectorType vectorType);
QStringList vectorTypeStringKeys();
QString vectorTypeToStringKey(VectorType vectorType);
VectorType vectorTypeFromStringKey(const QString &vectorType);

// vector center
QString vectorCenterString(VectorCenter vectorCenter);
QStringList vectorCenterStringKeys();
QString vectorCenterToStringKey(VectorCenter vectorCenter);
VectorCenter vectorCenterFromStringKey(const QString &vectorCenter);

#endif // UTIL_H
