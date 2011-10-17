// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
Gwenview: an image viewer
Copyright 2011 Aurélien Gâteau <agateau@kde.org>

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
#ifndef ABSTRACTIMAGEVIEW_H
#define ABSTRACTIMAGEVIEW_H

// Local

// KDE

// Qt
#include <QGraphicsWidget>

namespace Gwenview {


class AbstractImageViewPrivate;
/**
 *
 */
class AbstractImageView : public QGraphicsWidget {
	Q_OBJECT
public:
    AbstractImageView(QGraphicsItem* parent = 0);
	~AbstractImageView();

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

	qreal zoom() const;

	void setZoom(qreal zoom, const QPointF& center);

protected:
	virtual void updateCache() = 0;
	qreal mZoom;
	QPixmap mCachePix;

private:
	AbstractImageViewPrivate* const d;
};



} // namespace

#endif /* ABSTRACTIMAGEVIEW_H */