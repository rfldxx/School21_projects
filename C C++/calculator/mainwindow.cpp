#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cmath>
#include <cstdlib>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  scene = new QGraphicsScene();
  ui->Graphic->setScene(scene);

  for (int i = 0; i < n_memory + 1; i++)
    index[i] = i;

  ClrLog();
  on_Reset_clicked();
  on_checkBox_stateChanged(0);
  ui->Answer->setReadOnly(1);

  ui->Refresh->setDefault(true);
  ui->Refresh->setAutoDefault(false);

  ui->label->setFixedHeight(24);
  ui->label_2->setFixedHeight(24);
  ui->label_4->setFixedHeight(24);
  ui->label_6->setFixedHeight(24);

  ui->label_14->setFixedHeight(24);
  ui->label_16->setFixedHeight(24);
  ui->label_18->setFixedHeight(24);
  ui->label_20->setFixedHeight(24);
  ui->label_22->setFixedHeight(24);

  ui->lineEdit_11->setText("1");
  ui->lineEdit_12->setText("1");
  ui->lineEdit_13->setText("13000");
  ui->lineEdit_14->setText("13");
}

MainWindow::~MainWindow() {
  delete ui;
  delete scene;
}

void MainWindow::ShwLog(const char *s) {
  AddLog(s);
  AddLog("</pre>");
  this->ui->Loging->setText(LOG_INFO);
  this->ui->tabWidget->setCurrentWidget(this->ui->Log);
}
void MainWindow::AddLog(const char *s) { strcpy(LOG_INFO + strlen(LOG_INFO), s); }
void MainWindow::ClrLog() { LOG_INFO[5] = 0; }

void MainWindow::ShowData() {
  ClrLog();

  sprintf(LOG_INFO + strlen(LOG_INFO),
          "<pre>INFORMATION:<br>"
          "curr-write:  %d(%d)<br>"
          "loop:        %d<br>",
          curr % n_memory, curr, loop);

  for (int i = 0; i < curr && i < n_memory; i++) {
    int pos = index[i];

    sprintf(LOG_INFO + strlen(LOG_INFO),
            "<hr /><font color=\"%s\">%d.</font> N = %2d,  double temp = "
            "%d</pre>%s<br>",
            colors[i], i, Ncalculator[pos], last_plss[pos], PolkaNames[i]);
  }
  sprintf(LOG_INFO + strlen(LOG_INFO), "</pre>");

  this->ui->Loging->setText(LOG_INFO);
}

void MainWindow::Show_Info(QString val) {
  ui->maxX->setText("+5");
  ui->minX->setText("-5");

  this->ui->About_Calculator->setText(val);

  this->ui->tabWidget->setCurrentWidget(this->ui->Info);
}

#define AAA 230
#define BBB 100

void MainWindow::ClerPlot() {
  this->scene->clear();
  this->scene->addLine(-AAA, +BBB, +AAA, +BBB);
  this->scene->addLine(+AAA, -BBB, +AAA, +BBB);
  this->scene->addLine(-AAA, -BBB, +AAA, -BBB);
  this->scene->addLine(-AAA, -BBB, -AAA, +BBB);
}

void MainWindow::SnglPlot(int pos, double *f, double k, double add, double delta) {
  bool ok_prev = !std::isnan(f[0]) && !std::isinf(f[0]);
  bool ok_curr;
  QPen zvet{QColor(colors[pos])};

  for (int j = 1; j < n_precisoon; j++, ok_prev = ok_curr)
    if ((ok_curr = !std::isnan(f[j]) && !std::isinf(f[j])) && ok_prev) {
      double x[2] = {0, 1};

      double xa = (maxy - f[j - 1]) / (f[j] - f[j - 1]);
      double xb = (miny - f[j - 1]) / (f[j] - f[j - 1]);

      double ff[2] = {f[j - 1], f[j]};

      if (0. < xa && xa < 1.) {
        int t = ff[0] < maxy;
        x[t] = xa;
        ff[t] = maxy;
      }

      if (0. < xb && xb < 1.) {
        int t = ff[0] > miny;
        x[t] = xb;
        ff[t] = miny;
      }

      if (miny <= ff[0] && ff[0] <= maxy && miny <= ff[1] && ff[1] <= maxy) {
        this->scene->addLine(-AAA + (j - 1 + x[0]) * delta, add + k * ff[0],
                             -AAA + (j - 1 + x[1]) * delta, add + k * ff[1],
                             zvet);
      }
    }
}

