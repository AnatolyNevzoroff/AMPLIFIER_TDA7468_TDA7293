/////////////////////////////////////// ФУНКЦИИ / FUNCTIONS ////////////////////////////////////

//ФУНКЦИЯ ОБРАБОТКИ ПРЕРЫВАНИЯ ОТ СИГНАЛОВ С ЭНКОДЕРА
void rotaryEncoder(){
uint8_t encoderStatus=encoder.process();
if(encoderStatus){encoderCount=(encoderStatus==DIR_CW) ? 1 : -1;}}


//ФУНКЦИЯ ОБНУЛЕНИЯ ПОЛОЖЕНИЯ ЭНКОДЕРА
//void Clear_Encoder(){myEnc.write(0);oldPosition=0;newPosition=0;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБНУЛЯЕМ ТАЙМЕР АВТОВОЗВРАТА В ОСНОВНОЕ МЕНЮ, ПОДНИМАЕМ ФЛАГ
void Autoret_TIMER(){timer_AUTORET=millis();FL_autoret=true;}
////////////////////////////////////////////////////////////////////////////////////////////////


//СПЕЦСИМВОЛЫ БОЛЬШИХ ЖИРНЫХ ЦИФР ДЛЯ LCD 1602 (ГРОМКОСТЬ, ЧАСЫ ТИП 1. (секунды между цифрами))
void Big_BOLD_numbers(){
byte v1[8]={31,31,31,31,31,31,31,31};lcd.createChar(1,v1);//ПОЛНОЕ ЗАПОЛНЕНИЕ СИМВОЛА
byte v2[8]={31,31,31, 0, 0, 0, 0, 0};lcd.createChar(2,v2);//ВЕРХНЯЯ ПЕРЕКЛАДИНА
byte v3[8]={ 0, 0, 0, 0, 0,31,31,31};lcd.createChar(3,v3);//НИЖНЯЯ ТОЛСТАЯ ПЕРЕКЛАДИНА
byte v4[8]={31,31, 0, 0, 0, 0,31,31};lcd.createChar(4,v4);//ВЕРХНЯЯ И НИЖНЯЯ ПЕРЕКЛАДИНА
byte v5[8]={ 0, 0, 0, 0, 0, 0,31,31};lcd.createChar(5,v5);}//НИЖНЯЯ ТОНКАЯ ПЕРЕКЛАДИНА
////////////////////////////////////////////////////////////////////////////////////////////////


//СПЕЦСИМВОЛЫ УКАЗАТЕЛЕЙ ПОДПУНКТОВ МЕНЮ ДЛЯ LCD 1602
void Pointers(){
byte v6[8]={24,28,30,31,30,28,24, 0};lcd.createChar(6,v6);//ТРЕУГОЛЬНИК (ЗАПОЛНЕННЫЙ) 
byte v7[8]={24,28,14, 7,14,28,24, 0};lcd.createChar(7,v7);//СТРЕЛОЧКА (ПОЛАЯ)
triangle=uint8_t(6);//ТРЕУГОЛЬНИК (ЗАПОЛНЕННЫЙ) 
arrow=uint8_t(7);}  //СТРЕЛОЧКА (ПОЛАЯ) 
////////////////////////////////////////////////////////////////////////////////////////////////


