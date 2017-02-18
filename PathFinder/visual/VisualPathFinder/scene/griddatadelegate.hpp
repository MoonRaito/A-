#ifndef GRIDDATADELEGATE_HPP
#define GRIDDATADELEGATE_HPP

#include <QScopedPointer>
#include "boost/foreach.hpp"
#include "gridscene.h"
#include "cellitem.h"

#include <QDebug>

template <class Finder>
class GridDataDelegate : public GridScene::GridSceneDelegate
{
public:
    typedef typename Finder::grid_t grid_t;
    typedef typename Finder::pnode_vector_t pnode_vector_t;

    GridDataDelegate(GridScene *scene, Finder *finder);
    virtual void onPrepared(int row, int column);
    virtual void onCellItemCreated(CellItem * const cellItem);
    virtual void onCellItemChanged(CellItem * const cellItem);
    virtual void onSearchStarted(CellItem * const startCellItem,
                                 CellItem * const endCellItem);

protected:
    void syncWalkable(CellItem * const cellItem);
    void notifyNodesChanged();
    void notifyShortestPath(const pnode_vector_t &pnodes);
    void setCellData(CellItem * const cellItem,
                     const typename grid_t::pnode_t &node);
#ifndef NDEBUG
    void debugPrint();
#endif
    Finder *m_finder;
    QScopedPointer<grid_t> m_grid;
};

template <class Finder>
GridDataDelegate<Finder>::GridDataDelegate(GridScene *scene, Finder *finder) :
    GridScene::GridSceneDelegate(scene), m_finder(finder)
{
    Q_ASSERT(m_finder);
}

template <class Finder>
void GridDataDelegate<Finder>::onPrepared(int row, int column)
{
    m_grid.reset(new grid_t(column, row));
}

template <class Finder>
void GridDataDelegate<Finder>::onCellItemCreated(CellItem * const cellItem)
{
    syncWalkable(cellItem);
}

template <class Finder>
void GridDataDelegate<Finder>::onCellItemChanged(CellItem * const cellItem)
{
    syncWalkable(cellItem);
}

template <class Finder>
void GridDataDelegate<Finder>::onSearchStarted(CellItem * const startCellItem,
                                               CellItem * const endCellItem)
{
    if (m_finder && m_grid) {
        const QPoint &startPoint =
                startCellItem->data(GridScene::kCellAxis).toPoint();
        const QPoint &endPoint =
                endCellItem->data(GridScene::kCellAxis).toPoint();
        pnode_vector_t nodes = m_finder->FindPath(
                startPoint.x(), startPoint.y(),
                endPoint.x(), endPoint.y(), *m_grid);
        if (m_scene) {
            notifyNodesChanged();
            notifyShortestPath(nodes);
        }
        m_finder->ResetGrid(*m_grid);
    }
}

template <class Finder>
void GridDataDelegate<Finder>::syncWalkable(CellItem * const cellItem)
{
    if (m_grid) {
        const QPoint &p = cellItem->data(GridScene::kCellAxis).toPoint();
        bool unwalkable = cellItem->cellType() == CellItem::kCellUnwalkable;
        m_grid->SetWalkableAt(p.x(), p.y(), !unwalkable);
    }
}

template <class Finder>
void GridDataDelegate<Finder>::notifyNodesChanged()
{
    typedef typename grid_t::size_type size_t;
    typedef typename grid_t::pnode_t pnode_t;
    typedef typename grid_t::pnode_grid_t pnode_grid_t;
    pnode_grid_t nodes = m_grid->nodes();
    size_t w = m_grid->width(),
           h = m_grid->height();
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            pnode_t &node = (*nodes)[i][j];
            // node must be opened while it's closed
            if (node->opened/* || node->closed*/) {
                CellItem *cellItem = m_scene->cellItemAt(node->x, node->y);
                setCellData(cellItem, node);
                m_scene->callbackCellItemChanged(cellItem);
            }
        }
    }
}

template <class Finder>
void GridDataDelegate<Finder>::notifyShortestPath(const pnode_vector_t &pnodes)
{
    GridScene::CellItemVector cells;
    if (pnodes) {
        typedef typename grid_t::pnode_t pnode_t;
        BOOST_FOREACH(pnode_t &node, *pnodes) {
            cells.push_back(m_scene->cellItemAt(node->x, node->y));
        }
    }
    m_scene->callbackShortestPath(cells);
}

template <class Finder>
void GridDataDelegate<Finder>::setCellData(CellItem * const cellItem,
                                           const typename grid_t::pnode_t &node)
{
    cellItem->setData(GridScene::kCellF, node->f);
    cellItem->setData(GridScene::kCellG, node->g);
    cellItem->setData(GridScene::kCellH, node->h);
    //cellItem->setData(GridScene::kCellOpened, node->opened);
    cellItem->setData(GridScene::kCellClosed, node->closed);
}

#ifndef NDEBUG
template <class Finder>
void GridDataDelegate<Finder>::debugPrint()
{
    typedef typename grid_t::size_type size_t;
    typedef typename grid_t::pnode_t pnode_t;
    typedef typename grid_t::pnode_grid_t pnode_grid_t;
    pnode_grid_t nodes = m_grid->nodes();
    size_t w = m_grid->width(),
           h = m_grid->height();
    QDebug dbg = qDebug();
    for (size_t i = 0; i < h; ++i) {
        for (size_t j = 0; j < w; ++j) {
            pnode_t &node = (*nodes)[i][j];
            dbg.nospace() << "(" << node->x
                          << " " << node->y
                          << " " << node->walkable
                          << ") ";
        }
        dbg << "\n";
    }
}
#endif

#endif // GRIDDATADELEGATE_HPP
