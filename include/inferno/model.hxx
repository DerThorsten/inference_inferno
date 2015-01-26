#ifndef INFERNO_MODEL_HXX
#define INFERNO_MODEL_HXX

#include "inferno/inferno.hxx"
#include "inferno/discrete_value_table.hxx"
#include "inferno/small_vector.hxx"
namespace inferno{


template<class FACTOR >
class DiscreteFactorBase{
public:
    ValueType operator()(const LabelType * conf)const{
        return factor()->valueTable()->eval(conf);
    }
    ValueType operator()(const LabelType l0)const{
        return factor()->valueTable()->eval(l0);
    }

    ValueType operator()(const LabelType l0, 
                         const LabelType l1)const{
        return factor()->valueTable()->eval(l0, l1);
    }

    ValueType operator()(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2)const{
        return factor()->valueTable()->eval(l0, l1, l2);
    }

    ValueType operator()(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2, 
                         const LabelType l3)const{
        return factor()->valueTable()->eval(l0, l1, l2, l3);
    }

    ValueType operator()(const LabelType l0, 
                         const LabelType l1, 
                         const LabelType l2, 
                         const LabelType l3,
                         const LabelType l4)const{
        return factor()->valueTable()->eval(l0, l1, l2, l3, l4);
    }
private:
    const FACTOR * factor()const{
        return static_cast<const FACTOR *>(this);
    }
    FACTOR * factor(){
        return static_cast<FACTOR *>(this);
    }
};

/*
    
*/

class DiscreteFactor : public DiscreteFactorBase<DiscreteFactor> {
public:

    template<class VI_T>
    DiscreteFactor(const DiscreteValueTable * vt,
                   std::initializer_list<VI_T> list)
    :   vis_(list),
        vt_(vt){

    }

    template<class VI_ITER>
    DiscreteFactor(const DiscreteValueTable * vt,
                   const VI_ITER viBegin, 
                   const VI_ITER viEnd)
    :   vis_(viBegin, viEnd),
        vt_(vt){

    }
    const DiscreteValueTable * valueTable()const{
        return vt_;
    }   
    size_t arity()const{
        return vis_.size();
    }
    LabelType shape(const size_t d)const{
        return vt_->shape(d);
    }
    Vi vi(const size_t d)const{
        return vis_[d];
    }


private:
    const std::vector<Vi> vis_;
    const DiscreteValueTable * vt_;

};



template<class MODEL>
class DiscreteGraphicalModelBase{
public:

private:
    const MODEL * model()const{
        return static_cast<const MODEL *>(this);
    }
    MODEL * model(){
        return static_cast<MODEL *>(this);
    }
};


class DiscreteGraphicalModel : 
    public DiscreteGraphicalModelBase<DiscreteGraphicalModel>{


    private:
        typedef std::vector<DiscreteFactor>  FactorVector;
        typedef typename FactorVector::const_iterator FactorIterator;

    public:
        typedef const DiscreteFactor * ConstFactorPtr;
        //
        FactorIterator factorsBegin()const{
            return factors_.begin();
        }

        FactorIterator factorsEnd()const{
            return factors_.end();
        }

        DiscreteGraphicalModel(const uint64_t nVar, const LabelType nLabes)
        :   nVar_(nVar),
            nos_(1, nLabes),
            valueTables_(),
            factors_(){
        }
        const uint64_t addValueTable( DiscreteValueTable * vt){
            valueTables_.push_back(vt);
            return valueTables_.size()-1;
        }   
        template<class VI_ITER>
        const uint64_t addFactor(const uint64_t vti , VI_ITER viBegin, VI_ITER viEnd){
            factors_.push_back(DiscreteFactor(valueTables_[vti], viBegin, viEnd));
            return factors_.size()-1;
        }
        template<class VI_T>
        const uint64_t addFactor(const uint64_t vti , std::initializer_list<VI_T>  list){
            factors_.push_back(DiscreteFactor(valueTables_[vti], list));
            return factors_.size()-1;
        }
    private:
        const uint64_t nVar_;
        std::vector<LabelType>              nos_;
        std::vector<DiscreteValueTable * >  valueTables_;
        std::vector<DiscreteFactor>         factors_;
        

};



template<class MODEL>
class FactorsOfVariables{
public:
    typedef MODEL Model;
    FactorsOfVariables(const Model & model)
    : model_(model){

        // 

    }   
private:
    const Model & model_;
    std::vector<SmallVector< > >
};


}

#endif 
