// vim: set tabstop=4 shiftwidth=4 noexpandtab
/*
Gwenview - A simple image viewer for KDE
Copyright (c) 2000-2003 Aur�lien G�teau
 
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
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef GVPIXMAP_H
#define GVPIXMAP_H

// Qt includes
#include <qcstring.h>
#include <qobject.h>
#include <qimage.h>

// KDE includes
#include <kurl.h>
#include <kprinter.h>

class GVPixmapPrivate;

/**
 * The application document. Should be renamed GVDoc.
 */
class GVPixmap : public QObject {
Q_OBJECT
public:
	enum ModifiedBehavior { Ask=0, SaveSilently=1, DiscardChanges=2 };
	enum CommentState { None=0, ReadOnly=1, Valid=ReadOnly, Writable=3 };
	
	GVPixmap(QObject*);
	~GVPixmap();

	// Properties
	const QImage& image() const { return mImage; }
	KURL url() const;
	const KURL& dirURL() const { return mDirURL; }
	const QString& filename() const { return mFilename; }
	int width() const { return mImage.width(); }
	int height() const { return mImage.height(); }
	bool isNull() const { return mImage.isNull(); }
	const QString& imageFormat() const { return mImageFormat; }

	void setModifiedBehavior(ModifiedBehavior);
	ModifiedBehavior modifiedBehavior() const;

	CommentState commentState() const;
	QString comment() const;
	void setComment(const QString&);

public slots:
	void setURL(const KURL&);
	void setDirURL(const KURL&);
	void setFilename(const QString&);
	void reload();

	/**
	 * Save to the current file.
	 * Returns false if failed.
	 */
	bool save();
	void saveAs();
	
	/** print the selected file */
	void print(KPrinter *pPrinter);
	
	/**
	 * If the image has been modified, prompt the user to save the changes.
	 * Returns true if it's safe to switch to another image.
	 */
	bool saveIfModified();

	// "Image manipulation"
	void rotateLeft();
	void rotateRight();
	void mirror();
	void flip();

signals:
	/**
	 * Emitted when the class starts to load the image.
	 */
	void loading();

	/**
	 * Emitted when the class has finished loading the image.
	 * Also emitted if the image could not be loaded.
	 */
	void loaded(const KURL& dirURL,const QString& filename);

	/**
	 * Emitted when the image has been modified.
	 */
	void modified();

	/**
	 * Emitted when the image has been saved on disk.
	 */
	void saved(const KURL& url);

	/**
	 * Emitted when the image has been reloaded.
	 */ 
	void reloaded(const KURL& url);

private:
	QImage mImage;
	KURL mDirURL;
	QString mFilename;
	QString mImageFormat;
	bool mModified;

	// Store compressed data. Usefull for lossless manipulations.
	QByteArray mCompressedData;

	GVPixmapPrivate* d;

	void reset();
	void load();
	bool saveInternal(const KURL&,const QString& format);
};


#endif
