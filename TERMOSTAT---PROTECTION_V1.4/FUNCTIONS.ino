////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ФУНКЦИИ / FUNCTIONS                                   //
////////////////////////////////////////////////////////////////////////////////////////////////

//ФУНКЦИЯ ОПРОСА ЕДИНСТВЕННОГО ДАТЧИКА ТЕМПЕРАТУЫ DS18B20 ПОДКЛЮЧЕННОГО ПО ТРЁХПРОВОДНОЙ СХЕМЕ
void TEMPERATURA(){ 

uint8_t data[2];//byte массив для значений температуры 
ds.reset();       //Инициализация совместно со сбросом шины данных
ds.write(0xCC);   //Сброс поиска датчика по адресу или ds.select(addr);
//ds.write(0x7F);//Точность в градусах 0,5=1F; 0,25=3F; 0,125=5F; 0,0625=7F (7F по умолчанию)
//ds.write(0x44,0);//0x44 - Запрос на измерение температуры с переносом данных в память
//0 - если у DS18B20 обычное питание (3 провода, по умолчанию), 1 - если паразитное (2 провода)
ds.write(0x44);   //Запрос на измерение и рассчёт температуры
delay(100);       //Обязательная пауза для расчёта температуры 
//При обычном питании 100 ms, при паразитном 750 ms 
//Не имеет смысла использовать millis() так как ничем другим МК не занимается
ds.reset();       //Обязательная повторная инициализация совместно со сбросом шины данных
ds.write(0xCC);   //ПОВТОРНЫЙ сброс поиска датчика по адресу или ds.select(addr);
ds.write(0xBE);   //Запись данных в память датчика
//for(uint8_t i=0;i<9;i++){data[i]=ds.read();}//Можно прочитать все 9 байт 
//и проверить CRC (он 8-ой), но достаточно только двух первых где содержится температура
data[0]=ds.read();//Чтение из памяти датчика byte low 
data[1]=ds.read();//Чтение из памяти датчика byte high 

//Складываем 2 значения массива типа "byte" и получаем температуру типа "int"
int16_t temp=((data[1]<<8)|data[0]);//Сумма двух первых byte

//Переводим int во float и делим на 16 в формате float (16.0), что равно умножению на 0.0625
//Это соответствует разрешающей способности датчика 12 bit (по умолчанию)
float celsius=temp/16.0;//Температура в градусах Цельсия с точностью до шести сотых (0,06)

//Итог работы функции - значение температуры для глобальной переменной Input
Input=round(celsius*10)/10.0;//Округляем с точностью до десятых (-55.0 ... +125.0)

//int Temperature=(Input*10);//Переводим температуру в целое число

//Для контроля работоспособности датчика выводим значения в монитор серийного порта
#ifdef LOG_ENABLE_TEMP
//Serial.print(F("Сумма двух первых byte: "));Serial.println(data[1]*256+data[0]);
Serial.print(F("Температура НЕПОСРЕДСТВЕННО С ДАТЧИКА: "));Serial.println(temp);
Serial.print(F("Температура С ТОЧНОСТЬЮ ДО СОТЫХ: "));Serial.println(celsius);
Serial.print(F("Температура ОКРУГЛЁННАЯ ДО ДЕСЯТЫХ: "));Serial.println(Input,1);
//Serial.print(F("Температура КАК ЦЕЛОЕ ЧИСЛО: "));Serial.println(Input_TEMP);
Serial.println();Serial.println();
#endif

}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////




