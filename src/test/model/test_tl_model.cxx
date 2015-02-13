#define BOOST_TEST_MODULE TlModelTest

#include <boost/test/unit_test.hpp>
#include "inferno_test/test.hxx"

#include "inferno/inferno.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"


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
        return i< NFunctionTypes-1 ? i : 0 ;
    }

 


template <typename... VALUE_TABLE>
struct TlModel
{

public:



    typedef std::tuple< VALUE_TABLE ... >           TupleOfValueTables;
    typedef std::tuple<std::vector<VALUE_TABLE>...> TupleOfValueTableVectors;
    static const std::size_t NFunctionTypes = std::tuple_size<TupleOfValueTableVectors>::value;
    TupleOfValueTableVectors tupleOfValueTableVectors_;


    // functors 

    struct EvalFunctor{
        typedef ValueType result_type;
        template<class VT>
        result_type operator() (const VT & vt){
            return vt.eval(l_);
        }
        const DiscreteLabel * l_;
    };


    ValueType eval(const DiscreteLabel * label)const{
        EvalFunctor f({label});
        return callFunctorImpl(0,0,f);
    }


private:

    template<class FUNCTOR>
    typename FUNCTOR::result_type callFunctorImpl(const uint8_t fType,const  uint64_t functionIndex, FUNCTOR & functor)const{
        switch(fType){
            case 0 :{
                const auto & vt = std::get<getSaveIndex(0,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }
            case 1 :{
                const auto & vt = std::get<getSaveIndex(1,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }
            case 2 :{
                const auto & vt = std::get<getSaveIndex(2,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }
            case 3 :{
                const auto & vt = std::get<getSaveIndex(3,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }
            case 4 :{
                const auto & vt = std::get<getSaveIndex(4,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }
            case 5 :{
                const auto & vt = std::get<getSaveIndex(5,NFunctionTypes)>(tupleOfValueTableVectors_)[functionIndex];
                return functor(vt);
            }

        }
    }


};

}


BOOST_AUTO_TEST_CASE(TestFactor)
{
    using namespace inferno;

    typedef TlModel<
        value_tables::UnaryValueTable,
        value_tables::PottsValueTable
    > Model;

    Model model;

    DiscreteLabel l[1] = {1};
    model.eval(l);
}

