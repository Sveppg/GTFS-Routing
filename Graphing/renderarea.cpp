#include "renderarea.h"
#include <QPaintEvent>
#include <QPainter>
#include <iostream>
#include <cmath>
#include <QDebug>
#include <limits>

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent),
    m_minLat(std::numeric_limits<double>::max()),
    m_maxLat(std::numeric_limits<double>::lowest()),
    m_minLon(std::numeric_limits<double>::max()),
    m_maxLon(std::numeric_limits<double>::lowest())
{}

void RenderArea::setHighlight(const std::string& routId){
    m_highlightedRouteId = routId;
    update();
}

void RenderArea::setNetworkData(const std::unordered_map<std::string, std::vector<bht::Shape>>& Shapes,
                                const std::unordered_map<std::string, std::string>& colors) {
    m_routeShapes = Shapes;   // Update outbound shapes
    m_routeColors = colors;           // Update route colors
    for (const auto& [routeId, shapes] : Shapes){
        for (const auto& shape : shapes) {
            m_minLat = std::min(m_minLat, shape.latitude);
            m_maxLat = std::max(m_maxLat, shape.latitude);
            m_minLon = std::min(m_minLon, shape.longitude);
            m_maxLon = std::max(m_maxLon, shape.longitude);
        }
    }
    update();
}


void RenderArea::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white); // Hintergrundfarbe setzen

    if (m_routeShapes.empty()) {
        std::cout << "no Route found -> don't want to draw shit" << std::endl;
        return; // Keine Routen vorhanden, nichts zu zeichnen
    }
    double scaleX = (m_maxLon - m_minLon) == 0.0 ? 1.0 : width() / (m_maxLon - m_minLon);
    double scaleY = (m_maxLat - m_minLat) == 0.0 ? 1.0 : height() / (m_maxLat - m_minLat);
    double scale = std::min(scaleX, scaleY);
    // std::cout << "ScaleX: " << scaleX << ", ScaleY: " << scaleY << ", Scale: " << scale << std::endl;

    for (const auto &[routeId, shapes] : m_routeShapes) {
        // C++11/14/17-kompatible Farbauswahl
       // QColor color = Qt::black; // Standardfarbe setzen
       // QColor color = (routeId ==   m_highlightedRouteId) ? QColor("#ff00ff") : QColor(Qt::black); // Hervorheben des Trip
        QColor color = (routeId ==   m_highlightedRouteId) ? QColor("#ff00ff") : QColor(Qt::black); // Hervorheben des Tr

        for (const auto &pair : m_routeColors) {

            if (pair.first == routeId) { // Prüfen, ob die aktuelle ID übereinstimmt
                // std::cout<<pair.first<<"   " <<pair.second <<std::endl;
                color = QColor(QString::fromStdString("#"+pair.second)); // Füge '#' hinzu
                break; // Wenn gefunden, beenden wir die Schleifes
            }
        }
        //painter.setPen(QPen(color, 1));
        painter.setPen(QPen(color, (routeId == m_highlightedRouteId) ? 6 : 1)); // Dickere Linie für den hervorgehobenen Trip
        for (size_t i = 1; i < shapes.size(); ++i) {

            if (shapes[i - 1].sequence == 0 && shapes[i].sequence == 0) continue; // Wenn beide Sequenzen 0 sind, überspringen

            double x1 = (shapes[i - 1].longitude - m_minLon) * scale;
            double y1 = (m_maxLat - shapes[i - 1].latitude) * scale;
            double x2 = (shapes[i].longitude - m_minLon) * scale;
            double y2 = (m_maxLat - shapes[i].latitude) * scale;


            if (shapes[i].sequence != 0)
                painter.drawLine(QPointF(x1, y1), QPointF(x2, y2)); // Zeichne Linie zwischen den Punkten
        }
    }
}
