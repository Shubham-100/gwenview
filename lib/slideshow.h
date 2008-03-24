// vim: set tabstop=4 shiftwidth=4 noexpandtab:
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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/
#ifndef SLIDESHOW_H
#define SLIDESHOW_H

#include <lib/gwenviewlib_export.h>

// Qt
#include <QObject>

// KDE
#include <kurl.h>

class QAction;
class QTimer;

namespace Gwenview {

struct SlideShowPrivate;
class GWENVIEWLIB_EXPORT SlideShow : public QObject {
	Q_OBJECT
public:
	SlideShow(QObject* parent);
	virtual ~SlideShow();

	void start(const QList<KUrl>& urls);
	void stop();

	QWidget* intervalWidget() const;

	QAction* loopAction() const;
	QAction* randomAction() const;

	/** @return true if the slideshow is running */
	bool isRunning() const;

	void setCurrentUrl(const KUrl& url);

public Q_SLOTS:
	void setInterval(double);

Q_SIGNALS:
	void goToUrl( const KUrl& );
	/**
	 * Slideshow has been started or stopped
	 */
	void stateChanged(bool running);

private Q_SLOTS:
	void slotTimeout();
	void updateConfig();
	void slotRandomActionToggled(bool on);

private:
	SlideShowPrivate* const d;
};

} // namespace

#endif // SLIDESHOW_H
