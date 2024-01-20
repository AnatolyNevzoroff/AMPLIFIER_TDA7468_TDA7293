////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//                      УСИЛИТЕЛЬ МОЩНОСТИ С АУДИОПРОЦЕССОРОМ TDA7468D                        //
//                      POWER AMPLIFIER WITH TDA7468D AUDIO PROCESSOR)                        //
//                                    ВЕРСИЯ (VERSION) 2.0                                    //
//                   КОД ОТ АНАТОЛИЯ НЕВЗОРОВА (CODE BY ANATOLY NEVZOROFF)                    //
//                            https://github.com/AnatolyNevzoroff                             //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////



// #define LOG_ENABLE //ВЫВОД ДАННЫХ В СЕРИЙНЫЙ ПОРТ (ДЛЯ ВКЛЮЧЕНИЯ РАСКОММЕНТИРОВАТЬ)


//КОДЫ КНОПОК IR ПУЛЬТА (БОЛЬШОЙ ПУЛЬТ)
#define IR_VOL_UP 0x8F807    //Кнопка ГРОМКОСТЬ "+" (БОЛЬШЕ)
#define IR_VOL_DOWN 0x802FD  //Кнопка ГРОМКОСТЬ "-" (МЕНЬШЕ)
#define IR_MENU_UP 0x8F00F   //Кнопка ВВЕРХ ПО МЕНЮ
#define IR_MENU_DOWN 0x808F7 //Кнопка ВНИЗ ПО МЕНЮ
#define IR_PREV_MENU 0x8827D //Кнопка ПРЕДЫДУЩЕЕ МЕНЮ
#define IR_NEXT_MENU 0x8E817 //Кнопка СЛЕДУЮЩЕЕ МЕНЮ (ДУБЛИРУЕТ MENU_key)
#define IR_POWER 0x800FF     //Кнопка POWER/STANDBY (ON/OFF) (ДУБЛИРУЕТ POWER_key)
#define IR_MUTE 0x818E7      //Кнопка MUTE (ON/OFF)
#define IR_MAIN_MENU 0x8D827 //Кнопка ГЛАВНОЕ МЕНЮ
#define IR_FFFFF 0xFFFFFFFF  //Команда ПОВТОРА

//ВЫВОДЫ НА ПЛАТЕ ARDUINO
#define ENCODER_PIN_A 2   //D2 К ВЫВОДУ ЭНКОДЕРА (DT ИЛИ S1) 
#define ENCODER_PIN_B 3   //D3 К ВЫВОДУ ЭНКОДЕРА (CLK ИЛИ S2)
#define ENCODER_key 4     //D4 КНОПКА ПЕРЕХОДА ПО МЕНЮ (КНОПКА ЭНКОДЕРА SW ИЛИ ДУ IR_MENU_DOWN)
#define POWER_key 12      //D12 КНОПКА ВКЛЮЧЕНИЯ POWER/STANDBY (ДУБЛИРУЕТ IR_POWER)
#define MENU_key 11       //D11 КНОПКА ПЕРЕКЛЮЧЕНИЯ МЕЖДУ МЕНЮ (ДУБЛИРУЕТ IR_NEXT_MENU)
#define POWER_RELAY_out 13//D13 ПИТАНИЕ УСИЛИТЕЛЯ POWER RELAY ON/OFF (Белый)
#define IR_INPUT 15       //D15 (А1) ВХОД ИК ДАТЧИКА INPUT INFRARED RECEIVER

//ДОПОЛНИТЕЛЬНЫЕ КОНСТАНТЫ
#define p10000 10000L     //СЛУЖЕБНАЯ ЗАДЕРЖКА ДЛЯ ТАЙМЕРА ТИП ДАННЫХ "L" (long)
#define p2000 2000L       //СЛУЖЕБНАЯ ЗАДЕРЖКА ДЛЯ ТАЙМЕРА 
#define p1000 1000L       //СЛУЖЕБНАЯ ЗАДЕРЖКА ДЛЯ ТАЙМЕРА 
#define LongPress 500L    //ДЛИТЕЛЬНАЯ СЛУЖЕБНАЯ ЗАДЕРЖКА ДЛЯ ТАЙМЕРА КНОПКИ
#define ShortPress 200L   //КОРОТКАЯ СЛУЖЕБНАЯ ЗАДЕРЖКА ДЛЯ ТАЙМЕРА КНОПКИ
#define Surround 1        //SURROUND MODE OFF (выключение режима Surround)
#define INgain 0          //GAIN = 0dB (предусиление фазоинверторов)
#define Mixing 3          //MIXING = 0% (фазоинвертор В %)




//ПОДКЛЮЧАЕМЫЕ БИБЛИОТЕКИ
#include <Wire.h>
#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
//#include <Encoder.h>
#include "Rotary.h"
#include <EEPROM.h>
#include <DS3231.h>
#include <TDA7468.h>

Rotary encoder=Rotary(ENCODER_PIN_A, ENCODER_PIN_B);//ОБЪЯВЛЯЕМ ПИНЫ ЭНКОДЕРА DT (S2) и CLK (S1)
LiquidCrystal_I2C lcd(0x27,16,2);//УСТАНАВЛИВАЕМ АДРЕС И ПАРАМЕТРЫ ДИСПЛЕЯ
IRrecv irrecv(IR_INPUT);//УКАЗЫВАЕМ ВЫВОД ПОДКЛЮЧЕНИЯ IR ПРИЁМНИКА 
decode_results ir;//ОБЪЯВЛЯЕМ ir ДЛЯ БИБЛИОТЕКИ IR ПРИЁМНИКА 
TDA7468 tda;//ОБЪЯВЛЯЕМ tda ДЛЯ БИБЛИОТЕКИ АУДИОПРОЦЕССОРА
DS3231 rtc(SDA,SCL);Time tmr;//ОБЪЯВЛЯЕМ tmr ДЛЯ БИБЛИОТЕКИ ЧАСОВ


//ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
//unsigned long от 0 до 4294967295
uint32_t timer_AUTORET,timer_MENU,timer_BTN,timer_WATCH;
uint32_t IRkey;//ПЕРЕМЕННАЯ ДЛЯ ВРЕМЕННОГО ХРАНЕНИЯ КОДОВ КНОПОК ПДУ

//int от -32768 до 32767  
//int16_t ;

//byte от 0 до 255
uint8_t Power,TimeDate_SAVE,Wait_return,lastMIN,LCD_light;
uint8_t SELECT_Input[4],Input,InputIDX,ShowInput,Gain;//МАССИВ И ПЕРЕМЕННЫЕ ДЛЯ ВХОДОВ
uint8_t POWER_btn,POWER_btn_status,MENU_btn,MENU_btn_status,ENCODER_btn,ENCODER_btn_status;
uint8_t pointer,triangle,arrow;//СПЕЦСИМВОЛЫ

