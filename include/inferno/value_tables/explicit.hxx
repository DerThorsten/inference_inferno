#ifndef INFERNO_VALUE_TABLES_EXPLICIT_HXX
#define INFERNO_VALUE_TABLES_EXPLICIT_HXX

#include <cstdint>
#include <vector>


#include "inferno/inferno.hxx"
#include "inferno/value_tables/base_discrete_value_table.hxx"
#include "inferno/utilities/shape_walker.hxx"
#include "inferno/utilities/marray_wrap.hxx"
namespace inferno{
namespace value_tables{



namespace detail_explicit{

template<class STORAGE>
class  ExplicitBase : public DiscreteValueTableBase{
public:
    template<class STORAGE_IN>
    ExplicitBase(const STORAGE_IN & storage)
    :   DiscreteValueTableBase(),
        storage_(storage){
    }
    virtual inferno::ValueType eval(const DiscreteLabel *conf)const{
        return storage_(conf);
    }
    virtual inferno::ValueType eval(const DiscreteLabel l0)const{
        return storage_(size_t(l0));
    }
    virtual inferno::ValueType eval(const DiscreteLabel l0,const DiscreteLabel l1)const{
        return storage_(size_t(l0), size_t(l1));
    }
    virtual inferno::ValueType eval(const DiscreteLabel l0,const DiscreteLabel l1,
                           const DiscreteLabel l2)const{
        return storage_(std::size_t(l0), std::size_t(l1), 
                        std::size_t(l2));
    }
    virtual inferno::ValueType eval(const DiscreteLabel l0,const DiscreteLabel l1,
                           const DiscreteLabel l2,const DiscreteLabel l3)const{
        return storage_(std::size_t(l0), std::size_t(l1), 
                        std::size_t(l2), std::size_t(l3));
    }
    virtual inferno::ValueType eval(const DiscreteLabel l0,const DiscreteLabel l1,
                           const DiscreteLabel l2,const DiscreteLabel l3,
                           const DiscreteLabel l4)const{
        return storage_(std::size_t(l0), std::size_t(l1), 
                        std::size_t(l2), std::size_t(l3),
                        std::size_t(l4));
    }
    virtual DiscreteLabel shape(const uint32_t d) const{
        return storage_.shape(d);
    }
    virtual uint32_t  arity()const{
        return storage_.dimension();
    }
    virtual uint64_t size()const{
        return storage_.size();
    }
    virtual void bufferShape(DiscreteLabel * buffer)const{
        std::copy(storage_.shapeBegin(), storage_.shapeEnd(), buffer);
    }
    virtual void bufferValueTable(inferno::ValueType * buffer)const{
        std::copy(storage_.begin(), storage_.end(), buffer);
    }
private:
    STORAGE storage_;
};

} // end namespace detail_explicit


typedef detail_explicit::ExplicitBase< ValueMarray >  ExplicitBaseValueMarray;
typedef detail_explicit::ExplicitBase< ValueMarrayView >  ExplicitBaseValueMarrayView;

/** \brief multidimensional array
    
    This value table can encode any discrete 
    value table explicitly.
    Therefore the values of all configurations
    are stored explicitly within his value table.        

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class Explicit : public ExplicitBaseValueMarray 
{
public:
    template<class STORAGE_IN>
    Explicit(const STORAGE_IN & storage)
    : ExplicitBaseValueMarray(storage){

    }
};

/** \brief view to a multidimensional array
    
    This value table can view
    to a ValueMarrayView of any arity and shape.
    This value table does not own or hold
    the values itself, but a view 
    to the actual storage.        

    \ingroup value_tables
    \ingroup discrete_value_tables
*/
class ExplicitView : ExplicitBaseValueMarrayView
{
public:
    template<class STORAGE_IN>
    ExplicitView(const STORAGE_IN & storage)
    : ExplicitBaseValueMarrayView(storage){

    }
};

} // end namespace value_tables
} // end namespace inferno

#endif /*INFERNO_VALUE_TABLES_EXPLICIT_HXX*/
