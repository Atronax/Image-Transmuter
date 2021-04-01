#include "historylabel.h"

#include <QMouseEvent>
#include <QDebug>

HistoryLabel::HistoryLabel(const QString& message, QWidget *parent)
    : QLabel(message, parent),
      m_currentMessageIndex(0)
{
    MakeConnections();
}

HistoryLabel::~HistoryLabel()
{

}

void HistoryLabel::wheelEvent(QWheelEvent *ev)
{
    qDebug() << "In HistoryLabel::wheelEvent";

    int movement = ev->angleDelta().ry();

    if (movement > 0)
    {
        qDebug() << "UP";
        emit userChangedTheMessage(true);
    }
    else
    {
        qDebug() << "DOWN";
        emit userChangedTheMessage(false);
    }

    ev->accept();
}

void HistoryLabel::MakeConnections()
{
    connect (this, SIGNAL(userChangedTheMessage(bool)), this, SLOT(OnUserChangedTheMessage(bool)));
}

void HistoryLabel::ShowCurrentMessage()
{
    qDebug() << QString("Current message %1 is \"%2\"").arg(m_currentMessageIndex).arg(m_messages.at(m_currentMessageIndex));

    if (!m_messages.isEmpty())
        setText(m_messages.at(m_currentMessageIndex));
}

void HistoryLabel::AddMessage(const QString &message)
{
    qDebug() << m_messages;

    m_messages.append(message);
    m_currentMessageIndex = m_messages.count() - 1;

    ShowCurrentMessage();
}

void HistoryLabel::OnUserChangedTheMessage(bool toPrevious)
{
    qDebug() << "in HistoryLabel::OnUserChangedTheMessage";

    if (toPrevious)
    {
        --m_currentMessageIndex;
        if (m_currentMessageIndex < 0)
            m_currentMessageIndex = 0;
    }
    else
    {
        ++m_currentMessageIndex;
        if (m_currentMessageIndex > m_messages.count() - 1)
            m_currentMessageIndex = m_messages.count() - 1;
    }

    qDebug() << QString("Current message has index: %1").arg(m_currentMessageIndex);

    ShowCurrentMessage();
}