//char от -128 до 127
int8_t menu,smenu1,smenu2,smenu3,smenu4,trig1,trig2,trig3,trig4;
int8_t setmin,sethour,setdate,setmon,setdow,setyear;
int8_t Volume,Bass,Treble,Balance,Buffer,Mute,Bal_R,Bal_L;
int8_t Watch_Type,encoderCount,TrueP;

//boolean от false до true 
bool FL_autoret,FL_start_MENU,FL_start_POWER=true,FL_delay_MENU,FL_delay_ENCODER;
bool FL_com1,FL_com2,MUTE_animat,WATCH_menu,SET_TIME_DATE_menu;//FL_IR_led,





/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                          S E T U P                                          //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
void setup(){

#ifdef LOG_ENABLE
Serial.begin(9600);
#endif


//Wire.begin();//ЗАПУСКАЕМ ШИНУ I2C (МОЖНО НЕ УКАЗЫВАТЬ, ЗАПУСКАЕТСЯ ИЗ ДРУГИХ БИБЛИОТЕК)
irrecv.enableIRIn();//ЗАПУСКАЕМ ПРИЁМ ДАНННЫХ С ИНФРАКРАСНОГО ПРИЁМНИКА
rtc.begin();//ЗАПУСКАЕМ МОДУЛЬ ЧАСОВ
lcd.init();//ЗАПУСКАЕМ ЖИДКОКРИСТАЛИЧЕСКИЙ ЭКРАН


pinMode(ENCODER_key,INPUT);//D4 КНОПКА ПЕРЕМЕЩЕНИЯ ПО МЕНЮ (КНОПКА ЭНКОДЕРА ДУБЛИРУЕТ IR_MENU_DOWN)
pinMode(POWER_key,INPUT); //D12 КНОПКА ВКЛЮЧЕНИЯ POWER/STANDBY (ДУБЛИРУЕТ IR_POWER)
pinMode(MENU_key,INPUT); //D11 КНОПКА ПЕРЕКЛЮЧЕНИЯ МЕЖДУ МЕНЮ (ДУБЛИРУЕТ IR_NEXT_MENU)
pinMode(POWER_RELAY_out,OUTPUT);//D13 ПИТАНИЕ УСИЛИТЕЛЯ POWER RELAY + ИНДИКАТОР POWER LED (Красный)
pinMode(IR_INPUT,INPUT); //ВХОД ИК ДАТЧИКА INPUT INFRARED RECEIVER

digitalWrite(POWER_RELAY_out,HIGH);//ОТКЛЮЧАЕМ POWER RELAY И ИНДИКАТОР ВКЛЮЧЕНИЯ


//ФУНКЦИЯ "rotaryEncoder" ВЫЗЫВАЕМАЯ ПРЕРЫВАНИЕМ 
attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A),rotaryEncoder,CHANGE);
attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B),rotaryEncoder,CHANGE);


/////////////////////////////////////////////////////////////////////////////////////////////////
//                                          EEPROM READ                                        //
//                           СЧИТЫВАЕМ ЗНАЧЕНИЯ ПЕРЕМЕННЫХ ИЗ EEPROM                           //
/////////////////////////////////////////////////////////////////////////////////////////////////
//if(EEPROM.read(30)!=0){for(int i=0;i<31;i++){EEPROM.update(i,0);}}
Volume=EEPROM.read(0);//Уровень громкости 0...62 dB (0...62)
Treble=EEPROM.read(1)-7;//Уровень высоких частот -14...+14 dB (-7...+7) (в памяти 0...14)
Bass=EEPROM.read(2)-7;//Уровень низких частот -14...+14 dB (-7...+7) (в памяти 0...14)
Input=EEPROM.read(3);//Выбранный вход (0,1,2)
Bal_L=EEPROM.read(4);//Понижение уровня в ЛЕВОМ канале 0...62 dB (0...62)
Bal_R=EEPROM.read(5);//Понижение уровня в ПРАВОМ канале 0...62 dB (0...62)
Watch_Type=EEPROM.read(6);//Тип отображаемых часов в режиме Stanby (0,1,2) (1...3)
Wait_return=EEPROM.read(7);//Время автоматического возврата в режим регулировки громкости (5...30 сек.)
LCD_light=EEPROM.read(8);//Подсветка экрана в режиме Stanby (0,1)
Buffer=EEPROM.read(12);//Состояние буферной чуствительности (0,1)
Balance=EEPROM.read(14)-Volume;//Баланс может изменяться в пределах установленной громкости
//Из ячеек с 20 по 22 извлекаем Gain для каждого из 3 входов
for(byte d=0;d<4;d++){SELECT_Input[d]=EEPROM.read(20+d);}
Gain=SELECT_Input[Input];//Присваиваем Gain уровень в зависимости от входа
if(Wait_return<5)Wait_return=15;//Установка Wait_return=10сек. при САМОМ первом включении 


}//END SETUP 
/////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                           L O O P                                           //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){


/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      IR REMOTE CONTROL                                      //
//                      ОБРАБАТЫВАЕМ КОМАНДЫ С ИНФРАКРАСНОГО ПРИЁМНИКА                         //
/////////////////////////////////////////////////////////////////////////////////////////////////

