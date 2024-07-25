#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace svg
{

    struct Point
    {
        Point() = default;
        Point(double x, double y)
            : x(x), y(y)
        {
        }
        double x = 0;
        double y = 0;
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
    public:
        Circle &SetCenter(Point center);
        Circle &SetRadius(double radius);

    private:
        void RenderObject(const RenderContext &context) const override;
        Point center_ = {0.0, 0.0};
        double radius_ = 1.0;
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
        Polyline &AddPoint(Point point);

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
        uint32_t size_ = 1;
        std::string font_family_ = "";
        std::string font_weight_ = "";
        std::string data_ = "";
        void RenderObject(const RenderContext &context) const override;
    public:
        Text &SetPosition(Point pos);
        Text &SetOffset(Point offset);
        Text &SetFontSize(uint32_t size);
        Text &SetFontFamily(std::string font_family);
        Text &SetFontWeight(std::string font_weight);
        Text &SetData(std::string data);
    };

    class Document
    {
        std::vector<std::unique_ptr<Object>> objects_;

    public:
        Document() = default;
        void AddPtr(std::unique_ptr<Object> &&obj);
        void Add(Object &obj);
        // Выводит в ostream svg-представление документа
        void Render(std::ostream &out) const;
        // Прочие методы и данные, необходимые для реализации класса Document
    };

} // namespace svg

namespace util
{
    std::string SwapSpecSymbols(const std::string &data_)
    {
        std::string quoted_string {data_.cbegin(), data_.cend()};
        quoted_string = quoted_string.replace(quoted_string.begin(), quoted_string.end(), "&", "&amp;");
        quoted_string = quoted_string.replace(quoted_string.begin(), quoted_string.end(), "\"", "&quot;");
        quoted_string = quoted_string.replace(quoted_string.begin(), quoted_string.end(), "'", "&apos;");
        quoted_string = quoted_string.replace(quoted_string.begin(), quoted_string.end(), "<", "&lt;");
        quoted_string = quoted_string.replace(quoted_string.begin(), quoted_string.end(), ">", "&gt;");
        return quoted_string;
    }
}