void MainWindow::AddsPlot(int pos) {
  double *xx = (double *)malloc(n_precisoon * sizeof(double));
  double delta_x = (finsh - start) / (n_precisoon - 1);
  for (int j = 0; j < n_precisoon; j++)
    xx[j] = start + j * delta_x;

  int dst = index[pos];
  double *f = calculate_graph(calculator[dst], Ncalculator[dst], last_plss[dst], n_precisoon, xx);
  free(xx);

  double k = -(BBB - 5) * 2. / (maxy - miny);
  double add = (BBB - 5) - k * miny;
  double delta = AAA * 2. / (n_precisoon - 1);
  SnglPlot(pos, f, k, add, delta);
  free(f);
}

void MainWindow::PlotGraphic() {
  if (!curr)
    return;

  double *xx = (double *)malloc(n_precisoon * sizeof(double));
  double *f[n_memory];

  double delta_x = (finsh - start) / (n_precisoon - 1);
  for (int j = 0; j < n_precisoon; j++)
    xx[j] = start + j * delta_x;

  double ayi = -1e10, byi = +1e10;
  for (int i = 0; i < curr && i < n_memory; i++) {
    int pos = index[i];
    f[i] = calculate_graph(calculator[pos], Ncalculator[pos], last_plss[pos], n_precisoon, xx);
    double *yy = f[i];
    for (int j = 0; j < n_precisoon; j++)
      if (!std::isnan(yy[j]) && !std::isinf(yy[j])) {
        if (yy[j] > ayi) ayi = yy[j];
        if (yy[j] < byi) byi = yy[j];
      }
  }
  free(xx);

  if (ayi == byi) {
    ayi += 1;
    byi -= 1;
  }

  if (!user_scale) {
    maxy = ayi;
    miny = byi;

    ui->maxY->setText(QString::number(maxy));
    ui->minY->setText(QString::number(miny));

    qDebug() << " PLOT graphic!";
    qDebug() << miny << maxy;
    qDebug() << QString::number(miny, 'g', 6) << QString::number(maxy, 'g', 6);
    user_scale = 0; // тупое изменение переменой из-за двух строчек выше`
  }

  double k = -(BBB - 5) * 2. / (maxy - miny);
  double add = (BBB - 5) - k * miny;
  double delta = AAA * 2. / (n_precisoon - 1);
  for (int i = 0; i < curr && i < n_memory; i++) {
    SnglPlot(i, f[i], k, add, delta);
    free(f[i]);
  }
}

