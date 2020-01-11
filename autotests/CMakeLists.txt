
set(headerdetection_SRCS
    test_headerdetection.cpp
    ../licenseregistry.cpp
)
qt5_add_resources(headerdetection_SRCS
    testdata.qrc
    ../licenses.qrc
)
add_executable(test_headerdetection ${headerdetection_SRCS})
target_link_libraries(test_headerdetection
    Qt5::Test
)
add_test(NAME test_headerdetection COMMAND test_headerdetection)
ecm_mark_as_test(test_headerdetection)
