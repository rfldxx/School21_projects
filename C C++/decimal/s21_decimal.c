#include "s21_decimal.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// эти функции выделяют первую/последнюю половину unsigned
unsigned first(unsigned a) { return (a & 0xFFFF0000) >> 16; }
unsigned secnd(unsigned a) { return a & 0x0000FFFF; }


// Двоичное представление Decimal состоит из 1-разрядного знака, 96-разрядного целого числа
// и коэффициента масштабирования, используемого для деления 96-разрядного целого числа и указания того,
// какая его часть является десятичной дробью.
// Коэффициент масштабирования неявно равен числу 10, возведенному в степень в диапазоне от 0 до 28.
// Следовательно, двоичное представление Decimal имеет вид ((от -2^96 до 2^96) / 10^(от 0 до 28)),
// где -(2^96-1) равно минимальному значению, а 2^96-1 равно максимальному значению.


// Об порядке битов:
//
// bits[0], bits[1], и bits[2] содержат младшие, средние и старшие 32 бита 96-разрядного целого числа соответственно.
//
// bits[3] содержит коэффициент масштабирования и знак, и состоит из следующих частей:
//    Биты от 0 до 15, младшее слово, не используются и должны быть равны нулю.
//    Биты с 16 по 23 должны содержать показатель степени от 0 до 28, который указывает степень 10 для разделения целого числа.
//    Биты с 24 по 30 не используются и должны быть равны нулю.
//    Бит 31 содержит знак; 0 означает положительный, а 1 означает отрицательный.


// получение степени 10ки
int poww(s21_decimal a) {
  int bits = (a.bits[3] & 0x00FF0000) >> 16;
  return bits - (bits & 0x80 ? 256 : 0);
}

// запись степени в нужное место
void spw(s21_decimal* a, int powe) {
  a->bits[3] &= 0xFF00FFFF;  // QUAD(8, 0)
  a->bits[3] |= 0x00FF0000 & (powe << 16);
}

int zero(s21_decimal a) { return !a.bits[0] && !a.bits[1] && !a.bits[2]; }
int neg(s21_decimal a) { return (a.bits[3] & QUAD(8, 0)) != 0; }
int sign(s21_decimal a) { return !zero(a) * (1 - 2 * neg(a)); }

//--------------------------------------------------------------------------------
// Здесь реализовано "+" и "*" unsigned-ов с учетом переполнения
// Создаются функции unsigned name(unsigned* a, unsigned b, unsigned* prev)
//      Если подробнее, то считается результат:
//              result =  "a" op "b" + "prev"   (op = "+" или "*")
//      вообще result требует битов как два unsigned-а, поэтому
//      "младший" unsigned записывается в "a", а "старший" в "prev"
//              result ~ unsigned_1 unsigned_2 = unsigned_1 * 2^32 + unsigned_2
//                                             =   "prev"   * 2^32 +    "a"
//      функция возвращает то на сколько переполнился unsigned, т.е. "prev"

unsigned (*part[])(unsigned) = {&secnd, &first};
#define TEMPLATE_split(name, temp_op, extra_op)              \
  unsigned name(unsigned* a, unsigned b, unsigned* prev) {   \
    for (int i = 0; i < 2; i++) {                            \
      unsigned temp = part[i](*a) temp_op(b) + secnd(*prev); \
      *prev = extra_op + first(*prev) + first(temp);         \
      *a &= 0xFFFF << (!i << 4);                             \
      *a |= secnd(temp) << (i << 4);                         \
    }                                                        \
    return *prev;                                            \
  }

TEMPLATE_split(ppcim, * secnd, part[i](*a) * first(b));  // такая функция для *
TEMPLATE_split(ppcis, + part[i], );                      // такая функция для +


