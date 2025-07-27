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

        QString sender = msg.isIncoming ? msg.peerID : "You";
        QString text = msg.message;

        QFontMetrics fm(option.font);
        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WordWrap);

        const int bubblePadding = 12;
        const int bubbleMargin = 12;
        const int lineSpacing = 4;

        int maxTextWidth = option.rect.width() * 0.6;

        QRect senderRect = fm.boundingRect(sender);
        QRect messageRect = fm.boundingRect(QRect(0, 0, maxTextWidth, INT_MAX), Qt::TextWordWrap, text);

        int bubbleWidth = qMax(senderRect.width(), messageRect.width()) + bubblePadding * 2;
        int bubbleHeight = senderRect.height() + lineSpacing + messageRect.height() + bubblePadding * 2;

        QRect bubbleRect;
        if (msg.isIncoming) {
            bubbleRect = QRect(option.rect.left() + bubbleMargin,
                               option.rect.top() + bubbleMargin,
                               bubbleWidth,
                               bubbleHeight);
        } else {
            bubbleRect = QRect(option.rect.right() - bubbleMargin - bubbleWidth,
                               option.rect.top() + bubbleMargin,
                               bubbleWidth,
                               bubbleHeight);
        }

        // Colors
        QColor bubbleColor = msg.isIncoming ? QColor("#e0e0e0") : QColor("#0078d7");
        QColor textColor = msg.isIncoming ? Qt::black : Qt::white;

        // Bubble drawing
        // Tail (Messenger like)
        QPolygon tail;
        if (msg.isIncoming) {
            QPoint p1 = bubbleRect.topLeft() + QPoint(-6, 15);
            QPoint p2 = bubbleRect.topLeft() + QPoint(0, 10);
            QPoint p3 = bubbleRect.topLeft() + QPoint(0, 20);
            tail << p1 << p2 << p3;
        } else {
            QPoint p1 = bubbleRect.topRight() + QPoint(6, 15);
            QPoint p2 = bubbleRect.topRight() + QPoint(0, 10);
            QPoint p3 = bubbleRect.topRight() + QPoint(0, 20);
            tail << p1 << p2 << p3;
        }

        painter->setBrush(bubbleColor);
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(tail);

        painter->setBrush(bubbleColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(bubbleRect, 10, 10);

        // Text
        painter->setPen(textColor);
        QRect senderArea = QRect(bubbleRect.left() + bubblePadding,
                                 bubbleRect.top() + bubblePadding,
                                 bubbleRect.width() - bubblePadding * 2,
                                 senderRect.height());

        QRect textArea = QRect(bubbleRect.left() + bubblePadding,
                               senderArea.bottom() + lineSpacing,
                               bubbleRect.width() - bubblePadding * 2,
                               messageRect.height());

        painter->drawText(senderArea, sender, textOption);
        painter->drawText(textArea, text, textOption);

        painter->restore();

    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        Message msg = index.data(Qt::UserRole).value<Message>();
        QFontMetrics fm(option.font);

        int maxTextWidth = option.rect.width() > 0 ? option.rect.width() * 0.6 : 400;

        QRect senderRect = fm.boundingRect(msg.isIncoming ? msg.peerID : "You");
        QRect messageRect = fm.boundingRect(QRect(0, 0, maxTextWidth, INT_MAX), Qt::TextWordWrap, msg.message);

        int height = senderRect.height() + 4 + messageRect.height() + 24; // 4 is spacing + 24 as padding top/bottom
        return QSize(option.rect.width(), height);
    }
};
