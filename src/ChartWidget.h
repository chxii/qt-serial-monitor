#pragma once
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

class ChartWidget : public QChartView
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);

public slots:
    void addPoint(double value);
    void clear();

private:
    QLineSeries  *m_series;
    QValueAxis   *m_axisX;
    QValueAxis   *m_axisY;
    qint64        m_sampleCount = 0;
    static constexpr int MAX_POINTS = 200;
};