//СПЕЦСИМВОЛЫ БОЛЬШИХ ЦИФР ДЛЯ LCD 1602 (ЧАСЫ ТИП 2. (секунды и температура справа))
void Big_CLOCK_numbers(){
byte v1[8] = { 7, 7, 7, 7, 7, 7, 7, 7};//ВЕРТИКАЛЬНАЯ ПАЛОЧКА ВО ВСЮ ВЫСОТУ (СПРАВА)
byte v2[8] = { 7, 7, 0, 0, 0, 0, 0, 0};//КОРОТКАЯ ГОРИЗОНТАЛЬНАЯ ПАЛОЧКА (СВЕРХУ СПРАВА)
byte v3[8] = { 0, 0, 0, 0, 0, 0,31,31};//ДЛИННАЯ ГОРИЗОНТАЛЬНАЯ ПАЛОЧКА (СНИЗУ)
byte v4[8] = {31,31, 0, 0, 0, 0,31,31};//ДВЕ ДЛИННЫЕ ГОРИЗОНТАЛЬНЫЕ ПАЛОЧКИ (СВЕРХУ И СНИЗУ)
byte v5[8] = {28,28, 0, 0, 0, 0,28,28};//ДВЕ КОРОТКИЕ ГОРИЗОНТАЛЬНЫЕ ПАЛОЧКИ (СВЕРХУ, СНИЗУ И СЛЕВА)
byte v6[8] = {28,28,28,28,28,28,28,28};//ВЕРТИКАЛЬНАЯ ПАЛОЧКА ВО ВСЮ ВЫСОТУ (СЛЕВА)
byte v7[8] = { 0, 0, 0, 0, 0, 0, 7, 7};//КОРОТКАЯ ГОРИЗОНТАЛЬНАЯ ПАЛОЧКА (СНИЗУ СПРАВА)
byte v8[8] = {31,31, 0, 0, 0, 0, 0, 0};//ДЛИННАЯ ГОРИЗОНТАЛЬНАЯ ПАЛОЧКА (СВЕРХУ)
lcd.createChar(1,v1);lcd.createChar(2,v2);lcd.createChar(3,v3);lcd.createChar(4,v4);
lcd.createChar(5,v5);lcd.createChar(6,v6);lcd.createChar(7,v7);lcd.createChar(8,v8);}
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ "ПРАВИЛЬНОЙ ПЕЧАТИ" ЦИФР С УЧЁТОМ ЗНАКА "+"
void TRUE_print(){
if(TrueP>-10&&TrueP<10){lcd.print(' ');}
if(TrueP==0){lcd.print(' ');}
if(TrueP>0){lcd.print('+');}
lcd.print(TrueP);}
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ ЗНАЧЕНИЯ ГРОМКОСТИ В ГЛАВНОМ МЕНЮ
void ShowVOLUME_menu(){
uint8_t i,d1,d2,d3,d4,d5,d6,e1,e2,e3;
int8_t a[3];

//ПЕЧАТАЕМ КУРСОР ИЛИ ПРОБЕЛ И "VOLUME"
lcd.setCursor(0,0);if(smenu1==0){lcd.write(triangle);}else{lcd.print(' ');}lcd.print("VOLUME");

//РАЗБИВАЕМ ЗНАЧЕНИЕ ГРОМКОСТИ НА 2 ЦИФРЫ
a[0]=Volume/10;
a[1]=Volume%10;
//ЦИКЛ ФОРМИРУЮЩИЙ БОЛЬШИЕ ЦИФРЫ ГРОМКОСТИ
for(i=0;i<2;i++){
switch(i){
  case 0: e1=9, e2=10,e3=11;break;
  case 1: e1=13,e2=14,e3=15;break;}
switch(a[i]){
  case 0: d1=1,d2=2,d3=1, d4=1, d5=3, d6=1;break;
  case 1: d1=2,d2=1,d3=32,d4=5, d5=1, d6=5;break;
  case 2: d1=2,d2=4,d3=1, d4=1, d5=3, d6=3;break;
  case 3: d1=2,d2=4,d3=1, d4=3, d5=3, d6=1;break;
  case 4: d1=1,d2=3,d3=1, d4=32,d5=32,d6=1;break;
  case 5: d1=1,d2=4,d3=2, d4=3, d5=3, d6=1;break;
  case 6: d1=1,d2=4,d3=2, d4=1, d5=3, d6=1;break;
  case 7: d1=2,d2=2,d3=1, d4=32,d5=32,d6=1;break;
  case 8: d1=1,d2=4,d3=1, d4=1, d5=3, d6=1;break;
  case 9: d1=1,d2=4,d3=1, d4=3, d5=3, d6=1;break;}
lcd.setCursor(e1,0);lcd.write(uint8_t(d1));
lcd.setCursor(e2,0);lcd.write(uint8_t(d2));
lcd.setCursor(e3,0);lcd.write(uint8_t(d3));
lcd.setCursor(e1,1);lcd.write(uint8_t(d4));
lcd.setCursor(e2,1);lcd.write(uint8_t(d5));
lcd.setCursor(e3,1);lcd.write(uint8_t(d6));}//КОНЕЦ ЦИКЛА
}//END BIG VOLUME DIGITS
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ ИНДИКАТОРА, ТЕКУЩЕГО ВХОДА И УРОВНЯ ЧУСТВИТЕЛЬНОСТИ НА НЁМ В ГЛАВНОМ МЕНЮ
void ShowINPUT_menu(){
lcd.setCursor(0,1);
if(smenu1==1||smenu1==2){lcd.write(pointer);}else{lcd.print(' ');}
if(smenu1!=1){ShowInput=Input;}else{ShowInput=InputIDX;}
  switch(ShowInput){
    case 1: lcd.print("RCA");break;
    case 2: lcd.print("Jk1");break;
    case 3: lcd.print("Jk2");break;}
Gain=SELECT_Input[ShowInput];TrueP=Gain*2;TRUE_print();
}//END MAIN MENU 
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ НАСТРОЕК ТЕМБРА В МЕНЮ TONE CONTROL SETTING BASS, TREBLE
void Show_menu_2(){

//ИНДИКАТОР-УКАЗАТЕЛЬ И ЗНАЧЕНИЕ ОТКЛОНЕНИЯ НИЗКИХ ЧАСТОТ
lcd.setCursor(0,0);
if(smenu2==0){lcd.write(pointer);}else{lcd.print(' ');}
lcd.setCursor(1,0);lcd.print("BASS   ");
TrueP=Bass*2;TRUE_print();lcd.print(" dB");

//ИНДИКАТОР-УКАЗАТЕЛЬ И ЗНАЧЕНИЕ ОТКЛОНЕНИЯ ВЫСОКИХ ЧАСТОТ
lcd.setCursor(0,1);
if(smenu2==1){lcd.write(pointer);}else{lcd.print(' ');}
lcd.setCursor(1,1);lcd.print("TREBLE ");
TrueP=Treble*2;TRUE_print();lcd.print(" dB");
}//END MENU TONE CONTROL SETTING
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ НАСТРОЕК MENU BALANCE, BUFFER GAIN, WAIT RETURN В МЕНЮ № 3
void Show_menu_3(){

//ОТКЛОНЕНИЕ БАЛАНСА (ТОЛЬКО В СТОРОНУ УМЕНЬШЕНИЯ)
lcd.setCursor(0,0);
if(smenu3==0){lcd.write(pointer);}else{lcd.print(' ');}
lcd.setCursor(1,0);lcd.print("LEFT");
if(Balance<0){lcd.print('-');lcd.print(Bal_L);}else{lcd.print(' ');lcd.print(Bal_L);}lcd.print(' ');
lcd.setCursor(9,0);lcd.print("RIGHT");
if(Balance>0){lcd.print('-');lcd.print(Bal_R);}else{lcd.print(' ');lcd.print(Bal_R);};lcd.print(' ');

//ВКЛЮЧИТЬ ИЛИ ОТКЛЮЧИТЬ ЧУСТВИТЕЛЬНОСТЬ (УВЕЛИЧЕНИЕ НА 6 dB)
lcd.setCursor(0,1);
if(smenu3==1){lcd.write(pointer);}else{lcd.print(' ');}
lcd.setCursor(1,1);lcd.print("GAIN");
switch(Buffer){
  case 0:lcd.print(" 0");break;
  case 1:lcd.print("+6");break;}

//ПРОДОЛЖИТЕЛЬНОСТЬ ПАУЗЫ ДО ВОЗВРАТА В РЕЖИМ РЕГУЛИРОВКИ ГРОМКОСТИ В СЕКУНДАХ ОТ 5 ДО 30
lcd.setCursor(8,1);
if(smenu3==2){lcd.write(pointer);}else{lcd.print(' ');}
lcd.setCursor(9,1);lcd.print("RET ");
if(Wait_return<10){lcd.print(' ');}lcd.print(Wait_return);lcd.print('s');

}//END MENU BALANCE, BUFFER GAIN, WAIT RETURN 
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ ВРЕМЕНИ И ДНЯ НЕДЕЛИ ИЗ МОДУЛЯ DS3231
void timedow(){lcd.setCursor(0,0);
lcd.print(rtc.getTimeStr());lcd.print(' ');lcd.print(rtc.getDOWStr());
}//END MENU
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ ДАТЫ И ТЕМПЕРАТУРЫ СО ВСТРОЕННОГО В МОДУЛЬ ЧАСОВ ДАТЧИКА ИЗ МОДУЛЯ DS3231
void datetemp(){lcd.setCursor(0,1);
lcd.print(rtc.getDateStr());lcd.print("  ");
lcd.print(rtc.getTemp(),0);lcd.print((char)223);lcd.print('C');
}//END MENU
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ЗАПРОСА И ЗАПИСИ В ПЕРЕМЕННЫЕ ТЕКУЩИХ ЗНАЧЕНИЙ ВРЕМЕНИ И ДАТЫ ИЗ МОДУЛЯ DS3231
void GET_time(){
tmr=rtc.getTime();
sethour=tmr.hour;
setmin=tmr.min;
setdate=tmr.date;
setmon=tmr.mon;
setyear=(tmr.year-2000);
setdow=tmr.dow;
}//END MENU
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ЗАПИСИ ТЕКУЩИХ ЗНАЧЕНИЙ ВРЕМЕНИ И ДАТЫ В МОДУЛЬ DS3231
void SET_time(){
rtc.setTime(sethour,setmin,0);
rtc.setDate(setdate,setmon,(setyear+2000));
rtc.setDOW(setdow);
}//END MENU
////////////////////////////////////////////////////////////////////////////////////////////////


