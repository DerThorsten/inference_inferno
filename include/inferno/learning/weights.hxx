#ifndef INFERNO_LEARNING_WEIGHTS_HXX
#define INFERNO_LEARNING_WEIGHTS_HXX

// vigra
#include <vigra/multi_array.hxx>

// inferno
#include "inferno/inferno.hxx"

namespace inferno{
namespace learning{




    /** \brief Weights class holding weights for structured learning
    */
    class Weights : public vigra::MultiArray<1, WeightType>{
    private:
        typedef vigra::MultiArray<1, WeightType> BaseType;
    public:
        typedef uint32_t WeightDescriptor;
        Weights(const size_t nWeights = 0 , const WeightType initValue = WeightType())
        : BaseType(typename BaseType::difference_type(nWeights), initValue){

        }

    private:

    };

    typedef Weights WeightVector;
    /** \brief Weights class holding weights for structured learning
    */
    class WeightMatrix : public std::vector< WeightVector >{
    private:
        typedef std::vector< WeightVector > BaseType;
    public:
        typedef uint32_t WeightDescriptor;

        WeightMatrix(const WeightVector & weightVector, const size_t arrayLength = 0)
        : BaseType(arrayLength, weightVector){

        }


        template<class F>
        void pertubate(
            const WeightVector & source, 
            WeightMatrix & noise,
            F && functor
        ){
            INFERNO_CHECK_OP(this->size(),==, noise.size(),"");

            for(size_t i=0; i<noise.size(); ++i){
                auto & w = (*this)[i];
                auto & n = noise[i];
                INFERNO_CHECK_OP(source.size(),==, n.size(),"");
                INFERNO_CHECK_OP(w.size(),== ,n.size(),"");
                for(size_t wi=0; wi<w.size(); ++wi){
                    n[wi] = functor();
                    w[wi] = source[wi] + n[wi];
                }
            }
        }

        template<class WEIGHTS>
        void weightedSum(const WEIGHTS & w, WeightVector & sum){
            INFERNO_CHECK_OP(sum.size(), != ,0,"");
            INFERNO_CHECK_OP(w.size(), == , this->size(), "wrong sizes");
            for(size_t i=0; i<this->size(); ++i){
                auto & thisW  = (*this)[i];
                INFERNO_CHECK_OP(sum.size(), == , thisW.size(), "wrong sizes");

                for(size_t wi=0; wi<thisW.size(); ++wi){
                    sum[wi] += w[i]*thisW[wi];
                }
            }
        }

    private:

    };

} // end namespace inferno::learning
} // end namespace inferno

#endif /*INFERNO_LEARNING_WEIGHTS_HXX*/
