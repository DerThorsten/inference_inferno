#ifndef INFERNO_UTILITIES_LINE_SEARCH_HXX
#define INFERNO_UTILITIES_LINE_SEARCH_HXX



namespace inferno{
namespace utilities{
namespace line_search{


class BinarySearch{
public:
    struct Options{
    };
    BinarySearch(const Options options = Options())
    : options_(options){
    }

    template<class F>
    std::result_of<F()> operator()(F && f){

    }
private:
    Options options_;
};

} // end namespace inferno::utilities::line_search
} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_LINE_SEARCH_HXX*/
