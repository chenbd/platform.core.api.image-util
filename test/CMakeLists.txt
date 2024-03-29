SET(fw_test "${fw_name}-test")

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_test} REQUIRED glib-2.0)

FOREACH(flag ${${fw_test}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
    MESSAGE(${flag})
ENDFOREACH()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -pie -Wall")

aux_source_directory(. sources)
FOREACH(src ${sources})
    GET_FILENAME_COMPONENT(src_name ${src} NAME_WE)
    MESSAGE("${src_name}")
    ADD_EXECUTABLE(${fw_test} ${src})
    TARGET_LINK_LIBRARIES(${fw_test} ${fw_name} ${${fw_test}_LDFLAGS})
ENDFOREACH()

INSTALL(TARGETS ${fw_test} DESTINATION bin)
