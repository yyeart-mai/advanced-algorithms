# Диагностика памяти словаря PATRICIA

## AddressSanitizer

Отдельная сборка с AddressSanitizer прошла консольные тесты, включая 20 000 смешанных команд с seed `20261120`. Код выхода проверки `0`. Автоматическая проверка утечек в AddressSanitizer отключена, её выполняет отдельный запуск `leaks`. [Тесты CLI](../../../../tests/check_cli.py) · [Протокол](asan.log)

```sh
make -C lab1 BUILD_DIR=build/asan \
  CXXFLAGS='-std=c++11 -O1 -g -fsanitize=address -fno-omit-frame-pointer -Wall -Wextra -Wpedantic'
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 \
  python3 lab1/tests/check_cli.py lab1/build/asan/dictionary
```

## Проверка утечек

Вход на 21 команду покрывает оба случая удаления, две замены дерева через `Load` и выход с тремя ключами. Все 21 ответ совпали с ожидаемыми. `leaks` проверил 189 выделенных блоков общим размером 21 КБ и сообщил о 0 утечек и 0 потерянных байтов. Код выхода `0`. В протоколе есть сообщение об ограничении на показ содержимого памяти процесса без отладочных прав. [Вход](leaks.in) · [Ожидаемые ответы](leaks.expected) · [Протокол](leaks.log)

Из каталога сценария запуск выполняется на оптимизированной сборке.

```sh
/usr/bin/leaks --atExit -- ../../../../build/dictionary < leaks.in > leaks.log 2>&1
```

[Профиль CPU](../cpu/profile.md) · [Сводка замеров](../../summary.md)
