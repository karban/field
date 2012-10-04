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

#include "value.h"

#include "gui/chart.h"
#include "pythonlabagros.h"
#include "scene.h"

Value::Value(const QString &str, bool evaluateExpression)
    : m_isEvaluated(false)
{
    fromString(str);

    if (evaluateExpression)
        evaluate(true);
}

Value::~Value()
{

}

double Value::number()
{
    if (!m_isEvaluated)
        evaluate(true);

    return m_number;
}

double Value::value(double key)
{
    return number();
}

double Value::value(const Point3 &point)
{
    evaluate(point, true);

    return number();
}

double Value::value(double time, const Point3 &point)
{
    evaluate(time, point, true);

    return number();
}

QString Value::toString() const
{
    return m_text;
}

void Value::fromString(const QString &str)
{
    this->setText(str);
}

bool Value::evaluate(bool quiet)
{
    return evaluate(0.0, Point3(), quiet);
}

bool Value::evaluate(double time, bool quiet)
{
    return evaluate(time, Point3(), quiet);
}

bool Value::evaluate(const Point3 &point, bool quiet)
{
    return evaluate(0.0, point, quiet);
}

bool Value::evaluate(double time, const Point3 &point, bool quiet)
{
    bool signalBlocked = currentPythonEngineAgros()->signalsBlocked();
    currentPythonEngineAgros()->blockSignals(true);

    currentPythonEngineAgros()->runExpression(QString("time = %1").arg(time), false);

    // eval expression
    ExpressionResult expressionResult = currentPythonEngineAgros()->runExpression(m_text, true);
    if (expressionResult.error.isEmpty())
    {
        m_number = expressionResult.value;
    }
    else
    {
        if (!quiet)
            QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Error"), expressionResult.error);
    }

    if (!signalBlocked)
        currentPythonEngineAgros()->blockSignals(false);

    m_isEvaluated = true;
    return expressionResult.error.isEmpty();
}

// ***********************************************************************************

ValueLineEdit::ValueLineEdit(QWidget *parent, bool hasTimeDep)
    : QWidget(parent), m_hasTimeDep(hasTimeDep)
{
    m_minimum = -numeric_limits<double>::max();
    m_minimumSharp = -numeric_limits<double>::max();
    m_maximum = numeric_limits<double>::max();
    m_maximumSharp = numeric_limits<double>::max();

    // create controls
    txtLineEdit = new QLineEdit(this);
    txtLineEdit->setToolTip(tr("This textedit allows using variables."));
    txtLineEdit->setText("0");
    connect(txtLineEdit, SIGNAL(textChanged(QString)), this, SLOT(evaluate()));
    connect(txtLineEdit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));

    lblValue = new QLabel(this);
    lblInfo = new QLabel();

    // timedep value
#ifdef Q_WS_MAC
    btnEditTimeDep = new QToolButton();
    btnEditTimeDep->setIcon(icon("three-dots"));
    btnEditTimeDep->setMaximumHeight(txtLineEdit->height() - 4);
#else
    btnEditTimeDep = new QPushButton(icon("three-dots"), "");