//ФУНКЦИЯ ПЕЧАТИ БОЛЬШИХ ЧАСОВ ТИП 1. (секунды посредине)
void BIG_watch(){
int8_t sign[4];uint8_t i,d1,d2,d3,d4,d5,d6,e1,e2,e3;
//tmr=rtc.getTime();//ЗАПРАШИВАЕМ У МОДУЛЯ DS3231 АКТУАЛЬНОЕ ВРЕМЯ
//ЗАДАЁМ КОНКРЕТНЫЕ ЗНАЧЕНИЯ ЧЕТЫРЁХ ПЕРЕМЕННЫХ ДЛЯ МАССИВА sign
sign[0]=tmr.hour/10;//ПЕРВАЯ ЦИФРА ЧАСОВ
sign[1]=tmr.hour%10;//ВТОРАЯ ЦИФРА ЧАСОВ
sign[2]=tmr.min/10;//ПЕРВАЯ ЦИФРА МИНУТ
sign[3]=tmr.min%10;//ВТОРАЯ ЦИФРА МИНУТ
//ЗА ОДИН ИЗ 4 ПРОХОДОВ ЦИКЛА ПЕЧАТАЕМ ТОЛЬКО ОДНУ ЦИФРУ
for(i=0;i<4;i++){
switch(i){
  case 0: e1=0, e2=1, e3=2; break;//ПОЗИЦИИ КУРСОРА ТРЁХ СИМВОЛОВ ПЕРВОЙ ЦИФРЫ ЧАСОВ
  case 1: e1=4, e2=5, e3=6; break;//ПОЗИЦИИ КУРСОРА ТРЁХ СИМВОЛОВ ВТОРОЙ ЦИФРЫ ЧАСОВ
  case 2: e1=9, e2=10,e3=11;break;//ПОЗИЦИИ КУРСОРА ТРЁХ СИМВОЛОВ ПЕРВОЙ ЦИФРЫ МИНУТ
  case 3: e1=13,e2=14,e3=15;break;}//ПОЗИЦИИ КУРСОРА ТРЁХ СИМВОЛОВ ВТОРОЙ ЦИФРЫ МИНУТ
//ИЗ МАССИВА sign ВЫБИРАЕМ КОНКРЕТНОЕ ЗНАЧЕНИЕ И ЗАПОЛНЯЕМ 6 ПЕРЕМЕННЫХ СПЕЦСИМВОЛАМИ
switch(sign[i]){
  case 0: d1=1,d2=2,d3=1, d4=1, d5=3, d6=1;break;
  case 1: d1=2,d2=1,d3=32,d4=5, d5=1, d6=5;break;
  case 2: d1=2,d2=4,d3=1, d4=1, d5=3, d6=3;break;
  case 3: d1=2,d2=4,d3=1, d4=3, d5=3, d6=1;break;
  case 4: d1=1,d2=3,d3=1, d4=32,d5=32,d6=1;break;
  case 5: d1=1,d2=4,d3=2, d4=3, d5=3, d6=1;break;
  case 6: d1=1,d2=4,d3=2, d4=1, d5=3, d6=1;break;
  case 7: d1=2,d2=2,d3=1, d4=32,d5=32,d6=1;break;
  case 8: d1=1,d2=4,d3=1, d4=1, d5=3, d6=1;break;
  case 9: d1=1,d2=4,d3=1, d4=3, d5=3, d6=1;break;}
//ПЕЧАТАЕМ 6 ВЫБРАННЫХ СПЕЦСИМВОЛОВ, СНАЧАЛА В 3 ПОЗИЦИЯХ ВЕРХНЕЙ СТРОКИ ЗАТЕМ В НИЖНЕЙ
lcd.setCursor(e1,0);lcd.write(uint8_t(d1));
lcd.setCursor(e2,0);lcd.write(uint8_t(d2));
lcd.setCursor(e3,0);lcd.write(uint8_t(d3));
lcd.setCursor(e1,1);lcd.write(uint8_t(d4));
lcd.setCursor(e2,1);lcd.write(uint8_t(d5));
lcd.setCursor(e3,1);lcd.write(uint8_t(d6));}//КОНЕЦ ЦИКЛА
}//END OF FUNCTION


