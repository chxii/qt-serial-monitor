#pragma once
#include <QWidget>
#include <QPropertyAnimation>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

struct NavItem {
    QString text;
    int     pageIndex;
    QPushButton *btn = nullptr;
};

class SidebarNav : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int sidebarWidth READ sidebarWidth WRITE setSidebarWidth)

public:
    explicit SidebarNav(QWidget *parent = nullptr);

    void addNavItem(const QString &text, int pageIndex);
    void setUserInfo(const QString &name, const QString &role);
    void setCurrentIndex(int pageIndex);

    int  sidebarWidth() const { return width(); }
    void setSidebarWidth(int w) { setFixedWidth(w); }

signals:
    void pageChanged(int pageIndex);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void selectItem(NavItem &item);

    QVBoxLayout *m_navLayout;
    QLabel      *m_userAvatar;
    QLabel      *m_userName;
    QLabel      *m_userRole;
    QVector<NavItem> m_items;
    int m_currentPage = -1;
};