//ЕСЛИ ПРИ ОПРОСЕ IR ПРИЁМНИКА В БУФЕРЕ ИМЕЕТСЯ КОД КНОПКИ ОТЛИЧНЫЙ ОТ 0, ПОЛУЧАЕМ ЕГО В ВИДЕ
//ЗНАЧЕНИЯ ПЕРЕМЕННОЙ ir.value (ЗАДАНО КОМАНДОЙ decode_results ir) 
if(irrecv.decode(&ir)){

#ifdef LOG_ENABLEIRkey
Serial.print("0x");Serial.println(ir.value,HEX);//В МОНИТОРЕ ПОРТА МОЖНО ПОСМОТРЕТЬ КОД КНОПКИ 
#endif

//ЕСЛИ С ПДУ ПОЛУЧЕН КОД КНОПКИ НЕ ЯВЛЯЮЩИЙСЯ КОДОМ ПОВТОРА, ЗАПИСЫВАЕМ КОД КНОПКИ 
//ВО ВРЕМЕННУЮ ПЕРЕМЕННУЮ IRkey, И БЛОКИРУЕМ МК НА ПРИЁМ ЛЮБЫХ КОДОВ СОЗДАВАЯ ПАУЗУ ShortPress, 
//ТЕМ САМЫМ ПРЕДОТВРАЩАЕМ НЕНУЖНОЕ ВЫПОЛНЕНИЕ КОДОВ ПОВТОРА КОТОРЫЕ СЛЕДУЮТ СРАЗУ ЗА КОДОМ КНОПКИ
//С ОЧЕНЬ КОРОТКОЙ ПАУЗОЙ ЗАДАННОЙ ПДУ, 
//ЕСЛИ ПОЛЬЗОВАТЕЛЬ УДЕРЖИВАЕТ КНОПКУ ДОЛЬШЕ ЧЕМ ShortPress УСЛОВИЕ НЕ ВЫПОЛНЯЕТСЯ И КОМАНДЫ 
//ПОВТОРА ПРОХОДЯТ БЕЗ ЗАДЕРЖЕК
if(ir.value!=IR_FFFFF){IRkey=ir.value;delay(ShortPress);}

//СБРАСЫВАЕМ ТЕКУЩЕЕ ЗНАЧЕНИЕ ПОЛУЧЕННОГО КОДА И ПЕРЕКЛЮЧАЕМ IR ПРИЁМНИК НА ПРИЁМ СЛЕДУЮЩЕГО
//ПРИ ЭТОМ ЗНАЧЕНИЕ ir.value НЕ МЕНЯЕТСЯ И СБРАСЫВАЕТСЯ КОМАНДОЙ ir.value=0
irrecv.resume();}
////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////
//                                      POWER KEY ON/OFF                                       //
//                        КНОПКА ПЕРЕКЮЧЕНИЯ РЕЖИМОВ POWER / STANDBY                           //
/////////////////////////////////////////////////////////////////////////////////////////////////

//ТОЛЬКО В ДАННОМ ПРОЕКТЕ! В КАЧЕСТВЕ ЭКСПЕРИМЕНТА, КНОПКИ "POWER" И "MENU" ПОДКЛЮЧЕНЫ С
//ПОСТОЯННОЙ ПОДТЯЖКОЙ ЧЕРЕЗ РЕЗИСТОРЫ 10кОм К ЗЕМЛЕ!, И ОБЫЧНОЕ СОСТОЯНИЕ КНОПКИ - "LOW"
//ПРИ ЗАМЫКАНИИ ЧЕРЕЗ КНОПКУ НА ВХОД ПОДАЁТСЯ +5В И СОСТОЯНИЕ СТАНОВИТЬСЯ - "HIGH"
//В ДРУГИХ ПРОЕКТАХ, КАК У ЭНКОДЕРА, ТО ЕСТЬ ВСЁ НАОБОРОТ :-))

//ДЛЯ СОВСЕМ НАЧИНАЮЩИХ, ПОЯСНЮ РАБОТУ ПРОВЕРЕННОГО АЛГОРИТМА ЖЕЛЕЗНОЙ КНОПКИ

