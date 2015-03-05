/** \file inference_options.hxx 
    \brief  InferenceOptions object implementation

    \warning Not yet finished
*/
#ifndef INFERNO_INFERENCE_INFERENCE_OPTIONS_HXX
#define INFERNO_INFERENCE_INFERENCE_OPTIONS_HXX

#include <iostream>
#include <iomanip>
#include <set>
#include <map>
#include <string>

#include <boost/any.hpp> 
    
#include "inferno/inferno.hxx"


namespace inferno{
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
        :   checkOptions_(false),
            storage_()
        {

        }
        void checkOptions(const bool check){
            checkOptions_ = check;
        }
        bool checkOptions()const{
            return checkOptions_;
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
        bool getOption(const std::string & key, const DEFAULT_VAL & defaultVal, T & val)const{
            const auto i = storage_.find(key);
            if(i==storage_.end()){
                val = defaultVal;
                return false;
            }
            else{
                val = get<T>(key);
                return true;
            }
        }

        template<class T, class DEFAULT_VAL>
        bool getOption(
            const std::string & key, 
            const DEFAULT_VAL & defaultVal, 
            T & val,
            std::set<std::string> & keys
        )const{
            const auto i = storage_.find(key);
            if(i==storage_.end()){
                val = defaultVal;
                return false;
            }
            else{
                keys.erase(key);
                val = get<T>(key);
                return true;
            }
        }

        template<class T, class DEFAULT_VAL>
        T getOption(
            const std::string & key, 
            const DEFAULT_VAL & defaultVal, 
            std::set<std::string> & keys
        )const{
            const auto i = storage_.find(key);
            if(i==storage_.end()){
                return static_cast<T>(defaultVal);
                return false;
            }
            else{
                keys.erase(key);
                return  get<T>(key);
            }
        }

        template<class T, class DEFAULT_VAL>
        T getOption(
            const std::string & key, 
            const DEFAULT_VAL & defaultVal
        )const{
            const auto i = storage_.find(key);
            if(i==storage_.end()){
                return static_cast<T>(defaultVal);
                return false;
            }
            else{
                return  get<T>(key);
            }
        }

        std::set<std::string> keys()const{
            std::set<std::string> ret;
            for(const auto & kv : storage_)
                ret.insert(kv.first);
            return ret;
        }

        std::string asString()const{
            std::stringstream ss;
            printToStream(ss);
            return ss.str();
        }
        template<class INF>
        void checkForLeftovers(const std::set<std::string> & keys)const{
            if(!keys.empty()){
                std::stringstream ss;
                ss<<"\nThe following option(s) are set but must not be used within the desired algorithm:\n\n";
                for(const auto & key  : keys){
                    ss<<" - "<<key<<"\n";
                }
                ss<<"\nHere is a list of the allowed options with their default values\n";
                auto  dOpt = InferenceOptions();
                INF::defaultOptions(dOpt);
                dOpt.printToStream(ss);
                throw RuntimeError(ss.str());
            }
        }

    private:
        bool checkOptions_;
        mutable std::map<std::string, std::pair<AnyTypeInfo, boost::any> > storage_;
    };
}
}


#endif /* INFERNO_INFERENCE_INFERENCE_OPTIONS_HXX */
