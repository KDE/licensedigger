#ifndef TEST_HEADERDETECTION_H
#define TEST_HEADERDETECTION_H

#include <QObject>

class TestHeaderDetection : public QObject
{
    Q_OBJECT

public:
    TestHeaderDetection() = default;

private Q_SLOTS:
    void detectLGPL20orlater();
    void detectLGPL20only();
    void detectLGPL20_or_LGPL30_or_KDE();

private:
    void detectForIdentifier(const QString &spdxMarker);
};
#endif
