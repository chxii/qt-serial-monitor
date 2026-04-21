#include "CardWidget.h"
#include <QPainter>
#include <QPainterPath>

CardWidget::CardWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void CardWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF r = rect().adjusted(2, 2, -2, -2); // room for shadow

    // Shadow
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 10));
    p.drawRoundedRect(r.adjusted(0, 2, 0, 2), m_radius, m_radius);

    // Card fill
    p.setBrush(QColor("#FFFFFF"));
    if (m_showBorder)
        p.setPen(QPen(QColor("#EAECF0"), 1));
    else
        p.setPen(Qt::NoPen);
    p.drawRoundedRect(r, m_radius, m_radius);
}
