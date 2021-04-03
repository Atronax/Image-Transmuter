#ifndef HISTORYLINEEDIT_H
#define HISTORYLINEEDIT_H

#include <QLabel>
#include <QStringList>
#include <QWheelEvent>

class HistoryLabel : public QLabel
{
    Q_OBJECT

public:
    explicit HistoryLabel(const QString& message, QWidget* parent);
    ~HistoryLabel();

    void wheelEvent(QWheelEvent *event) override;

private:
    void MakeConnections();
    void ShowCurrentMessage();

    // List of messages and index of current message.
    int m_currentMessageIndex;
    QStringList m_messages;

signals:
    void userChangedTheMessage(bool toPrevious);

public slots:
    void AddMessage(const QString& message);
    void OnUserChangedTheMessage(bool toPrevious);
};

#endif // HISTORYLINEEDIT_H
