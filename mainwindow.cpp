#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDoubleValidator>
#include <cmath>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);
    QStringList headers;
    headers << "X" << "Y";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->lineEditX->setValidator(new QDoubleValidator(this));
    ui->lineEditY->setValidator(new QDoubleValidator(this));

    connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(onAddPointClicked()));
    connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(onRemovePointClicked()));
    connect(ui->pushButtonCalculate, SIGNAL(clicked()), this, SLOT(onCalculateClicked()));
    connect(ui->pushButtonClear, SIGNAL(clicked()), this, SLOT(onClearAllClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddPointClicked()
{
    bool xOk, yOk;
    double x = ui->lineEditX->text().toDouble(&xOk);
    double y = ui->lineEditY->text().toDouble(&yOk);

    if (!xOk || !yOk) {
        QMessageBox::warning(this, "Input Error", "Please enter valid numeric coordinates");
        return;
    }

    points.append(Point(x, y));
    updateTable();
    ui->lineEditX->clear();
    ui->lineEditY->clear();
}

void MainWindow::onRemovePointClicked()
{
    if (points.isEmpty()) {
        QMessageBox::warning(this, "Remove Error", "No points to remove");
        return;
    }

    // В Qt 4 нет removeLast, используем remove с индексом последнего элемента
    points.remove(points.size() - 1);
    updateTable();
}

void MainWindow::onClearAllClicked()
{
    points.clear();
    updateTable();
    ui->lineEditSelfIntersecting->clear();
    ui->lineEditPerimeter->clear();
    ui->lineEditDrawWithoutLifting->clear();
}

void MainWindow::updateTable()
{
    ui->tableWidget->setRowCount(points.size());

    for (int i = 0; i < points.size(); ++i) {
        QTableWidgetItem *xItem = new QTableWidgetItem(QString::number(points[i].x));
        QTableWidgetItem *yItem = new QTableWidgetItem(QString::number(points[i].y));

        xItem->setFlags(xItem->flags() & ~Qt::ItemIsEditable);
        yItem->setFlags(yItem->flags() & ~Qt::ItemIsEditable);

        ui->tableWidget->setItem(i, 0, xItem);
        ui->tableWidget->setItem(i, 1, yItem);
    }
}

double MainWindow::crossProduct(const Point &a, const Point &b, const Point &c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool MainWindow::onSegment(const Point &p, const Point &a, const Point &b)
{
    if (p.x <= std::max(a.x, b.x) && p.x >= std::min(a.x, b.x) &&
        p.y <= std::max(a.y, b.y) && p.y >= std::min(a.y, b.y)) {
        return true;
    }
    return false;
}

bool MainWindow::segmentsIntersect(const Point &a1, const Point &a2, const Point &b1, const Point &b2)
{
    double d1 = crossProduct(b1, b2, a1);
    double d2 = crossProduct(b1, b2, a2);
    double d3 = crossProduct(a1, a2, b1);
    double d4 = crossProduct(a1, a2, b2);

    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        return true;
    }

    if (fabs(d1) < 1e-9 && onSegment(a1, b1, b2)) return true;
    if (fabs(d2) < 1e-9 && onSegment(a2, b1, b2)) return true;
    if (fabs(d3) < 1e-9 && onSegment(b1, a1, a2)) return true;
    if (fabs(d4) < 1e-9 && onSegment(b2, a1, a2)) return true;

    return false;
}

double MainWindow::distance(const Point &a, const Point &b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool MainWindow::isSelfIntersecting()
{
    int n = points.size();
    if (n < 4) return false;

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;

        for (int k = i + 2; k < n; ++k) {
            int l = (k + 1) % n;

            // Пропускаем смежные ребра
            if (l == i || l == j) continue;

            if (segmentsIntersect(points[i], points[j], points[k], points[l])) {
                return true;
            }
        }
    }

    return false;
}

double MainWindow::calculatePerimeter()
{
    int n = points.size();
    if (n < 2) return 0.0;
    if (n == 2) return 2.0 * distance(points[0], points[1]);

    double perimeter = 0.0;

    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        perimeter += distance(points[i], points[j]);
    }

    return perimeter;
}

bool MainWindow::canDrawWithoutLifting()
{
    return points.size() >= 2;
}

void MainWindow::onCalculateClicked()
{
    if (points.size() < 3) {
        QMessageBox::warning(this, "Calculation Error", "At least 3 points are required for analysis");
        return;
    }

    bool selfIntersecting = isSelfIntersecting();
    ui->lineEditSelfIntersecting->setText(selfIntersecting ? "Yes" : "No");

    double perimeter = calculatePerimeter();
    ui->lineEditPerimeter->setText(QString::number(perimeter, 'f', 3));

    bool canDraw = canDrawWithoutLifting();
    ui->lineEditDrawWithoutLifting->setText(canDraw ? "Yes" : "No");
}
