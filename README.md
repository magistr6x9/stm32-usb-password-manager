# STM32 USB Password Manager

* [Описание](#description)
* [История создания](#history)
* [Сборка и установка прошивки](#building)
* [Инструкция по работе с устройством](#manual)
* [Пример работы с устройством](#device_usage)
* [Пример работы с программой set_new_password.py](#program_usage)
* [Лицензия](#license)

<a name="description"></a>
## Описание

Устройство представляет из себя эмулятор USB-HID клавиатуры и позволяет автоматически набирать пароли, сохранённые в его энергонезависимой памяти, при нажатии на основной клавиатуре управляющей комбинации клавиш.

<a name="history"></a>
## История создания

Проект вдохновлён идеей [DIY USB Password Generator](https://codeandlife.com/2012/03/03/diy-usb-password-generator/) от [Joonas Pihlajamaa](http://joonaspihlajamaa.com/). Первая реализация была выполнена на микроконтроллере _ATTiny85_ на основе проекта [LittleWire](https://littlewire.github.io/) с добавлением к нему кнопки и светодиода. Затем код был портирован на процессоры _STM32_ с аппаратным _USB_ и реализован в качестве прошивки для плат [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html) и Blue Pill.

<a name="building"></a>
## Сборка и установка прошивки

При разработке использовался инструмент [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html), редактор [Visual Studio Code](https://code.visualstudio.com) и операционная система [Linux Mint](https://linuxmint.com/).

В [Visual Studio Code](https://code.visualstudio.com) устанавливались плагины _C/C++_, _Cortex-Debug_, _stm32-for-vscode_, _Markdown Preview Github Styling_ и _Russian Language Pack_.

### Установка необходимых пакетов

`# apt-get install gcc-arm-none-eabi openocd`

### Компиляция

Перед запуском процесса сборки необходимо убедиться, что содержимое файла _board.h_ в каталоге _Inc_ соответствует целевой плате.

* Сборка для [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html):

      #define _MAPLE_MINI_BOARD
      // #define _BLUE_PILL_BOARD

* Сборка для Blue Pill:

      // #define _MAPLE_MINI_BOARD
      #define _BLUE_PILL_BOARD

`# make`

### Установка прошивки

Для китайского программатора в форм-факторе флешки St-Link v2:

`openocd -f stm32f1x_stlinkv2_dongle.cfg -c 'program build/stm32-usb-password-manager.elf verify reset exit'`

<a name="manual"></a>
## Инструкция по работе с устройством

### Общая информация

Устройство отслеживает нажатия на основной клавиатуре клавиш _CapsLock_, _ScrollLock_ и _NumLock_ или, если быть точнее, отслеживает состояние соответствующих этим клавишам светодиодных индикаторов. Если между последовательными нажатиями указанных клавиш проходит менее одной секунды, то нажатия считаются командой, которая будет обработана интерпретатором команд устройства и выполнена при совпадении введённой команды с одной из заданных в коде управляющих последовательностей.

### Первое подключение

После подключения к компьютеру на плате начинает мигать светодиод. Это - режим ввода пин-кода, где пин-код используется в качестве "защиты от дурака". Пин-код есть комбинация из восьми последовательных нажатий клавиш CapsLock, ScrollLock и NumLock.

Примеры пин-кода:

* _CapsLock_-_CapsLock_-_NumLock_-_NumLock_-_CapsLock_-_CapsLock_-_NumLock_-_NumLock_
* _CapsLock_-_ScrollLock_-_NumLock_-_ScrollLock_-_NumLock_-_NumLock_-_CapsLock_-_CapsLock_

__Важно!__ Пин-код не запоминается в памяти устройства и используется только для шифровки и расшифровки сохранённых паролей. То есть для правильной работы пин-код, введённый в сеансе, когда пароль сохраняется в памяти устройства, и пин-код в сеансе, когда пароль выводится, должны совпадать.

О том, что пин-код принят устройство просигнализирует быстрым двойным миганием светодиода, после чего светодиод погаснет. Теперь устройство готово к приёму команд.

### Генерация нового пароля

* Двойное повторение последовательности _CapsLock-ScrollLock-NumLock_ - генерация нового 8-символьного пароля.

* Двойное повторение последовательности _CapsLock-NumLock-ScrollLock_ - генерация нового 12-символьного пароля.

* Двойное повторение последовательности _ScrollLock-CapsLock-NumLock_ - генерация нового 15-символьного пароля.

Команду можно повторять сколько угодно раз до желаемого результата.

### Просмотр последнего сгенерированного пароля

Двойное повторение последовательности _ScrollLock-NumLock_ - набирает последний сгенерированный пароль для просмотра или проверки.

### Запись последнего сгенерированного пароля в память устройства

Двойное повторение последовательности _ScrollLock-NumLock_ + последующее двойное нажатие на одну из клавиш _CapsLock_, _NumLock_, _ScrollLock_ записывает последний сгенерированный пароль в ячейку 0,1 или 2 соответственно. Также пароль запоминается во внутренней истории паролей.

__Важно!__ После ввода команды для подтверждения необходимо в течение 3 секунд при мигающем светодиоде нажать кнопку USER на [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html) или замкнуть перемычкой контакты SWCLC и GND на Blue Pill.

### Набор пароля, сохранённого в памяти устройства

Четырехкратное нажатие на одну из клавиш _CapsLock_, _NumLock_ или _ScrollLock_ "набирает" сответственно 0й, 1й, 2й пароли. Пароли могут быть записаны в EEPROM из программы set_new_password.py или при подаче последовательно команды генерации нового пароля и команды сохранения пароля в памяти устройства.

### Работа с историей сохранённых паролей

Двойное повторение последовательности _CapsLock-ScrollLock-NumLock-NumLock_ выводит историю сохраненных паролей. Команду удобнее всего выполнять в каком-нибудь текстовом редакторе, например, в Блокноте.

Двойное повторение последовательности _CapsLock-NumLock-ScrollLock-ScrollLock_ стирает историю паролей.

__Важно!__ После ввода каждой из команд для подтверждения необходимо в течение 3 секунд при мигающем светодиоде нажать кнопку USER на [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html) или замкнуть перемычкой контакты SWCLC и GND на Blue Pill.

### Сброс пин-кода

Нажать и удерживать в течение 3 секунд кнопку USER на [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html) или перемычку на контактах SWCLC и GND на Blue Pill. Должен замигать светодиод после чего устройство переходит в режим приёма пин-кода.

<a name="device_usage"></a>
## Пример работы с устройством

1. Подключить устройство к порту USB и дождаться мигания светодиода
1. Убедиться, что нажатие на основной клавиатуре каждой из клавиш _CapsLock_, _ScrollLock_ и _NumLock_ вызывает изменение состояния на клавиатуре соответствующего светодиода. Возможно в Linux вам потребуется выполнить команду, чтобы включить ScrollLock:

       # xmodmap -e 'add mod3 = Scroll_Lock'

1. Нажать последовательно с паузами не более одной секунды последовательность _CapsLock_-_CapsLock_-_NumLock_-_NumLock_-_CapsLock_-_CapsLock_-_NumLock_-_NumLock_. Светодиод должен мигнуть два раза и погаснуть.
Пин-код принят
1. Открыть любой текстовый редактор или перейти в режим командной строки, открыв приложение терминала
1. Нажать последовательно _CapsLock-ScrollLock-NumLock-CapsLock-ScrollLock-NumLock_. Устройство выведет на экран "Generating...done." Пароль длиной 8 символов сгенерирован
1. Нажать последовательно _ScrollLock-NumLock-ScrollLock-NumLock_.
Устройство выведет на экран новый сгенерированный пароль
1. Можно повторять шаги 5 и 6 сколь угодно раз, пока пароль не будет удовлетворять всем требованиям
1. Нажать последовательно _ScrollLock-NumLock-ScrollLock-NumLock-CapsLock-CapsLock_, на устройстве замигает светодиод. Нажать кнопку USER на [Maple Mini](http://docs.leaflabs.com/static.leaflabs.com/pub/leaflabs/maple-docs/latest/hardware/maple-mini.html) или замкнуть перемычкой контакты SWCLC и GND на Blue Pill. Устройство выведет на экран "Stored in EEPROM." Пароль сохранён
1. Нажмите 4 раза клавишу _CapsLock_. Устройство должно вывести на экран сохранённый пароль
1. Be happy :)

<a name="program_usage"></a>
## Пример использования программы set_new_password.py

1. Установить интерпретатор Python 3.x (Как правило уже установлен в каждом современном Linux дистрибутиве)

1. Установить пакет pyusb-1.1.0

        # pip3 install pyusb==1.1.0

1. Выполнить шаги 1, 2, 3 из предыдущего раздела
1. Запустить программу:

        # sudo /usr/bin/python3 ./set_new_passsword.py

    Программа должна вывести данные подключенного устройства:

        == Target device ==

          Manufacturer : xx@xx.xx
          Product      : STM32 USB Password Manager
          Serial       : XXXXXXXXXXXX

1. Ввести желаемый пароль и нажать _Enter_.
1. Нажать последовательно _ScrollLock-NumLock-ScrollLock-NumLock_.
Устройство выведет на экран только что набранный пароль
1. Выполнить шаги 8 и 9 из предыдущего раздела
1. Be happy!

<a name="license"></a>
## Лицензия
Сведения о лицензии можно найти в файле [LICENSE](./LICENSE).