void MainWindow::on_Read_clicked() {
  if (this->up_time_polka) {
    qDebug() << " [QT] Read: input is up to date";
    reScale();
    return;
  }

  ClrLog();

  auto typoiQStr = this->ui->Input->text();
  int temp_int = typoiQStr.size();
  if (temp_int > maxLixem / 4) { ShwLog("<font color=\"red\">SO LONG INPUT</font>"); return; }
  char input[maxLixem] = {0};
  char check = 1;
  for (int i = 0; check && i < typoiQStr.size(); i++)
    input[i] = check = typoiQStr[i].toLatin1();
  if (!check) { ShwLog("<font color=\"red\">INCORRECT SYMBOL</font>"); return; }
  input[typoiQStr.size()] = 0;

  int write_pos = index[n_memory];

  if (!omaru_polka(input, data_dst[write_pos], calculator[write_pos],
                   Ncalculator + write_pos, last_plss + write_pos,
                   LOG_INFO + strlen(LOG_INFO)))
    { ShwLog(""); return; }

  if (!exist_x(calculator[write_pos], Ncalculator[write_pos])) { // это числовое выражение
    show_x = 1;
    on_checkBox_stateChanged(1);

    double xx = 0;
    double *yy = calculate_graph(calculator[write_pos], Ncalculator[write_pos],
                                 last_plss[write_pos], 1, &xx);
    this->ui->Answer->setText(QString::number(yy[0]));
    free(yy);

    AddLog("</pre>");
    this->ui->Loging->setText(LOG_INFO);
    this->ui->Read->setStyleSheet("background-color: green");
    return;
  }

  up_time_polka = 1;

  // это функция f(x)
  on_checkBox_stateChanged(0);

  int ii = 0;
  for (; ii < curr && ii < n_memory && strcmp(PolkaNames[ii], LOG_INFO); ii++);

  if (ii < curr && ii < n_memory) { // уже было
    qDebug() << "IT EXIST BEFORE IN" << ii;
    sprintf(LOG_INFO + strlen(LOG_INFO), "<hr /><br>IT EXIST BEFORE: %d", ii);

    AddLog("</pre>");
    this->ui->Loging->setText(LOG_INFO);
    this->ui->Read->setStyleSheet("background-color: lightblue");
    return;
  }

  double news = ui->minX->text().toDouble();
  double newf = ui->maxX->text().toDouble();

  if (start != news || finsh != newf) {
    start = news;
    finsh = newf;

    ClerPlot();

    int user_scenario = user_scale;
    user_scale = 1;
    PlotGraphic();
    user_scale = user_scenario;
  }

  // надо сохранить данные
  int pos = curr % n_memory;
  std::swap(index[pos], index[n_memory]);

  if (curr++ == 0) {
    qDebug() << "This is first graphic";
    PlotGraphic();
    ui->Refresh->setStyleSheet("background-color: lightgray");
  } else AddsPlot(pos);

  if (!loop) { // это была дозапись
    strcpy(PolkaNames[pos], LOG_INFO);

    qDebug() << "IN" << curr << "ADD: " << PolkaNames[curr % n_memory];

    this->ui->Read->setStyleSheet("background-color: green");

    if (curr >= n_memory) loop = 1;
  } else { // это было перезаписывание
    qDebug() << "REWRITE" << curr << "FROM" << PolkaNames[pos];

    int l = strlen(LOG_INFO);

    sprintf(LOG_INFO + l, "<hr /><br>REWRITE %d:<br>%s", pos, PolkaNames[pos] + 5);

    strncpy(PolkaNames[pos], LOG_INFO, l);
    PolkaNames[pos][l] = 0;

    qDebug() << "          TO  " << PolkaNames[pos];
    this->ui->Read->setStyleSheet("background-color: #fcdd76");
  }

  AddLog("</pre>");
  this->ui->Loging->setText(LOG_INFO);
  return;
}

void MainWindow::reScale() {
  qDebug() << " [QT] reScale:       user scale" << user_scale;

  on_checkBox_stateChanged(0);

  start = ui->minX->text().toDouble();
  finsh = ui->maxX->text().toDouble();

  if (ui->minY->text().isEmpty() || ui->maxY->text().isEmpty()) user_scale = 0;

  if (user_scale) {
    miny = ui->minY->text().toDouble();
    maxy = ui->maxY->text().toDouble();
  }

  ClerPlot();
  PlotGraphic();
  ui->Refresh->setStyleSheet("background-color: lightgray");

  qDebug() << miny << maxy;
  qDebug() << QString::number(miny, 'g', 6) << QString::number(maxy, 'g', 6);
  qDebug() << " [QT] reScale (end): user scale" << user_scale;
}

void MainWindow::on_Refresh_clicked() {
  qDebug() << " [QT] Refresh:       user scale" << user_scale;

  if (show_x) {
    show_x = 0;
    on_checkBox_stateChanged(0);
    this->ui->Read->setStyleSheet("background-color: lightgray");
    return;
  }

  ShowData();
  reScale();
  ui->Input->clear();

  qDebug() << " [QT] Refresh (end): user scale" << user_scale;
}

void MainWindow::on_checkBox_stateChanged(int arg1) {
  this->ui->Graphic->setVisible(arg1 == 0);
  this->ui->Precision->setVisible(arg1 == 0);
  this->ui->Precision_label->setVisible(arg1 == 0);
  this->ui->minX->setVisible(arg1 == 0);
  this->ui->maxX->setVisible(arg1 == 0);
  this->ui->minY->setVisible(arg1 == 0);
  this->ui->maxY->setVisible(arg1 == 0);

  this->ui->Answer->setVisible(arg1 != 0);
  this->ui->Answer_label->setVisible(arg1 != 0);
}

void MainWindow::on_checkBox_2_stateChanged(int arg1) {
  qDebug() << "Switch shift:" << arg1;
  this->ui->tabWidget->setCurrentWidget(this->ui->Info);
}