POWER_btn=digitalRead(POWER_key);//ОПРАШИВАЕМ СОСТОЯНИЕ ВХОДА С КНОПКОЙ "POWER"
//1. УСЛОВИЕ 
if(POWER_btn!=POWER_btn_status){
//ЕСЛИ POWER_btn ВДРУГ СТАЛА "HIGH" ПРИ ЭТОМ POWER_btn_status ПО УМОЛЧАНИЮ "LOW" ЗНАЧИТ
//УСЛОВИЕ ВЫПОЛНЕНО, НО ЛУЧШЕ ПОДОЖДАТЬ 0,03сек. МОГЛА И ПОМЕХА ПРОСКОЧИТЬ!
delay(30);
//А ЕСЛИ ЭТО НЕ ПОМЕХА И КНОПКА ДЕЙСТВИТЕЛЬНО НАЖАТА? 
//ВСЁ РАВНО ПОДОЖДЁМ ПОКА КОНТАКТЫ В КНОПКЕ ПРЕКРАТЯТ "ДРЕБЕЗГ"
//ДЛЯ ОЧЕНЬ ПЛОХИХ КНОПОК ЭТО ЗНАЧЕНИЕ МОЖНО УВИЛИЧИТЬ ДО 50, ДЛЯ ХОРОШИХ СНИЗИТЬ ДО 10
//И ЕЩЁ РАЗ ОПРОСИМ СОСТОЯНИЕ ВХОДА С КНОПКОЙ "POWER"
POWER_btn=digitalRead(POWER_key);
//2. УСЛОВИЕ
if(POWER_btn==HIGH&&POWER_btn_status==LOW){
//ЕСЛИ ТЕКУЩЕЕ ЗНАЧЕНИЕ POWER_btn ТАК И ОСТАЛОСЬ "HIGH", А POWER_btn_status "LOW" 
//НИКАКАЯ ЭТО НЕ ПОМЕХА! КНОПКА ТОЧНО НАЖАТА!
Power=!Power;FL_start_POWER=true;//ВЫПОЛНЯЕМ КОМАНДЫ СВЯЗАННЫЕ С НАЖАТИЕМ КНОПКИ
//ЧТОБ НЕ ВЫПОЛНЯЛОСЬ ПЕРВОЕ УСЛОВИЕ И КНОПКА НЕ СРАБАТЫВАЛА ПОКА POWER_btn НЕ ИЗМЕНИТ СВОЁ
//СОСТОЯНИЕ, (ПОКА КНОПКА НАЖАТА) ВЫПОЛНЯЕМ КОМАНДУ POWER_btn_status=POWER_btn 
//ПРИРАВНИВАЕМ POWER_btn_status К POWER_btn, (ПОКА КНОПКА НАЖАТА ОБЕ ПЕРЕМЕННЫЕ СТАНУТ "HIGH").
}POWER_btn_status=POWER_btn;}
//ВОТ И ВСЁ, КНОПКА СРАБОТАЛА И ЗАБЛОКИРОВАНА ОТ БЕСКОНЕЧНОГО ПОВТОРА, 
//НО САМОЕ ИНТЕРЕСНОЕ ПРОИСХОДИТ ДАЛЬШЕ, КОГДА КНОПКА ОКАЗЫВАЕТСЯ ОТПУЩЕНА, 
//ПОСЛЕ ОЧЕРЕДНОГО ОПРОСА ВХОДА, ПЕРЕМЕННАЯ POWER_btn 
//СТАНОВИТСЯ "LOW" ПРИ ЭТОМ POWER_btn_status ОСТАЁТСЯ "HIGH" 
//И СНОВА ВЫПОЛНЯЕТСЯ ПЕРВОЕ УСЛОВИЕ, 
//СНОВА ЖДЁМ 0,03 сек. 
//ПОВТОРНО ОПРАШИВАЕМ ВХОД С КНОПКОЙ "POWER", POWER_btn ТАК И ОСТАЁТСЯ "LOW"
//КНОПКА ТОЧНО ОТПУЩЕНА! НО ОБЕ ПЕРЕМЕННЫЕ НЕ СООТВЕТСТВУЮТ ВТОРОМУ УСЛОВИЮ И НИКАКИЕ КОМАНДЫ 
//СВЯЗАННЫЕ С КНОПКОЙ НЕ БУДУТ ВЫПОЛНЕНЫ, 
//ЗАТО БУДЕТ ЕЩЁ РАЗ ВЫПОЛНЕНА КОМАНДА POWER_btn_status=POWER_btn 
//КОТОРАЯ ПЕРЕВЕДЁТ POWER_btn_status ОПЯТЬ В ПЕРВОНАЧАЛЬНОЕ СОСТОЯНОЕ "LOW" 
//И ОПЯТЬ НЕ БУДЕТ ВЫПОЛНЯТЬСЯ ПЕРВОЕ УСЛОВИЕ ПОКА НЕ ИЗМЕНИТСЯ СОСТОЯНИЕ POWER_btn 
//МК БУДЕТ БЕСКОНЕЧНО ОПРАШИВАТЬ СОСТОЯНИЕ ВХОДА ДО НАЖАТИЯ КНОПКИ
////////////////////////////////////////////////////////////////////////////////////////////////
if(ir.value==IR_POWER){Power=!Power;FL_start_POWER=true;ir.value=0;}//ТУТ И ПОЯСНЯТЬ НЕЧЕГО...
////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//                                          POWER ON                                          //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////
if(Power==true){

//УСТАНАВЛИВАЕМ НАЧАЛЬНЫЕ ПАРАМЕТРЫ ПРИ ВКЛЮЧЕНИИ 
if(FL_start_POWER==true){
Gain=SELECT_Input[Input];Mute=true;
menu=1;smenu1=0;FL_start_MENU=true;FL_com1=true;
Big_BOLD_numbers();Pointers();encoderCount=0;lcd.clear();
digitalWrite(POWER_RELAY_out,LOW);//POWER ON, RED LED ON
lcd.backlight();//ОБЯЗАТЕЛЬНАЯ НЕОТКЛЮЧАЕМАЯ ПОДСВЕТКА ЭКРАНА В РЕЖИМЕ POWER ON (LCD LIGHT ON)
FL_start_POWER=false;}




////////////////////////////////////////////////////////////////////////////////////////////////
//                                         MENU KEY                                           //
//                             КНОПКИ ПЕРЕКЛЮЧЕНИЯ МЕЖДУ МЕНЮ                                 //
////////////////////////////////////////////////////////////////////////////////////////////////
//ПЕРЕКЛЮЧЕНИЕ МЕЖДУ МЕНЮ КНОПКОЙ НА ПАНЕЛИ (СЛЕДУЮЩЕЕ МЕНЮ)
MENU_btn=digitalRead(MENU_key);
if(MENU_btn!=MENU_btn_status){delay(30);MENU_btn=digitalRead(MENU_key);
if(MENU_btn==HIGH&&MENU_btn_status==LOW){
menu++;if(menu>3){menu=1;}FL_start_MENU=true;Autoret_TIMER();
lcd.clear();}
MENU_btn_status=MENU_btn;}

//ПЕРЕКЛЮЧЕНИЕ МЕЖДУ МЕНЮ КНОПКАМИ ПУЛЬТА ДУ
if(ir.value==IR_PREV_MENU||ir.value==IR_NEXT_MENU){
if(ir.value==IR_NEXT_MENU){menu++;if(menu>3){menu=1;}}
if(ir.value==IR_PREV_MENU){menu--;if(menu<1){menu=3;}}
ir.value=0;FL_start_MENU=true;Autoret_TIMER();
lcd.clear();}





/////////////////////////////////////////////////////////////////////////////////////////////////
//                                          MUTE KEY                                           //
//                        КНОПКА ПЕРЕКЛЮЧЕНИЯ НА ДУ РЕЖИМА MUTE ON/OFF                         //
/////////////////////////////////////////////////////////////////////////////////////////////////
if(ir.value==IR_MUTE){Mute=!Mute;tda.setOutput(Mute);ir.value=0;lcd.clear();
switch(Mute){
  case 0: FL_autoret=false;menu=0;break;//MUTE ON  
  case 1: FL_autoret=true;menu=1;break;}//MUTE OFF 
}//MUTE KEY END




/////////////////////////////////////////////////////////////////////////////////////////////////
//                                         РЕЖИМ "MUTE"                                        //
//                     ПОПЕРЕМЕННО ВЫВОДИМ НА ЭКРАН ДАТУ/ВРЕМЯ ИЛИ "MUTE"                      //
/////////////////////////////////////////////////////////////////////////////////////////////////
if(Mute==false){
if(millis()>timer_MENU){MUTE_animat=!MUTE_animat;timer_MENU=millis()+p2000;
lcd.setCursor(0,MUTE_animat);lcd.print(F("      MUTE      "));
if(MUTE_animat){timedow();}else{datetemp();}}
}//MUTE MENU END




/////////////////////////////////////////////////////////////////////////////////////////////////
//                          АВТОВОЗВРАТ В РЕЖИМ "РЕГУЛИРОВКА ГРОМКОСТИ"                        //
//                           ЛИБО ЧЕРЕЗ ЗАДАННЫЙ ИНТЕРВАЛ БЕЗДЕЙСТВИЯ                          //
//                                    ЛИБО КНОПКОЙ С ПУЛЬТА                                    //
/////////////////////////////////////////////////////////////////////////////////////////////////
if(FL_autoret==true&&millis()-timer_AUTORET>Wait_return*p1000||ir.value==IR_MAIN_MENU){
ir.value=0;
if(menu>1){lcd.clear();}
menu=1;FL_start_MENU=true;
FL_autoret=false;
}//END MODE RETURN TO THE MAIN MENU 




////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//                                        ГЛАВНОЕ МЕНЮ                                        //
//               (VOLUME 0...62 dB / INPUT SELECTION / GAIN INPUT 0...+14 dB)                 //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////
if(menu==1){

//ЗАГРУЖАЕМ ГЛАВНОЕ МЕНЮ
if(FL_start_MENU==true){//ShowVOLUME_menu();ShowINPUT_menu();
Mute=true;audio();//ОТКЛЮЧАЕМ РЕЖИМ ТИШИНЫ (MUTE OFF) 
pointer=triangle;smenu1=0;ShowInput=Input;FL_com1=true;FL_autoret=false;
FL_start_MENU=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБРАБАТЫВАЕМ НАЖАТИЕ НА КНОПКУ ЭНКОДЕРА
ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn!=ENCODER_btn_status){delay(10);ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn==LOW&&ENCODER_btn_status==HIGH){
FL_delay_ENCODER=true;timer_BTN=millis();//ПОДНИМАЕМ ФЛАГ КНОПКИ, ОБНУЛЯЕМ ТАЙМЕР
}ENCODER_btn_status=ENCODER_btn;}

//ЕСЛИ КНОПКА ЭНКОДЕРА БЫЛА НАЖАТА, ОТПУЩЕНА, И ЗАКОНЧИЛАСЬ "КОРОТКАЯ ПАУЗА"
if(FL_delay_ENCODER==true&&ENCODER_btn==HIGH&&millis()-timer_BTN>ShortPress){
//ПЕРЕКЛЮЧАЕМСЯ МЕЖДУ ВЫБОРОМ ВХОДА И УРОВНЕМ ГРОМКОСТИ    
smenu1++;if(smenu1>1){smenu1=0;}FL_com1=true;//FL_autoret=false;
FL_delay_ENCODER=false;}

//УДЕРЖИВАЯ КНОПКУ ЭНКОДЕРА БОЛЕ ЧЕМ "ДОЛГОЕ НАЖАТИЕ" 
//ПЕРЕКЛЮЧАЕМСЯ В РЕЖИМ ИЗМЕНЕНИЯ ЧУСТВИТЕЛЬНОСТИ ВХОДОВ
if(FL_delay_ENCODER==true&&ENCODER_btn_status==LOW&&millis()-timer_BTN>LongPress){
if(smenu1==2){smenu1=1;}
else{smenu1=2;}FL_com1=true;//FL_autoret=false;
FL_delay_ENCODER=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ПЕРЕКЛЮЧАЕМСЯ МЕЖДУ РЕЖИМАМИ КНОПКАМИ С ПУЛЬТА
//ВАРИАНТ 1. КНОПКИ "ВНИЗ" И "ВВЕРХ" ДЕЙСТВУЮТ ОДИНАКОВО (МНЕ ТАК УДОБНО)
if(ir.value==IR_MENU_DOWN||ir.value==IR_MENU_UP){
smenu1++;if(smenu1>2){smenu1=0;}ir.value=0;FL_com1=true;}//FL_autoret=false;
//ВАРИАНТ 2. КНОПКИ ДЕЙСТВУЮТ В СВОИХ НАПРАВЛЕНИЯХ (МНЕ ТАК НЕ УДОБНО, ВЫБИРАЙТЕ САМИ)
//if(ir.value==IR_MENU_DOWN){smenu1++;if(smenu1>2){smenu1=0;}ir.value=0;FL_com1=true;FL_autoret=false;}
//if(ir.value==IR_MENU_UP){smenu1--;if(smenu1<0){smenu1=2;}ir.value=0;FL_com1=true;FL_autoret=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБРАБАТЫВАЕМ КОМАНДЫ С КНОПКИ ЭНКОДЕРА ИЛИ КНОПОК ПУЛЬТА
if(FL_com1==true){
switch(smenu1){

//РЕЖИМ РЕГУЛИРОВКИ ГРОМКОСТИ И ФИЗИЧЕСКОГО ПЕРЕКЛЮЧЕНИЯ ВХОДА (ЕСЛИ ОН БЫЛ ИЗМЕНЁН)
  case 0: trig1=Volume;pointer=triangle;
if(ShowInput!=Input){Input=ShowInput;audio();}//tda.setInput(Input-1);
FL_autoret=false;break;

//РЕЖИМ ВЫБОРА ВХОДА БЕЗ ФАКТИЧЕСКОГО ПЕРЕКЛЮЧЕНИЯ 
  case 1: trig1=Input;InputIDX=Input;pointer=arrow;Autoret_TIMER();break;

//РЕЖИМ РЕГУЛИРОВКИ УСИЛЕНИЯ ВЫБРАННОГО (ИЛИ ОСТАВЛЕННОГО БЕЗ ИЗМЕНЕНИЯ) ВХОДА.
//ЕСЛИ ВХОД БЫЛ ИЗМЕНЁН, ФИЗИЧЕСКИ ПЕРЕКЛЮЧАЕМ ЕГО АУДИОПРОЦЕССОРОМ
  case 2: if(ShowInput!=Input){Input=ShowInput;}//tda.setInput(Input-1);
Gain=SELECT_Input[Input];audio();trig1=Gain;pointer=triangle;Autoret_TIMER();break;}

//ПЕРЕПЕЧАТЫВАЕМ МЕНЮ ПРИ КАЖДОМ ИЗМЕНЕНИИ РЕЖИМА
ShowVOLUME_menu();ShowINPUT_menu();

FL_com1=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ЭНКОДЕРОМ ИЛИ КНОПКАМИ ПУЛЬТА МЕНЯЕМ ПАРАМЕТРЫ РЕЖИМОВ
if(encoderCount!=0){trig1=trig1+encoderCount;encoderCount=0;FL_com2=true;}
if(ir.value==IR_VOL_UP||ir.value==IR_FFFFF&&IRkey==IR_VOL_UP){trig1++;ir.value=0;FL_com2=true;}
if(ir.value==IR_VOL_DOWN||ir.value==IR_FFFFF&&IRkey==IR_VOL_DOWN){trig1--;ir.value=0;FL_com2=true;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБРАБАТЫВАЕМ КОМАНДЫ С ЭНКОДЕРА ИЛИ КНОПОК ПУЛЬТА
if(FL_com2==true){
switch(smenu1){

//МЕНЯЕМ ГРОМКОСТЬ
  case 0: Volume=constrain(trig1,abs(Balance),62);audio();trig1=Volume;
ShowVOLUME_menu();break;//ПЕРЕПЕЧАТЫВАЕМ ФРАГМЕНТ МЕНЮ

//МЕНЯЕМ ПЕРЕМЕННУЮ ОТВЕЧАЮЩУЮ ТОЛЬКО ЗА ОТОБРАЖЕНИЕ ВЫБИРАЕМОГО ВХОДА (БЕЗ ПЕРЕКЛЮЧЕНИЯ)
  case 1: if(trig1>3){trig1=1;}if(trig1<1){trig1=3;}InputIDX=trig1;
timer_AUTORET=millis();ShowINPUT_menu();break;//ПЕРЕПЕЧАТЫВАЕМ ФРАГМЕНТ МЕНЮ

//МЕНЯЕМ ЧУСТВИТЕЛЬНОСТЬ ВЫБРАННОГО ВХОДА
  case 2: Gain=constrain(trig1,0,7);trig1=Gain;SELECT_Input[Input]=Gain;audio();
timer_AUTORET=millis();ShowINPUT_menu();break;}//ПЕРЕПЕЧАТЫВАЕМ ФРАГМЕНТ МЕНЮ

FL_com2=false;}
////////////////////////////////////////////////////////////////////////////////////////////////
}//END MAIN MENU





////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
//                  МЕНЮ НАСТРОЙКИ ТЕМБРОБЛОКА (BASS, TREBLE -14...+14 dB)                    //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////
if(menu==2){

//ПЕЧАТАЕМ МЕНЮ С ТЕКУЩИМИ ПАРАМЕТРАМИ BASS ИЛИ TREBLE
if(FL_start_MENU==true){Show_menu_2();pointer=triangle;FL_start_MENU=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//КНОПКОЙ ЭНКОДЕРА ИЛИ ПУЛЬТА ДУ ВЫБИРАЕМ ЧТО РЕГУЛИРУЕМ ВЫСОКИЕ ИЛИ НИЗКИЕ ЧАСТОТЫ
ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn!=ENCODER_btn_status||ir.value==IR_MENU_DOWN||ir.value==IR_MENU_UP){
delay(10);ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn==LOW&&ENCODER_btn_status==HIGH||ir.value==IR_MENU_DOWN||ir.value==IR_MENU_UP){
smenu2=!smenu2;ir.value=0;Autoret_TIMER();FL_com1=true;
}ENCODER_btn_status=ENCODER_btn;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ПРИСВАИВАЕМ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ ЗНАЧЕНИЕ BASS ИЛИ TREBLE В ЗАВИСИМОСТИ ОТ ТОГО ЧТО ВЫБРАНО
if(FL_com1==true){
switch(smenu2){
  case 0: trig2=Bass;break;
  case 1: trig2=Treble;break;}
Show_menu_2();//ПЕЧАТАЕМ МЕНЮ ТОЛЬКО РАДИ ИЗМЕНИВШЕГО ПОЛОЖЕНИЕ КУРСОРА-ИНДИКАТОРА
FL_com1=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ЭНКОДЕРОМ ИЛИ КНОПКАМИ ПУЛЬТА МЕНЯЕМ ЗНАЧЕНИЕ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ
//if(newPosition!=oldPosition){trig2=trig2+newPosition;Clear_Encoder();Autoret_TIMER();FL_com2=true;}
if(encoderCount!=0){trig2=trig2+encoderCount;encoderCount=0;Autoret_TIMER();FL_com2=true;}
if(ir.value==IR_VOL_UP||ir.value==IR_FFFFF&&IRkey==IR_VOL_UP){trig2++;
ir.value=0;Autoret_TIMER();FL_com2=true;}
if(ir.value==IR_VOL_DOWN||ir.value==IR_FFFFF&&IRkey==IR_VOL_DOWN){trig2--;
ir.value=0;Autoret_TIMER();FL_com2=true;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБРАБАТЫВАЕМ КОМАНДЫ С ЭНКОДЕРА ИЛИ КНОПОК ПУЛЬТА
if(FL_com2==true){
//У BASS И TREBLE ОДИНАКОВЫЕ ГРАНИЧНЫЕ ЗНАЧЕНИЯ, ПОЭТОМУ ДОСТАТОЧНО ОГРАНИЧИТЬ ПРОМЕЖУТОЧНУЮ 
//ПЕРЕМЕННУЮ И НЕ ВЫПОЛНЯТЬ ПРОВЕРКУ ДЛЯ КАЖДОГО ПАРАМЕТРА ТЕМБРОБЛОКА ОТДЕЛЬНО
trig2=constrain(trig2,-7,7);
//ПРИСВАИВАЕМ BASS ИЛИ TREBLE ЗНАЧЕНИЕ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ 
switch(smenu2){
  case 0: Bass=trig2;break;
  case 1: Treble=trig2;break;}
audio();//ОТСЫЛАЕМ ТЕКУЩЕЕ ЗНАЧЕНИЕ BASS ИЛИ TREBLE В АУДИОПРОЦЕССОР
Show_menu_2();//ПЕРЕПЕЧАТЫВАЕМ МЕНЮ С НОВЫМ ЗНАЧЕНИЕМ BASS ИЛИ TREBLE
FL_com2=false;}
}//END TONE CONTROL SETTING MENU
////////////////////////////////////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////////////////////////////////////
//                                        МЕНЮ НАСТРОЙКИ                                      //
//                                    БАЛАНСА МЕЖДУ КАНАЛАМИ                                  //
//                       УВЕЛИЧЕНИЯ ЧУСТВИТЕЛЬНОСТИ ВХОДА (0 ИЛИ +6 dB)                       //
//                 ВРЕМЕНИ ВОЗВРАТА В РЕЖИМ РЕГУЛИРОВКИ ГРОМКОСТИ (5...30 сек.)               //
////////////////////////////////////////////////////////////////////////////////////////////////
if(menu==3){

//ПЕЧАТАЕМ МЕНЮ С ТЕКУЩИМИ ПАРАМЕТРАМИ 
if(FL_start_MENU==true){Show_menu_3();pointer=triangle;FL_start_MENU=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//КНОПКОЙ ЭНКОДЕРА ВЫБИРАЕМ ЧТО РЕГУЛИРУЕМ (БАЛАНС, ЧУСТВИТЕЛЬНОСТЬ, АВТОВОЗВРАТ)
ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn!=ENCODER_btn_status){delay(10);ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn==LOW&&ENCODER_btn_status==HIGH){
smenu3++;if(smenu3>2){smenu3=0;}Autoret_TIMER();FL_com1=true;
}ENCODER_btn_status=ENCODER_btn;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ПЕРЕКЛЮЧАЕМСЯ МЕЖДУ РЕЖИМАМИ КНОПКАМИ С ПУЛЬТА
if(ir.value==IR_MENU_DOWN){smenu3++;if(smenu3>2){smenu3=0;}ir.value=0;Autoret_TIMER();FL_com1=true;}
if(ir.value==IR_MENU_UP){smenu3--;if(smenu3<0){smenu3=2;}ir.value=0;Autoret_TIMER();FL_com1=true;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ПРИСВАИВАЕМ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ ЗНАЧЕНИЯ В ЗАВИСИМОСТИ ОТ ТОГО ЧТО ВЫБРАНО
if(FL_com1==true){
switch(smenu3){
  case 0: trig3=Balance;break;
  case 1: trig3=Buffer;break;
  case 2: trig3=Wait_return;break;}
Show_menu_3();//ПЕЧАТАЕМ МЕНЮ ТОЛЬКО РАДИ ИЗМЕНИВШЕГО ПОЛОЖЕНИЕ КУРСОРА-ИНДИКАТОРА
FL_com1=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ЭНКОДЕРОМ ИЛИ КНОПКАМИ ПУЛЬТА МЕНЯЕМ ЗНАЧЕНИЕ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ
//if(newPosition!=oldPosition){trig3=trig3+newPosition;Clear_Encoder();Autoret_TIMER();FL_com2=true;}
if(encoderCount!=0){trig3=trig3+encoderCount;encoderCount=0;Autoret_TIMER();FL_com2=true;}
if(ir.value==IR_VOL_UP||ir.value==IR_FFFFF&&IRkey==IR_VOL_UP){trig3++;
ir.value=0;Autoret_TIMER();FL_com2=true;}
if(ir.value==IR_VOL_DOWN||ir.value==IR_FFFFF&&IRkey==IR_VOL_DOWN){trig3--;
ir.value=0;Autoret_TIMER();FL_com2=true;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ОБРАБАТЫВАЕМ КОМАНДЫ С ЭНКОДЕРА ИЛИ КНОПОК ПУЛЬТА
if(FL_com2==true){
//ПРИСВАИВАЕМ BALANCE, BUFFER GAIN, WAIT RETURN ЗНАЧЕНИЕ ПРОМЕЖУТОЧНОЙ ПЕРЕМЕННОЙ 
switch(smenu3){
  case 0: Balance=constrain(trig3,-Volume,Volume);
if(Balance<=0){Bal_L=abs(Balance);}if(Balance>=0){Bal_R=Balance;}trig3=Balance;
tda.setVol_R(Volume-Bal_R);tda.setVol_L(Volume-Bal_L);break;
  case 1: Buffer=trig3;if(Buffer>1){Buffer=0;}if(Buffer<0){Buffer=1;}trig3=Buffer;
tda.setSurround(Surround,INgain,Mixing,Buffer);break;//ОТСЫЛАЕМ Buffer В АУДИОПРОЦЕССОР
  case 2: Wait_return=constrain(trig3,5,30);trig3=Wait_return;break;}
Show_menu_3();//ПЕРЕПЕЧАТЫВАЕМ МЕНЮ С НОВЫМ ЗНАЧЕНИЯМИ BALANCE ИЛИ BUFFER GAIN ИЛИ WAIT RETURN
FL_com2=false;}
////////////////////////////////////////////////////////////////////////////////////////////////
}//END MENU BALANCE, BUFFER GAIN, WAIT RETURN


}//END POWER ON






/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                         POWER OFF                                           //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
if(Power==false){

//НАЧАЛЬНЫЕ ПАРАМЕТРЫ В РЕЖИМЕ STANDBY
if(FL_start_POWER==true){
Mute=0;audio();EEPROM_UPDATE();digitalWrite(POWER_RELAY_out,HIGH);
FL_autoret=false;WATCH_menu=true;SET_TIME_DATE_menu=false;FL_start_MENU=true;
FL_start_POWER=false;}
////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////
//                        MENU KEY / КНОПКА НА ПАНЕЛИ (СЛЕДУЮЩЕЕ МЕНЮ)                        //
//               ВЫБОР ТИПА (СТИЛЯ) ОТОБРАЖАЕМЫХ ЧАСОВ ИЛИ ПРИ ДЛИТЕЛЬНОМ НАЖАТИИ             //
//                ПЕРЕКЛЮЧЕНИЕ МЕЖДУ МЕНЮ ЧАСОВ И МЕНЮ УСТАНОВКИ ВРЕМЕНИ И ДАТЫ               //
////////////////////////////////////////////////////////////////////////////////////////////////

MENU_btn=digitalRead(MENU_key);
if(MENU_btn!=MENU_btn_status){delay(30);MENU_btn=digitalRead(MENU_key);
if(MENU_btn==HIGH&&MENU_btn_status==LOW){
FL_delay_MENU=true;timer_BTN=millis();//ПОДНИМАЕМ ФЛАГ КНОПКИ, ОБНУЛЯЕМ ТАЙМЕР
}MENU_btn_status=MENU_btn;}
////////////////////////////////////////////////////////////////////////////////////////////////

//ЕСЛИ КНОПКА МЕНЮ БЫЛА НАЖАТА, ОТПУЩЕНА, И ЗАКОНЧИЛАСЬ "КОРОТКАЯ ПАУЗА"
if(FL_delay_MENU==true&&MENU_btn==LOW&&millis()-timer_BTN>ShortPress){
//ПЕРЕКЛЮЧАЕМСЯ МЕЖДУ ТИПОМ (СТИЛЕМ) ЧАСОВ    
if(WATCH_menu==true){Watch_Type++;if(Watch_Type>2){Watch_Type=0;}
timer_WATCH=millis()+p1000;lastMIN=60;}FL_com1=true;
FL_delay_MENU=false;}
////////////////////////////////////////////////////////////////////////////////////////////////

//УДЕРЖИВАЯ КНОПКУ МЕНЮ БОЛЕ ЧЕМ 1 СЕК.
if(FL_delay_MENU==true&&MENU_btn_status==HIGH&&millis()-timer_BTN>p1000){
//ПЕРЕКЛЮЧАЕМСЯ МЕЖДУ МЕНЮ ВЫБОРА ТИПА (СТИЛЯ) ЧАСОВ И МЕНЮ УСТАНОВКИ ВРЕМЕНИ И ДАТЫ 
SET_TIME_DATE_menu=!SET_TIME_DATE_menu;WATCH_menu=!WATCH_menu;FL_start_MENU=true;
FL_delay_MENU=false;}
////////////////////////////////////////////////////////////////////////////////////////////////


//ПЕРЕКЛЮЧЕНИЕ МЕЖДУ УКАЗАННЫМИ МЕНЮ КНОПКАМИ ПУЛЬТА ДУ
if(ir.value==IR_PREV_MENU||ir.value==IR_NEXT_MENU){
SET_TIME_DATE_menu=!SET_TIME_DATE_menu;WATCH_menu=!WATCH_menu;FL_start_MENU=true;
ir.value=0;}




////////////////////////////////////////////////////////////////////////////////////////////////
//                       МЕНЮ ДЕМОНСТРАЦИИ ОДНОГО ИЗ ТРЁХ ТИПОВ ЧАСОВ                         //
//              ТИП 1. (BIG WATCH) / ТИП 2. (BIG CLOCK) / ТИП 3. (TIME & DATE)                //
////////////////////////////////////////////////////////////////////////////////////////////////
if(WATCH_menu==true){

//УСТАНАВЛИВАЕМ НАЧАЛЬНЫЕ ПАРАМЕТРЫ МЕНЮ  
if(FL_start_MENU==true){
timer_WATCH=millis()+p1000;lastMIN=60;FL_com1=true;
if(LCD_light==0){lcd.noBacklight();}else{lcd.backlight();}//ПОДСВЕТКА ЭКРАНА ON/OFF
FL_start_MENU=false;}

//ЗАГРУЖАЕМ В ПАМЯТЬ LCD1602 СИМВОЛЫ ПСЕВДОГРАФИКИ В ЗАВИСИМОСТИ ОТ ТИПА ЧАСОВ
if(FL_com1==true){
lcd.clear();
switch(Watch_Type){
  case 0: Big_BOLD_numbers();break;
  case 1: Big_CLOCK_numbers();break;}
FL_com1=false;}

//ПЕЧАТАЕМ ВРЕМЯ КАЖДУЮ СЕКУНДУ
if(millis()-timer_WATCH>=p1000){timer_WATCH=millis();
switch(Watch_Type){
  case 0: tmr=rtc.getTime();Show_SECONDS_ONLY();
if(lastMIN!=tmr.min){BIG_watch();lastMIN=tmr.min;}break;
  case 1: tmr=rtc.getTime();Show_SECONDS_TEMPERATURE();
if(lastMIN!=tmr.min){BIG_clock_temp();lastMIN=tmr.min;}break;
  case 2: timedow();datetemp();break;}}
////////////////////////////////////////////////////////////////////////////////////////////////



if(ir.value==IR_MENU_UP){Watch_Type++;if(Watch_Type>2){Watch_Type=0;}
timer_WATCH=millis()+p1000;lastMIN=60;FL_com1=true;ir.value=0;}
if(ir.value==IR_MENU_DOWN){Watch_Type--;if(Watch_Type<0){Watch_Type=2;}
timer_WATCH=millis()+p1000;lastMIN=60;FL_com1=true;ir.value=0;}



////////////////////////////////////////////////////////////////////////////////////////////////
//                        LCD NIGHT BACKLIGHT (STANDBY LIGHT ON/OFF)                          //
//            КНОПКА ПОДСВЕТКИ ЭКРАНА ON/OFF (КНОПКОЙ ЭНКОДЕРА ИЛИ "MUTE" НА ПДУ)             //
////////////////////////////////////////////////////////////////////////////////////////////////
ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn!=ENCODER_btn_status||ir.value==IR_MUTE){
delay(10);ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn==LOW&&ENCODER_btn_status==HIGH||ir.value==IR_MUTE){
LCD_light=!LCD_light;LCD_light?(lcd.backlight()):(lcd.noBacklight());ir.value=0;
}ENCODER_btn_status=ENCODER_btn;}
////////////////////////////////////////////////////////////////////////////////////////////////

}//END MENU ВЫБОРА ТИПА ЧАСОВ






////////////////////////////////////////////////////////////////////////////////////////////////
//                                   SET TIME & DATE MENU                                     //
//                               МЕНЮ УСТАНОВКИ ВРЕМЕНИ И ДАТЫ                                //
////////////////////////////////////////////////////////////////////////////////////////////////
if(SET_TIME_DATE_menu==true){

//УСТАНАВЛИВАЕМ НАЧАЛЬНЫЕ ПАРАМЕТРЫ МЕНЮ  
if(FL_start_MENU==true){
lcd.clear();TimeDate_SAVE=0;FL_com1=true;FL_com2=true;
lcd.backlight();//ОБЯЗАТЕЛЬНАЯ ПОДСВЕТКА ЭКРАНА 
lcd.setCursor(0,0);lcd.print(F("SET: TIME & DATE"));
GET_time();
FL_start_MENU=false;}


ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn!=ENCODER_btn_status){delay(10);ENCODER_btn=digitalRead(ENCODER_key);
if(ENCODER_btn==LOW&&ENCODER_btn_status==HIGH){
smenu4++;if(smenu4>6){smenu4=0;}FL_com1=true;FL_com2=true;LINE1_clear();
}ENCODER_btn_status=ENCODER_btn;}

if(ir.value==IR_MENU_DOWN){
smenu4++;if(smenu4>6){smenu4=0;}FL_com1=true;FL_com2=true;ir.value=0;LINE1_clear();}
if(ir.value==IR_MENU_UP){
smenu4--;if(smenu4<0){smenu4=6;}FL_com1=true;FL_com2=true;ir.value=0;LINE1_clear();}

if(FL_com1==true){
  switch(smenu4){
    case 0: trig4=sethour;break;
    case 1: trig4=setmin;break;
    case 2: trig4=setdate;break;
    case 3: trig4=setmon;break;
    case 4: trig4=setyear;break;
    case 5: trig4=setdow;break;
    case 6: trig4=TimeDate_SAVE;break;}
   FL_com1=false;}
   
if(encoderCount!=0){trig4=trig4+encoderCount;encoderCount=0;FL_com2=true;}
if(ir.value==IR_VOL_UP||ir.value==IR_FFFFF&&IRkey==IR_VOL_UP){
trig4++;ir.value=0;FL_com2=true;}
if(ir.value==IR_VOL_DOWN||ir.value==IR_FFFFF&&IRkey==IR_VOL_DOWN){
trig4--;ir.value=0;FL_com2=true;}

if(FL_com2==true){
lcd.setCursor(0,1);
switch(smenu4){
  case 0:sethour=trig4;if(sethour>23){sethour=0;}if(sethour<0){sethour=23;}
lcd.print(F("HOUR "));lcd.print(sethour);lcd.print(' ');FL_com1=true;break;
  case 1:setmin=trig4;if(setmin>59){setmin=0;}if(setmin<0){setmin=59;}
lcd.print(F("MINUTE "));lcd.print(setmin);lcd.print(' ');FL_com1=true;break;
  case 2:setdate=trig4;if(setdate>31){setdate=1;}if(setdate<1){setdate=31;}
lcd.print(F("DAY MONTH "));lcd.print(setdate);lcd.print(' ');FL_com1=true;break;
  case 3:setmon=trig4;if(setmon>12){setmon=1;}if(setmon<1){setmon=12;}
lcd.print(F("MONTH "));lcd.print(setmon);lcd.print(' ');FL_com1=true;break;
  case 4:setyear=trig4;setyear=constrain(setyear,23,50);
lcd.print(F("YEAR 20"));lcd.print(setyear);FL_com1=true;break;
  case 5:setdow=trig4;if(setdow>7){setdow=1;}if(setdow<1){setdow=7;}
lcd.print(F("DAY WEEK "));lcd.print(setdow);FL_com1=true;break;
  case 6:TimeDate_SAVE=trig4;lcd.print(F("TAP +/- TO SAVE!"));FL_com1=true;break;}
if(TimeDate_SAVE!=0){SET_time();smenu4=0;TimeDate_SAVE=0;
lcd.setCursor(0,0);lcd.print(F("SETTINGS UPDATED"));LINE1_clear();delay(2000);
WATCH_menu=true;SET_TIME_DATE_menu=false;FL_start_MENU=true;}
  FL_com2=false;}
}//END SET TIME & DATE MENU


}//END STANDBY (POWER OFF)


}//END LOOP


/////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                             //
//                                            END                                              //
//                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
