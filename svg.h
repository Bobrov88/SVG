#define _USE_MATH_DEFINES
#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <string_view>

namespace svg
{
    class Drawable;
    class ObjectContainer;
    class Object;

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
    class Circle final : public Object
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
    class Polyline final : public Object
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
    class Text final : public Object
    {
        Point pos_ = {0.0, 0.0};
        Point offset_ = {0.0, 0.0};
        uint32_t font_size_ = 1;
        std::string font_family_ = "";
        std::string font_weight_ = "";
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

namespace shapes
{
    class Triangle : public svg::Drawable
    {
        svg::Point p1_ = {0.0, 0.0};
        svg::Point p2_ = {0.0, 0.0};
        svg::Point p3_ = {0.0, 0.0};

    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3);
        void Draw(svg::ObjectContainer &container) const override;
        ~Triangle() override;
    };

    class Star : public svg::Drawable
    {
        svg::Point center_ = {0.0, 0.0};
        double outer_radius_ = 0.0;
        double inner_radius_ = 0.0;
        int num_rays_ = 0;

    public:
        Star(svg::Point center, double outer_radius, double inner_radius, int num_rays);
        void Draw(svg::ObjectContainer &container) const override;
        ~Star() override;
    };

    class Snowman : public svg::Drawable
    {
        svg::Point head_center_ = {0.0, 0.0};
        double head_radius_ = 0.0;

    public:
        Snowman(svg::Point head_center, double head_radius);
        void Draw(svg::ObjectContainer &container) const override;
        ~Snowman() override;
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
void DrawPicture(Container &container, svg::ObjectContainer &target)
{
    DrawPicture(std::begin(container), std::end(container), target);
}

namespace util
{
    void SwapSpecSymbols(std::ostream &out, std::string_view data_);
    template <typename Value>
    void RenderProperties(std::ostream &out, std::string_view propname, Value propvalue);
    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays);
}