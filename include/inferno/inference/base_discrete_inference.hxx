/** \file base_discrete_inference.hxx 
    \brief  Functionality which is shared between all inference algorithms
    is implemented in this header.

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX
#define INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX

#include <iostream>
#include <iomanip>
#include <map>

#include <boost/any.hpp> 

#include "inferno/inferno.hxx"
#include "inferno/utilities/delegate.hxx"
#include "inferno/inference/utilities/movemaker.hxx"
#include "inferno/inference/inference_options.hxx"
#include "inferno/model/factors_of_variables.hxx"

namespace inferno{

/** \namespace inferno::inference
    \brief subnamespace of inferno
    where all inference related
    functionality is implemented.

    All inference algorithms and related
    functionality is implemented 
    within inferno::inference.
*/
namespace inference{


    class InferenceOptions{
    public:
        
        enum AnyTypeInfo{
            DoubleType,
            IntegerType,
            BoolType,
            StringType,
            InferenceOptionsType,
            BoostAny
        };
        typedef std::pair<AnyTypeInfo, boost::any>   P;

        InferenceOptions()
        :   empty_(true),
            storage_()
        {

        }

        // FLOATS
        void set(const std::string & name, const double val){
            storage_[name] = P(DoubleType, static_cast<double>(val));
        }
        void set(const std::string & name, const float val){
            storage_[name] = P(DoubleType, static_cast<double>(val));
        }
        void get(const std::string & name, double & val)const{
            val = static_cast<float>(boost::any_cast<double>(storage_[name].second));
        }
        void get(const std::string & name, float & val)const{
            val = static_cast<float>(boost::any_cast<double>(storage_[name].second));
        }

        // INTEGERS
        void set(const std::string & name, const int64_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, int64_t & val)const{
            val = static_cast<int64_t>(boost::any_cast<int64_t>(storage_[name].second));
        }
        void set(const std::string & name, const uint64_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, uint64_t & val)const{
            val = static_cast<uint64_t>(boost::any_cast<int64_t>(storage_[name].second));
        }

        void set(const std::string & name, const int32_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, int32_t & val)const{
            val = static_cast<int32_t>(boost::any_cast<int64_t>(storage_[name].second));
        }
        void set(const std::string & name, const uint32_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, uint32_t & val)const{
            val = static_cast<uint32_t>(boost::any_cast<int64_t>(storage_[name].second));
        }

        void set(const std::string & name, const int16_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, int16_t & val)const{
            val = static_cast<int16_t>(boost::any_cast<int64_t>(storage_[name].second));
        }
        void set(const std::string & name, const uint16_t val){
            storage_[name] = P(IntegerType, static_cast<int64_t>(val));
        }
        void get(const std::string & name, uint16_t & val)const{
            val = static_cast<uint16_t>(boost::any_cast<int64_t>(storage_[name].second));
        }

        // BOOL
        void set(const std::string & name, const bool val){
            storage_[name] = P(BoolType, static_cast<bool>(val));
        }
        void get(const std::string & name, bool & val)const{
            val = static_cast<bool>(boost::any_cast<bool>(storage_[name].second));
        }
        // string
        void set(const std::string & name, const std::string & val){
            storage_[name] = P(StringType, static_cast<std::string>(val));
        }
        void get(const std::string & name, std::string & val)const{
            val = static_cast<std::string>(boost::any_cast<std::string>(storage_[name].second));
        }
        // inference options
        void set(const std::string & name, const InferenceOptions & val){
            storage_[name] = P(InferenceOptionsType, static_cast<InferenceOptions>(val));
        }
        void get(const std::string & name, InferenceOptions & val)const{
            val = static_cast<InferenceOptions>(boost::any_cast<InferenceOptions>(storage_[name].second));
        }

