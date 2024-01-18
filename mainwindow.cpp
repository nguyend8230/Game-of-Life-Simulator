#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // set the lines for the grid
    for(int i = 0; i < GRID_WIDTH; i++) {
        for(int j = 0; j < GRID_HEIGHT; j++) {
            sq_list.push_back(QRect(i*CELL_DIMENSION,j*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
        }
    }

    ui->setupUi(this);
    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QRect rect = event->rect();

    if(rect.height() > CELL_DIMENSION || rect.width() > CELL_DIMENSION) {
        painter.fillRect(rect,Qt::white);
        painter.drawRects(sq_list);
        for(const auto &it: as_const(alive_cells)) {
            if(alive_cells.contains((QList<int> {it[0],it[1]}))) {
                painter.fillRect(QRect(it[0]*CELL_DIMENSION,it[1]*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION),Qt::black);
            }
        }
    }
    else {
        int col = int(rect.x()/CELL_DIMENSION);
        int row = int(rect.y()/CELL_DIMENSION);

        // if the cell is alive, paint it black
        if(alive_cells.contains((QList<int> {col,row}))) {
            painter.fillRect(rect,Qt::black);
        }
        // if the cell is dead, paint it white
        else {
            painter.fillRect(rect,Qt::white);
            painter.drawRect(rect);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    int col = int(event->position().x()/CELL_DIMENSION);
    int row = int(event->position().y()/CELL_DIMENSION);
    QList<int> cell {col,row};

    // if the click is out of bound or if there's currently a simulation running
    if(col >= GRID_WIDTH || row >= GRID_HEIGHT || simulation_toggle) return;

    flipped_cells.insert(cell);
    flip_cell(cell);

    update(QRect(col*CELL_DIMENSION,row*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
}

void MainWindow::on_start_sim_button_clicked() {
    simulation_toggle = !simulation_toggle;
    if(simulation_toggle) {
        clean_flipped_cells();
        simulate_all();
    }
}

void MainWindow::on_reset_button_clicked() {
    // stop all animation
    simulation_toggle = false;

    neighbors_count.clear();
    alive_cells.clear();
    flipped_cells.clear();
    update();
}
