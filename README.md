# Эмулятор многоканального мультиметра

Эмулятор представляет из себя клент-серверное приложение на базе сокета межпроцессного взаимодействия, реализованного для систем Unix/Linux.
Согласно ТЗ, приложение эмулирует сигналы и состояния многоканального мультиметра на стороне сервера, и возвращает данные по запросу клиента.

## Архитектура и взаимодействие

[![architectur.png](https://i.postimg.cc/sxjgnMPp/architectur.png)](https://postimg.cc/2btYy8K6)

## Сервер

Сервер запускает работу эмулятора, который открывает 12 виртуальных измерительных каналов мультиметра. Каждый канал выполняет работу в отдельно выделенном потоке. 
Сервер открывает сокет соединения и ожидает подключения клиентов. Для обработки сообщений каждого клиента выделяется отдельный поток.
Сервер выполняется без поддержки GUI  и отображает сообщения о состоянии в консоли запуска приложения. 

## Клиент

После запуска приложение-клиент проверяет соединение с сервером. Если соединение не установлено, клиент ожидает запуск сервера, и при открытии сокета сервером, автоматически устанавливает соединение. По умолчанию клиент инициализируется двенадцатью виртуальными измерительными каналами, каждый из которых устанавливает соединение с сервером и выполняется в отдельном потоке. 

## Пользовательский интерфейс

Клиентское приложение реализует пользовательский интерфейс с использованием QML. Управление каналами мультиметра осуществляется интерактивно, стандартными устройствами ввода (в текущей версии только мышью, ввод с клавиатуры будет реализован в следующем расширении). Окно клиентского приложения представляет из себя список контролеров каналов, каждый из которых содержит: номер канала, кнопку активации измерения, слайдер переключения диапазона значений канала, поле отображения текущего значения измерения, поле отображения текущего состояния канала. 

**Рис. 1 Ожидает соединения**  

[![client-disconn.png](https://i.postimg.cc/FHY5L0Cg/client-disconn.png)](https://postimg.cc/zyYcmHrV)

**Рис. 2 Все каналы готовы к работе**  

[![client-idle.png](https://i.postimg.cc/tTtLjk7y/client-idle.png)](https://postimg.cc/N9FN811P)

**Рис. 3 Каналы с активированным процессом измерения и получения данных**  

[![client-measure.png](https://i.postimg.cc/fTg1DJW3/client-measure.png)](https://postimg.cc/D8LxcfXF)

**Рис. 4 Каналы в состоянии ошибки или занятости**  

[![client-on-Data2.png](https://i.postimg.cc/VLSD9416/client-on-Data2.png)](https://postimg.cc/qzTs4sNV)

### Контролер канала

Каждый контролер подсвечивается цветом, позволяющим быстро определить текущее состояние канала. 
- кнопка измерения: отправляет серверу сигнал с запросом активации измерения для канала принадлежащего контроллеру. Кнопка переключается в активное состояние только после получения подтверждения о начале измерения от сервера
- слайдер диапазона: доступен только тогда, когда виртуальный канал находится в состоянии готовности (Idle) . В других состояниях канала изменение диапазона измерения не доступно. При нажатии на слайдер диапазона, ползунок смещается, подсвечивая границы диапазона измерения. Границы диапазона представлены максимальным и минимальным значением диапазона, например 1. - 10^-3. 
- поле значения: отображает значение измерения соответствующее выбранному диапазону, получаемому от сервера и подсвечивается, когда канал получает статус измерения (Measure). 
- поле статуса: отображает иконку, соответствующую текущему статусу канала на сервере. 

## Реализация 

Для реализации приложений использовался стандарт языка С++17. Сервер реализован только с использованием стандартных библиотек без использования фреймворков.  Приложение клиента реализовано с помощью фреймворка Qt с использованием QML. 

### Дополнения к ТЗ

1. Для правильной работы клиента добавлен статус на стороне клиента Disconnected, который отображается в статусе канала, если соединение канала с сервером отсутствует.
2. Количество каналов по умолчанию установлено в 12. 

## Сборка и инсталляция приложения  

Для сборки приложения необходимы:  
Компилятор gcc 11.4  
Cmake 3.15   
Qt 5.15  

### Процесс сборки:
загрузить проект на локальный хост, перейти в каталог проекта и выполнить в консоли следующие команды:

```
mkdir build  
cd build  
cmake ../
cmake --build ./
```

После окончания сборки, исполняемые файлы приложений должны находится в каталоге cборки ./build/bin.

## Расширения

Планируется расширить проект следующими функциями:

1. Добавить диалоговое окно настройки цветов пользовательского интерфейса. 
2. Добавить возможность сохранения виртуальнах каналов добавленных пользователем
3. Добавить возможность сохранять значения измерений канала в сессии. 
4. Добавить возможность сопоставления измерений между различными каналами или сессиями одного канала с одинаковым диапазоном. 
5. Добавить поддержку выбора режима и отображения единиц измерения в каналах.
6. Добавить возможность сохранения данных измерения в файлы или БД.
7. Для представления данных измерения, получаемых из канала, реализовать виджет гистограммы. При нажатии кнопкой мыши на поле значения, под контроллером канала должен разворачиваться вид гистограммы, отображающий дискретные значения измерений. 

**Рис. 5 Возможный вариант реализации**  

[![Multimetr-2.jpg](https://i.postimg.cc/054CNmmq/Multimetr-2.jpg)](https://postimg.cc/t7h6Ss2r)

