#include "qt/graphics/QtGraphicsView.h"

#include <QPropertyAnimation>
#include <QScrollBar>
#include <QMouseEvent>

QtGraphicsView::QtGraphicsView(QWidget* parent)
	: QGraphicsView(parent)
	, m_zoomFactor(1.0f)
	, m_appZoomFactor(1.0f)
{
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

float QtGraphicsView::getZoomFactor() const
{
	return m_appZoomFactor;
}

void QtGraphicsView::setAppZoomFactor(float appZoomFactor)
{
	m_appZoomFactor = appZoomFactor;
	updateTransform();
}

qreal QtGraphicsView::zoom() const
{
	return m_zoomFactor;
}

void QtGraphicsView::setZoom(qreal zoom)
{
	m_zoomFactor = qBound(0.1, zoom, 100.0);
	updateTransform();
}

void QtGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()))
	{
		m_last = event->pos();
	}

	QGraphicsView::mousePressEvent(event);
}

void QtGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && !itemAt(event->pos()) && event->pos() == m_last)
	{
		emit emptySpaceClicked();
	}

	QGraphicsView::mouseReleaseEvent(event);
	viewport()->setCursor(Qt::ArrowCursor);
}

void QtGraphicsView::keyPressEvent(QKeyEvent* event)
{
	float delta = 150;
	float x = 0.0f;
	float y = 0.0f;

	if (event->key() == Qt::Key_W)
	{
		if (event->modifiers() == Qt::ShiftModifier)
		{
			updateZoom(200.0f, true);
			return;
		}

		y -= delta;
	}
	else if (event->key() == Qt::Key_A)
	{
		x -= delta;
	}
	else if (event->key() == Qt::Key_S)
	{
		if (event->modifiers() == Qt::ShiftModifier)
		{
			updateZoom(-200.0f, true);
			return;
		}

		y += delta;
	}
	else if (event->key() == Qt::Key_D)
	{
		x += delta;
	}
	else
	{
		return;
	}

	if (x != 0)
	{
		QPropertyAnimation* anim = new QPropertyAnimation(horizontalScrollBar(), "value");
		anim->setDuration(100);
		anim->setStartValue(horizontalScrollBar()->value());
		anim->setEndValue(horizontalScrollBar()->value() + x);
		anim->start();
	}
	if (y != 0)
	{
		QPropertyAnimation* anim = new QPropertyAnimation(verticalScrollBar(), "value");
		anim->setDuration(100);
		anim->setStartValue(verticalScrollBar()->value());
		anim->setEndValue(verticalScrollBar()->value() + y);
		anim->start();
	}
}

void QtGraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->modifiers() == Qt::ShiftModifier && event->delta() != 0.0f)
	{
		updateZoom(event->delta(), false);
		return;
	}

	QGraphicsView::wheelEvent(event);
}

void QtGraphicsView::updateTransform()
{
	float zoomFactor = m_appZoomFactor * m_zoomFactor;
	setTransform(QTransform(zoomFactor, 0, 0, zoomFactor, 0, 0));
}

void QtGraphicsView::updateZoom(float delta, bool animate)
{
	float factor = 1.0f + 0.001 * delta;

	if (factor <= 0.0f)
	{
		factor = 0.000001;
	}

	float newZoom = m_zoomFactor * factor;

	if (animate)
	{
		QPropertyAnimation* anim = new QPropertyAnimation(this, "zoom");
		anim->setDuration(100);
		anim->setStartValue(m_zoomFactor);
		anim->setEndValue(newZoom);
		anim->start();
	}
	else
	{
		setZoom(newZoom);
	}
}