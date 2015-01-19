#include "inferno/inferno.hxx"


namespace inferno{

    RuntimeError::RuntimeError(const std::string& message)
       :  base(std::string("Inferno error: ") + message){
    }

}