// поэлементное добавления массива b (из n элементов)
// в массив a (по умолчанию размер а - 3 элемента (при n <= 3), иначе размер - n элементов)
//
// pcas(a, b, 2):
//      a:     a[0]      a[1]       a[2]
//      b:     b[0]      b[1]
// ->   a:  a[0]+b[0]  a[1]+b[1]    a[2]
// !Однако в этом примере не отображенно что при сложении unsigned может переполниться
//  и тогда вышедшая часть переносится в следующий элемент массива
//
// Функция возвращает то что не вместилось в последний элемент
// Например для входных данных pcas(a, b, 2)
//      a:  2^32 - 1    2^32 - 1    2^32 - 1
//      b:     1           0
// ->   a:     0           0           0
//   return: 1
unsigned pcas(unsigned* a, const unsigned* b, const int n) {  // partial clear sum
    unsigned prev = 0;
    for (int i = 0; i < max(n, 3); i++) ppcis(a + i, i < n ? b[i] : 0, &prev);
    return prev;
}

// аналогично, только мы добавляем не массив а один unsigned
unsigned pcis(unsigned* a, unsigned b) { return b ? pcas(a, &b, 1) : 0; }

//--------------------------------------------------------------------------------
// Реаизация умножения decimal-а на 10

// Здесь "табличные" данные которые, если кратко, характерезуют результат
// деления вида t*2^(4p) / 10, где t - цифра(от 0 до 9), а p - натуральное число
//      t*2^(4p) / 10  ==  (5*l + k)*2^(4p-1) / 5  ==  l*2^(4p-1) + k*2^(4p-1) / 5
//                          ~~~~~~~                                 ~~~~~~~~~~~~~~
//                            ^^^                                        ^^^
//       Можно рассмотреть t = 5*l+k                                     |||
//       где l - либо 0, либо 1, а                           Эти "таблицы" ответственны
//           k - от 0 до 4                                   за это слагоаемое
//
const unsigned norm[] = {QUAD(0), QUAD(9), QUAD(3), QUAD(C), QUAD(6)};
const unsigned quad[] = {QUAD(0), QUAD(1, 9), QUAD(3), QUAD(4, C), QUAD(6)};
const unsigned ostt[] = {0, 6, 2, 8, 4};

// проверка что при умножении на 10 текущий децимал будет вмещаться в свои 96 бит
#define noRETURN(i, val) if (a->bits[i] != val) return a->bits[i] < val
int ccmt(s21_decimal* a) {  // c.. c. ca.. can multiply ten?
  noRETURN(2, quad[1]);
  noRETURN(1, norm[1]);
  noRETURN(0, norm[1]);
  return 1;
}

// функция умножает децимал на 10 и возвращает 1
// если умножение не возможно то возвращает 0
int cmt(s21_decimal* a) {  // certanly multiply ten
    if (!ccmt(a)) return 0;
    for (unsigned prev = 0, i = 0; i < 3; i++) ppcim(a->bits + i, 10, &prev);
    return 1;
}

//--------------------------------------------------------------------------------
// Реаизация деления decimal-а на 10

// Показывает нужно ли округление (a - показывает четное ли число, ost - остаток)
//      norm_round  = 0 - банковское округление
//      norm_round != 0 - адекватное округление
unsigned fund_round(unsigned a, unsigned ost, int norm_round) {
  return ost <= 5 ? ost == 5 && (norm_round || a & 1) : 1;
}

// функция делит децимал на 10 и возвращает остаток от деления
// так же производит округление в зависимости от round:
//      round = 0 - отбрасывание остатка
//      round = 1 - банковское округление
//      round = 2 - адекватное округление
int cdt(unsigned* bits, int round) {  // c.. c. ca.. can devide ten?
  unsigned ost = 0, r = 0;
  for (int i = 0; i < 3; i++) {
      r = bits[i] % 10;
      bits[i] /= 10;
      ost += i ? ostt[r%5] : r;
      if(i) bits[i-1] += (r/5 << 31) + quad[r % 5];
  }
  pcis(bits, norm[r%5] + ost/10);
    
  if(round) pcis(bits, fund_round(*bits, ost%10, round-1));
  return ost % 10;
}

