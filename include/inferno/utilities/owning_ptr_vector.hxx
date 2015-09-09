#ifndef INFERNO_UTILITIES_OWNING_PTR_VECTOR_HXX
#define INFERNO_UTILITIES_OWNING_PTR_VECTOR_HXX

#include <cstdint>
#include <vector>

namespace inferno{
namespace utilities{


    /** \brief vector deleting the pointers contained in the vector on destruction
        
        \warning This class should not be used from pure C++ 11,
            but only to generate glue code with boost::python
    
        This class is used only very rarely. Currently only
        in the boost::python code used to export the python bindings.
        There we use this vector to hold possible polymorphic instances
        of loss functions.
    
        We currently cannot really use cpp 11 smart ptrs in boost::python 
        since they are not yet implemented in boost::python.
        Therefore we use a workaround with std::auto_ptr to 
        transfer ownership from python to cpp.
        In this workaround we use this vector only on the python 
        side in a save fashion.
    */
    template<class T>
    class OwningPtrVector : public std::vector<T*>
    {
    private:
        typedef std::vector<T*> Base;
    public:
        OwningPtrVector(const uint64_t size = 0)
        :   Base(size){
            for(size_t i=0; i < size; ++i){
                (*this)[i] = nullptr;
            }
        }

        ~OwningPtrVector(){
            for(size_t i=0; i < this->size(); ++i){
                if((*this)[i] != nullptr){
                    auto ptr = (*this)[i];
                    delete ptr;
                }
            }
        }
    };

} // end namespace inferno::utilities
} // end namespace inferno

#endif /*INFERNO_UTILITIES_OWNING_PTR_VECTOR_HXX*/