void MainWindow::on_Input_textChanged(const QString &arg1) {
  Q_UNUSED(arg1);

  this->ui->Read->setStyleSheet("background-color: lightgray");
  this->up_time_polka = 0;
}

void MainWindow::on_Reset_clicked() {
  ClerPlot();

  this->ui->minX->setText("-5");
  this->ui->maxX->setText("+5");

  this->ui->minY->setText("");
  this->ui->maxY->setText("");
  user_scale = 0;

  show_x = 0;

  ui->Input->clear();
  ui->Read->setStyleSheet("background-color: lightgray");
  ui->Refresh->setStyleSheet("background-color: lightgray");

  curr = 0;
  loop = 0;

  up_time_polka = 0;
  ui->Precision->setValue(0);

  on_checkBox_stateChanged(0);

  strcpy(LOG_INFO + strlen(LOG_INFO), "<hr><pre>RESET!</pre>");
  this->ui->Loging->setText(LOG_INFO);
  ClrLog();
}

void MainWindow::on_Precision_valueChanged(int value) {
  n_precisoon = 10 + (value / 99.) * (value / 99.) * (1000 - 10);
  ClerPlot();
  int swave = user_scale;
  user_scale = 1;
  PlotGraphic();
  user_scale = swave;
}

void MainWindow::on_minY_textChanged(const QString &arg1) {
  user_scale = 1;
  qDebug() << " [QT] change y scale: " << user_scale;
  on_maxX_textChanged(arg1);
}
void MainWindow::on_maxY_textChanged(const QString &arg1) {
  user_scale = 1;
  qDebug() << " [QT] change y scale: " << user_scale;
  on_maxX_textChanged(arg1);
}
void MainWindow::on_minX_textChanged(const QString &arg1) { on_maxX_textChanged(arg1); }
void MainWindow::on_maxX_textChanged(const QString &arg1) {
  Q_UNUSED(arg1);
  ui->Refresh->setStyleSheet("background-color: lightblue");
}

void MainWindow::on_Input_returnPressed() { on_Read_clicked(); }

void MainWindow::on_minX_returnPressed() { ui->maxX->setFocus(); }
void MainWindow::on_maxX_returnPressed() { ui->Refresh->setFocus(); }
void MainWindow::on_maxY_returnPressed() { ui->minY->setFocus(); }
void MainWindow::on_minY_returnPressed() { ui->Refresh->setFocus(); }

void MainWindow::on_pushButton_clicked() {
  bool ok1, ok2, ok3;
  double S = ui->lineEdit->text().toDouble(&ok1);
  int n = ui->lineEdit_2->text().toInt(&ok2);
  double p = ui->lineEdit_3->text().toDouble(&ok3);

  if (!ok1 || !ok2 || !ok3 || S <= 0 || n <= 0 || p < 0) {
    ui->pushButton->setStyleSheet("background-color: red");
    return;
  }

  p /= 100;

  ui->pushButton->setStyleSheet("background-color: lightgreen");

  qDebug() << ui->comboBox->currentIndex();
  if (ui->comboBox->currentIndex() == 0) { // фикс платеж
    double x;
    if (p < 1e-6) x = 1 / (n * (1. - (n + 1) * p / 2));
    else          x = p / (1. - 1. / std::pow(1 + p, n));

    ui->lineEdit_4->setText(QString::number(x * S));
    ui->lineEdit_5->setText(QString::number(100 * (n * x - 1)));
    ui->lineEdit_6->setText(QString::number(n * x * S));
    return;
  }

  ui->lineEdit_4->setText(QString::number((p + (p + 1) / n) * S) + " -  " + QString::number(p * S / n) + "*i");
  ui->lineEdit_5->setText(QString::number(50 * (n + 1) * p));
  ui->lineEdit_6->setText(QString::number((n + 1) * p * S / 2 + S));
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1) { on_lineEdit_textChanged(arg1); }
void MainWindow::on_comboBox_editTextChanged   (const QString &arg1) { on_lineEdit_textChanged(arg1); }
void MainWindow::on_lineEdit_2_textChanged     (const QString &arg1) { on_lineEdit_textChanged(arg1); }
void MainWindow::on_lineEdit_3_textChanged     (const QString &arg1) { on_lineEdit_textChanged(arg1); }
void MainWindow::on_lineEdit_textChanged(const QString &arg1) {
  Q_UNUSED(arg1);
  ui->pushButton->setStyleSheet("background-color: lightgray");
}