//--------------------------------------------------------------------------------
// рассматривается выражение вида
//          id*2^96 + decimal
// где id - цифра от 0 до 9, where - часть децимала ответственная за 96-битное число
//
// затем это выражение делится на 10 и результат сохраняется в децимале
//
// функция всегда возвращает 1
// (это сделано чтобы в одном месте можно было сделать pow -= turn_up(..); вместо turn_up(..); pow--;)
int turn_up(unsigned id, unsigned* where) {
  unsigned t = id%5, ost = ostt[t] + cdt(where, 0), add[] = {norm[t] + ost/10, norm[t], quad[t] + (id/5 << 31)};
  add[0] += fund_round(*where ^ add[0], ost % 10, 0);
  return !pcas(where, add, 3);
}

int s21_mul(s21_decimal a, s21_decimal b, s21_decimal* result) {
  unsigned aska[7] = {0}, pow = poww(a) + poww(b);

  for (int l = 0; l <= 4; l++)
    for (int i = max(0, l-2); i < min(3, l+1) ; i++) {
      unsigned val[2] = {a.bits[i]};
      ppcim(val, b.bits[l - i], val + 1);
      pcas(aska + l, val, 2);
    }

  for (; aska[3] || aska[4] || aska[5] || aska[6] || (int)pow > 28; pow--)
    turn_up(cdt(aska + 3, 0), aska);
    
  for (int i = 0; i < 3; i++) result->bits[i] = aska[i];
  result->bits[3] = ((neg(a) ^ neg(b)) << 31) + ((0xFF & pow) << 16);
  return (!zero(a) && !zero(b) && zero(*result)) ? 2 : (int)pow < 0 ? 1 + neg(*result) : 0;
}

//--------------------------------------------------------------------------------
// функция принимает два децимала 'a', 'b' причем! важно чтобы poww(b) >= poww(a)
// т.е. точка в десятичном представление децимала в переменой 'b' была оттодвинута дальше
// например:
//    a:      123120292 (poww = 3)   ~   123120.292
//    b:   123120292112 (poww = 6)   ~   123120.292112
//
// функция возвращает знак разности модулей:  sign(|a| - |b|)
// т.е.   если |a| >  |b|  то  return  1
//             |a| == |b|  то  return  0
//             |a|  < |b|  то  return -1
//
// в процессе работы функция модифицирует (без потери точности) переменную 'a'
// путем умножения её на 10, если после вызова этой функции нужно чтобы
// степень 'a' была соответсвенно увеличена, то надо передать пременную
//  rei != 0  -  установка степени в переменой 'a'
int adif(s21_decimal* a, s21_decimal* b, int rei) {
  int p1 = poww(*a), n = poww(*b) - p1, ans = 0;
  for (; n && cmt(a); n--, p1++);
  if (rei) spw(a, p1);
  if (n) return 1;
  
  for (int i = 2; !ans && i >= 0; i--)
      ans = (a->bits[i] > b->bits[i]) - (a->bits[i] < b->bits[i]);
  // изначально тут было ans = a->bits[i] - b->bits[i] однако при такой
  // реализации бывают очень неприятные переполнения переменой (а я так всё время до этого делал...)
  return ans;
}

// макрос  ALB нужен чтобы первый и второй аргументы adif(arg1, arg2) соответствовали
// необходимому для adif условию:   poww(b) >= poww(a)
//
// макрос DALB является "шаблоном" для создания функций "сравнения" требуемых в задании
// эти функции выдают результат основанный на сравнении функции adif с нулем
#define ALB(a, b, t) (poww(a) <= poww(b) ? adif(&a, &b, t) : -adif(&b, &a, t))
#define DALB(name, op) int s21_is_##name(s21_decimal a, s21_decimal b) {                \
return (zero(a) ? -sign(b) : sign(a) * (sign(a) == sign(b) ? ALB(a, b, 0) : 1)) op 0;   \
}

