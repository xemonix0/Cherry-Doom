string(TIMESTAMP PROJECT_DATE "%Y-%m-%d" UTC)

configure_file(io.github.xemonix0.Cherry-Doom.metainfo.in
               io.github.xemonix0.Cherry-Doom.metainfo.xml
               ESCAPE_QUOTES @ONLY)

if(UNIX AND NOT APPLE)
    install(FILES io.github.xemonix0.Cherry-Doom.desktop DESTINATION share/applications)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/io.github.xemonix0.Cherry-Doom.metainfo.xml"
            DESTINATION share/metainfo)
    install(FILES io.github.xemonix0.Cherry-Doom-setup.desktop DESTINATION share/applications)
    install(FILES cherry-doom.png DESTINATION share/icons/hicolor/128x128/apps)
    install(FILES cherry-doom-setup.png DESTINATION share/icons/hicolor/128x128/apps)
endif()
