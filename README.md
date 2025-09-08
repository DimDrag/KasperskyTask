# Тестовое задание

## О реализации

- Программа написана с использованием языка C++ 17-го стандарта
- Программа является кросс-платформенной - тестировалась на Microsoft Windows 10 Home и Ubuntu 22.04.5 LTS

- Тесты написаны с помощью Google Test

- Для вычисления MD5 хэша файлов использован проект [hash-library][https://github.com/stbrumme/hash-library]

- Для парсинга опций командной строки был использован [cxxopts][https://github.com/jarro2783/cxxopts]

[https://github.com/stbrumme/hash-library]: https://github.com/stbrumme/hash-library
[https://github.com/jarro2783/cxxopts]: https://github.com/jarro2783/cxxopts

## Сборка и тестирование

Для сборки проекта необходимо находясь в корневой директории проекта прописать команды

```shell
cmake -S . -B build
cmake --build build
```

Запускать тесты можно из любой директории. Главное не удалять файлы, лежащие в tests/data, поскольку это "тренировочные" файлы для тестов.

Обновлять зависимости можно с помощью команды

```bash
git submodule update --init --recursive
```

## Логи

Помимо пути к "вредоносному" файлу, его хэшу и вердикту, в файл лога пишется время записи лога. Пример содержимого файла лога:

```
----------------Scanner log start----------------
10:00:00
File path: filepath.txt
MD5 hash:  hash
Verdict:   verdict
-----------------Scanner log end-----------------
```

## Запуск

По окончанию сканирования указанной папки, помимо краткой информации о сканировании утилита выдаст также информацию о папках, в которые не удалось зайти. Я посчитал, что нельзя игнорировать такие ошибки, и о них нужно явно говорить пользователю. Пример вывода утилиты, если ей встретились недоступные папки:

```
Time elapsed:      6735 ms
Processed files:   20
Malware files:     6
Failed to analyse: 4
Scan finished, but following errors occurred:
  C:\Users\user\testdir\no_read_dir is not readable
  C:\Users\user\testdir\subdir\no_read_dir2 is not readable
```

Если утилите не указать необходимые параметры, она явно об этом сообщит

```
Missing required option: --path
  Path to folder to check for malicious files
Missing required option: --base
  Path to file with malicious hash database
Missing required option: --log
  Path to log file
```

Если указанные опции неправильные, пользователю также будет дан подробный ответ

```
The following errors occurred:
  Incorrect database file extension
  The specified log file path is not a file
  The specified directory path does not exist
```
