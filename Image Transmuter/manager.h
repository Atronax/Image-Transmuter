#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QTimer>

#include "logic.h"
#include "gui.h"

class Manager : public QWidget
{
    Q_OBJECT

public:
    explicit Manager(QWidget *parent = nullptr);
    ~Manager();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void Init();
    void MakeConnections();
    void MakeRoundedCorners(int radius_tl, int radius_tr, int radius_bl, int radius_br);
    void LoadQSS(const QString& filename);

    Logic *m_logic;
    GUI   *m_gui;

    // User interface
    const int cornerRadius = 15;
    QPoint m_mousePosition;
    QHBoxLayout *m_layout;

    // Stylesheet stuff.
    QTimer m_timer;
    QString m_qss;

signals:

public slots:
    void OnTimer();
    void OnUpdateUI(const QString& size);

};

#endif // CONTROLLER_H
