#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDoubleValidator>
#include <QSpinBox>
#include <QTextEdit>
#include <cmath>
#include <algorithm>
#include <limits>

const double INF = 1e9;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , N(0)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);

    ui->spinBoxVertices->setMinimum(2);
    ui->spinBoxVertices->setMaximum(10);
    ui->spinBoxVertices->setValue(3);

    ui->lineEditStart->setValidator(new QIntValidator(1, 10, this));
    ui->lineEditEnd->setValidator(new QIntValidator(1, 10, this));

    connect(ui->pushButtonGenerate, SIGNAL(clicked()), this, SLOT(onGenerateMatrixClicked()));
    connect(ui->pushButtonFind, SIGNAL(clicked()), this, SLOT(onFindPathClicked()));
    connect(ui->pushButtonRandomize, SIGNAL(clicked()), this, SLOT(onRandomizeMatrixClicked()));
    connect(ui->pushButtonClear, SIGNAL(clicked()), this, SLOT(onClearMatrixClicked()));

    log("Program started. Generate adjacency matrix.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(const QString &message)
{
    ui->textEditConsole->append(message);
}

void MainWindow::onGenerateMatrixClicked()
{
    N = ui->spinBoxVertices->value();

    ui->tableWidget->setColumnCount(N);
    ui->tableWidget->setRowCount(N);

    QStringList headers;
    for (int i = 1; i <= N; ++i) {
        headers << QString::number(i);
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setVerticalHeaderLabels(headers);

    adjacencyMatrix.resize(N);
    for (int i = 0; i < N; ++i) {
        adjacencyMatrix[i].resize(N);
        for (int j = 0; j < N; ++j) {
            adjacencyMatrix[i][j] = 0.0;
        }
    }

    updateTable();

    log(QString("Adjacency matrix created for %1 vertices").arg(N));
}

void MainWindow::onRandomizeMatrixClicked()
{
    if (N == 0) {
        QMessageBox::warning(this, "Error", "Generate matrix first");
        return;
    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                adjacencyMatrix[i][j] = 0.0;
            } else {
                if (rand() % 100 < 70) {
                    adjacencyMatrix[i][j] = (rand() % 200 + 1) / 10.0;
                    adjacencyMatrix[j][i] = adjacencyMatrix[i][j];
                } else {
                    adjacencyMatrix[i][j] = 0.0;
                    adjacencyMatrix[j][i] = 0.0;
                }
            }
        }
    }

    updateTable();
    log("Matrix filled with random values");
}

void MainWindow::onClearMatrixClicked()
{
    if (N == 0) return;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            adjacencyMatrix[i][j] = 0.0;
        }
    }

    updateTable();
    log("Matrix cleared");
}

void MainWindow::updateTable()
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(adjacencyMatrix[i][j], 'f', 2));
            item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tableWidget->setItem(i, j, item);
        }
    }
}

bool MainWindow::validateMatrix()
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            bool ok;
            QTableWidgetItem *item = ui->tableWidget->item(i, j);
            if (!item) {
                QMessageBox::warning(this, "Error", QString("Cell [%1,%2] is empty").arg(i+1).arg(j+1));
                return false;
            }

            double value = item->text().toDouble(&ok);
            if (!ok || value < 0) {
                QMessageBox::warning(this, "Error", QString("Cell [%1,%2] contains invalid value").arg(i+1).arg(j+1));
                return false;
            }

            adjacencyMatrix[i][j] = value;
        }
    }

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            if (fabs(adjacencyMatrix[i][j] - adjacencyMatrix[j][i]) > 1e-6) {
                log(QString("Warning: matrix is asymmetric [%1,%2] != [%2,%1]").arg(i+1).arg(j+1));
            }
        }
    }

    return true;
}

void MainWindow::onFindPathClicked()
{
    if (N == 0) {
        QMessageBox::warning(this, "Error", "Generate matrix first");
        return;
    }

    bool startOk, endOk;
    int start = ui->lineEditStart->text().toInt(&startOk);
    int end = ui->lineEditEnd->text().toInt(&endOk);

    if (!startOk || !endOk) {
        QMessageBox::warning(this, "Error", "Enter valid vertex numbers");
        return;
    }

    if (start < 1 || start > N || end < 1 || end > N) {
        QMessageBox::warning(this, "Error", "Vertex numbers must be from 1 to " + QString::number(N));
        return;
    }

    start--;
    end--;

    if (!validateMatrix()) {
        return;
    }

    log("\n--- Adjacency Matrix ---");
    for (int i = 0; i < N; ++i) {
        QString row;
        for (int j = 0; j < N; ++j) {
            row += QString("%1 ").arg(adjacencyMatrix[i][j], 6, 'f', 2);
        }
        log(row);
    }

    dijkstra(start, end);
}

void MainWindow::dijkstra(int start, int end)
{
    QVector<double> dist(N, INF);
    QVector<int> prev(N, -1);
    QVector<bool> visited(N, false);

    dist[start] = 0;

    log(QString("\n--- Dijkstra's Algorithm: finding path from %1 to %2 ---")
        .arg(start + 1).arg(end + 1));

    for (int count = 0; count < N - 1; ++count) {
        double minDist = INF;
        int minIndex = -1;

        for (int v = 0; v < N; ++v) {
            if (!visited[v] && dist[v] < minDist) {
                minDist = dist[v];
                minIndex = v;
            }
        }

        if (minIndex == -1) break;

        visited[minIndex] = true;

        log(QString("Step %1: selected vertex %2 (distance = %3)")
            .arg(count + 1).arg(minIndex + 1).arg(dist[minIndex]));

        for (int v = 0; v < N; ++v) {
            double weight = adjacencyMatrix[minIndex][v];
            if (!visited[v] && weight > 0 && dist[minIndex] + weight < dist[v]) {
                dist[v] = dist[minIndex] + weight;
                prev[v] = minIndex;
                log(QString("  Updating vertex %1: new distance = %2")
                    .arg(v + 1).arg(dist[v]));
            }
        }
    }

    log("\n--- Results ---");
    log(QString("Shortest distance from %1 to %2: %3")
        .arg(start + 1).arg(end + 1)
        .arg(dist[end] >= INF ? "unreachable" : QString::number(dist[end], 'f', 3)));

    if (dist[end] < INF) {
        QVector<int> path;
        for (int v = end; v != -1; v = prev[v]) {
            path.push_back(v);
        }

        QString pathStr;
        for (int i = path.size() - 1; i >= 0; --i) {
            pathStr += QString::number(path[i] + 1);
            if (i > 0) pathStr += " -> ";
        }

        log(QString("Shortest path: %1").arg(pathStr));
        log(QString("Path length: %1").arg(dist[end], 0, 'f', 3));
    } else {
        log(QString("Path from %1 to %2 does not exist").arg(start + 1).arg(end + 1));
    }

    log("-------------------\n");
}
