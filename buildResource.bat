@ECHO OFF
C:\Qt\Qt5.12.12\5.12.12\msvc2017_64\bin\rcc.exe -binary .\qdarkstyle\style.qrc -o build\resources.rcc
copy .\build\resources.rcc build\Release\resources.rcc 
copy .\build\resources.rcc build\Debug\resources.rcc
PAUSE