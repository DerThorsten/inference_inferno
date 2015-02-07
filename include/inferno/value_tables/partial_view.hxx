#ifndef INFERNO_VALUE_TABLES_PARTIAL_VIEW_HXX
#define INFERNO_VALUE_TABLES_PARTIAL_VIEW_HXX

#include <cstdint>
#include <vector>


#include "inferno/utilities/small_vector.hxx"
#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"

namespace inferno{


namespace value_tables{



/** \brief discrete potts function with an arity of 2.
       

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class  PartialView : public DiscreteValueTableBase{
public:
    PartialView(const DiscreteValueTableBase * fullVt = NULL)
    :   DiscreteValueTableBase(),
        fullVt_(fullVt), 
    {
        if(fullVt_!=NULL)
            this->setFullVt(fullVt);
    }
    virtual ValueType eval(const LabelType *conf)const{
        return conf[0] == conf[1] ? 0.0 : beta_;
    }
    virtual ValueType eval(const LabelType l1, const LabelType l2)const{
        return l1==l2 ? 0 : beta_;
    }
    virtual LabelType shape(const uint32_t d) const{
        return nl_;
    }
    virtual uint32_t  arity()const{
        return 2;
    }
    virtual bool isGeneralizedPotts() const{
        return true;
    }
    virtual bool isPotts() const{
        return true;
    }
    void setFullVt(const DiscreteValueTableBase * fullVt){
        fullVt_ = fullVt;
        fullConfBuffer_.resize(fullVt->arity());
    }
private:
    const DiscreteValueTableBase *          fullVt_;
    mutable SmallVector<DiscreteLabel>      fullConfBuffer_;
    SmallVector<uint32_t>                   fixedLabelsPos_;
    SmallVector<DiscreteLabel>              fixedLabels_;
};


} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_PARTIAL_VIEW_HXX*/
