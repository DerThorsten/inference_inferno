/** \file general_discrete_tl_model.hxx 
    \brief  functionality for inferno::GeneralDiscreteGraphicalModel is implemented in this header.
*/
#ifndef INFERNO_MODEL_GENERAL_DISCRETE_TL_MODEL_HXX
#define INFERNO_MODEL_GENERAL_DISCRETE_TL_MODEL_HXX


#include <boost/iterator/counting_iterator.hpp>

#include "inferno/inferno.hxx"
#include "inferno/model/base_discrete_model.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"



namespace inferno{


template <class T, class Tuple>
struct Index;

template <class T, class... Types>
struct Index<T, std::tuple<T, Types...> > {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct Index<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + Index<T, std::tuple<Types...> >::value;
};


constexpr int getSaveIndex( int i, int NFunctionTypes)
{
    return i< NFunctionTypes ? i : 0 ;
}

/// \cond
namespace detail_tl_model{
namespace functors{
    struct ValueTable{
        ValueTable(){}
        typedef const value_tables::DiscreteValueTableBase * result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return &vt;
        }
    };
    struct EvalN{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval(l_);
        }
        const DiscreteLabel * l_;
    };
    struct BufferShape{
        typedef void result_type;
        template<class VT>
        result_type operator()(const VT & vt){
            return vt.bufferShape(s_);
        }
        DiscreteLabel * s_;
    };
    struct BufferValueTable{
        typedef void result_type;
        template<class VT>
        result_type operator()(const VT & vt){
            return vt.bufferValueTable(v_);
        }
        ValueType * v_;
    };
    struct Eval1{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval1(l0_);
        }
        const DiscreteLabel  l0_;
    };
    struct Eval2{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval2(l0_, l1_);
        }
        const DiscreteLabel  l0_,l1_;
    };
    struct Eval3{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval3(l0_, l1_, l2_);
        }
        const DiscreteLabel  l0_,l1_,l2_;
    };
    struct Eval4{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval4(l0_, l1_, l2_, l3_);
        }
        const DiscreteLabel  l0_,l1_,l2_,l3_;
    };
    struct Eval5{
        typedef ValueType result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.eval5(l0_, l1_, l2_, l3_, l4_);
        }
        const DiscreteLabel  l0_,l1_,l2_,l3_,l4_;
    };
    struct IsPotts{
        typedef bool result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.isPotts(beta);
        }
        ValueType & beta;
    };
    struct Size{
        Size(){}
        typedef uint64_t result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.size();
        }
    };
    struct IsGeneralizedPotts{
        IsGeneralizedPotts(){}
        typedef bool result_type;
        template<class VT>
        result_type operator()(const VT & vt)const{
            return vt.isGeneralizedPotts();
        }
    };

}

}
/// \endcond





template<class MODEL>
class TlModelFactor  : public DiscreteFactorBase<TlModelFactor<MODEL> > {
public:
    TlModelFactor(
        const MODEL *  model, 
        const uint32_t arity, 
        const uint64_t visOffset,
        const uint8_t  vtTypePos,
        const uint64_t vtIndex
    )
    :   
        model_(model),
        arity_(arity),
        visOffset_(visOffset),
        vtTypePos_(vtTypePos),
        vtIndex_(vtIndex){
    }

    uint32_t arity()const{
        return arity_;
    }

    Vi vi(const size_t d)const{
        return model_->factorsVi_[visOffset_ + d];
    }

    DiscreteLabel shape(const size_t d)const{
        return model_->nLabels(model_->factorsVi_[visOffset_ + d]);
    }

    const value_tables::DiscreteValueTableBase * valueTable()const{
        const detail_tl_model::functors::ValueTable f;
        return callFunctor(f);
    }
    bool size()const{
        const detail_tl_model::functors::Size f;
        return size(f);
    }
    bool isPotts(ValueType & beta)const{
        const detail_tl_model::functors::IsPotts f = {beta};
        return callFunctor(f);
    }
    bool isGeneralizedPotts()const{
        const detail_tl_model::functors::IsGeneralizedPotts f;
        return callFunctor(f);
    }

    void bufferShape(DiscreteLabel * buffer)const{
        detail_tl_model::functors::BufferShape f = {buffer};
        return callFunctor(f);
    }

