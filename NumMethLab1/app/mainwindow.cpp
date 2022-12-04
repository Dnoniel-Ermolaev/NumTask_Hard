#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidgetMain2->setHorizontalHeaderLabels(QStringList() << "Xn" << "V1n" << "V1_2n" << "V1n - V1_2n"<< "V1_Fn" << "V2n" << "V2_2n" << "V2n - V2_2n" << "V2_Fn"<< "||ОЛП||" << "h_n" << "Умнож. шага" << "Дел. шага"<< "Un(1)" << "|Un(1) - V_Fn(1)|"<< "Un(2)" << "|Un(2) - V_F(2)n|");
    ui->tableWidgetMain2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->graphMain2U1->xAxis->setLabel("x");
    ui->graphMain2U1->yAxis->setLabel("Численное решение");
    ui->graphMain2U2->xAxis->setLabel("x");
    ui->graphMain2U2->yAxis->setLabel("Точное решение");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonMain2Run_clicked()
{
    double u1 = ui->lineEditMain2U1->text().toDouble();
    double u2 = ui->lineEditMain2U2->text().toDouble();
    double b = ui->lineEditMain2B->text().toDouble();
    double h0 = ui->lineEditMain2H0->text().toDouble();
    double EPS = ui->lineEdit->text().toDouble();
    int N = ui->lineEditMain2N->text().toInt();
    double eps = ui->lineEditMain2LP->text().toDouble();
    bool ctl = ui->radioButtonMain2CLP->isChecked();

    MainWork2 M(b, u1, u2, h0, N, ctl, eps);
    M.EPS=EPS;
    M.Run();
    QVector<double> x(M.grid.begin(), M.grid.end());
    QVector<double> y1(M.final_num_values_1.begin(), M.final_num_values_1.end());
    QVector<double> y2(M.final_num_values_2.begin(), M.final_num_values_2.end());

    QVector<double> U(M.true_values_1.begin(), M.true_values_1.end());
    QVector<double> U2(M.true_values_2.begin(), M.true_values_2.end());
    //QVector<double> u2(M.final_num_values_2.begin(), M.final_num_values_2.end());
    //qDebug() << "u1:\n" << y1 << "\nu2:\n" << y2;
    int j = 0;
    QVector<int> delim;
    delim.push_back(1);
    bool up = std::abs(y1[0]) < std::abs(y1[1]);
    while(j < y1.size()-1)
    {
        if ((y1[j] < y1[j+1] && !up)||(y1[j] > y1[j+1] && up))
        {
            delim.push_back(j+1);
            up = !up;
        }
        j++;
    }
    delim.push_back(j+1);
    //qDebug() << "\nDelim: " << delim << "\n";
    QVector<QVector<double>> y1_parts(delim.size()-1);
    QVector<QVector<double>> y2_parts(delim.size()-1);
    for (int i = 0; i < y1_parts.size(); i++)
    {
        y1_parts[i] = QVector<double>(y1.begin()+delim[i]-1, y1.begin()+delim[i+1]);
        y2_parts[i] = QVector<double>(y2.begin()+delim[i]-1, y2.begin()+delim[i+1]);
        //qDebug() << "\nparts " << i << ":\n" << y1_parts[i];
    }

    ui->graphMain2U1->addGraph();
    ui->graphMain2U1->graph(0)->setData(x, y1, true);
    ui->graphMain2U1->graph(0)->setPen(QPen(QColor(255,1, 255, 255)));
    ui->graphMain2U1->graph(0)->rescaleAxes();

    ui->graphMain2U1->addGraph();
    ui->graphMain2U1->graph(1)->setData(x, y2, true);
    ui->graphMain2U1->graph(1)->setPen(QPen(QColor(10,10, 255, 255)));
    ui->graphMain2U1->graph(1)->rescaleAxes();
    ui->graphMain2U1->setInteraction(QCP::iRangeZoom,true);
    ui->graphMain2U1->setInteraction(QCP::iRangeDrag,true);
    ui->graphMain2U1->replot();

    ui->graphMain2U2->addGraph();
    ui->graphMain2U2->graph(0)->setData(x, U, true);
    ui->graphMain2U2->graph(0)->setPen(QPen(QColor(255,1, 255, 255)));
    ui->graphMain2U2->graph(0)->rescaleAxes();

    ui->graphMain2U2->addGraph();
    ui->graphMain2U2->graph(1)->setData(x, U2, true);
    ui->graphMain2U2->graph(1)->setPen(QPen(QColor(10,10, 255, 255)));
    ui->graphMain2U2->graph(1)->rescaleAxes();
    ui->graphMain2U2->setInteraction(QCP::iRangeZoom,true);
    ui->graphMain2U2->setInteraction(QCP::iRangeDrag,true);
    ui->graphMain2U2->replot();

    M.grid_step.pop_back();
    M.grid_step.insert(M.grid_step.begin(), h0);

    ui->tableWidgetMain2->clearContents();
    ui->tableWidgetMain2->setRowCount(0);

    double maxOLP = 0;
    double maxTrueDiff_1 = 0;
    double indMaxTrueDiff_1 = 0;
    double maxTrueDiff_2 = 0;
    double indMaxTrueDiff_2 = 0;
    M.grid_step[0]=0;
    int indMaxStep = 0, indMinStep = 0;
    for (int i = 0; i < M.grid.size(); i++)
    {
        QTableWidgetItem *x = new QTableWidgetItem(QString::number(M.grid[i]));
        QTableWidgetItem *v1 = new QTableWidgetItem(QString::number(M.num_values_1[i]));
        QTableWidgetItem *v1_2 = new QTableWidgetItem(QString::number(M.d_num_values_1[i]));
        QTableWidgetItem *diff1 = new QTableWidgetItem(QString::number(M.num_values_1[i] - M.d_num_values_1[i]));
        QTableWidgetItem *v2 = new QTableWidgetItem(QString::number(M.num_values_2[i]));
        QTableWidgetItem *v2_2 = new QTableWidgetItem(QString::number(M.d_num_values_2[i]));
        QTableWidgetItem *diff2 = new QTableWidgetItem(QString::number(M.num_values_2[i] - M.d_num_values_2[i]));
        double olp1 = std::abs(M.d_num_values_1[i] - M.num_values_1[i]) / 3 * 8;
        double olp2 = std::abs(M.d_num_values_2[i] - M.num_values_2[i]) / 3 * 8;
        double tmp1 = std::max(olp1, olp2);
        if (tmp1 > maxOLP)
            maxOLP = tmp1;
        QTableWidgetItem *OLP = new QTableWidgetItem(QString::number(tmp1));
        QTableWidgetItem *U1 = new QTableWidgetItem(QString::number(M.true_values_1[i]));
        QTableWidgetItem *U2 = new QTableWidgetItem(QString::number(M.true_values_2[i]));
        double tmp2 = M.grid_step[i];
        if (M.grid_step[indMaxStep] < tmp2)
            indMaxStep = i;
        if (M.grid_step[indMinStep] > tmp2)
            indMinStep = i;
        QTableWidgetItem *h = new QTableWidgetItem(QString::number(M.grid_step[i]));
        int tmp4 = i > 0 ? M.div2[i] - M.div2[i-1] : 0;
        int tmp5 = i > 1 ? M.mult2[i-1] - M.mult2[i-2] : 0;
        QTableWidgetItem *c1 = new QTableWidgetItem(QString::number(tmp4));
        QTableWidgetItem *c2 = new QTableWidgetItem(QString::number(tmp5));

        double tmp3_1 = std::abs(M.true_values_1[i] - M.final_num_values_1[i]);
                if (tmp3_1 > maxTrueDiff_1)
                {
                    maxTrueDiff_1 = tmp3_1;
                    indMaxTrueDiff_1 = i;
                }
                double tmp3_2 = std::abs(M.true_values_2[i] - M.final_num_values_2[i]);
                        if (tmp3_2 > maxTrueDiff_2)
                        {
                            maxTrueDiff_2 = tmp3_2;
                            indMaxTrueDiff_2 = i;
                        }
        QTableWidgetItem *true_diff_1 = new QTableWidgetItem(QString::number(tmp3_1));
        QTableWidgetItem *true_diff_2 = new QTableWidgetItem(QString::number(tmp3_2));
        QTableWidgetItem *final_v1 = new QTableWidgetItem(QString::number(M.final_num_values_1[i]));
        QTableWidgetItem *final_v2 = new QTableWidgetItem(QString::number(M.final_num_values_2[i]));

        ui->tableWidgetMain2->insertRow(i);
        ui->tableWidgetMain2->setVerticalHeaderItem(i, new QTableWidgetItem(QString::number(i)));
        ui->tableWidgetMain2->setItem(i, 0, x);
        ui->tableWidgetMain2->setItem(i, 1, v1);
        ui->tableWidgetMain2->setItem(i, 2, v1_2);
        ui->tableWidgetMain2->setItem(i, 3, diff1);
        ui->tableWidgetMain2->setItem(i, 4, final_v1);
        ui->tableWidgetMain2->setItem(i, 5, v2);
        ui->tableWidgetMain2->setItem(i, 6, v2_2);
        ui->tableWidgetMain2->setItem(i, 7, diff2);
        ui->tableWidgetMain2->setItem(i, 8, final_v2);
        ui->tableWidgetMain2->setItem(i, 9, OLP);
        ui->tableWidgetMain2->setItem(i, 10, h);
        ui->tableWidgetMain2->setItem(i, 11, c1);
        ui->tableWidgetMain2->setItem(i, 12, c2);
        ui->tableWidgetMain2->setItem(i, 13, U1);
        ui->tableWidgetMain2->setItem(i, 14, true_diff_1);
        ui->tableWidgetMain2->setItem(i, 15, U2);
        ui->tableWidgetMain2->setItem(i, 16, true_diff_2);

    }
    QString ref;
    if (ctl)
    {ref = "№ варианта задания: (6)\n"
            "Тип задачи: Жёсткая система\n"
            "Метод: Метод Рунге Кутта порядка p=2\n"
            "Начальные значения:\n"
            "X_0=0\tU1_0="+QString::number(u1)+"\tU2_0="+QString::number(u2)+"\t(Условия задачи Коши)\n"
            "b="+QString::number(b)+"\tEps1 = "+QString::number(EPS)+"\t(Условия остановки шага)\n"
            "h0="+QString::number(h0)+"\tNmax="+QString::number(N)+"\t(Начальный шаг интегрирования и максимальное число шагов метода)\n"
            "Eps2="+QString::number(eps)+"\t(Контроль модуля локальной погрешности)\n"
            "РЕЗУЛЬТАТ РАСЧЁТА:\n"
            "Контроль локальной погрешности: ВКЛ\n"
            "Число посчитанных точек N = "  + QString::number(M.grid.size()-1)
            + "\nРазница b-Xn = " +  QString::number(M.right_border - M.grid.back())
            + "\nCчёт остановился на точке Xn = " + QString::number(M.grid.back())
            + "\nПоследнее значение численного метода по первой функции полученное для точки Xn, V1_Fn =" + QString::number(M.final_num_values_1.back())
            + "\nПоследнее значение численного метода по первой функции полученное для точки Xn, V2_Fn=" + QString::number(M.final_num_values_1.back())
            + "\nМакс.глобальная погрешность для первой функции |Ui(1) - Vi_f(1)| = " + QString::number(maxTrueDiff_1) + " при x = " + QString::number(M.grid[indMaxTrueDiff_1])
            + "\nМакс.глобальная погрешность для второй функции|Ui(2) - Vi_f(2)| = " + QString::number(maxTrueDiff_2) + " при x = " + QString::number(M.grid[indMaxTrueDiff_2])
            + "\nmax||ОЛП|| = " + QString::number(maxOLP) + "\nОбщее число удвоений шага: " + QString::number(M.mult) + "\nОбщее число делений шага: "
            + QString::number(M.div) + "\nМаксимальное{h_n} = " + QString::number(M.grid_step[indMaxStep]) + " при x = " + QString::number(M.grid[indMaxStep])
            + "\nМинимальное{h_n} = " + QString::number(M.grid_step[indMinStep]) + " при x = " + QString::number(M.grid[indMinStep]);
    }
    else
    {ref = "№ варианта задания: (6)\n"
                           "Тип задачи: Жёсткая система\n"
                           "Метод: Метод Рунге Кутта порядка p=2\n"
                           "Начальные значения:\n"
                           "X_0=0\tU1_0="+QString::number(u1)+"\tU2_0="+QString::number(u2)+"\t(Условия задачи Коши)\n"
                           "b="+QString::number(b)+"\tEps1 = "+QString::number(EPS)+"\t(Условия остановки шага)\n"
                           "h0="+QString::number(h0)+"\tNmax="+QString::number(N)+"\t(Начальный шаг интегрирования и максимальное число шагов метода)\n"
                           "Eps2="+QString::number(eps)+"\t(Контроль модуля локальной погрешности)\n"
                           "РЕЗУЛЬТАТ РАСЧЁТА:\n"
                           "Контроль локальной погрешности: ВЫКЛ\n"
                           "Число посчитанных точек N = "  + QString::number(M.grid.size()-1)
                           + "\nРазница b-Xn = " +  QString::number(M.right_border - M.grid.back())
                           + "\nCчёт остановился на точке Xn = " + QString::number(M.grid.back())
                           + "\nПоследнее значение численного метода по первой функции полученное для точки Xn, V1_Fn =" + QString::number(M.final_num_values_1.back())
                           + "\nПоследнее значение численного метода по первой функции полученное для точки Xn, V2_Fn=" + QString::number(M.final_num_values_1.back())
                           + "\nМакс.глобальная погрешность для первой функции |Ui(1) - Vi_f(1)| = " + QString::number(maxTrueDiff_1) + " при x = " + QString::number(M.grid[indMaxTrueDiff_1])
                           + "\nМакс.глобальная погрешность для второй функции|Ui(2) - Vi_f(2)| = " + QString::number(maxTrueDiff_2) + " при x = " + QString::number(M.grid[indMaxTrueDiff_2])
                           + "\nmax||ОЛП|| = " + QString::number(maxOLP) + "\nОбщее число удвоений шага: " + QString::number(M.mult) + "\nОбщее число делений шага: "
                           + QString::number(M.div) + "\nМаксимальное{h_n} = " + QString::number(M.grid_step[indMaxStep]) + " при x = " + QString::number(M.grid[indMaxStep])
                           + "\nМинимальное{h_n} = " + QString::number(M.grid_step[indMinStep]) + " при x = " + QString::number(M.grid[indMinStep]);
    }
    ui->textBrowserMain2->setText(ref);
}


void MainWindow::on_pushButton_clicked()
{
    system ("cd C:\\Program Files\\Microsoft Office\\root\\Office16");
    system("start POWERPNT.EXE ..\\NumMethLab1\\readme.pptx");

}

