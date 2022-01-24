## mqtt-accelero

Тестовый проект для отладочной платы __stm32f4discovery__, который можно использовать как основу для будующих проектов.
![console](https://github.com/zeProdigy/mqtt-accelero/blob/main/screenshot.png)

### Основной функционал
Плата подключается по usb-ecm (поддерживается только linux'овый драйвер usb-ecm) к хосту, на котором запущен mqtt-брокер. Плата транслирует по mqtt данные с акселерометра, которые может забрать и использовать другой mqtt-клиент.

Тестовый клиент: https://github.com/zeProdigy/mqtt-frontend. Забирает данные и отрисовывает в браузере 3d кубик, которым можно управлять, вращая отладочную плату.
![animation](https://github.com/zeProdigy/mqtt-accelero/blob/main/gif.gif)

### Используемый стек
* FreeRTOS
* usb-ecm
* lwip+mqtt

## Сборка
### Требования
* arm-none-eabi-gcc
* cmake
* make/ninja
* git (опционально, для получения версии)
* st-flash (опционально, для прошивки)

### Сборка и прошивка
`cmake ..`

`make -j`

`st-flash write mqtt-accelero.bin 0x8000000`

## Отладка 
Доступна отладочная uart консоль
* TX - PA2
* RX - PA3