    void bufferValueTable(ValueType * buffer)const{
        detail_tl_model::functors::BufferValueTable f = {buffer};
        return callFunctor(f);
    }
    ValueType eval(const DiscreteLabel * conf)const{
        const detail_tl_model::functors::EvalN f = {conf};
        return callFunctor(f);
    }
    ValueType eval1(const DiscreteLabel l0)const{
        const detail_tl_model::functors::Eval1 f{l0};
        return callFunctor(f);
    }
    ValueType eval2(const DiscreteLabel l0, const DiscreteLabel l1)const{
        const detail_tl_model::functors::Eval2 f{l0,l1};
        return callFunctor(f);
    }
    ValueType eval3(const DiscreteLabel l0, const DiscreteLabel l1,
                    const DiscreteLabel l2)const{
        const detail_tl_model::functors::Eval3 f{l0,l1,l2};
        return callFunctor(f);
    }
    ValueType eval4(const DiscreteLabel l0, const DiscreteLabel l1,
                    const DiscreteLabel l2, const DiscreteLabel l3)const{
        const detail_tl_model::functors::Eval4 f{l0,l1,l2,l3};
        return callFunctor(f);
    }
    ValueType eval4(const DiscreteLabel l0, const DiscreteLabel l1,
                    const DiscreteLabel l2, const DiscreteLabel l3,
                    const DiscreteLabel l4)const{
        const detail_tl_model::functors::Eval5 f{l0,l1,l2,l3,l4};
        return callFunctor(f);
    }


private:
    template<class F>
    typename F::result_type callFunctor(F & f)const{
        return model_->callFunctorImpl(vtTypePos_, vtIndex_, f);
    }
    template<class F>
    typename F::result_type callFunctor(const F & f)const{
        return model_->callFunctorImpl(vtTypePos_, vtIndex_, f);
    }


    const MODEL * model_;
    uint32_t      arity_;
    uint64_t      visOffset_;
    uint8_t       vtTypePos_;
    uint64_t      vtIndex_;
};





template <typename... VALUE_TABLE>
struct TlModel : public DiscreteGraphicalModelBase<TlModel<VALUE_TABLE ... > > 
{
    typedef TlModelFactor<TlModel<VALUE_TABLE ...> > FactorType;
    typedef const FactorType * FactorProxy;
    friend class TlModelFactor<TlModel<VALUE_TABLE ...> >;

    template< class M, unsigned TL_SIZE>
    friend class WrapValueTables;

public:
    typedef boost::counting_iterator<uint64_t> FactorIdIter;
    typedef boost::counting_iterator<Vi> VariableIdIter;

    const static bool SortedVariableIds = true;
    const static bool SortedFactorIds = true;

    typedef std::pair<uint8_t, uint64_t> ValueTableId;

    typedef std::tuple< VALUE_TABLE ... >           TupleOfValueTables;
    typedef std::tuple<std::vector<VALUE_TABLE>...> TupleOfValueTableVectors;
    static const std::size_t NFunctionTypes = std::tuple_size<TupleOfValueTableVectors>::value;

    
    TlModel(const uint64_t nVar = 0, const DiscreteLabel nLabels = 0)
    :   
        nVar_(nVar),
        maxArity_(0),
        nLabels_(1,nLabels),
        tupleOfVtVecs_(),
        factorsVi_(),
        factors_(){
    }

    FactorIdIter factorIdsBegin()const{
        return FactorIdIter(0);
    }
    FactorIdIter factorIdsEnd()const{
        return FactorIdIter(factors_.size());
    }
    VariableIdIter variableIdsBegin()const{
        return VariableIdIter(0);
    }
    VariableIdIter variableIdsEnd()const{
        return VariableIdIter(nVar_);
    }


    DiscreteLabel nLabels(const Vi vi)const{
        return vi<nLabels_.size() ? nLabels_[vi] : nLabels_[0];
    }

    template<class VT>
    ValueTableId addValueTable(const VT & vt){

        auto & vtVec = std::get< Index<VT, TupleOfValueTables>::value >(tupleOfVtVecs_);
        ValueTableId vtid;
        vtid.first  = Index<VT, TupleOfValueTables>::value;
        vtid.second = vtVec.size();
        vtVec.push_back(vt);
        return vtid;
    }

    template<class T>
    Fi addFactor(const ValueTableId & vti,std::initializer_list<T> list){
        return addFactor(vti, list.begin(), list.end());
    }

    template<class ITER>
    Fi addFactor(const ValueTableId & vti, ITER visBegin, ITER visEnd){
        const uint64_t visOffset = factorsVi_.size();
        uint32_t arity = 0;
        while(visBegin!=visEnd){
            factorsVi_.push_back(*visBegin);
            ++visBegin;
            ++arity;
        }
        maxArity_ = std::max(maxArity_, arity);
        factors_.push_back( FactorType(this,arity, visOffset, vti.first, vti.second ));
        return factors_.size()-1;
    }

