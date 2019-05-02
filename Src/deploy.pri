# TODO: Возможно для lib следует использовать pre link
# Проверить это и реализовать третий параметр для указания

# @brief copyFiles копирование одного или нескольких файлов
# @param srcDir полное имя исходной директории
# @param dstDir полное имя конечной директории
# @param files имя одного или нескольких копируемых файлов
defineTest(copyFiles) {
    srcDir = $${1}
    dstDir = $${2}
    files = $${3}

    for(file, files) {
        srcFile = $${srcDir}/$${file}
        dstFile = $${dstDir}/$${file}

        win32 { # Коррекция разделителей с учетом ОС Windows
            srcFile ~= s,/,\\,g
            dstFile ~= s,/,\\,g
        }

        QMAKE_POST_LINK += \
            $$QMAKE_COPY $$quote($${srcFile}) $$quote($${dstFile}) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}
