#include <QApplication>

#include "mainwindow.h"

char test[] =
    "<table style=\"border-style: none;\">"
    "<tbody>"
    "<tr>"
    "<td style=\"width: 100%;\">&nbsp;</td>"
    "</tr>"
    "</tbody>"
    "</table>"
    "<h1 style=\"color: #5e9ca0;\">SMART CALCULATOR</h1>"
    "<hr />"
    "<h2 style=\"color: #2e6c80;\">Об операциях:</h2>"
    "<p>Реализованны необходимые функции, а также модуль:&nbsp; abs(..)&nbsp; "
    "или&nbsp; |..|</p>"
    "<p>Функции расматриваются как унарные опереторы, поэтому следующая<br "
    "/>запись допустима:&nbsp; &nbsp; f g x&nbsp; &nbsp; &harr;&nbsp; &nbsp; "
    "f( g(x) )&nbsp; &nbsp;&nbsp;</p>"
    "<p>Приоритет операций: [^]&nbsp; &gt;&nbsp; [function]&nbsp; &gt;&nbsp; "
    "[&times;&nbsp; &divide;]&nbsp; &gt;&nbsp; [%]&nbsp; &gt;&nbsp; [+ "
    "&minus;]<br />Унарный минус имеет наивысший приоретет ( обозначается как "
    "u(..) )</p>"
    "<p>Возведение в степень раскрывается как:&nbsp; &nbsp; a^b^c&nbsp; &nbsp; "
    "&harr;&nbsp; &nbsp; a^(b^c)</p>"
    "<hr />"
    "<h2 style=\"color: #2e6c80;\">Вкладки:</h2>"
    "<table style=\"height: 117px; width: 91.4742%; border-collapse: collapse; "
    "border: 2px solid white;\">"
    "<tbody>"
    "<tr style=\"height: 18px; border: 2px solid white;\">"
    "<td style=\"width: 29.0409%; height: 39px; text-align: right; border: 2px "
    "solid white;\"><strong>Calculator</strong></td>"
    "<td style=\"width: 4.79999%; height: 39px; border: 2px solid "
    "white;\">&nbsp;</td>"
    "<td style=\"width: 66.1592%; height: 39px; border: 2px solid white;\">"
    "<p>Если в выражении не было x то вычисляет значение, <br />иначе строит "
    "график</p>"
    "</td>"
    "</tr>"
    "<tr style=\"height: 18px; border: 2px solid white;\">"
    "<td style=\"width: 29.0409%; height: 39px; text-align: "
    "right;\"><strong>Bank</strong></td>"
    "<td style=\"width: 4.79999%; height: 39px; border: 2px solid "
    "white;\">&nbsp;</td>"
    "<td style=\"width: 66.1592%; height: 39px;\">Всякая банковская ерунда</td>"
    "</tr>"
    "<tr style=\"height: 18px; border: 2px solid white;\">"
    "<td style=\"width: 29.0409%; height: 39px; text-align: "
    "right;\"><strong>Log</strong></td>"
    "<td style=\"width: 4.79999%; height: 39px; border: 2px solid "
    "white;\">&nbsp;</td>"
    "<td style=\"width: 66.1592%; height: 39px;\">"
    "<p>Перекидывает сюда, если была ошибка<br />Если ошибки не было, то здесь "
    "<br />можно посмотреть какое выражение вычислялось</p>"
    "</td>"
    "</tr>"
    "</tbody>"
    "</table>"
    "<p>&nbsp;</p>";

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.Show_Info(test);
  w.show();
  return a.exec();
}