// здесь как раз эти функции и создаются
DALB(less,    <) DALB(less_or_equal,    <=) DALB(equal,     ==)
DALB(greater, >) DALB(greater_or_equal, >=) DALB(not_equal, !=)

int s21_negate(s21_decimal value, s21_decimal* result) {
  for (int i = 0; i < 4; i++) result->bits[i] = value.bits[i];
  result->bits[3] += 1 << 31;
  return 0;
}

//--------------------------------------------------------------------------------
// как и с обычными целыми типами удобно вместо вычитания числа добавлять "сопряженной" число
// эта функция для сопряжения
void conjugation(s21_decimal* a) {
  for (int i = 0; i < 3; i++) a->bits[i] = ~a->bits[i];
  pcis(a->bits, 1);
}

// функция ответственная за "сложение" (substract = 0)
//                     и за "сложение" с сопряженым bb (substract = 1)
// !при сложении не учитываются знаки децималов
//  конечный знак в result задается такой же как у aa
//
// т.е. результат работы функции это:
//      (substract = 0):   return = sign(aa) * (|aa| + |bb|)
//      (substract = 1):   return = sign(aa) * (|aa| - |bb|)
//
// небольшое отступление:
//    формально тут нет умножения на sign(aa), а задание знака в bits[3]
//    т.е. по итогу   return->bits[3] & 0x80000000  ==  aa.bits[3] & 0x80000000
//    я говорю sign так как   bits[3] & 0x80000000  ==  0  -  означает '+' у децимала
//                        а   bits[3] & 0x80000000  ==  1  -  означает '-' у децимала
//
// по скольку при вычитании добавляется сопряженный bb то важно чтоб в этом случаи |aa| >= |bb|
//      ( так как иначе результат (|aa| - |bb|) будет отрицательным и чтоб в result хранилась запись
//        несопряженного децимала, то потребуется дополнительно сопрячь и (|aa| - |bb|)
//        чего в коде не предусмотренно                                                               )
//
// conj_last - запись -1 в сопряженном виде
unsigned conj_last[3] = {QUAD(F), QUAD(F), QUAD(F)};
int sadd(s21_decimal aa, s21_decimal bb, s21_decimal* result, int subtract) {
    int last = 0, necon = poww(aa) > poww(bb);
    s21_decimal *a = necon ? &bb : &aa, *b = necon ? &aa : &bb;
    int p1 = poww(*a), n = poww(*b) - p1;

    for (; n && cmt(a); n--, p1++);
    while (n--) last = cdt(b->bits, n ? 1 : 0);
    
    for (int i = 0; i < 4; i++) result->bits[i] = aa.bits[i];
    if (subtract && !zero(bb)) conjugation(&bb);
    
    int extrn = fund_round(result->bits[0] ^ bb.bits[0], last, 0);
    unsigned extasy = pcas(result->bits, bb.bits, 3) && !subtract; //показывает переполнин ли децимал
    
    if(subtract && extrn) pcas(result->bits, conj_last, 3);
    else if(!subtract && !extasy) extasy = pcis(result->bits, extrn);
    
    spw(result, p1 -= extasy);
    if (!extasy) return 0;

    turn_up(1, result->bits);
    return p1 < 0 ? 1 + neg(aa) : 0;
}

// вызывают нужным образом sadd
int s21_add(s21_decimal a, s21_decimal b, s21_decimal* result) {
  if (neg(a) == neg(b)) return sadd(a, b, result, 0);
  return ALB(a, b, 1) < 0 ? sadd(b, a, result, 1) : sadd(a, b, result, 1);
}

int s21_sub(s21_decimal a, s21_decimal b, s21_decimal* result) {
  s21_negate(b, &b);
  return s21_add(a, b, result);
}

