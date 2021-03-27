TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


#Подключение библиотеки OpenMP
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

QMAKE_CXXFLAGS += -O1  # Оптимизация включена

# Линкуется библиотека, содержащая реализацию кривых
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../curvelib/build/release/ -lcurvelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../curvelib/build/debug/ -lcurvelib

INCLUDEPATH += $$PWD/../curvelib/include
DEPENDPATH += $$PWD/../curvelib/include
