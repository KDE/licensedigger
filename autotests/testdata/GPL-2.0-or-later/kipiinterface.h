/** ===========================================================
 * @file
 *
 * This file is a part of KDE project
 *
 *
 * @date   2009-11-21
 * @brief  kipi host test application
 *
 * @author Copyright (C) 2009-2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
 * @author Copyright (C) 2011-2018 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef __KIPIINTERFACE_H
#define __KIPIINTERFACE_H

// Libkipi includes

#include "interface.h"

namespace KIPI
{
    class ImageCollection;
    class ImageInfo;
}

using namespace KIPI;

namespace KXMLKipiCmd
{

class KipiInterface : public Interface
{
    Q_OBJECT

public:

    KipiInterface(QObject* const parent, const QString& name=QString());
    ~KipiInterface() override;

    ImageCollection        currentAlbum() override;
    ImageCollection        currentSelection() override;
    QList<ImageCollection> allAlbums() override;
    ImageInfo              info(const QUrl&) override;

    bool addImage(const QUrl& url, QString& errmsg) override;
    void delImage(const QUrl& url) override;
    void refreshImages(const QList<QUrl>& urls) override;

    int      features() const override;
    QVariant hostSetting(const QString& settingName);

    ImageCollectionSelector* imageCollectionSelector(QWidget* parent) override;
    UploadWidget*            uploadWidget(QWidget* parent) override;

    void addSelectedImages(const QList<QUrl>& images);
    void addSelectedImage(const QUrl& image);

    void addSelectedAlbums(const QList<QUrl>& albums);
    void addSelectedAlbum(const QUrl& album);

    void addAlbums(const QList<QUrl>& albums);
    void addAlbum(const QUrl& album);

    void thumbnails(const QList<QUrl>& list, int size) override;

    bool saveImage(const QUrl& url, const QString& format,
                   const QByteArray& data, uint width, uint height,
                   bool  sixteenBit, bool hasAlpha,
                   bool* cancel = nullptr) override;

    FileReadWriteLock* createReadWriteLock(const QUrl&) const override;
    MetadataProcessor* createMetadataProcessor()        const override;

private:

    QList<QUrl> m_selectedImages;
    QList<QUrl> m_selectedAlbums;
    QList<QUrl> m_albums;

private:

    friend class KipiUploadWidget;
    friend class KipiImageCollectionSelector;
};

} // namespace KXMLKipiCmd

#endif // __KIPIINTERFACE_H
