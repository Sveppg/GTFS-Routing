#include "renderarea.h"
#include <QPaintEvent>
#include <cmath>
#include <limits>
/*
RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent),
    m_minLat(std::numeric_limits<double>::max()),
    m_maxLat(std::numeric_limits<double>::lowest()),
    m_minLon(std::numeric_limits<double>::max()),
    m_maxLon(std::numeric_limits<double>::lowest())
{}

void RenderArea::setNetworkData(const std::unordered_map<std::string, std::vector<bht::Shape>>& Shapes, 
                                const std::unordered_map<std::string, std::string>& colors) {
    m_routeShapes = Shapes;   // Update outbound shapes
    m_routeColors = colors;           // Update route colors
    calculateBounds();                // Recalculate bounds for new shapes
    update();                         // Trigger repaint
}

void RenderArea::calculateBounds() {
    for (const auto &[routeId, shapes] : m_routeShapes) {
        for (const auto &shape : shapes) {
            m_minLat = std::min(m_minLat, shape.latitude);
            m_maxLat = std::max(m_maxLat, shape.latitude);
            m_minLon = std::min(m_minLon, shape.longitude);
            m_maxLon = std::max(m_maxLon, shape.longitude);
        }
    }
}

void RenderArea::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    if (m_routeShapes.empty()) {
        return;
    }

    double scaleX = (m_maxLon - m_minLon) == 0 ? 1.0 : width() / (m_maxLon - m_minLon);
    double scaleY = (m_maxLat - m_minLat) == 0 ? 1.0 : height() / (m_maxLat - m_minLat);
    double scale = std::min(scaleX, scaleY);

    for (const auto &[routeId, shapes] : m_routeShapes) {
        // C++11/14/17-kompatible Farbauswahl
        auto it = m_routeColors.find(routeId);
        QColor color = (it != m_routeColors.end()) 
                       ? QColor(QString::fromStdString(it->second))
                       : Qt::black; // Standardfarbe, falls nicht gefunden
        painter.setPen(QPen(color, 1));

        for (size_t i = 1; i < shapes.size(); ++i) {
            if (shapes[i - 1].sequence == 0 && shapes[i].sequence == 0) continue;
            double x1 = (shapes[i - 1].longitude - m_minLon) * scale;
            double y1 = (m_maxLat - shapes[i - 1].latitude) * scale;
            double x2 = (shapes[i].longitude - m_minLon) * scale;
            double y2 = (m_maxLat - shapes[i].latitude) * scale;
            if (shapes[i].sequence != 0)
                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }
}
*/


//für Square
RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

/**
 * Überlade die Methode paintEvent, welche immer dann aufgerufen wird wenn das
 * Widget neu gezeichnet werden muss.
 * @brief RenderArea::paintEvent Event welches beschreibt was/warum neu gezeichnet werden muss
 */
void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    // Infos zum QPainter
    // https://doc.qt.io/archives/qt-4.8/qpainter.html
    // mit dem auf dem Bildschirm Formen gezeichnet werden können
    QPainter painter(this);

    // Fülle das ganze Fenster (Startpunkt 0, 0 und width(), height() geben die Größe des Widgets zurück)
    // mit einem roten Rechteck (Farbcode 255, 0, 0 -> https://www.rapidtables.org/de/web/color/RGB_Color.html)
    painter.fillRect(QRect(0, 0, width() - 1, height() - 1), QColor(255, 0, 0));

    // Zum Zeichnen von Pfaden wird ein QPen verwendet, welcher die Farbe, Strichdicke, etc. enthält.
    // https://doc.qt.io/archives/qt-4.8/qpen.html
    QPen pen = QPen();
    pen.setColor(QColor(255, 255, 0));
    pen.setWidth(10);
    painter.setPen(pen);
    // Zum Zeichnen der unterschiedlichen Formen gibt es am QPainter-Objekt die passenden Methoden
    // wie bspw. drawRect für Rechtecke, drawEllipse für Ellipsen/Kreise oder drawLines/drawPolyline für
    // Linien und Linienzüge (https://doc.qt.io/archives/qt-4.8/qpainter.html#public-functions)
    QRect rect(10, 20, 80, 60);
    painter.drawRect(rect);
}

