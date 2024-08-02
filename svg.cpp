#include "svg.h"

namespace svg
{
    using namespace std::literals;

    std::ostream &operator<<(std::ostream &os, const StrokeLineCap &line_cap)
    {
        using namespace std::string_view_literals;
        switch (line_cap)
        {
        case StrokeLineCap::BUTT:
            os << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            os << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            os << "square"sv;
            break;
        default:
            os << ""sv;
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const StrokeLineJoin &line_cap)
    {
        using namespace std::string_view_literals;
        switch (line_cap)
        {
        case StrokeLineJoin::ARCS:
            os << "arcs"sv;
            break;
        case StrokeLineJoin::BEVEL:
            os << "bevel"sv;
            break;
        case StrokeLineJoin::MITER:
            os << "miter"sv;
            break;
        case StrokeLineJoin::MITER_CLIP:
            os << "miter_clip"sv;
            break;
        case StrokeLineJoin::ROUND:
            os << "round"sv;
            break;
        default:
            os << ""sv;
            break;
        }
        return os;
    }

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
        using render::RenderProperties;
        using namespace std::literals;
        auto &out = context.out;
        out << "<circle"sv;
        RenderProperties(out, "cx"sv, center_.x);
        RenderProperties(out, "cy"sv, center_.y);
        RenderProperties(out, "r"sv, radius_);
        RenderAttr(out);
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
        out << "\"";
        RenderAttr(out);
        out << " />"sv;
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
        using namespace std::literals;
        using render::RenderProperties, render::RenderOptionalProperties;
        auto &out = context.out;
        out << "<text"sv;
        RenderProperties(out, "x"sv, pos_.x);
        RenderProperties(out, "y"sv, pos_.y);
        RenderProperties(out, "dx"sv, offset_.x);
        RenderProperties(out, "dy"sv, offset_.y);
        RenderProperties(out, "font-size"sv, font_size_);
        RenderOptionalProperties(out, "font-family"sv, font_family_);
        RenderOptionalProperties(out, "font-weight"sv, font_weight_);
        RenderAttr(out);
        out << ">"sv;
        render::ReplaceSymbols(out, data_);
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