//ФУНКЦИЯ ВЫБОРА ЯРКОСТИ ЭКРАНА В ЗАВИСИМОСТИ ОТ ОСВЕЩЁННОСТИ И ЗАДАННОГО УРОВНЯ
void LED_Brightness(){
int16_t Fotorezist=analogRead(LDR_SENSOR_IN_PIN);

//Загрубляем замеры АЦП чтоб яркость экрана не скакала при граничных состояниях
if(abs(Fotorezist_OLD-Fotorezist)>25){Fotorezist_OLD=Fotorezist;
//Если осещённость изменилась более чем на 30 единиц
switch (Fotorezist){
  case    0 ... 200:  LED_Intens=0; break;
  case  201 ... 400:  LED_Intens=1; break;
  case  401 ... 600:  LED_Intens=2; break;
  case  601 ... 800:  LED_Intens=3; break;
  case  801 ... 1024: LED_Intens=5; break;
//case  501 ... 600:  LED_Intens=5; break;
//case  601 ... 700:  LED_Intens=7; break;
//case  701 ... 800:  LED_Intens=9; break;
//case  801 ... 900:  LED_Intens=12;break;
//case  901 ... 1024: LED_Intens=15;break;
  }//Сопоставляем уровень с АЦП к уровню освещённости в пределах регулировок экрана
}//Текущий уровень освещённости получен

//Если уровень освещённости изменился, меняем яркость экрана
if(LED_Intens!=LED_Intens_OLD){LED_Intens_OLD=LED_Intens;
LC.setIntensity(0,LED_Intens);//УСТАНАВЛИВАЕМ ИНТЕНСИВНОСТЬ СВЕЧЕНИЯ ЭКРАНА (0 ... 15)
}

#ifdef LOG_ENABLE_LDR
//В монитор серийного порта выводим значение АЦП с резистивного делителя (0...1024)
Serial.print(F("ТЕКУЩЕЕ ЗНАЧЕНИЕ ФОТОРЕЗИСТОРА: "));Serial.println(Fotorezist);
Serial.print(F("ПРЕДЫДУЩЕЕ ЗНАЧЕНИЕ ФОТОРЕЗИСТОРА: "));Serial.println(Fotorezist_OLD);
//Итоговую яркость экрана (0...15)
Serial.print(F("УРОВЕНЬ ЯРКОСТИ ЭКРАНА: "));Serial.println(LED_Intens);
Serial.println();Serial.println();
#endif

}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////





//ПЕЧАТАЕМ ЗНАЧЕНИЯ В ОСНОВНОМ МЕНЮ setDigit setChar setRow
void Show_Menu_TERMOSTAT(){
//Подготавливаем данные для печати
int Input_TEMP=Input*10;//Переводим температуру в целое число
ar1=Input_TEMP/100%10;if(ar1==0){ar1=' ';}
ar2=Input_TEMP/10%10;
ar3=Input_TEMP%10;
ar4=' ';
ar5=' ';
SET_Temp=SET_Temp+Vol;//Заданную температуру изменяем на переменную кнопок
Vol=0;//Обнуляем переменную кнопок
if(SET_Temp<180){SET_Temp=180;}if(SET_Temp>300){SET_Temp=300;}
ar6=SET_Temp/100%10;if(ar6==0){ar6=' ';}
ar7=SET_Temp/10%10;
ar8=SET_Temp%10;
//ВЫВОДИМ ЗНАЧЕНИЯ НА ЭКРАН
  LC.setChar(0,7,ar1,false);
  LC.setChar(0,6,ar2,true);
  LC.setChar(0,5,ar3,false);
  LC.setChar(0,4,ar4,false);
  LC.setChar(0,3,ar5,false);
  LC.setChar(0,2,ar6,false);
  LC.setChar(0,1,ar7,true);
  LC.setChar(0,0,ar8,false);
}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////




//ПЕЧАТАЕМ ЗНАЧЕНИЯ ТЕМПЕРАТУРЫ В РЕЖИМЕ "POWER OFF" 
void Show_Menu_STANDBY(){
int Temperature=(Input*10);//Переводим температуру в целое число
//Обрезаем диапазон допустимых температур
if(Temperature>990){Temperature=990;}if(Temperature<-990){Temperature=-990;}

int Input_TEMP=abs(Temperature);//Избавляемся от знака 
ar2=Input_TEMP/100%10;//Первый разряд градусов
ar3=Input_TEMP/10%10;//Второй разряд градусов
ar4=Input_TEMP%10;//Десятые доли градусов

//Манипулируем символом "-" и первым разрядом градусов для печати температуры
switch (Temperature){
  case 100 ... 990: //Для температуры в диапазоне от 99,0 до 10,0
LC.setRow(0,7,B00000000);//Символ " "
LC.setChar(0,6,ar2,false);//Первый разряд градусов
  break;
  case 0 ... 99: //Для температуры в диапазоне от 9,9 до 0,1
LC.setRow(0,7,B00000000);//Символ " "
LC.setRow(0,6,B00000000);//Символ " "
  break;
  case -99 ... -1: //Для температуры в диапазоне от -0,1 до -9,9
LC.setRow(0,7,B00000000);//Символ " "
LC.setRow(0,6,B00000001);//Символ "-"
  break;
  case -990 ... -100: //Для температуры в диапазоне от -10,0 до -99,0
LC.setRow(0,7,B00000001);//Символ "-"
LC.setChar(0,6,ar2,false);//Первый разряд градусов
  break;
}//Конец манипуляций
//При любом значении температуры печатаем:
 LC.setChar(0,5,ar3,true);//Второй разряд градусов с десятичной точкой
 LC.setChar(0,4,ar4,false);//Десятые доли градусов
  LC.setRow(0,2,B01100011);//Спецсимвол градуса
  LC.setRow(0,1,B01001110);//Символ "С"
}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////