#define Tar(a, n, type)                                                        \
  bank.a = ui->lineEdit_##n->text().to##type(&ok);                             \
  if (!ok || bank.a <= 0) { ui->pushButton_2->setStyleSheet("background-color: red"); return; }

void MainWindow::on_pushButton_2_clicked() {
  qDebug() << "CLICKED!";

  bool ok;
  Tar(S, 7, Double);
  Tar(n, 8, Int);
  Tar(p, 9, Double);
  Tar(k, 10, Int);
  Tar(p_frequency, 11, Int);
  Tar(t_frequency, 12, Int);
  Tar(T, 13, Double);
  Tar(t, 14, Double);

  bank.capitalization = ui->checkBox->isChecked();

  qDebug() << "capitalization " << bank.capitalization;

  bank.p /= 100 * bank.p_frequency;
  bank.t /= 100;

  bank.changes = (double *) malloc((bank.n + 1) * sizeof(double));
  for (int i = 0; i <= bank.n; i++) bank.changes[i] = 0;

  ui->pushButton_2->setStyleSheet("background-color: lightgray");
  ui->stackedWidget->setCurrentIndex(0);
  deposit_bred();
}

int MainWindow::deposit_bred() {
  double *evolve = (double *)malloc((bank.n + 1) * sizeof(double));
  evolve[0] = bank.S + bank.changes[0]; // проверку!

  double tax      = 0; // доход с прошлого снятия налога
  double all_tax  = 0;
  double plus     = 0;
  double all_plus = 0;

  for (int i = 1; i <= bank.n; i++) {
    plus = 0;

    if (i % bank.k == 0) { // плюс деньги
      for (int j = i - bank.k; j < i; j++) {
        qDebug() << "  +" << evolve[j] << "(" << j << ")";
        plus += evolve[j];
      }
      plus *= bank.p; // ? /bank.k
      qDebug() << "Add money " << plus;
    }

    tax += plus;
    if (i % bank.t_frequency == 0) { // налоги
      qDebug() << "Curren income (to tax)" << tax;
      if (tax > bank.T)
        all_tax += bank.t * (tax - bank.T);
      tax = 0;
    }

    all_plus += plus;

    evolve[i] = evolve[i - 1] + bank.changes[i];
    if (bank.capitalization) evolve[i] += plus;

    if (evolve[i] <= 0) { free(evolve); return 0; }
  }

  ui->lineEdit_17->setText(QString::number(all_plus));
  ui->lineEdit_18->setText(QString::number(all_tax));
  ui->lineEdit_19->setText(QString::number(evolve[bank.n] - (bank.capitalization ? all_tax : 0) ));

  free(evolve);
  return 1;
}

void MainWindow::on_pushButton_3_clicked() {
  ui->stackedWidget->setCurrentIndex(1);
  ui->lineEdit_15->setText("");
  ui->lineEdit_16->setText("");
  ui->ADD->setStyleSheet("background-color: lightgray");
  qDebug() << "CLICKED again!";

  free(bank.changes);
}

void MainWindow::on_lineEdit_16_textChanged(const QString &arg1) { on_lineEdit_15_textChanged(arg1); }
void MainWindow::on_lineEdit_15_textChanged(const QString &arg1) {
  Q_UNUSED(arg1);
  ui->ADD->setStyleSheet("background-color: lightgray");
}

void MainWindow::on_ADD_clicked() {
  bool ok1, ok2;
  int i = ui->lineEdit_16->text().toInt(&ok1);
  double val = ui->lineEdit_15->text().toDouble(&ok2);

  qDebug() << "READ" << i << val;

  if (!ok1 || !ok2 || i < 0 || i >= bank.n || fabs(val) < 1e-6) {
    ui->ADD->setStyleSheet("background-color: red");
    return;
  }

  double prev = bank.changes[i];
  bank.changes[i] += val;
  if (bank.S + bank.changes[0] < 1e-6 || !deposit_bred()) {
    bank.changes[i] = prev;
    ui->ADD->setStyleSheet("background-color: red");
    return;
  }

  ui->ADD->setStyleSheet("background-color: green");
}
