#ifndef RENDERAREA_H
#define RENDERAREA_H
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <unordered_map>
#include <vector>
#include "network.h"


class RenderArea : public QWidget {
    Q_OBJECT

public:
    explicit RenderArea(QWidget *parent = nullptr);

    void setNetworkData(const std::unordered_map<std::string, std::vector<bht::Shape>>& Shapes,
                        const std::unordered_map<std::string, std::string>& colors);

    void setHighlight(const std::string& routId);
    //only for highlighting
    std::string m_highlightedRouteId;

protected:
    void paintEvent(QPaintEvent *event) override;

private:


    double m_minLat;
    double m_maxLat;
    double m_minLon;
    double m_maxLon;
    QPixmap m_cachedPixmap;
    std::unordered_map<std::string, std::vector<bht::Shape>> m_routeShapes;
    std::unordered_map<std::string, std::string> m_routeColors;
};




#endif // RENDERAREA_H
