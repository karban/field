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

#include "conf.h"
#include "scene.h"

#include "util/constants.h"

Config::Config() : eleConfig(NULL)
{
    load();
}

Config::~Config()
{
    save();
}

void Config::load()
{
    loadWorkspace();
    loadPostprocessor(NULL);
    loadAdvanced();
}

void Config::loadWorkspace()
{
    QSettings settings;

    // general
    guiStyle = settings.value("General/GUIStyle").toString();
    language = settings.value("General/Language", QLocale::system().name()).toString();
    defaultPhysicField = settings.value("General/DefaultPhysicField", "electrostatics").toString();

    checkVersion = settings.value("General/CheckVersion", true).toBool();
    lineEditValueShowResult = settings.value("General/LineEditValueShowResult", false).toBool();
    saveProblemWithSolution = settings.value("Solver/SaveProblemWithSolution", false).toBool();

    // zoom
    zoomToMouse = settings.value("Geometry/ZoomToMouse", true).toBool();

    // colors
    colorBackground = settings.value("SceneViewSettings/ColorBackground", COLORBACKGROUND).value<QColor>();
    colorGrid = settings.value("SceneViewSettings/ColorGrid", COLORGRID).value<QColor>();
    colorCross = settings.value("SceneViewSettings/ColorCross", COLORCROSS).value<QColor>();
    colorNodes = settings.value("SceneViewSettings/ColorNodes", COLORNODES).value<QColor>();
    colorEdges = settings.value("SceneViewSettings/ColorEdges", COLOREDGES).value<QColor>();
    colorLabels = settings.value("SceneViewSettings/ColorLabels", COLORLABELS).value<QColor>();
    colorContours = settings.value("SceneViewSettings/ColorContours", COLORCONTOURS).value<QColor>();
    colorVectors = settings.value("SceneViewSettings/ColorVectors", COLORVECTORS).value<QColor>();
    colorInitialMesh = settings.value("SceneViewSettings/ColorInitialMesh", COLORINITIALMESH).value<QColor>();
    colorSolutionMesh = settings.value("SceneViewSettings/ColorSolutionMesh", COLORSOLUTIONMESH).value<QColor>();
    colorHighlighted = settings.value("SceneViewSettings/ColorHighlighted", COLORHIGHLIGHTED).value<QColor>();
    colorSelected = settings.value("SceneViewSettings/ColorSelected", COLORSELECTED).value<QColor>();
    colorCrossed = settings.value("SceneViewSettings/ColorCrossed", COLORCROSSED).value<QColor>();
    colorNotConnected = settings.value("SceneViewSettings/ColorCrossed", COLORNOTCONNECTED).value<QColor>();

    // geometry
    nodeSize = settings.value("SceneViewSettings/NodeSize", GEOMETRYNODESIZE).toDouble();

    // font
    sceneFont = settings.value("SceneViewSettings/SceneFont", FONT).value<QFont>();

    // axes
    showAxes = settings.value("SceneViewSettings/ShowAxes", SHOWAXES).toBool();  

    // 3d
    scalarView3DLighting = settings.value("SceneViewSettings/ScalarView3DLighting", false).toBool();
    scalarView3DAngle = settings.value("SceneViewSettings/ScalarView3DAngle", 270).toDouble();
    scalarView3DBackground = settings.value("SceneViewSettings/ScalarView3DBackground", true).toBool();
}

void Config::loadPostprocessor(QDomElement *config)
{
    eleConfig = config;

    // active field
    activeField = readConfig("SceneViewSettings/ActiveField", QString());

    // view
    showPost3D = (SceneViewPost3DMode) readConfig("SceneViewSettings/ShowPost3D", (int) SCALARSHOWPOST3D);

    showScalarColorBar = readConfig("SceneViewSettings/ShowScalarColorBar", SHOWSCALARCOLORBAR);
    paletteType = (PaletteType) readConfig("SceneViewSettings/PaletteType", PALETTETYPE);
    paletteFilter = readConfig("SceneViewSettings/PaletteFilter", PALETTEFILTER);
    paletteSteps = readConfig("SceneViewSettings/PaletteSteps", PALETTESTEPS);

    eleConfig = NULL;
}

void Config::loadAdvanced()
{
    QSettings settings;
    // number of threads

    // global script
    globalScript = settings.value("Python/GlobalScript", "").toString();
}

void Config::save()
{
    saveWorkspace();
    saveAdvanced();
}

