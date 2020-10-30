# Пакет для анализа космических событий зарегистрированных в системе АЧС
Для получения кода нужно выполнить команду: <br />
```
git clone https://github.com/ivovtin/cosmicMuons
```

Данный пакет зависит от [KaFramework](https://github.com/ivovtin/KaFramework) и пакетов реконструкции указанных в Makefile.<br />

Для сборки нужно сделать make в диретории с пакетом<br />

Для перекачки сырых данных в root-файл необходимо на batch запустить программу с помощью следующей команды:
```
qsub batch_dataatc_cosm.sh
```

Прежде нужно расскоментировать обрабатываемые файлы или добавить их, и указать верно выходные пути! <br />


## 1. Построение долговременной стабильности для счетчиков АЧС

После получения root-файлов переходим в директорию cosmic, где для сборки делаем make. <br />

В файле cosmic.C перед сборкой необходимо указать верные выходные пути. <br />

Для запуска на batch выполнить команду:
```
qsub batch_cosm.sh
```

Результирующие картинки лежат здесь http://kedr.inp.nsk.su/~ovtin/atc_cosmic/stability/

## 2. Качество идентификации в режиме одного счетчика

Переходим в директорию kp_identification и для сборки делаем make.

Для запуска на batch выполнить команду:
```
qsub batch_kp_cosm.sh
```
Предварительно не забыть поменять пути к выходным директориям.

Результирующие картинки лежат здесь http://kedr.inp.nsk.su/~ovtin/atc_cosmic/one_cnt/kp_identification_2014/ или (2015, 2016 ...) <br />

## 3. Качество идентификации в режиме толстого счетчика

Переходим в директорию kp_identification_thick и для сборки делаем make.

Для запуска на batch выполнить команду:
```
qsub batch_kp_cosm.sh
```
Предварительно не забыть поменять пути к выходным директориям.

Результирующие картинки лежат здесь http://kedr.inp.nsk.su/~ovtin/atc_cosmic/thick_cnt/ <br />

Для получения зависимости вероятностей ложной идентификации и качества разделения от порога в числе ф.э. запустить:
```
qsub batch_kp_cosm_res.sh
```
Результирующие картинки лежат здесь http://kedr.inp.nsk.su/~ovtin/atc_cosmic/thick_cnt/thick_results_2014/ или (2015, 2016 ...) <br />

Для получения зависимости качества идентификации от времени для каждого толстого счетчика при различных порогах запустить:
```
./sigma_year
```
Результирующие картинки лежат здесь http://kedr.inp.nsk.su/~ovtin/atc_cosmic/thick_cnt/thick_results_allyears/ <br />


Другие полезные команды для просмотра событий:
```
bzcat /space/runs/daq021913.nat.bz2 | KDisplay -r -e3197

/home/ovtin/development/bin/KDisplay < /spool/users/ovtin/sim000004.dat -r -R22996
```
