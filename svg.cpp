#include "svg.h"

namespace shapes
{
    Triangle::Triangle(svg::Point p1, svg::Point p2, svg::Point p3) : p1_(p1), p2_(p2), p3_(p3) {}
    void Triangle::Draw(svg::ObjectContainer &container) const
    {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
    Triangle::~Triangle() {}

    Star::Star(svg::Point center, double outer_radius, double inner_radius, int num_rays) : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays) {}
    void Star::Draw(svg::ObjectContainer &container) const
    {
        container.Add(util::CreateStar(center_, outer_radius_, inner_radius_, num_rays_));
    }
    Star::~Star() {}

    Snowman::Snowman(svg::Point head_center, double head_radius) : head_center_(head_center), head_radius_(head_radius) {}
    void Snowman::Draw(svg::ObjectContainer &container) const {
        container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y+5*head_radius_}).SetRadius(head_radius_*2));
        container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y+2*head_radius_}).SetRadius(head_radius_*1.5));
        container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y}).SetRadius(head_radius_));
    }
    Snowman::~Snowman() {}
}

namespace svg
{
    using namespace std::literals;

    void Object::Render(const RenderContext &context) const
    {
        context.RenderIndent();
        RenderObject(context);
        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle &Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle &Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext &context) const
    {
        using util::RenderProperties;
        auto &out = context.out;
        out << "<circle"sv;
        RenderProperties(out, "cx"sv, center_.x);
        RenderProperties(out, "cy"sv, center_.y);
        RenderProperties(out, "r"sv, radius_);
        out << " />"sv;
    }

    Circle::~Circle()
    {
    }

    // ---------- Polyline ------------------

    Polyline &Polyline::AddPoint(Point point)
    {
        points_.push_back(std::move(point));
        return *this;
    }

    Polyline::~Polyline()
    {
    }

    void Polyline::RenderObject(const RenderContext &context) const
    {
        auto &out = context.out;
        out << "<polyline points=\""sv;
        bool isNotFirst = false;
        for (const auto &point : points_)
        {
            if (isNotFirst)
                out << " "sv;
            out << point.x << ","sv << point.y;
            isNotFirst = true;
        }
        out << "\" />"sv;
    }

    // ---------- Text ------------------

    Text &Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }
    Text &Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }
    Text &Text::SetFontSize(uint32_t size)
    {
        font_size_ = size;
        return *this;
    }
    Text &Text::SetFontFamily(std::string font_family)
    {
        font_family_ = std::move(font_family);
        return *this;
    }
    Text &Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = std::move(font_weight);
        return *this;
    }
    Text &Text::SetData(std::string data)
    {
        data_ = std::move(data);
        return *this;
    }
    void Text::RenderObject(const RenderContext &context) const
    {
        using util::RenderProperties;
        auto &out = context.out;
        out << "<text"sv;
        RenderProperties(out, "x"sv, pos_.x);
        RenderProperties(out, "y"sv, pos_.y);
        RenderProperties(out, "dx"sv, offset_.x);
        RenderProperties(out, "dy"sv, offset_.y);
        RenderProperties(out, "font-size"sv, font_size_);
        if (!font_family_.empty())
            RenderProperties(out, "font-family"sv, font_family_);
        if (!font_weight_.empty())
            RenderProperties(out, "font-weight"sv, font_weight_);
        out << ">"sv;
        util::SwapSpecSymbols(out, data_);
        out << "</text>"sv;
    }

    Text::~Text()
    {
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object> &&obj)
    {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream &out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext ctx{out, 2, 2};
        for (const auto &obj : objects_)
        {
            obj->Render(ctx);
        }
        out << "</svg>"sv;
    }
    Document::~Document() {}

} // namespace svg

namespace util
{
    void SwapSpecSymbols(std::ostream &out, std::string_view data_)
    {
        using namespace std::string_view_literals;
        for (char c : data_)
        {
            switch (c)
            {
            case '&':
                out << "&amp;"sv;
                break;
            case '<':
                out << "&lt;"sv;
                break;
            case '>':
                out << "&gt;"sv;
                break;
            case '\"':
                out << "&quot;"sv;
                break;
            case '\'':
                out << "&apos;"sv;
                break;
            default:
                out << c;
                break;
            }
        }
    }

    template <typename Value>
    void RenderProperties(std::ostream &out, std::string_view propname, Value propvalue)
    {
        using namespace std::literals;
        out << " "sv;
        out << propname;
        out << "=\""sv;
        out << propvalue;
        out << "\""sv;
    }

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
}