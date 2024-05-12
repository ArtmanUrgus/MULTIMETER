#Эмулятор многоканального мультиметра

Эмулятор представляет из себя клент-серверное приложение на базе сокета межпроцессного взаимодействия, реализованного для систем Unix/Linux.
Согласно ТЗ, приложение эмулирует сигналы и состояния многоканального мультиметра на стороне сервера, и возвращает данные по запросу клиента.

##Сервер

Сервер запускает работу эмулятора, который открывает 12 виртуальных измерительных каналов мультиметра. Каждый канал выполняет работу в отдельно выделенном потоке. 
Сервер открывает сокет соединения и ожидает подключения клиентов. Для обработки сообщений каждого клиента выделяется отдельный поток.
Сервер выполняется без поддержки GUI  и отображает сообщения о состоянии в консоли запуска приложения. 

##Клиент

После запуска приложение-клиент проверяет соединение с сервером. Если соединение не установлено, клиент ожидает запуск сервера, и при открытии сокета сервером, автоматически устанавливает соединение. По умолчанию клиент инициализируется двенадцатью виртуальными измерительными каналами, каждый из которых устанавливает соединение с сервером и выполняется в отдельном потоке. 

##Пользовательский интерфейс

Клиентское приложение реализует пользовательский интерфейс с использованием QML. Управление каналами мультиметра осуществляется интерактивно, стандартными устройствами ввода, клавиатурой и мышью. Окно клиентского приложения представляет из себя список контролеров каналов, каждый из которых содержит: номер канала, кнопку активации измерения, слайдер переключения диапазона значений канала, поле отображения текущего значения измерения, поле отображения текущего состояния канала. 

###Контролер канала

Каждый контролер подсвечивается цветом, позволяющим быстро определить текущее состояние канала. 
- кнопка измерения: отправляет серверу сигнал с запросом активации измерения для канала принадлежащего контроллеру. Кнопка переключается в активное состояние только после получения подтверждения о начале измерения от сервера
- слайдер диапазона: доступен только тогда, когда виртуальный канал находится в состоянии готовности (Idle) . В других состояниях канала изменение диапазона измерения не доступно. При нажатии на слайдер диапазона, ползунок смещается, подсвечивая границы диапазона измерения. Границы диапазона представлены максимальным и минимальным значением диапазона, например 1. - 10^-3. 
- поле значения: отображает значение измерения соответствующее выбранному диапазону, получаемому от сервера и подсвечивается, когда канал получает статус измерения (Measure). При нажатии кнопкой мыши на поле значения, под контроллером канала разворачивается вид гистограммы, отображающий дискретные значения измерений. 
- поле статуса: отображает иконку, соответствующую текущему статусу канала на сервере. 

##Реализация 

Для реализации приложений использовался стандарт языка программирования С++17. Сервер реализован только с использованием стандартных библиотек без использования фреймворков.  Приложение клиента реализовано с помощью фреймворка Qt с использованием QML. 

##Архитектура и взаимодействие



###Дополнения к ТЗ

1. Для правильной работы клиента добавлен статус клиента Disconnected, который отображается в статусе канала, если соединение канала с сервером отсутствует.
2. Для правильной работы сервера добавлен сигнал exit, отправляемый клиентом, для правильного завершения потока, выделенного клиенту. 
3. Количество каналов по умолчанию установлено в 12. 
4. Для представления данных измерения, получаемых из канала, реализуется виджет гистограммы.

##Сборка и инсталляция приложения  

Для сборки приложения необходимы:  
Компилятор gcc  
Cmake  
Qt 5.15  

Процесс сборки:
загрузить проект на локальный хост, перейти в каталог проекта и выполнить в консоли следующие команды:

'''
mkdir build  
cd build  
cmake  
make  
'''

После окончания сборки, исполняемые файлы приложений должны находится в каталоге bin проекта.

##Расширения

Планируется расширить проект следующими функциями:

1. Добавить диалоговое окно настройки цветов пользовательского интерфейса. 
2. Добавить возможность сохранения виртуальнах каналов добавленных пользователем
3. Добавить возможность сохранять значения измерений канала в сессии. 
4. Добавить возможность сопоставления измерений между различными каналами или сессиями одного канала с одинаковым диапазоном. 
5. Добавить поддержку выбора режима и отображения единиц измерения в каналах.
6. Добавить возможность сохранения данных измерения в файлы или БД. 

