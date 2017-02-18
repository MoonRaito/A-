#ifndef CELLITEM_H
#define CELLITEM_H

#include <QGraphicsRectItem>
#include <QSharedPointer>
#include <QTextOption>

#define CI_PROPERTY(varType, varName, setFunName) \
protected: varType m_##varName; \
public: inline varType varName(void) const { return m_##varName; } \
public: inline void setFunName(const varType &var) { m_##varName = var; }

class CellItemOption;

class CellItem : public QGraphicsRectItem
{
public:
    typedef QSharedPointer<CellItemOption> SharedOption;
    enum { Type = UserType + 1 };
    enum CellType {
        kCellNormal = 0,  // walkable
        kCellUnwalkable,
        kCellStart,
        kCellEnd,
        kCellOpened,
        kCellClosed,
    };

    explicit CellItem(const QRectF &rect,
                      QGraphicsItem *parent = 0,
                      SharedOption option = SharedOption());
    ~CellItem();

    // aid to swap its properties
    CellItem &operator=(const CellItem &cell);

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0);
    inline int type() const { return Type; }

protected:
    void draw(QPainter &painter);
    void drawText(QPainter &painter, const QRectF &rect,
                  const QString &text, Qt::Alignment alignment);

    SharedOption m_option;
    CI_PROPERTY(CellType, cellType, setCellType)
    CI_PROPERTY(QString, topLeftText, setTopLeftText)
    CI_PROPERTY(QString, topRightText, setTopRightText)
    CI_PROPERTY(QString, bottomLeftText, setBottomLeftText)
    CI_PROPERTY(QString, bottomRightText, setBottomRightText)
    CI_PROPERTY(bool, isTextDrawn, setTextDrawn)
};

class CellItemOption
{
public:
    CellItemOption();
    ~CellItemOption();

    inline QColor fillColor(CellItem::CellType cellType) const;
    inline void setFillColor(CellItem::CellType cellType, const QColor &color);

protected:
    QMap<CellItem::CellType, QColor> m_fillColorMap;
    CI_PROPERTY(qreal, strokeWidth, setStrokeWidth)
    CI_PROPERTY(QColor, strokeColor, setStrokeColor)
    CI_PROPERTY(QColor, textColor, setTextColor)

    friend class CellItem;
};

inline QColor CellItemOption::fillColor(CellItem::CellType cellType) const
{ return m_fillColorMap[cellType]; }
inline void CellItemOption::setFillColor(CellItem::CellType cellType, const QColor &color)
{ m_fillColorMap[cellType] = color; }

#endif // CELLITEM_H
