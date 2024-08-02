#include "svg.h"

svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays)
{
    svg::Polyline polyline;
    for (int i = 0; i <= num_rays; ++i)
    {
        double angle = 2 * M_PI * (i % num_rays) / num_rays;
        polyline.AddPoint({center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle)});
        if (i == num_rays)
        {
            break;
        }
        angle += M_PI / num_rays;
        polyline.AddPoint({center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle)});
    }
    return polyline;
}

namespace shapes
{
    class Triangle : public svg::Drawable
    {
        svg::Point p1_ = {0.0, 0.0};
        svg::Point p2_ = {0.0, 0.0};
        svg::Point p3_ = {0.0, 0.0};

    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3) : p1_(p1), p2_(p2), p3_(p3) {}
        void Draw(svg::ObjectContainer &container) const
        {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }
        ~Triangle() override {}
    };

    class Star : public svg::Drawable
    {
        svg::Point center_ = {0.0, 0.0};
        double outer_radius_ = 0.0;
        double inner_radius_ = 0.0;
        int num_rays_ = 0;

    public:
        Star(svg::Point center, double outer_radius, double inner_radius, int num_rays) : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays) {}
        void Draw(svg::ObjectContainer &container) const
        {
            container.Add(CreateStar(center_, outer_radius_, inner_radius_, num_rays_).SetFillColor("red").SetFillColor("red").SetStrokeColor("black"));
        }
        ~Star() override {}
    };

    class Snowman : public svg::Drawable
    {
        svg::Point head_center_ = {0.0, 0.0};
        double head_radius_ = 0.0;

    public:
        Snowman(svg::Point head_center, double head_radius) : head_center_(head_center), head_radius_(head_radius) {}
        void Draw(svg::ObjectContainer &container) const
        {
            container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y + 5 * head_radius_}).SetRadius(head_radius_ * 2).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y + 2 * head_radius_}).SetRadius(head_radius_ * 1.5).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y}).SetRadius(head_radius_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        }
        ~Snowman() override {}
    };
}

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer &target)
{
    for (auto it = begin; it != end; ++it)
    {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(const Container &container, svg::ObjectContainer &target)
{
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

int main()
{
    using namespace svg;
    using namespace shapes;
    using namespace std;

    vector<unique_ptr<svg::Drawable>> picture;

    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    // Так как документ реализует интерфейс ObjectContainer,
    // его можно передать в DrawPicture в качестве цели для рисования
    DrawPicture(picture, doc);

    // Выводим полученный документ в stdout
    doc.Render(cout);
}