#endif
    connect(btnEditTimeDep, SIGNAL(clicked()), this, SLOT(doOpenValueTimeDialog()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->addWidget(txtLineEdit, 1);
    layout->addWidget(lblInfo, 1);
    layout->addWidget(lblValue, 0, Qt::AlignRight);
    layout->addWidget(btnEditTimeDep, 0, Qt::AlignRight);

    setLayout(layout);

    setLayoutValue();
    evaluate();
}

void ValueLineEdit::setNumber(double value)
{
    txtLineEdit->setText(QString::number(value));
    evaluate();
}

double ValueLineEdit::number()
{
    if (evaluate())
        return m_number;
    else
        return 0.0;
}

void ValueLineEdit::setValue(Value value)
{
    txtLineEdit->setText(value.text());

    setLayoutValue();
    evaluate();
}

Value ValueLineEdit::value()
{
    return Value(txtLineEdit->text());
}

bool ValueLineEdit::evaluate(bool quiet)
{
    bool isOk = false;

    Value val = value();

    btnEditTimeDep->setVisible(m_hasTimeDep);

    if (val.evaluate(quiet))
    {
        if (val.number() <= m_minimumSharp)
        {
            setValueLabel(QString("<= %1").arg(m_minimumSharp), QColor(Qt::blue), true);
        }
        else if (val.number() >= m_maximumSharp)
        {
            setValueLabel(QString(">= %1").arg(m_maximumSharp), QColor(Qt::blue), true);
        }
        else if (val.number() < m_minimum)
        {
            setValueLabel(QString("< %1").arg(m_minimum), QColor(Qt::blue), true);
        }
        else if (val.number() > m_maximum)
        {
            setValueLabel(QString("> %1").arg(m_maximum), QColor(Qt::blue), true);
        }
        else if (!checkCondition(val.number()))
        {
            setValueLabel(QString("%1").arg(m_condition), QColor(Qt::red), true);
        }
        else
        {
            m_number = val.number();
            setValueLabel(QString("%1").arg(m_number, 0, 'g', 3), QApplication::palette().color(QPalette::WindowText),
                          Util::config()->lineEditValueShowResult);
            isOk = true;
        }
    }
    else
    {
        setValueLabel(tr("error"), QColor(Qt::red), true);
        setFocus();
    }

    if (isOk)
    {
        emit evaluated(false);
        return true;
    }
    else
    {
        emit evaluated(true);
        return false;
    }
}

bool ValueLineEdit::checkCondition(double value)
{
    if (m_condition.isEmpty()) return true;

    bool isOK = false;

    // FIXME: (Franta) replace -> LEX?
    QString condition = m_condition;
    condition.replace(QString("value"), QString::number(value));

    ExpressionResult result = currentPythonEngineAgros()->runExpression(condition, true);

    if (result.error.isEmpty())
    {
        if (!(fabs(result.value) < EPS_ZERO))
            isOK = true;
    }
    else
    {
        QPalette palette = txtLineEdit->palette();
        palette.setColor(QPalette::Text, QColor(Qt::red));
        txtLineEdit->setPalette(palette);

        txtLineEdit->setToolTip(tr("Condition couldn't be evaluated:\n%1").arg(result.error));
        isOK = true;
    }

    return isOK;
}

void ValueLineEdit::setLayoutValue()
{
    txtLineEdit->setVisible(false);
    lblValue->setVisible(false);
    lblInfo->setVisible(false);

    btnEditTimeDep->setVisible(m_hasTimeDep);
}

void ValueLineEdit::setValueLabel(const QString &text, QColor color, bool isVisible)
{
    lblValue->setText(text);
    QPalette palette = lblValue->palette();
    palette.setColor(QPalette::WindowText, color);
    lblValue->setPalette(palette);
    lblValue->setVisible(isVisible);
}

QSize ValueLineEdit::sizeHint() const
{
    return QSize(100, 10);
}

void ValueLineEdit::focusInEvent(QFocusEvent *event)
{
    txtLineEdit->setFocus(event->reason());
}

void ValueLineEdit::doOpenValueTimeDialog()
{
    ValueTimeDialog dialog;
    dialog.setValue(Value(txtLineEdit->text()));

    if (dialog.exec() == QDialog::Accepted)
    {
        txtLineEdit->setText(dialog.value().text());
        evaluate();
    }
}

// ****************************************************************************************************************

ValueTimeDialog::ValueTimeDialog(QWidget *parent) : QDialog(parent)
{
    setWindowIcon(icon("timefunction"));
    setWindowTitle(tr("Time function"));

    createControls();
    plotFunction();

    setMinimumSize(600, 400);

    QSettings settings;
    restoreGeometry(settings.value("ValueTimeDialog/Geometry", saveGeometry()).toByteArray());
}

ValueTimeDialog::~ValueTimeDialog()
{
    QSettings settings;
    settings.setValue("ValueTimeDialog/Geometry", saveGeometry());
}

void ValueTimeDialog::setValue(Value value)
{
    txtLineEdit->setText(value.text());

    // plot
    plotFunction();
}

void ValueTimeDialog::createControls()
{
    lblInfoError = new QLabel();

    QPalette palette = lblInfoError->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    lblInfoError->setPalette(palette);

    txtLineEdit = new QLineEdit(this);
    connect(txtLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkExpression()));

    txtTimeTotal = new ValueLineEdit();
    // txtTimeTotal->setValue(Util::problem()->config()->timeTotal());

    cmbPresets = new QComboBox();
    cmbPresets->addItem(tr("select a preset..."));
    cmbPresets->addItem(tr("constant"), "1.0");
    cmbPresets->addItem(tr("step"), QString("1.0*(time<%1)"));
    cmbPresets->addItem(tr("sine wave"), QString("1.0*sin(2*pi*1.0/%1*time)"));
    cmbPresets->addItem(tr("exp. step"), "1.0*(exp(-10/%1*time) - exp(-20/%1*time))");
    connect(cmbPresets, SIGNAL(currentIndexChanged(int)), this, SLOT(presetsChanged(int)));

    // chart
    chart = new Chart(this, true);
    // axis labels
    chart->setAxisTitle(QwtPlot::xBottom, tr("time"));
    chart->setAxisTitle(QwtPlot::yLeft, tr("value"));

    chartCurve = new QwtPlotCurve();
    chartCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    chartCurve->setStyle(QwtPlotCurve::NoCurve);
    chartCurve->setCurveAttribute(QwtPlotCurve::Inverted);
    chartCurve->setYAxis(QwtPlot::yLeft);
    // chartCurve->setSymbol(QwtSymbol(QwtSymbol::Diamond, Qt::red, QPen(Qt::blue, 1), QSize(10,10)));
    chartCurve->attach(chart);

    QGridLayout *controlsLayout = new QGridLayout();
    controlsLayout->addWidget(chart, 0, 0, 1, 4);
    controlsLayout->addWidget(new QLabel(tr("Function:")), 1, 0);
    controlsLayout->addWidget(txtLineEdit, 1, 1);
    controlsLayout->setColumnStretch(1, 1);
    controlsLayout->addWidget(cmbPresets, 1, 2, 1, 2);
    controlsLayout->addWidget(new QLabel(tr("Total time:")), 2, 2);
    controlsLayout->addWidget(txtTimeTotal, 2, 3);
    controlsLayout->addWidget(new QLabel(tr("Error:")), 2, 0);
    controlsLayout->addWidget(lblInfoError, 2, 1, 3, 1, Qt::AlignTop);
    controlsLayout->addWidget(new QLabel(""), 2, 1);
    controlsLayout->addWidget(new QLabel(""), 3, 1);

    // dialog buttons
    btnOk = new QPushButton(tr("Ok"));
    btnOk->setDefault(true);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(doAccept()));
    btnClose = new QPushButton(tr("Close"));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(doReject()));
    // btnPlot = new QPushButton(tr("Plot"));
    // connect(btnPlot, SIGNAL(clicked()), this, SLOT(plotFunction()));

    QHBoxLayout *layoutButtons = new QHBoxLayout();
    layoutButtons->addStretch();
    // layoutButtons->addWidget(btnPlot);
    layoutButtons->addWidget(btnOk);
    layoutButtons->addWidget(btnClose);

    // layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(controlsLayout);
    layout->addLayout(layoutButtons);

    setLayout(layout);
}

