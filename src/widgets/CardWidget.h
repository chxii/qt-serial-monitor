#pragma once
#include <QWidget>

class CardWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int radius READ radius WRITE setRadius)
    Q_PROPERTY(bool showBorder READ showBorder WRITE setShowBorder)

public:
    explicit CardWidget(QWidget *parent = nullptr);

    int  radius()    const { return m_radius; }
    bool showBorder() const { return m_showBorder; }

    void setRadius(int r)        { m_radius = r;    update(); }
    void setShowBorder(bool b)   { m_showBorder = b; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int  m_radius     = 10;
    bool m_showBorder = true;
};
