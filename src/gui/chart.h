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

#ifndef GUI_CHART_H
#define GUI_CHART_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>

#include "util.h"

class Chart : public QwtPlot
{
    Q_OBJECT
public:
    Chart(QWidget *parent = 0, bool showPicker = false);
    ~Chart();

    inline QwtPlotCurve *curve() { return m_curve; }
    void saveImage(const QString &fileName = "");

public slots:
   void setData(double *xval, double *yval, int count);
   void setData(QList<double> xval, QList<double> yval);

private:
    QwtPlotCurve *m_curve;


private slots:
    void pickerValueMoved(const QPoint &pos);
};

#endif // GUI_CHART_H
