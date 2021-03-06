////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnigraph_h
#define pnigraph_h

#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
#include "pnifixedpoint.h"
#include "esp_log.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Graph {

        constexpr static const char* TAG = "pnigraph";

    public:
        static const size_t IntBits = 10;
        static const size_t FracBits = 5;
        using Datum = FixedPoint< int32_t, IntBits, FracBits >;
        using DataPair = std::pair< Datum, Datum >;
        using Data = std::vector< Datum >;

        struct Point {
            Datum mXVal;
            Datum mYVal;
        };

        struct Axis {
            enum DynamicRange {
                Static,
                Dynamic,
                DynamicPeak
            };

            Data mData;
            Datum mSrcMin = 0;
            Datum mSrcMax = 0;
            Datum mCenter = 0;

            DataPair getMinMax() const {
                auto mm = std::minmax_element(mData.begin(), mData.end());
                return DataPair(*(mm.first), *(mm.second));
            }

            void setRange(Datum const& minVal, Datum const& centerVal, Datum const& maxVal) {
                mSrcMin = minVal; mCenter = centerVal; mSrcMax = maxVal;
            }

            void fillAsIndices(Datum start = 0) {
                auto end = mData.size();
                for(auto num = 0; num < end; ++num ) {
                    mData[ num ] = start + num;
                }
            }

            void updateDynamicRange(DynamicRange type) {
                auto range = getMinMax();
                if(type == Dynamic) {
                    mSrcMin = range.first;
                    mSrcMax = range.second;
                } else if (type == DynamicPeak) {
                    mSrcMin = std::min(mSrcMin, range.first);
                    mSrcMax = std::max(mSrcMax, range.second);
                }
            }
        };

        struct Viewport {
            size_t mXOrig;
            size_t mYOrig;
            size_t mXSize;
            size_t mYSize;
        };

        class Renderer {
            public:
                virtual void draw(Graph* graph) = 0;
                virtual void clearViewport(Graph* graph) = 0;
                virtual void clearAll(Graph* graph) = 0;
                virtual void refresh(Graph* graph, bool force = false) = 0;
        };

            // Dirty, dirty public data
        Axis mXAxis;
        Axis mYAxis;
        Viewport mViewport;
        Renderer* mRenderer = 0;
        Axis::DynamicRange mUseDynamicRange = Axis::Static;

        template< typename Type >
        void resize(size_t num, Type yVal) {
            mXAxis.mData.resize(num);
            mYAxis.mData.resize(num, yVal);
            mXAxis.fillAsIndices();
        }

        void draw() {
            if(mUseDynamicRange != Axis::Static) {
                // mXAxis.updateDynamicRange();
                mYAxis.updateDynamicRange(mUseDynamicRange);
            }
            mRenderer->draw(this);
        }

        void clearViewport() {
            mRenderer->clearViewport(this);
        }

        void clearAll() {
            mRenderer->clearAll(this);
        }

        void refresh(bool force = false) {
            mRenderer->refresh(this, force);
        }

        using MapFunc = std::function<void(size_t index, Point const& point)>;
        void map(MapFunc func) {
            assert(mXAxis.mData.size() == mYAxis.mData.size());
            for(size_t num = 0; num < mXAxis.mData.size(); ++num) {
                Point tmp = {mXAxis.mData[ num ], mYAxis.mData[ num ]};
                func(num, tmp);;
            }
        }

            // TODO: Should just have utility function that xforms one dimension,
            // and re-use that for x and y
        bool xformPoint(Point& point) {
            bool ret = true;

            // ESP_LOGI(TAG, "point = %d, %d", point.mXVal.get(), point.mYVal.get());

            auto xSrcSize = mXAxis.mSrcMax - mXAxis.mSrcMin;
            auto ySrcSize = mYAxis.mSrcMax - mYAxis.mSrcMin;

            // ESP_LOGI(TAG, "  y min max = %d, %d",  mYAxis.mSrcMin.get(),  mYAxis.mSrcMax.get());
            
            // ESP_LOGI(TAG, "  src size = %d, %d", xSrcSize.get(), ySrcSize.get());
            
            if( xSrcSize != 0 && ySrcSize != 0) {
                    // TODO: This clamps, but maybe we want to cull points instead.
                point.mXVal.clamp(mXAxis.mSrcMin, mXAxis.mSrcMax);
                point.mYVal.clamp(mYAxis.mSrcMin, mYAxis.mSrcMax);

                // ESP_LOGI(TAG, "  point 1 = %d, %d", point.mXVal.get(), point.mYVal.get());
                
                point.mXVal -= mXAxis.mSrcMin;
                point.mXVal *= mViewport.mXSize;
                point.mXVal /= xSrcSize;
                point.mXVal += mViewport.mXOrig;
    
                point.mYVal -= mYAxis.mSrcMin;
                point.mYVal *= mViewport.mYSize;
                point.mYVal /= ySrcSize;
                point.mYVal += mViewport.mYOrig;

                // ESP_LOGI(TAG, "  point 2 = %d, %d", point.mXVal.get(), point.mYVal.get());
            } else {
                ret = false;
            }
            
            return ret;
        }

    protected:

    private:

};

////////////////////////////////////////////////////////////////////

} // end namespace pni

////////////////////////////////////////////////////////////////////

#endif // pnigraph_h