        template<class T>
        T get(const std::string & name)const{
            T t;
            get(name, t);
            return t;
        }
        bool hasOption(const std::string name)const{
            return storage_.find(name)!=storage_.end();
        }
        template<class STREAM>
        STREAM  & printToStream(STREAM & stream, const size_t level=0)const{
            stream<<std::setw(level*4)<<""<<"[\n";
            for(const auto & keyVal : storage_){
                const auto & k = keyVal.first;
                const auto & v = keyVal.second;
                stream<<std::setw(level*4)<<""<<"    "<<keyVal.first;
                if(v.first==DoubleType)
                  stream<<" (double) : "<< get<double>(k) <<"\n";
                if(v.first==IntegerType)
                  stream<<" (integer) : "<< get<int64_t>(k) <<"\n";
                if(v.first==BoolType)
                  stream<<" (bool) : "<< get<bool>(k) <<"\n";
                if(v.first==StringType)
                  stream<<" (string) : "<< get<std::string>(k) <<"\n";
                if(v.first==InferenceOptionsType){
                    stream<<" (inference-opt.) : \n";
                    InferenceOptions subOpts = get<InferenceOptions>(k);
                    subOpts.printToStream(stream, level+1);
                }
            }
            stream<<std::setw(level*4)<<""<<"]\n";
            return stream;
        }

        template<class T, class DEFAULT_VAL>
        void getOption(const std::string & key, const DEFAULT_VAL & defaultVal, T & val)const{
            const auto i = storage_.find(key);
            if(i==storage_.end()){
                val = defaultVal;
            }
            else{
                val = get<T>(key);
            }
        }


        std::string asString()const{
            std::stringstream ss;
            printToStream(ss);
            return ss.str();
        }

    private:
        bool empty_;
        mutable std::map<std::string, std::pair<AnyTypeInfo, boost::any> > storage_;
    };


    

    template<class MODEL>
    class DiscreteInferenceBase{
    public:

        class Visitor{
        public:
            typedef DiscreteInferenceBase<MODEL> BaseInf;
            typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;

            typedef inferno::delegate1<void , BaseInf * > BeginCallBack;
            typedef inferno::delegate1<void , BaseInf * > VisitCallBack;
            typedef inferno::delegate1<void , BaseInf * > EndCallBack;


            void begin(BaseInf * inf){
                if(bool(beginCallBack))
                    beginCallBack(inf);
            }
            void visit(BaseInf * inf){
                if(bool(visitCallBack))
                    visitCallBack(inf);
            }
            void end(BaseInf * inf){
                if(bool(endCallBack))
                    endCallBack(inf);
            }


            BeginCallBack beginCallBack;
            VisitCallBack visitCallBack;
            EndCallBack endCallBack;
        };



        typedef MODEL Model;
        typedef DiscreteInferenceBase<MODEL> Self;
        typedef typename MODEL:: template VariableMap<DiscreteLabel> Conf;


        // MUST HAVE INTERACE
        //
        virtual std::string name()const = 0;
        // inference
        virtual void infer( Visitor  * visitor  = NULL) = 0 ;
        // get result
        virtual void conf(Conf & conf ) = 0;
        virtual DiscreteLabel label(const Vi vi ) = 0;
        // get model
        virtual const Model & model() const=0;
        // stop inference (via visitor)
        virtual void stopInference() = 0;



        // OPTIONAL INTERFACE

        // warm start related (do nothing default)
        virtual void setConf(const Conf & conf){}
        virtual void setUpperBound(const ValueType valueType){}
        virtual void setLowerBound(const ValueType valueType){}

        // get results optional interface
        virtual ValueType upperBound(){
            Conf confMap(this->model());
            this->conf(confMap);
            return this->model().eval(confMap);
        }
        virtual ValueType energy(){
            Conf confMap(this->model());
            this->conf(confMap);
            return this->model().eval(confMap);
        }
        virtual ValueType lowerBound(){
            return -1.0*std::numeric_limits<ValueType>::infinity();
        }

        // partial optimality
        // ???

        // model has changed
        virtual void graphChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChange\" so far"));
        }
        virtual void energyChange() {
            throw NotImplementedException(this->name()+std::string("does not support \"graphChanged\" so far"));
        }
        virtual void partialEnergyChange(const Fi * changedFacBegin, const Fi * changedFacEnd ) {
            throw NotImplementedException(this->name()+std::string("does not support \"partialEneryChange\" so far"));
        }
        
    };


    template<class CONCRETE_CLASS>
    DiscreteInferenceBase< typename CONCRETE_CLASS::Model > * inferenceFactory(
        const typename CONCRETE_CLASS::Model & model,
        const InferenceOptions & options
    ){
        return new CONCRETE_CLASS(model, options);
    }

}
}


#endif /* INFERNO_INFERENCE_BASE_DISCRETE_INFERENCE_HXX */