void Show_Menu_TERMOSTAT_PAUSE(){
LC.setRow(0,7,B01011011);//Символ "S"
LC.setRow(0,6,B00001111);//Символ "t"
LC.setRow(0,5,B01111110);//Символ "O"
LC.setRow(0,4,B01100111);//Символ "P"
//LC.setRow(0,3,B00000000);//Пустое поле
ar1=Secunds/100%10;if(Secunds<100){ar1=' ';}//Первый разряд СЕКУНД
ar2=Secunds/10%10;if(Secunds<10){ar2=' ';}//Второй разряд СЕКУНД
ar3=Secunds%10;//Трети разряд СЕКУНД
//ВЫВОДИМ НА ЭКРАН ОСТАВШЕЕСЯ КОЛИЧЕСТВО
LC.setChar(0,2,ar1,false);//Сотен секунд
LC.setChar(0,1,ar2,false);//Десятков секунд
LC.setChar(0,0,ar3,false);//Единиц секунд
}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////




void Show_Menu_SET_DELAY_PROTECTION(){
SET_Pause=SET_Pause+Vol;//Заданную ПАУЗУ изменяем на переменную кнопок
Vol=0;//Обнуляем переменную кнопок
if(SET_Pause>250){SET_Pause=10;}
if(SET_Pause<10){SET_Pause=250;}
ar1=SET_Pause/100%10;if(ar1==0){ar1=' ';}//Первый разряд СЕКУНД
ar2=SET_Pause/10%10;//Второй разряд СЕКУНД
ar3=SET_Pause%10;//Трети разряд СЕКУНД
//ВЫВОДИМ НА ЭКРАН
LC.setChar(0,7,ar1,false);//Сотен секунд
LC.setChar(0,6,ar2,false);//Десятков секунд
LC.setChar(0,5,ar3,false);//Единиц секунд
 LC.setRow(0,4,B10000000);//Пустое поле С ТОЧКОЙ
 LC.setRow(0,3,B10000000);
 LC.setRow(0,2,B10000000);
 LC.setRow(0,1,B10000000);
LC.setChar(0,0,Return,false);//КОЛИЧЕСТВО СЕКУНД ДО АВТОВЫХОДА ИЗ МЕНЮ
}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////




//ПЕРЕЗАПИСЫВАЕМ ЗНАЧЕНИЯ В EEPROM, НО ТОЛЬКО ЕСЛИ ОНИ ИЗМЕНИЛИСЬ
//(EEPROM.update / EEPROM.write) 
void EEPROM_UPDATE(){
if(SET_Temp!=SET_TempMOD){
temp00=highByte(SET_Temp);EEPROM.write(4,temp00);//ПИШЕМ СТАРШИЙ БАЙТ ПЕРЕМЕННОЙ SET_Temp
temp01=lowByte(SET_Temp);EEPROM.write(5,temp01);//ПИШЕМ МЛАДШИЙ БАЙТ ПЕРЕМЕННОЙ SET_Temp
SET_TempMOD=SET_Temp;}//ОБНОВЛЯЕМ ЗНАЧЕНИЕ ВРЕМЕННОЙ ПЕРЕМЕННОЙ
if(SET_Termostat!=SET_TermostatMOD){EEPROM.write(1,SET_Termostat);SET_TermostatMOD=SET_Termostat;}
if(SET_Pause!=SET_PauseMOD){EEPROM.write(2,SET_Pause);SET_PauseMOD=SET_Pause;}
if(SET_Protection!=SET_ProtectionMOD){EEPROM.write(3,SET_Protection);
SET_ProtectionMOD=SET_Protection;}
//if(LED_Show!=LED_ShowMOD){EEPROM.write(5,LED_Show);LED_ShowMOD=LED_Show;}
}//////////////////////////////////////////END FUNCTION//////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////////////////////////////
//                                             END                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////
