#pragma once
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

class DataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DataGenerator(QObject *parent = nullptr);
    void start();
    void stop();
    bool isRunning() const;
    void setFreq(double hz) { m_freq = hz; }
    void setAmp(double amp) { m_amp  = amp; }
    void setMode(bool sine) { m_sine = sine; }

signals:
    void dataReady(double value);

private slots:
    void tick();

private:
    QTimer        m_timer;
    QElapsedTimer m_elapsed;
    double m_freq = 2.0;
    double m_amp  = 1.0;
    bool   m_sine = true;
};