    FactorProxy operator[](const Fi fi)const{
        return  & (factors_[fi]);
    }

    uint32_t maxArity()const{
        return maxArity_;
    }
    template<class CONFIG>
    double eval(const CONFIG  & conf)const{
        double sum = 0.0;
        switch(maxArity_){
            case 1 :{
                for(size_t i=0; i<factors_.size(); ++i){
                    const FactorType & fac = factors_[i];
                    sum += fac.eval1(conf[fac.vi(0)]);
                }
                return sum;
            }
            case 2 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const FactorType & fac = factors_[i];
                    switch(fac.arity()){
                        case 1:
                            sum+= fac.eval1(conf[fac.vi(0)]);
                            break;
                        case 2:
                            sum+= fac.eval2(conf[fac.vi(0)],conf[fac.vi(1)]);
                            break;
                    }
                }
                return sum;
            }
            case 3 : {
                for(size_t i=0; i<factors_.size(); ++i){
                    const FactorType & fac = factors_[i];
                    const uint32_t arity = fac.arity();
                    switch(arity){
                        case 1:
                            sum+= fac.eval1(conf[fac.vi(0)]);
                            break;
                        case 2:
                            sum+= fac.eval2(conf[fac.vi(0)],conf[fac.vi(1)]);
                            break;
                        case 3:
                            sum+= fac.eval3(conf[fac.vi(0)],conf[fac.vi(1)],conf[fac.vi(2)]);
                            break;
                    }
                }
                return sum;
            }
            default : {
                std::vector<DiscreteLabel> confBuffer(maxArity_);
                for(size_t i=0; i<factors_.size(); ++i){
                    const FactorType & fac = factors_[i];
                    fac.getFactorConf(conf, confBuffer.begin());
                    sum += fac.eval(confBuffer.data());
                }
                return sum;
            }
        }
    }
private:
    template<unsigned int POS>
    const typename std::tuple_element<POS, TupleOfValueTables>::type &
    getValueTable(
        const uint64_t vtIndex
    )const{
        return std::get<getSaveIndex(POS,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
    }



    template<class FUNCTOR>
    typename FUNCTOR::result_type 
    callFunctorImpl(
        const uint8_t vtTypePos,
        const  uint64_t vtIndex, 
        FUNCTOR & functor
    )const{
        return vtTypePos == 0 ? functor(this-> template getValueTable<0>(vtIndex) ) :
                                functor(this-> template getValueTable<1>(vtIndex) );

        //switch(vtTypePos){
        //    case 0 :{
        //        const auto & vt = std::get<getSaveIndex(0,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //        return functor(vt);
        //    }
        //    case 1 :{
        //        const auto & vt = std::get<getSaveIndex(1,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //        return functor(vt);
        //    }
        //    //case 2 :{
        //    //    const auto & vt = std::get<getSaveIndex(2,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 3 :{
        //    //    const auto & vt = std::get<getSaveIndex(3,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 4 :{
        //    //    const auto & vt = std::get<getSaveIndex(4,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 5 :{
        //    //    const auto & vt = std::get<getSaveIndex(5,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //}
    }

    template<class FUNCTOR>
    typename FUNCTOR::result_type 
    callFunctorImpl(
        const uint8_t vtTypePos,
        const  uint64_t vtIndex, 
        const FUNCTOR & functor
    )const{
        return vtTypePos == 0 ? functor(std::get<getSaveIndex(0,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex]) :
                                functor(std::get<getSaveIndex(1,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex]);
        //switch(vtTypePos){
        //    case 0 :{
        //        const auto & vt = std::get<getSaveIndex(0,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //        return functor(vt);
        //    }
        //    case 1 :{
        //        const auto & vt = std::get<getSaveIndex(1,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //        return functor(vt);
        //    }
        //    //case 2 :{
        //    //    const auto & vt = std::get<getSaveIndex(2,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 3 :{
        //    //    const auto & vt = std::get<getSaveIndex(3,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 4 :{
        //    //    const auto & vt = std::get<getSaveIndex(4,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //    //case 5 :{
        //    //    const auto & vt = std::get<getSaveIndex(5,NFunctionTypes)>(tupleOfVtVecs_)[vtIndex];
        //    //    return functor(vt);
        //    //}
        //}
    }


    uint64_t                    nVar_;
    uint32_t                    maxArity_;
    std::vector<DiscreteLabel>  nLabels_;
    TupleOfValueTableVectors    tupleOfVtVecs_;
    std::vector<Vi>             factorsVi_;
    std::vector< FactorType >   factors_;
};


}

#endif 
