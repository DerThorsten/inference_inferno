#include "inferno/inferno.hxx"
#include "inferno/model/sparse_discrete_model.hxx"
#include "inferno/model/general_discrete_model.hxx"
#include "inferno/value_tables/potts.hxx"
#include "inferno/value_tables/unary.hxx"

#include "inferno/utilities/timer.hxx"
namespace inferno{






template<class MODEL>
struct  Benchmark{

    typedef typename MODEL :: template VariableMap<DiscreteLabel> Labels;

    struct EvalAt{
        EvalAt(const MODEL & model, bool atZero = true) 
        :   model_(model),labels_(model),atZero_(atZero){
            if(atZero_)
                for(auto vi : model.variableIds())
                    labels_[vi] = DiscreteLabel(0);
            else
                for(auto vi : model.variableIds())
                    labels_[vi] = vi % model.nLabels(vi);
        }
        std::string name()const{
            if(atZero_)
                return "evalAtZero";
            else
                return "evalAtMixed";
        }
        ValueType operator()()const{
            return model_.eval(labels_);
        }
        const MODEL & model_;
        Labels labels_;
        const bool atZero_;
    };

    static void run(const MODEL & model){
        std::cout<<"Start Benchmark\n-nVar "
            <<model.nVariables()<<"\n-nFac "<<model.nFactors()<<"\n";
        {
            Timing<EvalAt> t(EvalAt(model),10,2);
            std::cout<<t.name()<<" "<<t.mean()<<" +/- "<<t.stdev()<<"\n";
        }
        {
            Timing<EvalAt> t(EvalAt(model,false),10,2);
            std::cout<<t.name()<<" "<<t.mean()<<" +/- "<<t.stdev()<<"\n";
        }

    }
    

};



}
