set(CHERRYDOOM_DOCS
    boom.txt
    boomdeh.txt
    boomref.txt
    mapcolors_vanilla.cfg
    mbf.txt
    mbfedit.txt
    mbffaq.txt
    nughud.lmp
    nughud.md
    options.txt
    woofhud.lmp
    woofhud.md)

if(WIN32)
    install(FILES ${CHERRYDOOM_DOCS} DESTINATION docs)
else()
    install(FILES ${CHERRYDOOM_DOCS} DESTINATION "share/doc/${PROJECT_SHORTNAME}")
endif()
