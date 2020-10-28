# STM32 USB Password Manager

* [Описание](#description)
* [История создания](#history)
* [Сборка и установка прошивки](#building)
* [Инструкция по работе с устройством (wiki)](https://github.com/magistr6x9/stm32-usb-password-manager/wiki/%D0%98%D0%BD%D1%81%D1%82%D1%80%D1%83%D0%BA%D1%86%D0%B8%D1%8F-%D0%BF%D0%BE-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%B5-%D1%81-%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2%D0%BE%D0%BC)
* [Пример работы с устройством (wiki)](https://github.com/magistr6x9/stm32-usb-password-manager/wiki/%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80-%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D1%8B-%D1%81-%D1%83%D1%81%D1%82%D1%80%D0%BE%D0%B9%D1%81%D1%82%D0%B2%D0%BE%D0%BC)
* [Пример использования программы set_new_password.py (wiki)](https://github.com/magistr6x9/stm32-usb-password-manager/wiki/%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80-%D0%B8%D1%81%D0%BF%D0%BE%D0%BB%D1%8C%D0%B7%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D1%8F-%D0%BF%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D1%8B-set_new_password.py)
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

<a name="license"></a>
## Лицензия
Сведения о лицензии можно найти в файле [LICENSE](./LICENSE).
