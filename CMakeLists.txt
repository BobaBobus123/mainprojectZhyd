cmake_minimum_required(VERSION 3.16)

# 1. Назва проекту
project(mainprojectZhyd VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Знаходимо пакети
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

# 3. Створюємо список файлів
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    expense_dialog.cpp
    expense_dialog.h
    expense_dialog.ui
    database_manager.h
)

# 4. СТВОРЮЄМО ЦІЛЬ (Це має бути ПЕРЕД target_link_libraries)
add_executable(mainprojectZhyd ${PROJECT_SOURCES})

# 5. Прив'язуємо бібліотеки до вже створеної цілі
target_link_libraries(mainprojectZhyd PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)
