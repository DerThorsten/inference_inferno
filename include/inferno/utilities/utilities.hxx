#ifndef INFERNO_UTILITIES_UTILITIES_HXX
#define INFERNO_UTILITIES_UTILITIES_HXX



namespace inferno{
namespace utilities{
namespace line_search{


template<class T = size_t>
class IndexVector :std::vector<T>{
    IndexVector(T size){
        for(T i=0; i<size; ++i)
            (*this)[i] = i;
    }
    void reset(){
        for(T i=0; i<size; ++i)
            (*this)[i] = i;
    }


    void randomShuffle(){
        std::random_shuffle(this->begin(), this->end());
    }
};


} // end namespace inferno::utilities::line_search
} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_UTILITIES_HXX*/