//--------------------------------------------------------------------------------
// функция "сдвигает" биты в децимал (что-то типа <<1 и >>1)
// только она смотрит еще можно ли их сдвинуть (см. примеры наверное)
// возвращает 0 если так сделать нельзя
//
// еще тут есть переменная platz которая по сути показывает на сколько децимал
// сдвинут от исходного положения
// функция в случае успеха возвращает новое значение platz
//   (platz - считаем изначально равным единице, а не нулю
//    чтобы можно было различить вернулись ли мы к начальному
//    значению децималу или уже не можем его сдвигать        )
//
// Примеры:
//      Пошагово вызываем sdvig(bits+2, &platz, 0):
//                              bits[0]    bits[1]    bits[2]
//          (platz = 1)  a:    11100110   11010101   00010101
//          (platz = 2)  a:    11001100   10101011   00101011
//          (platz = 3)  a:    10011000   01010111   01010111
//          (platz = 4)  a:    00110000   10101111   10101110  -  дальше уже нельзя сдвигать
//                                                               (иначе уйдет единичка в bits[2])
//
//      Пошагово вызываем sdvig(bits, &platz, 1):
//                              bits[0]    bits[1]    bits[2]
//          (platz = 4)  a:    00110000   10101111   10101110
//          (platz = 3)  a:    10011000   01010111   01010111
//          (platz = 2)  a:    11001100   10101011   00101011
//          (platz = 1)  a:    11100110   11010101   00010101  -  дальше уже нельзя сдвигать
//                                                               (вернулись в исходное положение)
//
int sdvig(unsigned* a, int* platz, const int neg) {
    if (neg ? *platz == 1 : *a & (1 << 31)) return 0;
    for (int i = 0, p[2] = {1, 1 << 31}, d = 2 * neg - 1; i < 3; i++, a += d)
        *a = (!neg ? *a << 1 : *a >> 1) + ((i < 2) && (a[d] & p[!neg])) * p[neg];
  return *platz += -2 * neg + 1;
}

// производит деление децималов нацело
// по сути возвращает результат деления по модулю unsigned-а (младший unsigned короче)
// однако мы это будем использовать при делении схожих децималов, когда результат
// "нацело-деления" это по сути цифра
//
// так же в результате работы остаток от деления "записывается", лучше сказать храниться, в 'a'
//
// тут важно чтобы место куда записывается результат (unsigned* c) было
// изначально заполнено нулями (фиг знает почему так сделано какой-то пережиток)
int nazelo(s21_decimal* a, s21_decimal b, unsigned* c) {
  int platz = 1, useless = 1, t = 1; // t объявлена тут а не в форе, чтобы там было <80 строк
  while (adif(a, &b, 0) > 0 && sdvig(b.bits + 2, &platz, 0));

  if(adif(a, &b, 0) < 0) sdvig(b.bits, &platz, 1); // эту строку можно заменить на t = adif(a, &b, 0) >= 0
  for (; t || (sdvig(b.bits, &platz, 1) && sdvig(c + 2, &useless, 0)); t = 0)
    c[0] |= (adif(a, &b, 0) >= 0) && !sadd(*a, b, a, 1);
  return c[0];
}

