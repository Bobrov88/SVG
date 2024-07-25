#include "svg.h"

namespace svg
{
    using namespace std::literals;
    using util::SwapSpecSymbols;

    void Object::Render(const RenderContext &context) const
    {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
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
        auto &out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline &Polyline::AddPoint(Point point)
    {
        points_.push_back(std::move(point));
        return *this;
    }

    void Polyline::RenderObject(const RenderContext &context) const
    {
        auto &out = context.out;
        out << "<polyline points=\""sv;
        bool isNotFirst = false;
        for (const auto &point : points_)
        {
            if (isNotFirst)
                out << " ";
            out << point.x << ","sv << point.y;
            isNotFirst = true;
        }
        out << "\"/>"sv;
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
        size_ = size;
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
        auto &out = context.out;
        out << "<text x=\"" << pos_.x << "\" y=\"" << pos_.y << "\" dx=\"" << offset_.x << "\" dy=\"" << offset_.y << "\" ";
        out << "font-size=\"" << size_ << "\" font-family=\"" << font_family_ << "\" font-weight=\"" << font_weight_ << "\\>";
        out << SwapSpecSymbols(data_) << "</text>";
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object> &&obj)
    {
        objects_.push_back(std::move(obj));
    }

    void Document::Add(Object &obj)
    {
        AddPtr(std::make_unique<Object>(obj));
    }

    void Document::Render(std::ostream &out) const
    {
        for (const auto &obj : objects_)
        {
            obj->Render(out);
        }
    }

} // namespace svg