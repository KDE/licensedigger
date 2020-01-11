
### Test Detection of License Headers
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


### Test If All License Texts are Available
set(licensefilesavailable_SRCS
    test_licensefilesavailable.cpp
    ../licenseregistry.cpp
)
qt5_add_resources(licensefilesavailable_SRCS
    ../licenses.qrc
)
add_executable(test_licensefilesavailable ${licensefilesavailable_SRCS})
target_link_libraries(test_licensefilesavailable
    Qt5::Test
)
add_test(NAME test_licensefilesavailable COMMAND test_licensefilesavailable)
ecm_mark_as_test(test_licensefilesavailable)
