#ifndef INFERNO_UTILITIES_INDEX_VECTOR_HXX
#define INFERNO_UTILITIES_INDEX_VECTOR_HXX



namespace inferno{
namespace utilities{


template<class T = size_t>
class IndexVector : public std::vector<T>{
public:
    IndexVector(T n)
    : std::vector<T>(n){
        for(T i=0; i<n; ++i)
            (*this)[i] = i;
    }
    void reset(){
        for(T i=0; i<this->size(); ++i)
            (*this)[i] = i;
    }


    void randomShuffle(){
        std::random_shuffle(this->begin(), this->end());
    }
};


} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_INDEX_VECTOR_HXX*/
