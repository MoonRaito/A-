#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsView>
#include "scene/gridscene.h"
#include "scene/griddatadelegate.hpp"
#include "finders/astarfinder.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::MainWindow)
{
    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setMinimumSize(600, 400);
    m_ui->setupUi(this);
    setupUi();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    prepareScene(m_gridView->size());
}

void MainWindow::slotHeuristicRadioButtonToggled(bool checked)
{
    if (checked) {
        QObject *obj = sender();
        FinderOption::heuristic_t h;
        if (obj == m_ui->manhattanRButton) {
            h = heuristic::Manhattan();
        } else if (obj == m_ui->euclideanRButton) {
            h = heuristic::Euclidean();
        } else if (obj == m_ui->chebyshevRButton) {
            h = heuristic::Chebyshev();
        } else {
            Q_ASSERT(false);
        }
        m_finder->Option().heuristic = h;
    }
}

void MainWindow::slotOptionsCheckedButtonToggled(bool checked)
{
    QObject *obj = sender();
    if (obj == m_ui->allowDiagonalCBox) {
        m_finder->Option().allow_diagonal = checked;
    } else if (obj == m_ui->dontCrossCornersCBox) {
        m_finder->Option().dont_cross_corners = checked;
    } else {
        Q_ASSERT(false);
    }
}

void MainWindow::slotWeightLineEditTextChanged(const QString &text)
{
    m_finder->Option().weight = text.toInt();
}

void MainWindow::setupUi()
{
    m_gridScene = new GridScene(this);

    m_finder.reset(new AStarFinder);
    typedef GridDataDelegate<AStarFinder> Delegate;
    Delegate *delegate = new Delegate(m_gridScene, m_finder.data());
    m_gridScene->setDelegate(GridScene::SharedDelegate(delegate));

    m_gridView = new QGraphicsView(m_gridScene, this);
    m_gridView->setStyleSheet("border: 0px");
    m_gridView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_gridView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_ui->panelLayout->addWidget(m_gridView);

    m_ui->manhattanRButton->setChecked(true);
    m_ui->weightLineEdit->setValidator(new QIntValidator(1, 100, this));

    setupConnection();
}

void MainWindow::setupConnection()
{
    connect(m_ui->startButton, SIGNAL(clicked()),
            m_gridScene, SLOT(startSearch()));

    connect(m_ui->manhattanRButton, SIGNAL(toggled(bool)),
            this, SLOT(slotHeuristicRadioButtonToggled(bool)));
    connect(m_ui->euclideanRButton, SIGNAL(toggled(bool)),
            this, SLOT(slotHeuristicRadioButtonToggled(bool)));
    connect(m_ui->chebyshevRButton, SIGNAL(toggled(bool)),
            this, SLOT(slotHeuristicRadioButtonToggled(bool)));

    connect(m_ui->allowDiagonalCBox, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsCheckedButtonToggled(bool)));
    connect(m_ui->dontCrossCornersCBox, SIGNAL(toggled(bool)),
            this, SLOT(slotOptionsCheckedButtonToggled(bool)));

    connect(m_ui->weightLineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotWeightLineEditTextChanged(const QString &)));
}

void MainWindow::prepareScene(const QSize &size)
{
    m_gridScene->prepare(QRectF(0, 0, size.width(), size.height()));
}
