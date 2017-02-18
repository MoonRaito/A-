#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QScopedPointer>

class QGraphicsView;

class GridScene;
class AStarFinder;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void resizeEvent(QResizeEvent *);

private slots:
    void slotHeuristicRadioButtonToggled(bool checked);
    void slotOptionsCheckedButtonToggled(bool checked);
    void slotWeightLineEditTextChanged(const QString &text);

private:
    void setupUi();
    void setupConnection();
    void prepareScene(const QSize &size);

    Ui::MainWindow *m_ui;
    GridScene *m_gridScene;
    QGraphicsView *m_gridView;
    QScopedPointer<AStarFinder> m_finder;

    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
