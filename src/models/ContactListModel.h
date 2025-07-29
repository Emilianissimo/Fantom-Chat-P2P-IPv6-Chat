#ifndef CONTACTLISTMODEL_H
#define CONTACTLISTMODEL_H

#include "../utils/Structures.h"
#include <QAbstractListModel>
#include <QObject>

class ContactListModel : public QAbstractListModel
{
public:
    enum ContactRoles {
        ChatIDRole,
        ClientIDRole = Qt::UserRole + 1,
        LastMessageRole,
        OrderRole,
        IsActiveRole,
    };

    explicit ContactListModel(QObject *parent = nullptr);

    void setActive(const QString& chatID);
    void setContacts(const QVector<Contact>& contacts);
    void onNewMessage(const QString& chatID, const QString& clientID, const QString& message);

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
private:
    QVector<Contact> m_contacts;
};

#endif // CONTACTLISTMODEL_H
