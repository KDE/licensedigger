#ifndef TEST_HEADERDETECTION_H
#define TEST_HEADERDETECTION_H

#include <QObject>

class TestHeaderDetection : public QObject
{
    Q_OBJECT

public:
    TestHeaderDetection() = default;

private Q_SLOTS:
    void detectGPL20_or_GPL30_or_KDE();
    void detectGPL20only();
    void detectGPL20orlater();
    void detectGPL30orlater();
    void detectLGPL20orlater();
    void detectLGPL20only();
    void detectLGPL20onlyWithQtCommercialException();
    void detectLGPL21only();
    void detectLGPL21onlyOrLGPL30only();
    void detectLGPL21orlater();
    void detectLGPL20_or_LGPL30();
    void detectLGPL21_or_LGPL30_or_KDE();
    void detectLGPL30orlater();
    void detectLGPL30onlyOrGPL20orlater();
    void detectBSD2Clause();
    void detectBSD3Clause();
    void detectMIT();
    void detectLGPL21withQtLGPLexception_or_QtCommercial();
    void detectLGPL21withQtLGPLexceptionOrLGPL30withQtLGPLexception();
    void detectLGPL30_or_GPL20_or_GPL30_or_GPLKFQF_or_QtCommercial();

private:
    void detectForIdentifier(const QString &spdxMarker);
};
#endif