// производит одну итерацию по получению цифры после запятой при делении децималов
// возвращает эту полученую цифру
//
// эта функция нужна исключительно в s21_div поэтому что имеется в виду под итерацией
// можно посмотреть там, в общем это что-то типо
//      t  = [10*a / b]     (t - искомая цифра,  [.] - целая часть)
//      a  =  10*a - t*b
//      return t
//
// w - весьма специфическая переменая, её тип s21_decimal натянут,
//     т.к. она используется как две независимые переменые:
//            bits[0], [1], [2]  -  это есть 'b' деленый на 10
//            bits[3]            -  остаток от деления 'b' на 10
//
//#include <stdio.h>
int nightcore(s21_decimal* a, const s21_decimal* b, const s21_decimal* w) {
  unsigned t[3] = {0}, u = w->bits[3];
  if(cmt(a)) return nazelo(a, *b, t);

  // может такое случится что 'a' уже нельзя умножить на 10 (такое наверняка будет при больших 'b',
  //                                                         которые тоже нельзя умножить на 10    )
  //  Отступление:
  //      можно конечно вместо:
  //            a  = [10*a / b]
  //      рассмотреть:
  //            b  = b / 10
  //            a  = [a / b]
  //      но тогда потеряется точность из-за округления b
  //      Хотя возможно предлагаемый далее по тексту комплекс мероприятий (без потери точности b)
  //      является излишне "точным"
  //      Т.е. можно несколько (сколько?) раз сделать точно, а далее сделать вариант с делением b на 10.
  //      Однако так не сделано, так как не понятно сколько раз надо делать точный вариант, чтобы в
  //      масштабе децимала не было видно проблем из-за последующего округления b.
  //      Найден случай что нужны два точных раза
  //
  // Предлагаемый способ:
  //  * вспомним что имеем:
  //        w->bits[0], [1], [2]  -  это есть 'b' деленый на 10
  //        давай те обозначать этот "децимал" как ww (отметим что ww можно умножать на 10)
  //
  //        w->bits[3]            -  остаток от деления 'b' на 10
  //        а эту переменную обозначать за u
  //
  //        в этих обозначениях b есть:  b = 10*ww + u
  //
  //
  //  * искомая цифра:  tt  = [10*a / b]
  //    [x]:    ->  10*a - b*tt >= 0  ->  10*(a - ww*tt) >= u*tt
  //
  //
  //  * рассмотрим       t  = [a / ww]    (т.е. тоже самое что t = [a / ww] = [a / [b / 10]]
  //                                       чем-то похоже на    [10*a / b]  ~  [a / [b / 10]])
  //    Можно показать что эта t либо равна искомой цифре (tt = t), либо на 1 больше её (tt = t-1)
  //    Что значит что t больше tt? Это значит что знак в [x] сменится на '<'
  //    Т.е. по тому какое здесь неравенство:
  //    [y]:              10*(a - ww*t) <> u*t    --    мы можем узнать больше ли t чем tt или нет
  //
  //
  //  * если в [y] знак '<'  то:    tt = t-1
  //    следовательно чтобы узнать остаток от деления (см. 368 строчка):
  //          a  = 10*a - b*tt = 10*a - b*(t-1) = b - (u*t - 10*(a - ww*t))
  //          однако в данном случае выражение в последних скобках меньше нуля
  //    однако тут можно воспользоваться хаком с реалезацией функции sadd:
  //          то что если в результате вычитания модулей получается отрицательный результат, то в
  //          в децимале хранится сопряженое значение
  //
  //  * если в [y] знак '>=' то:   a = 10*(a-ww*t) - u*t

  nazelo(a, *w, t); // здесь происходит  a = a0 - ww*t
  cmt(a);           //               a = 10*(a0 - ww*t)
  
  s21_decimal mint = {{t[0] * u, 0, 0, u}};

  if(!a->bits[2] && !a->bits[1] && (a->bits[0]) < t[0] * u){
    pcas(a->bits, b->bits, 3);
    t[0]--;
  }

  sadd(*a, mint, a, 1);

  return t[0];
}


