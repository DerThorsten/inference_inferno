/** \file discrete_inference_base.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_VISITORS_HXX
#define INFERNO_INFERENCE_VISITORS_HXX

#include <iostream>
#include <iomanip>

#include "inferno/inferno.hxx"
#include "inferno/inference/discrete_inference_base.hxx"

namespace inferno{
namespace inference{



    template<class MODEL>
    class VerboseVisitor{
    public:
        typedef VerboseVisitor<MODEL> SelfType;
        typedef DiscreteInferenceBase<MODEL> BaseInf;
        typedef typename BaseInf::Visitor VisitorType;
        typedef typename VisitorType::Conf Conf;
        typedef typename VisitorType::BeginCallBack BeginCallBack;
        typedef typename VisitorType::VisitCallBack VisitCallBack;
        typedef typename VisitorType::LoggingCallBack LoggingCallBack;
        typedef typename VisitorType::EndCallBack EndCallBack;

        VerboseVisitor(
            const uint64_t printNth,
            const bool singleLine
        )
        :   visitor_(),
            printNth_(printNth),
            i_(0),
            nlFlag_(singleLine? '\r':'\n'){

           visitor_.beginCallBack = BeginCallBack:: template from_method<SelfType,&SelfType::begin>(this);
           visitor_.visitCallBack = VisitCallBack:: template from_method<SelfType,&SelfType::visit>(this);
           visitor_.endCallBack   = EndCallBack::   template from_method<SelfType,&SelfType::end>(this);     
           visitor_.loggingCallBack   = LoggingCallBack::   template from_method<SelfType,&SelfType::logging>(this);     
        }

        void begin(BaseInf * inf){
            const ValueType e = inf->energy();
            const ValueType lb = inf->lowerBound();
            std::cout<<std::left<<std::setw(9)<<"Begin"<<std::right<<std::setw(8)<<i_
                    <<"   Energy  "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<e
                    <<"   LowerBound   "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<lb<<"\n"
                ;
        }
        void visit(BaseInf * inf){
            if(i_==0 || (i_ % printNth_==0)){
                const ValueType e = inf->energy();
                const ValueType lb = inf->lowerBound();

                std::cout<<std::left<<std::setw(9)<<"Iteration"<<std::right<<std::setw(8)<<i_
                    <<"   Energy  "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<e
                    <<"   LowerBound   "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<lb;
                if(!logging_.empty()){
                    std::cout<<"  ";
                }
                for(const auto & kv : logging_){
                    std::cout<<kv.first<<"   "<<std::setw(7)<<std::showpos<<std::scientific<<std::setprecision(6)<<kv.second<<"  ";
                }
                std::cout<<nlFlag_;
            }
            ++i_;
        }
        void logging(BaseInf * inf, const std::string & name, const ValueType value){
            if(i_==0 || (i_ % printNth_==0)){
                logging_[name] = value;
            }
        }
        void end(BaseInf * inf){
            const ValueType e = inf->energy();
            const ValueType lb = inf->lowerBound();
            if(nlFlag_=='\r')
                std::cout<<"\n";
            std::cout<<std::left<<std::setw(9)<<"End"<<std::right<<std::setw(8)<<i_
                    <<"   Energy  "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<e
                    <<"   LowerBound   "<<std::setw(8)<<std::showpos<<std::scientific<<std::setprecision(7)<<lb<<"\n"
                ;
        }
        VisitorType & visitor(){
            return visitor_;
        }
    private:
        std::map<std::string, ValueType > logging_;
        VisitorType visitor_;
        uint64_t printNth_;
        uint64_t i_;
        char nlFlag_;
    };


} // end namespace inference
} // end namespace inferno


#endif /* INFERNO_INFERENCE_VISITORS_HXX */