//ИНДИКАЦИЯ СЕКУНД МЕЖДУ БОЛЬШИМИ ЦИФРАМИ ЧАСОВ (ТИП 1.)
void Show_SECONDS_ONLY(){
lcd.setCursor(7,1);if(tmr.sec<10){lcd.print('0');}lcd.print(tmr.sec);
}//END OF FUNCTION


//ФУНКЦИЯ ПЕЧАТИ БОЛЬШИХ ЧАСОВ ТИП 2. (секунды и температура справа)
void BIG_clock_temp(){
uint8_t i,d1,d2,d3,d4,d5,d6,e1,e2,e3;
int8_t a[4];
//tmr=rtc.getTime();
a[0]=tmr.hour/10;
a[1]=tmr.hour%10;
a[2]=tmr.min/10;
a[3]=tmr.min%10;
for(i=0;i<4;i++){
switch(i){
case 0: e1=0, e2=1, e3=2;break;
case 1: e1=3, e2=4, e3=5;break;
case 2: e1=7, e2=8, e3=9;break;
case 3: e1=10,e2=11,e3=12;break;}
switch(a[i]){
case 0: d1=1,d2=8,d3=6, d4=1, d5=3, d6=6;break;
case 1: d1=2,d2=6,d3=32,d4=32,d5=6, d6=32;break;
case 2: d1=2,d2=8,d3=6, d4=1, d5=4, d6=5;break;
case 3: d1=2,d2=4,d3=6, d4=7, d5=3, d6=6;break;
case 4: d1=1,d2=3,d3=6, d4=32,d5=32,d6=6;break;
case 5: d1=1,d2=4,d3=5, d4=7, d5=3, d6=6;break;
case 6: d1=1,d2=4,d3=5, d4=1, d5=3, d6=6;break;
case 7: d1=2,d2=8,d3=6, d4=32,d5=32,d6=6;break;
case 8: d1=1,d2=4,d3=6, d4=1, d5=3, d6=6;break;
case 9: d1=1,d2=4,d3=6, d4=7, d5=3, d6=6;break;}
lcd.setCursor(e1,0);lcd.write(uint8_t(d1));
lcd.setCursor(e2,0);lcd.write(uint8_t(d2));
lcd.setCursor(e3,0);lcd.write(uint8_t(d3));
lcd.setCursor(e1,1);lcd.write(uint8_t(d4));
lcd.setCursor(e2,1);lcd.write(uint8_t(d5));
lcd.setCursor(e3,1);lcd.write(uint8_t(d6));}
lcd.setCursor(6,0);lcd.print('.');lcd.setCursor(6,1);lcd.print('.');
}//END OF FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////


