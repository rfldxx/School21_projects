#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsScene>
#include <QMainWindow>

extern "C" {
#include "s21_polka.h"
}
#define n_memory 5

struct deposit {
  bool capitalization;
  double S, p, T, t, t_income;
  int n, k, p_frequency, t_frequency;
  double *changes;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void ShwLog(const char *s);
  void AddLog(const char *s);
  void ClrLog();

  void ShowData();

  void Show_Info(QString val);

  void SnglPlot(int pos, double *f, double k, double add, double delta);
  void AddsPlot(int pos);
  void ClerPlot();
  void PlotGraphic();

  int deposit_bred();

  void reScale();
private slots:
  void on_Read_clicked();

  void on_checkBox_stateChanged(int arg1);

  void on_checkBox_2_stateChanged(int arg1);

  void on_Input_textChanged(const QString &arg1);

  void on_Reset_clicked();

  void on_Precision_valueChanged(int value);

  void on_maxX_textChanged(const QString &arg1);

  void on_minX_textChanged(const QString &arg1);

  void on_Input_returnPressed();

  void on_minX_returnPressed();

  void on_maxX_returnPressed();

  void on_Refresh_clicked();

  void on_maxY_textChanged(const QString &arg1);

  void on_minY_textChanged(const QString &arg1);

  void on_maxY_returnPressed();

  void on_minY_returnPressed();

  void on_pushButton_clicked();

  void on_lineEdit_textChanged(const QString &arg1);

  void on_lineEdit_2_textChanged(const QString &arg1);

  void on_lineEdit_3_textChanged(const QString &arg1);

  void on_comboBox_editTextChanged(const QString &arg1);

  void on_comboBox_currentTextChanged(const QString &arg1);

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

  void on_lineEdit_15_textChanged(const QString &arg1);

  void on_lineEdit_16_textChanged(const QString &arg1);

  void on_ADD_clicked();

private:
  deposit bank;

  char PolkaNames[n_memory][2 * maxLixem];
  char data_dst[n_memory + 1][2 * maxLixem];
  info calculator[n_memory + 1][maxLixem];
  int Ncalculator[n_memory + 1];
  int last_plss[n_memory + 1];

  int index[n_memory + 1];

  int curr = 0;
  int loop = 0;

  const char colors[n_memory][8] = {"#a7fc00", "#4f7942", "#900020", "#945d0b", "#627478"};

  char LOG_INFO[1000] =
      "<pre><font color=\"red\">ERROR:<br>"
      "</font>x + 1 - 9 * <font color=\"red\">s</font> <br>"
      "<font color=\"red\">            ^</font><br>"
      "<font color=\"red\">            UNKNOWN_OPEARATION_err</font><br><hr "
      "/></pre>";

  int show_x = 0;
  int user_scale = 0;

  int up_time_polka = 0;
  int n_precisoon = 10;
  double maxy, miny;
  double start = -5, finsh = +5;

  Ui::MainWindow *ui;

  QGraphicsScene *scene;
};
#endif // MAINWINDOW_H
