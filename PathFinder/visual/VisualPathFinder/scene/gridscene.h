#ifndef GRIDSCENE_H
#define GRIDSCENE_H

#include <QGraphicsScene>
#include <QMutex>
#include <QSharedPointer>
#include <QVector>
#include <QList>

class QGraphicsLineItem;
class QGraphicsRectItem;
class CellItem;

class GridScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum State {
        kStateIdle = 0,
        kStateToggle,  // toggle cell walkable attribute
        kStateSwap,    // swap start/end cell with others
        kStateAnim,
    };
    enum CellDataType {
        kCellAxis = 0x00,
        kCellF,
        kCellG,
        kCellH,
        kCellOpened,
        kCellClosed,
    };
    typedef QVector<CellItem *> CellItemVector;
    typedef QVector<CellItemVector> CellItemGrid;
    typedef QVector<QGraphicsLineItem *> LineItemVector;
    typedef QList<CellItem *> CellItemList;

    class GridSceneDelegate {
    public:
        GridSceneDelegate(GridScene *scene) : m_scene(scene) {}
        virtual ~GridSceneDelegate() {}
        virtual void onPrepared(int row, int column) {}
        virtual void onCellItemCreated(CellItem * const cellItem) {}
        virtual void onCellItemChanged(CellItem * const cellItem) {}
        virtual void onSearchStarted(CellItem * const startCellItem,
                                     CellItem * const endCellItem) {}
    protected:
        GridScene *m_scene;
    };
    typedef QSharedPointer<GridSceneDelegate> SharedDelegate;

    explicit GridScene(QObject *parent = 0);
    ~GridScene();

    inline void setDelegate(SharedDelegate delegate);
    CellItem *cellItemAt(int x, int y);
    // Callback to notify the cellItem that should be changed.
    void callbackCellItemChanged(CellItem *cellItem);
    // Callback to notify the shortest path after find it.
    void callbackShortestPath(const CellItemVector &cells);

public slots:
    void prepare(const QRectF &rect);
    void startSearch();

protected:
    void setupCells(int row, int column);
    void itemSelectedAtScenePos(const QPointF &pos);
    void itemSelected(CellItem *cellItem);
    void toggleCellWalkable(CellItem *cellItem);
    void swapCellItem(CellItem *prev, CellItem *now);

    void addLines(const CellItemVector &cells);
    void addLine4TwoCells(CellItem *prev, CellItem *next);
    void clearLines();
    void resetChangedCells();
    bool isChangedCell(CellItem *cellItem, int &index);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    QSizeF m_cellSize;  // cell size option
    QPen m_linePen;     // line pen option

    State m_state;
    QGraphicsRectItem *m_gridItem;
    CellItem *m_startCell;
    CellItem *m_endCell;
    CellItem *m_selectedCell;
    QMutex m_mutex;
    SharedDelegate m_delegate;
    CellItemGrid m_cellGrid;
    CellItemList m_ChangedCells;
    LineItemVector m_lineVector;
};

inline void GridScene::setDelegate(SharedDelegate delegate)
{ m_delegate = delegate; }

#endif // GRIDSCENE_H
