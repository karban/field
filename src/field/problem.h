#ifndef PROBLEM_H
#define PROBLEM_H

#include "util.h"

class Problem;

//template <typename Scalar>
//class Solver;

class ProblemConfig : public QObject
{
    Q_OBJECT
public:
    ProblemConfig(QWidget *parent = 0);

    inline QString labelX() { return "X";  }
    inline QString labelY() { return "Y";  }
    inline QString labelZ() { return "Z";  }

    void clear();

    inline QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    inline QString fileName() const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; }

    // startup script
    inline QString startupscript() const { return m_startupscript; }
    void setStartupScript(const QString &startupscript) { m_startupscript = startupscript; emit changed(); }

    // description
    inline QString description() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }

    void refresh() { emit changed(); }

signals:
    void changed();

private:
    QString m_name;
    QString m_fileName;

    // harmonic
    double m_frequency;

    QString m_startupscript;
    QString m_description;

};

/// intented as central for solution process
/// shielded from gui and QT
/// holds data describing individual fields, means of coupling and solutions
class Problem : public QObject
{
    Q_OBJECT

signals:
    void timeStepChanged();
    void meshed();
    void solved();

    /// emited when an field is added or removed. Menus need to adjusted
    void fieldsChanged();

    /// emited when an field is added or removed. Menus need to adjusted
    void couplingsChanged();

public slots:
    // clear problem
    void clearSolution();
    void clearFieldsAndConfig();

public:
    Problem();
    ~Problem();

    QAction *actClearSolutions;

    inline ProblemConfig *config() const { return m_config; }

    void createStructure();

    // solve
    void solve();

    bool isSolved() const {  return m_isSolved; }
    bool isSolving() const { return m_isSolving; }

private:
    ProblemConfig *m_config;

    QTime m_timeElapsed;
    bool m_isSolving;
    int m_timeStep;
    bool m_isSolved;
};

#endif // PROBLEM_H
