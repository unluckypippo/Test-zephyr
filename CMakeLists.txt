cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(my_zephyr_app)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE src/main.c)

target_sources_ifdef(CONFIG_APP_MODULE_COUNTDOWN app PRIVATE src/modules/countdown.c)
target_sources_ifdef(CONFIG_APP_MODULE_BLINK app PRIVATE src/modules/blink.c)
target_sources_ifdef(CONFIG_APP_MODULE_BUTTON_I app PRIVATE src/modules/button_i.c)
target_sources_ifdef(CONFIG_APP_MODULE_TEMP app PRIVATE src/modules/temp_read.c)
