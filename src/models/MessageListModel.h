#pragma once
#ifndef MESSAGELISTMODEL_H
#define MESSAGELISTMODEL_H

#include "../utils/Structures.h"
#include <QAbstractListModel>
#include <QObject>
#include <QList>
#include <QString>

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MessageListModel(QObject *parent = nullptr);

    void setMessages(const QList<Message>& messages);
    void addMessage(const Message& message);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    QList<Message> m_messages;
};

#endif // MESSAGELISTMODEL_H
