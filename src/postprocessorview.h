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

#ifndef SCENEVIEWDIALOG_H
#define SCENEVIEWDIALOG_H

#include "util.h"

class SceneViewPreprocessor;
class SceneViewMesh;
class SceneViewPost2D;
class SceneViewPost3D;
class LineEditDouble;
class CollapsableGroupBoxButton;

class PostprocessorWidget : public QWidget
{
    Q_OBJECT

public:
    PostprocessorWidget(SceneViewPreprocessor *sceneGeometry,                      
                      SceneViewPost3D *scenePost3D,
                      QWidget *parent);

private slots:
    void doApply();

private:
    SceneViewPreprocessor *m_sceneGeometry;
    SceneViewPost3D *m_scenePost3D;

    QWidget *basic;

    QButtonGroup *butPost3DGroup;
    QRadioButton *radPost3DNone;
    QRadioButton *radPost3DScalarField3D;
    QRadioButton *radPost3DModel;

    // scalar field
    QComboBox *cmbPostScalarFieldVariable;
    QComboBox *cmbPostScalarFieldVariableComp;
    QCheckBox *chkScalarFieldRangeAuto;
    QLabel *lblScalarFieldRangeMin;
    QLabel *lblScalarFieldRangeMax;
    LineEditDouble *txtScalarFieldRangeMin;
    LineEditDouble *txtScalarFieldRangeMax;
    QLabel *lblScalarFieldRangeMinError;
    QLabel *lblScalarFieldRangeMaxError;

    // scalar field
    QCheckBox *chkShowScalarColorBar;
    QComboBox *cmbPalette;
    QCheckBox *chkPaletteFilter;
    QSpinBox *txtPaletteSteps;
    QCheckBox *chkScalarFieldRangeLog;
    LineEditDouble *txtScalarFieldRangeBase;
    QSpinBox *txtScalarDecimalPlace;

    // toolbar
    QToolBox *tbxPostprocessor;
    QPushButton *btnOK;

    void loadBasic();
    void saveBasic();

    void createControls();
    QWidget *controlsBasic();

    QWidget *groupPost3d;

    CollapsableGroupBoxButton *groupPostScalar;
    QWidget *groupPostScalarAdvanced;

    CollapsableGroupBoxButton *postScalarWidget();
    QWidget *postScalarAdvancedWidget();

    QStackedLayout *widgetsLayout;

    QWidget *meshWidget();
    QWidget *post3DWidget();

signals:
    void apply();

public slots:
    void updateControls();
    void refresh();

private slots:
    void doPaletteFilter(int state);

    void doScalarFieldRangeAuto(int state);
    void doScalarFieldRangeMinChanged();
    void doScalarFieldRangeMaxChanged();
    void doPostprocessorGroupClicked(QAbstractButton *button);
    void doScalarFieldLog(int state);
    void doScalarFieldExpandCollapse(bool collapsed);
};

#endif // SCENEVIEWDIALOG_H