//ИНДИКАЦИЯ СЕКУНД И ТЕМПЕРАТУРЫ СПРАВА ОТ БОЛЬШИХ ЦИФР ЧАСОВ (ТИП 2.)
void Show_SECONDS_TEMPERATURE(){
lcd.setCursor(13,0);
if(rtc.getTemp()>0){lcd.print('+');}else{lcd.print(' ');}lcd.print(rtc.getTemp(),0);
lcd.setCursor(14,1);
if(tmr.sec<10){lcd.print('0');}lcd.print(tmr.sec);
}//END OF FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////


//ЗАТИРАЕМ ПРОБЕЛАМИ СТРОКУ
void LINE1_clear(){lcd.setCursor(0,1);lcd.print(F("                "));}



/////////////////////////////////////////////////////////////////////////////////////////////////
//                      ФУНКЦИЯ ЗАПИСИ ПАРАМЕТРОВ В АУДИОПРОЦЕССОР TDA7468D                    //
/////////////////////////////////////////////////////////////////////////////////////////////////
void audio(){
tda.setInput(Input-1); // (IN1, IN2, IN3, IN4) 0...3 DEFAULT: (0)
tda.setGain(Gain); // (0...14 dB step 2dB) 0...7 DEFAULT: (0)
tda.setVol_R(Volume-Bal_R); // 0...62 DEFAULT: (0)
tda.setVol_L(Volume-Bal_L); // 0...62 DEFAULT: (0)
tda.setTreb_Bass(Treble,Bass); // -14...14 step 2 dB -7...7 DEFAULT: (0,0)
tda.setOutput(Mute); // Mute 0=on, 1=off DEFAULT: (1)
tda.setSurround(Surround,INgain,Mixing,Buffer); // DEFAULT: (1,0,3,0)
// Surround = SURROUND MODE 0=on, 1=off - включение/выключение режима Surround.
// INgain = GAIN (0dB,6dB,9dB,12dB) 0...3 DEFAULT: (0) - предусиление фазоинверторов перед \
подмешиванием сигнала в соседний канал.
// Mixing = MIXING  (100%, 50%, 25%, 0%, 100%, 50%, 25%, Мute)  0...7  DEFAULT: (3) \
- фазоинвертор с программно устанавливаемыми режимами. Mute - отключает фазоинвертор. \
INVERTING: (100%, 50%, 25%, 0%) 0...3  \
NON-INVERTING: (100%, 50%, 25%, Мute) 4...7
//ВНИМАНИЕ! \
Функция MIXING будет работать если линейные выходы (выводы 7, 22) через переходной конденсатор \
подключены ко входам фазоинверторов, соответственно левого и правого каналов(выводы 8, 21).
// Buffer = BUFFER GAIN  0=0dB, 1=6dB DEFAULT: (0) - предусиление буфера. \
Буфер нужен для согласования между каскадами и/или для осуществления небольшой \
задержки сигнала перед смесителем.

//tda.setAlc(mode,detector,circ,attack,tresh,rez); // DEFAULT: (0,0,0,0,0,0) 

// BASS ALC (Bass Automatic Level Control) - Функции автоматического регулирования \
уровня низкочастотного сигнала. В ДАННОМ ПРОЕКТЕ НЕ ИСПОЛЬЗУЮТСЯ!
// ДЛЯ ЛЮБИТЕЛЕЙ ЭКСПЕРЕМЕНТОВ:
// mode = ALC MODE 1=on, 0=off  - включение/выключение режима автоматического регулирования.
// detector = DETECTOR 1=on, 0=off  - включение/выключение детектора НЧ составляющей на выходе.
// circ = RELEASE CURRENT CIRCUIT 1=on, 0=off  - включение/выключение источника тока. \
Источник тока устанавливает режим работы дифференциального каскада системы регулирования.
// attack = ATTACK TIME RESISTOR (12.5kОм, 25kОм, 50kОм, 100kОм) 0...3 - программно \
устанавливаемый резистор делителя, формирующего сигнал управления системой регулирования.
// tresh = THRESHOLD (700mVrms, 485mVrms, 320mVrms, 170mVrms) 0...3 - пороговые уровни \
ограничения НЧ сигнала на выходе процессора, устанавливаемые программно.
// rez = ATTACK MODE (MODE 1: Fixed Resistor, MODE 2: Adaptive) 0...1 - выбор режима работы \
резистора делителя (Attack Time Resistor), \
MODE 1: Fixed Resistor - с фиксированным и установленным в ATTACK TIME RESISTOR \
значением сопротивления \
MODE 2: Adaptive - с изменяющимся сопротивлением плеча делителя в зависимости от \
уровня НЧ составляющей в выходном сигнале.
/////////////////////////////////////////////////////////////////////////////////////////////////
}//END audio MENU



/////////////////////////////////////////////////////////////////////////////////////////////////
//                                          EEPROM UPDATE                                      //
//                         ОБНОВЛЕНИЕ (ПЕРЕЗАПИСЬ) ПАРАМЕТРОВ В EEPROM                         //
/////////////////////////////////////////////////////////////////////////////////////////////////
void EEPROM_UPDATE(){
EEPROM.update(0,Volume);
EEPROM.update(1,Treble+7);
EEPROM.update(2,Bass+7);
EEPROM.update(3,Input);
EEPROM.update(4,Bal_L);
EEPROM.update(5,Bal_R);
EEPROM.update(6,Watch_Type);
EEPROM.update(7,Wait_return);
EEPROM.update(8,LCD_light);
EEPROM.update(12,Buffer);
EEPROM.update(14,Balance+Volume);
//ЦИКЛ ПЕРЕЗАПИСИ УРОВНЯ ЧУСТВИТЕЛЬНОСТИ ДЛЯ КАЖДОГО ИЗ 3 ВХОДОВ
for(byte d=0;d<4;d++){EEPROM.update(20+d,SELECT_Input[d]);}
}//END EEPROM UPDATE 



///////////////////////////////////////// END FUNCTIONS /////////////////////////////////////////
