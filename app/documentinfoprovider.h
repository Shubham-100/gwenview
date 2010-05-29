// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
Gwenview: an image viewer
Copyright 2010 Aurélien Gâteau <agateau@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
#ifndef DOCUMENTINFOPROVIDER_H
#define DOCUMENTINFOPROVIDER_H

// Qt

// KDE

// Local
#include <lib/thumbnailview/abstractdocumentinfoprovider.h>

namespace Gwenview {

class SortedDirModel;

class DocumentInfoProviderPrivate;
class DocumentInfoProvider : public AbstractDocumentInfoProvider {
	Q_OBJECT
public:
	DocumentInfoProvider(SortedDirModel* model);
	~DocumentInfoProvider();

	virtual bool isBusy(const KUrl& url);

	virtual bool isModified(const KUrl& url);

	virtual void thumbnailForDocument(const KUrl& url, ThumbnailGroup::Enum group, QPixmap* outPix, QSize* outFullSize) const;

private Q_SLOTS:
	void emitBusyStateChanged(const KUrl&, bool);
	void emitDocumentChanged(const KUrl&);

private:
	DocumentInfoProviderPrivate* const d;
};


} // namespace

#endif /* DOCUMENTINFOPROVIDER_H */
