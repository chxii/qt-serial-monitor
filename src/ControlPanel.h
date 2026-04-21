#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QRadioButton>
#include "widgets/CardWidget.h"

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);

    double freq() const;
    double amp()  const;
    bool   isSine() const;

signals:
    void connectToggled(bool connected);
    void paramsChanged(double freq, double amp, bool sine);

private slots:
    void onConnectClicked();
    void onSliderChanged();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QComboBox    *m_portCombo;
    QComboBox    *m_baudCombo;
    QComboBox    *m_bitsCombo;
    QComboBox    *m_parityCombo;
    QComboBox    *m_stopCombo;
    QRadioButton *m_rbSine;
    QRadioButton *m_rbRandom;
    QSlider      *m_freqSlider;
    QSlider      *m_ampSlider;
    QLabel       *m_freqLabel;
    QLabel       *m_ampLabel;
    QPushButton  *m_connectBtn;
    QLabel       *m_statusLabel;
    bool          m_connected = false;

    CardWidget *makeCard(const QString &title, QLayout *content);
};
