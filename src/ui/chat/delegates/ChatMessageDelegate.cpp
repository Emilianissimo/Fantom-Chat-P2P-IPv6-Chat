#include "../../../src/utils/Structures.h"
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>

class ChatMessageDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        painter->save();
        Message msg = index.data(Qt::UserRole).value<Message>();

        QFontMetrics fm(option.font);
        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WordWrap);

        QString sender = msg.isIncoming ? msg.peerID : "You";
        QString text = msg.message;

        int padding = 0;
        int maxWidth = option.rect.width() * 0.75;

        QRect senderRect = fm.boundingRect(sender);
        QRect textRect = fm.boundingRect(
            QRect(0, 0, maxWidth, INT_MAX), Qt::TextWordWrap, text
        );
        int totalHeight = senderRect.height() + textRect.height() + padding * 3;

        QRect bubbleRect;
        if (msg.isIncoming) {
            bubbleRect = QRect(
                option.rect.left() + padding,
                option.rect.top() + padding,
                qMax(senderRect.width(), textRect.width()) + padding * 2,
                totalHeight
            );
        } else {
            bubbleRect = QRect(
                option.rect.right() - (qMax(senderRect.width(), textRect.width()) + padding * 3),
                option.rect.top() + padding,
                qMax(senderRect.width(), textRect.width()) + padding * 2,
                totalHeight
            );
        }

        QColor bubbleColor = msg.isIncoming ? QColor("#e0e0e0") : QColor("#0078d7");
        QColor textColor = msg.isIncoming ? Qt::black : Qt::white;

        painter->setBrush(bubbleColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(bubbleRect, 10, 10);

        painter->setPen(textColor);
        painter->drawText(bubbleRect.adjusted(padding, padding, -padding, -padding - textRect.height()), sender);

        painter->drawText(
            bubbleRect.adjusted(padding, padding + senderRect.height() + padding, - padding, padding), text, textOption
        );

        painter->restore();

    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Message msg = index.data(Qt::UserRole).value<Message>();
        QFontMetrics fm(option.font);

        int maxWidth = option.rect.width() > 0 ? option.rect.width() * 0.75 : 300;
        QRect senderRect = fm.boundingRect(msg.isIncoming ? msg.peerID : "You");
        QRect textRect = fm.boundingRect(QRect(0, 0, maxWidth, INT_MAX), Qt::TextWordWrap, msg.message);

        int height = senderRect.height() + textRect.height() + 24;
        return QSize(option.rect.width(), height);
    }
};
