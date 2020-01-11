#ifndef TEST_LICENSEFILESAVAILABLE_H
#define TEST_LICENSEFILESAVAILABLE_H

#include <QObject>

class TestLicenseFilesAvailable : public QObject
{
    Q_OBJECT

public:
    TestLicenseFilesAvailable() = default;

private Q_SLOTS:
    void checkAll();
};
#endif
