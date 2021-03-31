#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QTimer>

#include "logic.h"
#include "gui.h"

class Controller : public QWidget
{
    Q_OBJECT

public:
    explicit Controller(QWidget *parent = nullptr);
    ~Controller();

private:
    void Init();
    void MakeConnections();
    void LoadQSS(const QString& filename);

    Logic *m_logic;
    GUI   *m_gui;

    QHBoxLayout *m_layout;

    QTimer m_timer;
    QString m_qss;

signals:

public slots:
    void OnTimer();

};

#endif // CONTROLLER_H