int s21_div(s21_decimal a, s21_decimal b, s21_decimal* result) {
  if (zero(b)) return 3;

  for (int i = 0; i < 3; i++) result->bits[i] = 0;
  unsigned t = 0, pow = poww(a) - poww(b), sign = neg(a) ^ neg(b);
  s21_decimal cdt_b = b;
  a.bits[3] = b.bits[3] = cdt_b.bits[3] = cdt(cdt_b.bits, 0);

  nazelo(&a, b, result->bits);
  for(; !zero(a) && 1 + (t = nightcore(&a, &b, &cdt_b)) && (int)pow < 28 && cmt(result); t = 0, pow++)
    pcis(result->bits, t);

  //printf("last %d\n", t);

  if (pcis(result->bits, fund_round(result->bits[0], t, 0)))
      pow -= turn_up(1, result->bits);
    
  while((int)pow < 0 && cmt(result))  pow++; //по идеи можно убрать эту строчку, убрав !zero(a) в форе
  result->bits[3] = (sign << 31) + ((0xFF & (int)pow) << 16);
  return (!zero(a) && zero(*result)) ? 2 : (int)pow < 0 ? 1 + neg(*result) : 0;
}

//--------------------------------------------------------------------------------
int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  dst->bits[1] = dst->bits[2] = 0;
  dst->bits[0] = src < 0 ? -src : src;
  dst->bits[3] = (src < 0) << 31;
  return 0;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  int pow = 0;
  dst->bits[0] = dst->bits[1] = dst->bits[2] = 0;
  double val = src < 0 ? -src : src;
  if (src == 0) return 0;

  // приводим val к промежутку   1 <= val < 10
  for (; val && val >= 10; val /= 10, pow--);
  for (; val && val < 1; val *= 10, pow++);
  if (pow < -28 || pow > 28) return 1;

  // получаем 7 значемых цифр
  for (int i = 0; !i || (pow < 28 && i < 7 && cmt(dst)); pow += (i++ != 0)) {
    pcis(dst->bits, val);
    val = (val - (int)val) * 10;
  }
  pcis(dst->bits, fund_round(dst->bits[0], val, 2)); // а какое тут округление ожидается!?
  if (pow == -22 && dst->bits[0] > 7922816) return 1;

  while (pow < 0 && cmt(dst)) pow++;
  dst->bits[3] = ((src < 0) << 31) + ((0xFF & pow) << 16);
  return 0;
}

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  for (int pow = poww(src); pow--;) cdt(src.bits, 0);
  *dst = neg(src) ? -src.bits[0] : src.bits[0];
  return src.bits[2] || src.bits[1] || (src.bits[0] >= (1u << 31) + neg(src));
}

int s21_from_decimal_to_float(s21_decimal src, float* dst) { // максимально в тупую
  double p2 = 1 << 16, ans = src.bits[0];
  for (int i = 1; i < 3; i++) ans += src.bits[i] * (p2 *= p2);

  for (int pow = poww(src); pow--;) ans /= 10;
  *dst = neg(src) ? -ans : ans;
  return 0;
}


// вынес общую часть в этот макрос чтобы не писать примерно одно и то же
#define SAME_SLACK(name, op, condition)                                 \
  int s21_##name(s21_decimal value, s21_decimal* result) {              \
    int inf_ost = 0;                                                    \
    for (int pow = poww(value); pow--;) inf_ost op cdt(value.bits, 0);  \
    if (condition inf_ost) pcis(value.bits, 1);                         \
    for (int i = 0; i < 3; i++) result->bits[i] = value.bits[i];        \
    result->bits[3] = neg(value) << 31;                                 \
    return 0;                                                           \
  }


// здесь inf_ost показывает есть ли у децимала "ненулевая" дробная часть
// "ненулевая" так как может быть децимал может быть вида 32032.000
SAME_SLACK(floor,   |=, neg(value) &&)

// здесь inf_ost запоминает цифру сразу после запятой, т.е.:
//         2323.1323019
//              ^
//   inf_ost =  1
SAME_SLACK(round,    =, 5 <=)

// здесь inf_ost делает по сути тоже самое, но это по итогу это не нужно, т.к.
// единственное место где inf_ost что-то делает в SAME_SLACK это
//      if (condition inf_ost) ...
// но тут condition (тут это '0 &&') всегда "ложный"
SAME_SLACK(truncate, =, 0 &&)
    
