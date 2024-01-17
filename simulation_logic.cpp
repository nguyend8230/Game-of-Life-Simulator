#include "mainwindow.h"

void MainWindow::set_neighbors_of_cell(int col, int row) {
    int arr[] = {1,0,-1,0,1,1,-1,-1,1};
    if(alive_cells.contains((QList<int> {col,row}))) {
        for(int i = 0; i < 8; i++) {
            if(col+arr[i] >= 0 && col+arr[i] < GRID_WIDTH && row+arr[i+1] >= 0 && row+arr[i+1] < GRID_HEIGHT) {
                neighbors_count[QList<int>{col+arr[i],row+arr[i+1]}]++;
            }
        }
    }
    else {
        for(int i = 0; i < 8; i++) {
            if(col+arr[i] >= 0 && col+arr[i] < GRID_WIDTH && row+arr[i+1] >= 0 && row+arr[i+1] < GRID_HEIGHT) {
                neighbors_count[QList<int>{col+arr[i],row+arr[i+1]}]--;
            }
        }
    }
}

void MainWindow::simulate() {
    if(alive_cells.empty()) {
        simulation_toggle = false;
        return;
    }

    int arr[] = {1,0,-1,0,1,1,-1,-1,1};

    // dead cells that can potentially be alive
    QSet<QList<int>> candidates;

    // cells that got flipped because of the simulation
    QList<QList<int>> flipped_cells;

    // set the candidates
    for(const auto& it: as_const(alive_cells)) {
        int col = it[0];
        int row = it[1];

        for(int i = 0; i < 8; i++) {
            if(col+arr[i] >= 0 && col+arr[i] < GRID_WIDTH &&
                row+arr[i+1] >= 0 && row+arr[i+1] < GRID_HEIGHT &&
                !alive_cells.contains(QList<int>{col+arr[i],row+arr[i+1]}))
            {
                candidates.insert(QList<int> {col+arr[i],row+arr[i+1]});
            }
        }
    }

    // go through the alive cells and remove them from alive_cells if needed
    for(auto it = alive_cells.constBegin(); it != alive_cells.constEnd(); it++) {
        int col = (*it)[0];
        int row = (*it)[1];

        // alive cell is dead
        if(neighbors_count[QList<int>{col,row}] < 2 || neighbors_count[QList<int>{col,row}] > 3) {
            flipped_cells.push_back(QList<int> {col,row});
        }
    }

    // remove elements from a set after iterating through it
    for(const QList<int>& fc: flipped_cells) {
        alive_cells.remove(fc);
    }

    // go through the candidate cells and put them in alive_cells if needed
    for(QList<int> c: candidates) {
        int col = c[0];
        int row = c[1];

        // dead cell is respawned
        if(neighbors_count[QList<int>{col,row}] == 3) {
            alive_cells.insert(QList<int> {col,row});
            flipped_cells.push_back(QList<int> {col,row});
        }
    }

    // update all the cells that need to be updated
    for(QList<int> fc: flipped_cells) {
        set_neighbors_of_cell(fc[0],fc[1]);
        update(QRect(fc[0]*CELL_DIMENSION,fc[1]*CELL_DIMENSION,CELL_DIMENSION,CELL_DIMENSION));
    }

    // remove the dead cells without any neighbors from neighbors_count
    erase_if(neighbors_count, [=](auto it) {
        return (!alive_cells.contains(it.key()) && !it.value());
    });
}

void MainWindow::simulate_all() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        simulate();
        if(!simulation_toggle) {
            timer->stop();
        }
    });
    timer->start(300);
}
