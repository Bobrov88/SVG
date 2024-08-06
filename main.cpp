#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>

namespace
{
    // Выполняет линейную интерполяцию значения от from до to в зависимости от параметра t
    uint8_t Lerp(uint8_t from, uint8_t to, double t)
    {
        return static_cast<uint8_t>(std::round((to - from) * t + from));
    }

    // Выполняет линейную интерполяцию Rgb цвета от from до to в зависимости от параметра t
    svg::Rgb Lerp(svg::Rgb from, svg::Rgb to, double t)
    {
        return {Lerp(from.red, to.red, t), Lerp(from.green, to.green, t), Lerp(from.blue, to.blue, t)};
    }

    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays)
    {
        using namespace svg;
        Polyline polyline;
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

} // namespace

namespace shapes
{

    class Star : public svg::Drawable
    {
    public:
        Star(svg::Point center, double outer_radius, double inner_radius, int num_rays)
            : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays)
        {
        }

        void Draw(svg::ObjectContainer &container) const override
        {
            using namespace std::literals;
            container.Add( //
                CreateStar(center_, outer_radius_, inner_radius_, num_rays_)
                    .SetFillColor("red"s)
                    .SetStrokeColor("black"s));
        }

    private:
        svg::Point center_;
        double outer_radius_;
        double inner_radius_;
        int num_rays_;
    };

    class Snowman : public svg::Drawable
    {
    public:
        Snowman(svg::Point head_center, double head_radius)
            : head_center_(head_center), head_radius_(head_radius)
        {
        }

        void Draw(svg::ObjectContainer &container) const override
        {
            using namespace svg;
            using namespace std::literals;

            const auto base_circle = Circle().SetFillColor("rgb(240,240,240)"s).SetStrokeColor("black");
            container.Add( //
                Circle(base_circle)
                    .SetCenter({head_center_.x, head_center_.y + 5 * head_radius_})
                    .SetRadius(2 * head_radius_));

            container.Add( //
                Circle(base_circle)
                    .SetCenter({head_center_.x, head_center_.y + 2 * head_radius_})
                    .SetRadius(1.5 * head_radius_));
            container.Add(Circle(base_circle).SetCenter(head_center_).SetRadius(head_radius_));
        }

    private:
        svg::Point head_center_;
        double head_radius_;
    };

    class Triangle : public svg::Drawable
    {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1), p2_(p2), p3_(p3)
        {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer &container) const override
        {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }

    private:
        svg::Point p1_, p2_, p3_;
    };

} // namespace shapes

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
    // using namespace svg;
    // using namespace shapes;
    // using namespace std;

    // vector<unique_ptr<svg::Drawable>> picture;
    // picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    // picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    // picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    // svg::Document doc;
    // DrawPicture(picture, doc);

    // const Text base_text = //
    //     Text()
    //         .SetFontFamily("Verdana"s)
    //         .SetFontSize(12)
    //         .SetPosition({10, 100})
    //         .SetData("Happy New Year!"s);
    // doc.Add(Text{base_text}
    //             .SetStrokeColor("yellow"s)
    //             .SetFillColor("yellow"s)
    //             .SetStrokeLineJoin(StrokeLineJoin::ROUND)
    //             .SetStrokeLineCap(StrokeLineCap::ROUND)
    //             .SetStrokeWidth(3));
    // doc.Add(Text{base_text}.SetFillColor("red"s));

    // doc.Render(cout);

    using namespace svg;
    using namespace std;

    Rgb start_color{0, 255, 30};
    Rgb end_color{20, 20, 150};

    const int num_circles = 10;
    Document doc;
    for (int i = 0; i < num_circles; ++i) {
        const double t = double(i) / (num_circles - 1);

        const Rgb fill_color = Lerp(start_color, end_color, t);
        doc.Add(Circle()
                    .SetFillColor(fill_color)
                    .SetStrokeColor("black"s)
                    .SetCenter({i * 20.0 + 40, 40.0})
                    .SetRadius(15));
    }
    doc.Render(cout);
}