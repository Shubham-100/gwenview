/*
Gwenview: an image viewer
Copyright 2007 Aurélien Gâteau <aurelien.gateau@free.fr>

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include "documentfactory.moc"

// Qt
#include <QDateTime>
#include <QMap>
#include <QUndoGroup>

// KDE
#include <kdebug.h>
#include <kurl.h>

namespace Gwenview {

#undef ENABLE_LOG
#undef LOG
//#define ENABLE_LOG
#ifdef ENABLE_LOG
#define LOG(x) kDebug() << x
#else
#define LOG(x) ;
#endif

static const int MAX_UNREFERENCED_IMAGES = 3;

/**
 * This internal structure holds the document and the last time it has been
 * accessed. This access time is used to "garbage collect" the loaded
 * documents.
 */
struct DocumentInfo {
	Document::Ptr mDocument;
	QDateTime mLastAccess;
};

/**
 * Our collection of DocumentInfo instances. We keep them as pointers to avoid
 * altering DocumentInfo::mDocument refcount, since we rely on it to garbage
 * collect documents.
 */
typedef QMap<KUrl, DocumentInfo*> DocumentMap;

struct DocumentFactoryPrivate {
	DocumentMap mMetaDataLoadedDocumentMap;
	DocumentMap mFullyLoadedDocumentMap;
	QUndoGroup mUndoGroup;

	/**
	 * Removes items in a map if they are no longer referenced elsewhere
	 */
	void garbageCollect(DocumentMap& map) {
		// Build a map of all unreferenced images
		typedef QMap<QDateTime, KUrl> UnreferencedImages;
		UnreferencedImages unreferencedImages;

		DocumentMap::Iterator
			it = map.begin(),
			end = map.end();
		for (;it!=end; ++it) {
			DocumentInfo* info = it.value();
			if (info->mDocument.count() == 1 && !info->mDocument->isModified()) {
				unreferencedImages[info->mLastAccess] = it.key();
			}
		}

		// Remove oldest unreferenced images. Since the map is sorted by key,
		// the oldest one is always unreferencedImages.begin().
		for (
			UnreferencedImages::Iterator unreferencedIt = unreferencedImages.begin();
			unreferencedImages.count() > MAX_UNREFERENCED_IMAGES;
			unreferencedIt = unreferencedImages.erase(unreferencedIt))
		{
			KUrl url = unreferencedIt.value();
			LOG("Collecting" << url);
			it = map.find(url);
			Q_ASSERT(it != map.end());
			delete it.value();
			map.erase(it);
		}
	}

	void logDocumentMap(const DocumentMap& map) {
		kDebug() << "map:";
		DocumentMap::ConstIterator
			it = map.constBegin(),
			end = map.constEnd();
		for(; it!=end; ++it) {
			kDebug() << "-" << it.key()
				<< "refCount=" << it.value()->mDocument.count()
				<< "lastAccess=" << it.value()->mLastAccess;
		}
	}

	QList<KUrl> mModifiedDocumentList;
};

DocumentFactory::DocumentFactory()
: d(new DocumentFactoryPrivate) {
}

DocumentFactory::~DocumentFactory() {
	qDeleteAll(d->mFullyLoadedDocumentMap);
	qDeleteAll(d->mMetaDataLoadedDocumentMap);
	delete d;
}

DocumentFactory* DocumentFactory::instance() {
	static DocumentFactory factory;
	return &factory;
}

Document::Ptr DocumentFactory::load(const KUrl& url, Document::LoadState loadState) {
	DocumentInfo* info = 0;

	DocumentMap::Iterator it = d->mFullyLoadedDocumentMap.find(url);

	if (it != d->mFullyLoadedDocumentMap.end()) {
		LOG(url.fileName() << "url in mFullyLoadedDocumentMap");
		info = it.value();
		info->mLastAccess = QDateTime::currentDateTime();
		return info->mDocument;
	}

	it = d->mMetaDataLoadedDocumentMap.find(url);
	if (it != d->mMetaDataLoadedDocumentMap.end()) {
		LOG(url.fileName() << "url in mMetaDataLoadedDocumentMap");
		if (loadState == Document::LoadMetaData) {
			info = it.value();
			info->mLastAccess = QDateTime::currentDateTime();
			return info->mDocument;
		} else {
			// Meta data already loaded, we need to load image pixels
			LOG(url.fileName() << "need to load image pixels");

			// Move DocumentInfo to the mFullyLoadedDocumentMap map
			info = it.value();
			d->mMetaDataLoadedDocumentMap.erase(it);
			d->mFullyLoadedDocumentMap[url] = info;

			// Start loading image pixels
			info->mDocument->finishLoading();
			return info->mDocument;
		}
	}

	// At this point we couldn't find the document in either maps

	// Start loading the document
	LOG(url.fileName() << "loading" << (loadState == Document::LoadAll ? "all" : "metadata"));
	Document* doc = new Document(url, loadState);
	connect(doc, SIGNAL(loaded(const KUrl&)),
		SLOT(slotLoaded(const KUrl&)) );
	connect(doc, SIGNAL(saved(const KUrl&)),
		SLOT(slotSaved(const KUrl&)) );
	connect(doc, SIGNAL(modified(const KUrl&)),
		SLOT(slotModified(const KUrl&)) );

	// Create DocumentInfo instance
	info = new DocumentInfo;
	Document::Ptr docPtr(doc);
	info->mDocument = docPtr;
	info->mLastAccess = QDateTime::currentDateTime();

	// Place DocumentInfo in the appropriate map
	DocumentMap& map =
		loadState == Document::LoadAll
		? d->mFullyLoadedDocumentMap
		: d->mMetaDataLoadedDocumentMap;

	map[url] = info;

	d->garbageCollect(map);
	#ifdef ENABLE_LOG
	d->logDocumentMap(map);
	#endif

	return docPtr;
}


QList<KUrl> DocumentFactory::modifiedDocumentList() const {
	return d->mModifiedDocumentList;
}


bool DocumentFactory::hasUrl(const KUrl& url, Document::LoadState loadState) const {
	const DocumentMap& map =
		loadState == Document::LoadAll
		? d->mFullyLoadedDocumentMap
		: d->mMetaDataLoadedDocumentMap;
	return map.contains(url);
}


void DocumentFactory::clearCache() {
	d->mFullyLoadedDocumentMap.clear();
	d->mMetaDataLoadedDocumentMap.clear();
}


void DocumentFactory::slotLoaded(const KUrl& url) {
	if (d->mModifiedDocumentList.contains(url)) {
		d->mModifiedDocumentList.removeAll(url);
		emit modifiedDocumentListChanged();
		emit documentChanged(url);
	}
}


void DocumentFactory::slotSaved(const KUrl& url) {
	d->mModifiedDocumentList.removeAll(url);
	emit modifiedDocumentListChanged();
	emit documentChanged(url);
}


void DocumentFactory::slotModified(const KUrl& url) {
	if (!d->mModifiedDocumentList.contains(url)) {
		d->mModifiedDocumentList << url;
		emit modifiedDocumentListChanged();
	}
	emit documentChanged(url);
}


QUndoGroup* DocumentFactory::undoGroup() {
	return &d->mUndoGroup;
}


} // namespace
