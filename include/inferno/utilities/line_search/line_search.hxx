#ifndef INFERNO_UTILITIES_LINE_SEARCH_LINE_SEARCH_HXX
#define INFERNO_UTILITIES_LINE_SEARCH_LINE_SEARCH_HXX



namespace inferno{
namespace utilities{
namespace line_search{


template<class ARG_TYPE, class VALUE_TYPE>
class BinarySearch{
public:
    typedef ARG_TYPE ArgType;
    struct Options{
    };
    BinarySearch(const Options options = Options())
    : options_(options){
    }

    template<class F>
    ArgType operator()(F && f, const ArgType lowerBound, const ArgType upperBound) 
    {
        
    }
private:
    Options options_;
};

} // end namespace inferno::utilities::line_search
} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_LINE_SEARCH_LINE_SEARCH_HXX*/
