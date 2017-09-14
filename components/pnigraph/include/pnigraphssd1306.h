////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnigraphssd1306_h
#define pnigraphssd1306_h

#include "pnigraph.h"
#include "ssd1306.hpp"
#include "driver/gpio.h"
#include "esp_log.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class GraphSsd1306Base : public Graph::Renderer {
    public:
        using Datum = Graph::Datum;
        using Point = Graph::Point;

    protected:
        constexpr static const char* TAG = "pnigraphssd1306.h";
        
        OLED* mOled = 0;
        bool mOledPointerOwned = true;

        virtual void drawOne(Graph* graph, Point const& point) = 0;

    public:
        enum Orientation {
            Vertical,
            Horizontal
        };

        Orientation mOrientation = Vertical;

        GraphSsd1306Base(gpio_num_t scl, gpio_num_t sda, ssd1306_panel_type_t type,
			uint8_t address = 0x78) {
                mOled = new OLED(scl, sda, type, address);
        }

        GraphSsd1306Base(OLED* oled) :
            mOled(oled),
            mOledPointerOwned(false) {
        }

        GraphSsd1306Base(GraphSsd1306Base const& rhs) :
            mOled (rhs.mOled),
            mOledPointerOwned(false),
            mOrientation(rhs.mOrientation),
            mClearColor(rhs.mClearColor),
            mDrawColor(rhs.mDrawColor) {}

        ~GraphSsd1306Base() {
            if(mOledPointerOwned) {
                delete mOled;
            }
        }

        ssd1306_color_t mClearColor = BLACK;
        ssd1306_color_t mDrawColor = WHITE;

        // virtual void draw(Graph* graph) = 0;
        virtual void clearViewport(Graph* graph) {
            mOled->fill_rectangle(
                graph->mViewport.mXOrig,
                graph->mViewport.mYOrig,
                graph->mViewport.mXSize,
                graph->mViewport.mYSize,
                mClearColor
            );
        }

        virtual void draw(Graph* graph) {
            drawMap(graph);
        }

        virtual void drawMap(Graph* graph) {
            graph->map([graph, this](size_t index, Point const& point) {
                auto dst(point);
                drawOne(graph, point);
            });
        }

            // probably only needed if OLED pointer is owned
        void init() {
            ESP_LOGI(TAG, "OLED initialization start");
            if (mOled && mOled->init())
            {
                ESP_LOGI(TAG, "OLED initialized successfully");
                // mOled->clear();
                // mOled->refresh(true);
            } else {
                ESP_LOGE(TAG, "OLED failed to initialize");
                delete mOled;
                mOled = 0;
            }
        }

            // probably only needed if OLED pointer is owned
        void deinit() {
            if(mOled) {
                mOled->term();
            }
        }

        virtual void refresh(Graph* graph, bool force = false) {
            if(mOled) {
                mOled->refresh(force);
            }
        }

        virtual void clearAll(Graph* graph) {
            if(mOled) {
                mOled->clear();
            }
        }

            // Utility methods
        void getStartPos(Graph* graph, Point& point) {
            if(mOrientation == Vertical) {
                point.mYVal = graph->mYAxis.mCenter;
            } else {
                point.mXVal = graph->mXAxis.mCenter;
            }
        }

        template< typename Type>
        static void swap(Type& lhs, Type& rhs) {
            Type tmp = lhs;
            lhs = rhs;
            rhs = tmp;
        }

        template< typename Type>
        static void order(Type& lower, Type& higher) {
            if(lower > higher) {
                swap(lower, higher);
            }
        }

        void fill_rectangle(Point const& srcOrig, Point const& dstOrig) {
            auto src(srcOrig);
            auto dst(dstOrig);

            order(src.mXVal, dst.mXVal);
            order(src.mYVal, dst.mYVal);
            
            dst.mXVal -= src.mXVal;
            dst.mYVal -= src.mYVal;

            mOled->fill_rectangle(src.mXVal.get(), src.mYVal.get(), dst.mXVal.get(), dst.mYVal.get(), mDrawColor);
        }

        void draw_vline(Point const& srcOrig, Point const& dstOrig) {
            auto src(srcOrig);
            auto dst(dstOrig);

            order(src.mXVal, dst.mXVal);
            order(src.mYVal, dst.mYVal);

            mOled->draw_vline(src.mXVal.get(), src.mYVal.get(), (dst.mYVal - src.mYVal).get(), mDrawColor);            
        }

        void draw_hline(Point const& srcOrig, Point const& dstOrig) {
            auto src(srcOrig);
            auto dst(dstOrig);

            order(src.mXVal, dst.mXVal);
            order(src.mYVal, dst.mYVal);

            mOled->draw_hline(src.mXVal.get(), src.mYVal.get(), (dst.mXVal - src.mXVal).get(), mDrawColor);            
        }
    
};

class GraphSsd1306BarLines : public GraphSsd1306Base {
    public:
        // GraphSsd1306BarLines(gpio_num_t scl, gpio_num_t sda, ssd1306_panel_type_t type,
        //     uint8_t address = 0x78) : GraphSsd1306Base(scl, sda, type, address) {}

        using GraphSsd1306Base::GraphSsd1306Base;

        virtual void drawOne(Graph* graph, Point const& dstOrig) {
            auto src(dstOrig);
            getStartPos(graph, src);
            graph->xformPoint(src);

            auto dst(dstOrig);
            graph->xformPoint(dst);            

            if(mOrientation == Vertical) {
                draw_vline(src, dst);
            } else {
                draw_hline(src, dst);
            }
        }
};

class GraphSsd1306BarRect : public GraphSsd1306Base {
public:
    // GraphSsd1306BarLines(gpio_num_t scl, gpio_num_t sda, ssd1306_panel_type_t type,
    //     uint8_t address = 0x78) : GraphSsd1306Base(scl, sda, type, address) {}

    using GraphSsd1306Base::GraphSsd1306Base;

    Datum mWidth = { 5 };

    virtual void drawOne(Graph* graph, Point const& dstOrig) {
        auto src(dstOrig);
        graph->xformPoint(src);

        auto dst(dstOrig);        
        getStartPos(graph, dst);
        graph->xformPoint(dst);

        if(mOrientation == Vertical) {
            dst.mXVal += mWidth;
        } else {
            dst.mYVal += mWidth;
        }

        fill_rectangle(src, dst);
    }
};



////////////////////////////////////////////////////////////////////

} // end namespace pni

////////////////////////////////////////////////////////////////////

#endif // pnigraphssd1306_h
