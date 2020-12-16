### Test Skip Parser
set(skipparsertest_SRCS
    test_skipparser.cpp
    ../licenseregistry.cpp
    ../skipparser.cpp
)
qt5_add_resources(skipparsertest_SRCS
    ../licenses.qrc
)
ecm_add_test(${skipparsertest_SRCS}
             TEST_NAME test_skipparser
             LINK_LIBRARIES Qt5::Test)

### Test Detection of License Headers
set(headerdetection_SRCS
    test_headerdetection.cpp
    ../licenseregistry.cpp
    ../directoryparser.cpp
    ../skipparser.cpp
)
qt5_add_resources(headerdetection_SRCS
    testdata.qrc
    ../licenses.qrc
)
ecm_add_test(${headerdetection_SRCS}
             TEST_NAME test_headerdetection
             LINK_LIBRARIES Qt5::Test)


### Test If All License Texts are Available
set(licensefilesavailable_SRCS
    test_licensefilesavailable.cpp
    ../licenseregistry.cpp
    ../skipparser.cpp
)
qt5_add_resources(licensefilesavailable_SRCS
    ../licenses.qrc
)
ecm_add_test(${licensefilesavailable_SRCS}
             TEST_NAME test_licensefilesavailable
             LINK_LIBRARIES Qt5::Test)


### Test Copyright Statement Conversion
set(copyrightconvert_SRCS
    test_copyrightconvert.cpp
    ../licenseregistry.cpp
    ../directoryparser.cpp
    ../skipparser.cpp
)
qt5_add_resources(copyrightconvert_SRCS
    ../licenses.qrc
)
ecm_add_test(${copyrightconvert_SRCS}
             TEST_NAME test_copyrightconvert
             LINK_LIBRARIES Qt5::Test)


### Test License Statement Conversion
set(licenseconvert_SRCS
    test_licenseconvert.cpp
    ../licenseregistry.cpp
    ../directoryparser.cpp
    ../skipparser.cpp
)
qt5_add_resources(licenseconvert_SRCS
    ../licenses.qrc
    testdata.qrc
)
ecm_add_test(${licenseconvert_SRCS}
             TEST_NAME test_licenseconvert
             LINK_LIBRARIES Qt5::Test)
