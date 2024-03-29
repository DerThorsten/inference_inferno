#ifndef INFERNO_LEARNING_WEIGHTS_HXX
#define INFERNO_LEARNING_WEIGHTS_HXX

// std
#include <map>

// boost
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

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

        // assignment operators
        Weights & operator = (const Weights & other){
            if(&other != this){
                this->reshape(other.shape());
                for(uint64_t i=0; i<other.shape(0); ++i){
                    (*this)[i] = other[i];
                }
            }
            return *this;
        }
        Weights & operator = (const WeightType & scalar){
            for(uint64_t i=0; i<this->shape(0); ++i){
                (*this)[i] = scalar;
            }
            return *this;
        }


        template<class WEIGHT_CONSTRAINTS>
        WeightType getNorm(
            size_t normType,
            const WEIGHT_CONSTRAINTS & wConstraints
        )const{
            ValueType l = 0;
            if(normType == 1){
                uint64_t wi = 0;
                for(const auto w : (*this) ){
                    if(!wConstraints.isFixed(wi)){
                        l += std::abs(w);
                    }
                    ++wi;
                }
            }
            else if(normType == 2){
                uint64_t wi = 0;
                for(const auto w : (*this) ){
                    if(!wConstraints.isFixed(wi)){
                        l += w*w;
                    }
                    ++wi;
                }
            }
            else{
                uint64_t wi = 0;
                for(const auto w : (*this) ){
                    if(!wConstraints.isFixed(wi)){
                        l += std::pow(std::abs(w), normType);
                    }
                    ++wi;
                }
            }
            return std::sqrt(l);
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


        template<class F, class WEIGHT_CONSTRAINTS>
        void pertubate(
            const WeightVector & source, 
            WeightMatrix & noise,
            const WEIGHT_CONSTRAINTS & weightConstraints,
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
                }

                // fix bounded weights
                for(const auto kv : weightConstraints.weightBounds()){
                    const auto wi = kv.first;
                    const auto lowerBound = kv.second.first;
                    const auto upperBound = kv.second.second;
                    if(w[wi] < lowerBound){
                       w[wi] = lowerBound; 
                       n[wi] = w[wi] - source[wi];
                    }
                    if(w[wi] > upperBound){
                       w[wi] = upperBound; 
                       n[wi] = w[wi] - source[wi];
                    }
                }

            }
        }

        template<class MEANS, class VARS, class RND_GEN>
        void gaussianWeights(
            const MEANS & means,
            const VARS & vars,
            RND_GEN & randGen
        ){
            // nWeights 
            auto nPertubation = this->size();
            auto nWeights = this->operator[](0).size();

            INFERNO_CHECK_OP(means.size(), == ,nWeights,"");
            INFERNO_CHECK_OP(vars.size(),  == ,nWeights,"");

            for(size_t w=0; w<nWeights; ++w){

                boost::normal_distribution<> nd(means[w], vars[w]);
                boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > normalDist(randGen, nd);

                for(size_t n=0; n<nPertubation; ++n){
                    this->operator[](n)[w] = normalDist();
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
