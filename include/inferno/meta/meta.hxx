#ifndef INFERNO_META_META_HXX
#define INFERNO_META_META_HXX



namespace inferno{
namespace meta{



    struct EndType{
    };

    struct UnknownType{
    };

    struct TriboolTrueType{
    };

    struct TriboolFalseType{
    };

    struct TriboolMaybeType{
    };

    struct UnknownNumberType{
    };

    struct COrder{
    };
    struct FortranOrder{
    };
    struct UnknownOrder{
    };

    template<int64_t N>
    struct Int64Type{
        const static int64_t value = N;
    };
    
    
} // end namespace inferno::meta
} // end namespace inferno

#endif /*INFERNO_META_META_HXX*/
