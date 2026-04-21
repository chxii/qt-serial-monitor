#include "ControlPanel.h"
#include "widgets/CardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPainter>
#include <QButtonGroup>

static QLabel *sectionHeader(const QString &text)
{
    auto *l = new QLabel(text);
    l->setStyleSheet(
        "font-size:10px; font-weight:700; color:#667085;"
        "letter-spacing:1px; text-transform:uppercase;"
        "padding-bottom:4px;"
    );
    return l;
}

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent)
{
    setFixedWidth(220);

    // ── Connection card ──────────────────────────────────────
    m_portCombo   = new QComboBox(); m_portCombo->addItem("Virtual");
    m_baudCombo   = new QComboBox();
    m_baudCombo->addItems({"9600","19200","38400","57600","115200"});
    m_baudCombo->setCurrentText("115200");
    m_bitsCombo   = new QComboBox(); m_bitsCombo->addItems({"8","7","6","5"});
    m_parityCombo = new QComboBox(); m_parityCombo->addItems({"None","Even","Odd"});
    m_stopCombo   = new QComboBox(); m_stopCombo->addItems({"1","2"});

    auto *form = new QFormLayout();
    form->setSpacing(8);
    form->setContentsMargins(0,0,0,0);
    form->addRow("Port",   m_portCombo);
    form->addRow("Baud",   m_baudCombo);
    form->addRow("Bits",   m_bitsCombo);
    form->addRow("Parity", m_parityCombo);
    form->addRow("Stop",   m_stopCombo);

    auto *connLayout = new QVBoxLayout();
    connLayout->setSpacing(8);
    connLayout->setContentsMargins(0,0,0,0);
    connLayout->addWidget(sectionHeader("CONNECTION"));
    connLayout->addLayout(form);

    // ── Signal card ──────────────────────────────────────────
    m_rbSine   = new QRadioButton("Sine Wave");
    m_rbRandom = new QRadioButton("Random Noise");
    m_rbSine->setChecked(true);

    auto *bg = new QButtonGroup(this);
    bg->addButton(m_rbSine);
    bg->addButton(m_rbRandom);

    // Freq slider
    m_freqSlider = new QSlider(Qt::Horizontal);
    m_freqSlider->setRange(5, 50);   // 0.5–5.0 Hz (×10)
    m_freqSlider->setValue(20);      // default 2.0 Hz
    m_freqLabel  = new QLabel("Freq: 2.0 Hz");
    m_freqLabel->setStyleSheet("font-size:11px; color:#667085;");

    // Amp slider
    m_ampSlider = new QSlider(Qt::Horizontal);
    m_ampSlider->setRange(1, 50);    // 0.1–5.0 (×10)
    m_ampSlider->setValue(10);       // default 1.0
    m_ampLabel  = new QLabel("Amp: 1.0");
    m_ampLabel->setStyleSheet("font-size:11px; color:#667085;");

    auto *sigLayout = new QVBoxLayout();
    sigLayout->setSpacing(8);
    sigLayout->setContentsMargins(0,0,0,0);
    sigLayout->addWidget(sectionHeader("SIGNAL GENERATOR"));
    sigLayout->addWidget(m_rbSine);
    sigLayout->addWidget(m_rbRandom);
    sigLayout->addWidget(m_freqLabel);
    sigLayout->addWidget(m_freqSlider);
    sigLayout->addWidget(m_ampLabel);
    sigLayout->addWidget(m_ampSlider);

    // ── Status + button ──────────────────────────────────────
    m_statusLabel = new QLabel("● Disconnected");
    m_statusLabel->setStyleSheet("font-size:12px; color:#667085;");

    m_connectBtn = new QPushButton("Connect");
    m_connectBtn->setProperty("class", "primary");
    m_connectBtn->setFixedHeight(36);
    m_connectBtn->setCursor(Qt::PointingHandCursor);

    // ── Root layout ──────────────────────────────────────────
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *scroll = new QVBoxLayout();
    scroll->setContentsMargins(16, 16, 16, 16);
    scroll->setSpacing(16);
    scroll->addWidget(makeCard("", connLayout));
    scroll->addWidget(makeCard("", sigLayout));
    scroll->addStretch();
    scroll->addWidget(m_statusLabel);
    scroll->addSpacing(8);
    scroll->addWidget(m_connectBtn);

    root->addLayout(scroll);

    // ── Connections ──────────────────────────────────────────
    connect(m_connectBtn,  &QPushButton::clicked,        this, &ControlPanel::onConnectClicked);
    connect(m_freqSlider,  &QSlider::valueChanged,       this, &ControlPanel::onSliderChanged);
    connect(m_ampSlider,   &QSlider::valueChanged,       this, &ControlPanel::onSliderChanged);
    connect(m_rbSine,      &QRadioButton::toggled,       this, [this](bool){ onSliderChanged(); });
}

void ControlPanel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor("#FFFFFF"));
    p.setPen(QPen(QColor("#EAECF0"), 1));
    p.drawLine(width()-1, 0, width()-1, height());
}

double ControlPanel::freq() const { return m_freqSlider->value() / 10.0; }
double ControlPanel::amp()  const { return m_ampSlider->value()  / 10.0; }
bool   ControlPanel::isSine() const { return m_rbSine->isChecked(); }

void ControlPanel::onSliderChanged()
{
    m_freqLabel->setText(QString("Freq: %1 Hz").arg(freq(), 0, 'f', 1));
    m_ampLabel->setText(QString("Amp:  %1").arg(amp(),  0, 'f', 1));
    emit paramsChanged(freq(), amp(), isSine());
}

void ControlPanel::onConnectClicked()
{
    m_connected = !m_connected;
    if (m_connected) {
        m_connectBtn->setText("Disconnect");
        m_connectBtn->setProperty("class", "danger");
        m_statusLabel->setText("● Connected");
        m_statusLabel->setStyleSheet("font-size:12px; color:#16A34A; font-weight:600;");
    } else {
        m_connectBtn->setText("Connect");
        m_connectBtn->setProperty("class", "primary");
        m_statusLabel->setText("● Disconnected");
        m_statusLabel->setStyleSheet("font-size:12px; color:#667085;");
    }
    // Force QSS re-evaluation after property change
    m_connectBtn->style()->unpolish(m_connectBtn);
    m_connectBtn->style()->polish(m_connectBtn);
    emit connectToggled(m_connected);
}

CardWidget *ControlPanel::makeCard(const QString &, QLayout *content)
{
    auto *card = new CardWidget();
    auto *wrap = new QVBoxLayout(card);
    wrap->setContentsMargins(12, 12, 12, 12);
    wrap->addLayout(content);
    return card;
}
