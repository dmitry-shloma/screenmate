# TODO: create copy and move functions
#defineTest(copyToDestDir) {
#    FILE_MASK = $$QMAKE_TARGET
#    FILES = $${BUILD_DIR}/$${FILE_MASK}

#    for(file, FILES) {
#        win32 {
#            # Replace slashes in paths with backslashes for Windows
#            win32: file ~= s,/,\\,g
#            win32: DESTDIR ~= s,/,\\,g
#        }
#        QMAKE_POST_LINK += $$QMAKE_COPY_FILE -a $$quote($$file) $$quote($$DESTDIR) $$escape_expand(\\n\\t)
#    }
#    export(QMAKE_POST_LINK)
#}
