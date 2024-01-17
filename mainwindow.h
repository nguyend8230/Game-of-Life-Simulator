#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSet>
#include <QTimer>
#include <QScreen>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QApplication>

#define GRID_WIDTH 125
#define GRID_HEIGHT 75
#define CELL_DIMENSION 10

QT_BEGIN_NAMESPACE

using namespace std;

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_reset_button_clicked();
    void on_start_sim_button_clicked();

private:
    Ui::MainWindow *ui;

    // -----VARIABLES-----

    bool simulation_toggle = false;

    // list of squares in the grid
    QList<QRect> sq_list;

    // set to represent the alive cells for faster access
    QSet<QList<int>> alive_cells;

    // // 2d list to keep track of the number of neighbors a cell has
    QMap<QList<int>,int> neighbors_count;

    // -----FUNCTIONS-----

    //simulation functions
    void set_neighbors_of_cell(int col, int row);
    void simulate();
    void simulate_all();

    // events
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
};
#endif // MAINWINDOW_H