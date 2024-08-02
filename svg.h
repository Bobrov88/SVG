#define _USE_MATH_DEFINES
#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string_view>
#include <optional>

using Color = std::string;
inline const Color NoneColor{"none"};

namespace svg
{
    namespace render
    {
        inline void Render(std::ostream &out, std::string_view data_)
        {
            out << data_;
        }

        template <typename Value>
        inline void Render(std::ostream &out, std::string_view propname, Value propvalue)
        {
            using namespace std::literals;
            out << " "sv << propname << "=\""sv << propvalue << "\""sv;
        }

        inline void ReplaceSymbols(std::ostream &out, std::string_view data_)
        {
            using namespace std::string_view_literals;
            for (auto c : data_)
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
        inline void RenderOptionalProperties(std::ostream &out, std::string_view propname, Value propvalue)
        {
            if (propvalue)
            {
                Render(out, propname, *propvalue);
            }
        }

        template <typename Value>
        inline void RenderProperties(std::ostream &out, std::string_view propname, Value propvalue)
        {
            if (propvalue)
            {
                Render(out, propname, propvalue);
            }
        }
    }

    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE
    };

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND
    };

    class Drawable;
    class ObjectContainer;
    class Object;
    template <typename Owner>
    class PathProps;

    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer &container) const = 0;
        virtual ~Drawable() = default;
    };

    class ObjectContainer
    {
    public:
        template <typename ObjType>
        void Add(ObjType obj)
        {
            AddPtr(std::make_unique<ObjType>(std::move(obj)));
        }
        virtual void AddPtr(std::unique_ptr<Object> &&) = 0;
        virtual ~ObjectContainer() = default;
    };

    struct Point
    {
        Point() = default;
        Point(double x, double y)
            : x(x), y(y)
        {
        }
        double x = 0.0;
        double y = 0.0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext
    {
        RenderContext(std::ostream &out)
            : out(out)
        {
        }

        RenderContext(std::ostream &out, int indent_step, int indent = 0)
            : out(out), indent_step(indent_step), indent(indent)
        {
        }

        RenderContext Indented() const
        {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const
        {
            for (int i = 0; i < indent; ++i)
            {
                out.put(' ');
            }
        }

        std::ostream &out;
        int indent_step = 0;
        int indent = 0;
    };

    template <typename Owner>
    class PathProps
    {
        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
        Owner &AsOwner()
        {
            return static_cast<Owner &>(*this);
        }

    public:
        Owner &SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner &SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return AsOwner();
        }
        Owner &SetStrokeWidth(double width)
        {
            stroke_width_ = width;
            return AsOwner();
        }
        Owner &SetStrokeLineCap(StrokeLineCap line_cap)
        {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }
        Owner &SetsTrokeLineJoin(StrokeLineJoin line_join)
        {
            stroke_line_join_ = line_join;
            return AsOwner();
        }
        virtual ~PathProps() = default;

    protected:
        void RenderAttr(std::ostream &out) const
        {
            using namespace std::string_view_literals;
            using render::RenderOptionalProperties;
            RenderOptionalProperties(out, "fill"sv, fill_color_);
            RenderOptionalProperties(out, "stroke"sv, stroke_color_);
            RenderOptionalProperties(out, "stroke-width"sv, stroke_width_);
            RenderOptionalProperties(out, "stroke-linecap"sv, stroke_line_cap_);
            RenderOptionalProperties(out, "stroke-linejoin"sv, stroke_line_join_);
        }
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object
    {
    public:
        void Render(const RenderContext &context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext &context) const = 0;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle>
    {
        void RenderObject(const RenderContext &context) const override;
        Point center_ = {0.0, 0.0};
        double radius_ = 1.0;

    public:
        Circle() = default;
        Circle &SetCenter(Point center);
        Circle &SetRadius(double radius);
        ~Circle() override;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline>
    {
        std::vector<Point> points_;
        void RenderObject(const RenderContext &context) const override;

    public:
        Polyline() = default;
        Polyline &AddPoint(Point point);
        ~Polyline() override;
        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text>
    {
        Point pos_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t font_size_ = 1;
        std::optional<std::string> font_family_;
        std::optional<std::string> font_weight_;
        std::string data_ = "";
        void RenderObject(const RenderContext &context) const override;

    public:
        Text() = default;
        Text &SetPosition(Point pos);
        Text &SetOffset(Point offset);
        Text &SetFontSize(uint32_t size);
        Text &SetFontFamily(std::string font_family);
        Text &SetFontWeight(std::string font_weight);
        Text &SetData(std::string data);
        ~Text() override;
    };

    class Document : public ObjectContainer
    {
        std::vector<std::unique_ptr<Object>> objects_;

    public:
        Document() = default;
        void AddPtr(std::unique_ptr<Object> &&obj) override;
        void Render(std::ostream &out) const;
        ~Document() override;
    };

} // namespace svg

template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer &target)
{
    for (auto it = begin; it != end; ++it)
    {
        (*it)->Draw(target);
    }
}

template <typename Container>
void DrawPicture(Container &container, svg::ObjectContainer &target)
{
    DrawPicture(std::begin(container), std::end(container), target);
}