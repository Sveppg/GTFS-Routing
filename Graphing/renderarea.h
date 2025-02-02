#ifndef RENDERAREA_HPP
#define RENDERAREA_HPP
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <unordered_map>
#include <vector>
#include "network.h"

/*
class RenderArea : public QWidget {
    Q_OBJECT

public:
    explicit RenderArea(QWidget *parent = nullptr);

    void setNetworkData(const std::unordered_map<std::string, std::vector<bht::Shape>>& Shapes,
                        const std::unordered_map<std::string, std::string>& colors);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void calculateBounds();

    double m_minLat;
    double m_maxLat;
    double m_minLon;
    double m_maxLon;
    QPixmap m_cachedPixmap;
    std::unordered_map<std::string, std::vector<bht::Shape>> m_routeShapes;
    std::unordered_map<std::string, std::string> m_routeColors;
};
*/


// for square
class RenderArea : public QWidget
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

};

#endif // RENDERAREA_H