void Config::saveWorkspace()
{
    QSettings settings;

    // general
    settings.setValue("General/GUIStyle", guiStyle);
    settings.setValue("General/Language", language);
    settings.setValue("General/DefaultPhysicField", defaultPhysicField);

    settings.setValue("General/CheckVersion", checkVersion);
    settings.setValue("General/LineEditValueShowResult", lineEditValueShowResult);
    settings.setValue("General/SaveProblemWithSolution", saveProblemWithSolution);

    // font
    settings.setValue("SceneViewSettings/SceneFont", sceneFont);

    // zoom
    settings.setValue("General/ZoomToMouse", zoomToMouse);

    // colors
    settings.setValue("SceneViewSettings/ColorBackground", colorBackground);
    settings.setValue("SceneViewSettings/ColorGrid", colorGrid);
    settings.setValue("SceneViewSettings/ColorCross", colorCross);
    settings.setValue("SceneViewSettings/ColorNodes", colorNodes);
    settings.setValue("SceneViewSettings/ColorEdges", colorEdges);
    settings.setValue("SceneViewSettings/ColorLabels", colorLabels);
    settings.setValue("SceneViewSettings/ColorContours", colorContours);
    settings.setValue("SceneViewSettings/ColorVectors", colorVectors);
    settings.setValue("SceneViewSettings/ColorInitialMesh", colorInitialMesh);
    settings.setValue("SceneViewSettings/ColorSolutionMesh", colorSolutionMesh);
    settings.setValue("SceneViewSettings/ColorInitialMesh", colorHighlighted);
    settings.setValue("SceneViewSettings/ColorSolutionMesh", colorSelected);

    // geometry
    settings.setValue("SceneViewSettings/NodeSize", nodeSize);

    // 3d
    settings.setValue("SceneViewSettings/ScalarView3DLighting", scalarView3DLighting);
    settings.setValue("SceneViewSettings/ScalarView3DAngle", scalarView3DAngle);
    settings.setValue("SceneViewSettings/ScalarView3DBackground", scalarView3DBackground);
}

void Config::savePostprocessor(QDomElement *config)
{
    eleConfig = config;

    // active field
    writeConfig("SceneViewSettings/ActiveField", activeField);

    writeConfig("SceneViewSettings/ShowPost3D", showPost3D);

    // scalar view
    writeConfig("SceneViewSettings/ShowScalarColorBar", showScalarColorBar);
    writeConfig("SceneViewSettings/PaletteType", paletteType);
    writeConfig("SceneViewSettings/PaletteFilter", paletteFilter);
    writeConfig("SceneViewSettings/PaletteSteps", paletteSteps);

    eleConfig = NULL;
}

void Config::saveAdvanced()
{
    QSettings settings;

    // global script
    settings.setValue("Python/GlobalScript", globalScript);
}

bool Config::readConfig(const QString &key, bool defaultValue)
{
    if (eleConfig)
    {
        QString att = key; att.replace("/", "_");
        if (eleConfig->hasAttribute(att))
            return (eleConfig->attribute(att).toInt() == 1) ? true : false;
    }

    return defaultValue;
}

int Config::readConfig(const QString &key, int defaultValue)
{
    if (eleConfig)
    {
        QString att = key; att.replace("/", "_");
        if (eleConfig->hasAttribute(att))
            return eleConfig->attribute(att).toInt();
    }

    return defaultValue;
}

double Config::readConfig(const QString &key, double defaultValue)
{
    if (eleConfig)
    {
        QString att = key; att.replace("/", "_");
        if (eleConfig->hasAttribute(att))
            return eleConfig->attribute(att).toDouble();
    }

    return defaultValue;
}

QString Config::readConfig(const QString &key, const QString &defaultValue)
{
    if (eleConfig)
    {
        QString att = key; att.replace("/", "_");
        if (eleConfig->hasAttribute(att))
            return eleConfig->attribute(att);
    }

    return defaultValue;
}

void Config::writeConfig(const QString &key, bool value)
{
    QString att = key; att.replace("/", "_");
    eleConfig->setAttribute(att, value);
}

void Config::writeConfig(const QString &key, int value)
{
    QString att = key; att.replace("/", "_");
    eleConfig->setAttribute(att, value);
}

void Config::writeConfig(const QString &key, double value)
{
    QString att = key; att.replace("/", "_");
    eleConfig->setAttribute(att, value);
}

void Config::writeConfig(const QString &key, const QString &value)
{
    QString att = key; att.replace("/", "_");
    eleConfig->setAttribute(att, value);
}
