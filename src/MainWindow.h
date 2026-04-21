#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QVector>
#include <QPair>
#include <QElapsedTimer>
#include "ControlPanel.h"
#include "ChartWidget.h"
#include "LogPanel.h"
#include "DataGenerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onConnectToggled(bool connect);
    void onDataReady(double value);
    void onParamsChanged(double freq, double amp, bool sine);
    void exportCsv();

private:
    ControlPanel  *m_control;
    ChartWidget   *m_chart;
    LogPanel      *m_log;
    DataGenerator *m_gen;

    QLabel *m_statSamples;
    QLabel *m_statMin;
    QLabel *m_statMax;
    QLabel *m_statAvg;
    QLabel *m_statRate;

    qint64 m_count     = 0;
    double m_min       =  1e9;
    double m_max       = -1e9;
    double m_sum       = 0.0;
    qint64 m_rateCount = 0;
    QElapsedTimer m_rateTimer;

    // All recorded samples for CSV export
    struct Sample { QString timestamp; double value; };
    QVector<Sample> m_samples;
};
