#ifndef INFERNO_MODELS_MAP_ITERATORS_HXX
#define INFERNO_MODELS_MAP_ITERATORS_HXX

#include <boost/iterator/transform_iterator.hpp>
#include "inferno/utilities/utilities.hxx"

namespace inferno{
namespace models{

template<class MAP>
class DescriptorToMapConstRef{
typedef typename MAP::const_reference const_reference;
public:
    typedef const_reference result_type;
    DescriptorToMapConstRef(const MAP & map)
    : map_(&map){
    }    
    template<class DESCRIPTOR>
    const_reference operator()(const DESCRIPTOR & d)const{
        return (*map_)[d];
    }
private:
    const MAP * map_;
};


template<class MAP>
class DescriptorToMapRef{
typedef typename MAP::reference reference;
public:
    typedef reference result_type;
    DescriptorToMapRef(MAP & map)
    : map_(&map){
    }    
    template<class DESCRIPTOR>
    reference operator()(const DESCRIPTOR & d){
        return map_[d];
    }
private:
    MAP * map_;
};


template<class MODEL, class MAP>
struct MapIterTypeHelper{
    typedef MAP Map;
    typedef typename MODEL::FactorDescriptorIter FactorDescriptorIter;
    typedef typename MODEL::UnaryDescriptorIter UnaryDescriptorIter;
    typedef typename MODEL::VariableDescriptorIter VariableDescriptorIter;

    typedef DescriptorToMapConstRef<Map> ConstUnaryFunction;
    typedef boost::transform_iterator<ConstUnaryFunction, FactorDescriptorIter>      ConstFactorMapIter;
    typedef boost::transform_iterator<ConstUnaryFunction, UnaryDescriptorIter>       ConstUnaryMapIter;
    typedef boost::transform_iterator<ConstUnaryFunction, VariableDescriptorIter>    ConstVariableMapIter;
    typedef utilities::ConstIteratorRange<ConstFactorMapIter>   ConstFactorMapIterRange;
    typedef utilities::ConstIteratorRange<ConstUnaryMapIter>    ConstUnaryMapIterRange;
    typedef utilities::ConstIteratorRange<ConstVariableMapIter> ConstVariableMapIterRange;

    //typedef DescriptorToMapRef<Map> UnaryFunction;
    //typedef boost::transform_iterator<UnaryFunction, FactorDescriptorIter>      FactorMapIter;
    //typedef boost::transform_iterator<UnaryFunction, UnaryDescriptorIter>       UnaryMapIter;
    //typedef boost::transform_iterator<UnaryFunction, VariableDescriptorIter>    VariableMapIter;
    //typedef utilities::IteratorRange<FactorMapIter>   FactorMapIterRange;
    //typedef utilities::IteratorRange<UnaryMapIter>    UnaryMapIterRange;
    //typedef utilities::IteratorRange<VariableMapIter> VariableMapIterRange;
};


template<class MODEL, class MAP>
typename MapIterTypeHelper<MODEL, MAP>::ConstFactorMapIterRange
factorMapIter(const MODEL & model,const MAP & map){
    // iterators 
    typedef typename MapIterTypeHelper<MODEL, MAP>::ConstFactorMapIterRange Range;
    typedef typename MapIterTypeHelper<MODEL, MAP>::ConstUnaryFunction ConstUnaryFunction;
    auto unary = ConstUnaryFunction(map);
    auto iterBegin = boost::make_transform_iterator(model.factorDescriptorsBegin(),unary);
    auto iterEnd = boost::make_transform_iterator(model.factorDescriptorsEnd(),unary);
    return Range(iterBegin, iterEnd);
}

template<class MODEL, class MAP>
typename MapIterTypeHelper<MODEL, MAP>::ConstFactorMapIterRange
variableMapIter(const MODEL & model,const MAP & map){
    // iterators 
    typedef typename MapIterTypeHelper<MODEL, MAP>::ConstVariableMapIterRange Range;
    typedef typename MapIterTypeHelper<MODEL, MAP>::ConstUnaryFunction ConstUnaryFunction;
    auto unary = ConstUnaryFunction(map);
    auto iterBegin = boost::make_transform_iterator(model.variableDescriptorsBegin(),unary);
    auto iterEnd = boost::make_transform_iterator(model.variableDescriptorsEnd(),unary);
    return Range(iterBegin, iterEnd);
}

//template<class MODEL, class MAP>
//typename MapIterTypeHelper<MODEL, MAP>::FactorMapIterRange
//factorMapIter(const MODEL & model, MAP & map){
//    // iterators 
//    typedef typename MapIterTypeHelper<MODEL, MAP>::FactorMapIterRange Range;
//    typedef  MapIterTypeHelper<MODEL, MAP>::UnaryFunction UnaryFunction;
//    auto unary = UnaryFunction(map);
//    auto iterBegin = boost::make_transform_iterator(model.factorDescriptorsBegin(),unary);
//    auto iterEnd = boost::make_transform_iterator(model.factorDescriptorsEnd(),unary);
//    return Range(iterBegin, iterEnd);
//}

} // end namespace inferno::models
} // inferno

#endif /* INFERNO_MODELS_MAP_ITERATORS_HXX */
