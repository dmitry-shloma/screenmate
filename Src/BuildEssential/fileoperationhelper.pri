# @brief copyFile копирование файла
# @param srcDir полное имя исходного файла
# @param dstDir полное имя конечного файла
defineTest(copyFile) {
    srcFile = $${1}
    dstFile = $${2}

    win32 {
        # Коррекция формата пути к файлу с учетом ОС Windows
        srcFile ~= s,/,\\,g
        dstFile ~= s,/,\\,g
    }
    QMAKE_POST_LINK = $$QMAKE_COPY_FILE $$quote($${srcFile}) $$quote($${dstFile})
    export(QMAKE_POST_LINK)
}

# defineTest копирование содержимого директории
# srcDir исходная директория
# dstDir конечная директория
# mask маска копируемых файлов
#defineTest(copyDir) {
#    message($${1})
#    message($${2})
#    message($${3})
##    FILE_MASK = $$QMAKE_TARGET
##    FILES = $${BUILD_DIR}/$${FILE_MASK}

##    for(file, FILES) {
##        win32 {
##            # Коррекция формата пути к файлу с учетом ОС Windows
##            win32: file ~= s,/,\\,g
##            win32: DESTDIR ~= s,/,\\,g
##        }
##        message($${file})

##        QMAKE_POST_LINK += $$QMAKE_COPY_FILE -a $$quote($$file) $$quote($$DESTDIR) $$escape_expand(\\n\\t)
##    }
##    export(QMAKE_POST_LINK)
#}