void ValueTimeDialog::presetsChanged(int index)
{
    if (cmbPresets->currentIndex() > 0)
    {
        if (txtTimeTotal->value().evaluate())
        {
            QString preset = cmbPresets->itemData(cmbPresets->currentIndex()).toString().arg(txtTimeTotal->value().number() / 2.0);

            txtLineEdit->setText(preset);
            cmbPresets->setCurrentIndex(0);

            plotFunction();
        }
    }
}

void ValueTimeDialog::checkExpression()
{
    // eval time
    currentPythonEngineAgros()->runExpression(QString("time = %1").arg(0.0), false);

    // eval expression
    ExpressionResult expressionResult;
    expressionResult = currentPythonEngineAgros()->runExpression(txtLineEdit->text(), true);
    lblInfoError->setText(expressionResult.error.trimmed());
    if (expressionResult.error.isEmpty())
        plotFunction();
    else
        txtLineEdit->setFocus();
}

void ValueTimeDialog::plotFunction()
{
    // plot solution
    int count = 200;

    double *xval = new double[count];
    double *yval = new double[count];

    double totalTime = txtTimeTotal->value().number();

    // time step
    double dt = totalTime / (count + 1);

    Value val(txtLineEdit->text());
    for (int i = 0; i < count; i++)
    {
        xval[i] = i*dt;

        if (!val.evaluate(xval[i], true))
            break;
        yval[i] = val.number();
    }

    chart->setData(xval, yval, count);

    delete [] xval;
    delete [] yval;
}

void ValueTimeDialog::doAccept()
{
    accept();
}

void ValueTimeDialog::doReject()
{
    reject();
}

