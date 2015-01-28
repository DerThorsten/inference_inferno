#pragma once
#ifndef INFERNO_UTILITIES_SMALL_VECTOR
#define	INFERNO_UTILITIES_SMALL_VECTOR

#include <vector>
#include <algorithm>

#include "inferno/inferno.hxx"

namespace inferno {


/// \cond HIDDEN_SYMBOLS
namespace detail_small_vector{
    template<class TAG>
    struct AssignmentHelper{

        template<class SV,class ITERATOR>
        static void assign(
            SV & vec,
            ITERATOR begin, 
            ITERATOR end
        ){
            vec.resize(std::distance(begin, end));
            std::copy(begin, end, vec.begin());
        }
    };

    template<>
    struct AssignmentHelper<std::input_iterator_tag>{

        template<class SV,class ITERATOR>
        static void assign(
            SV & vec,
            ITERATOR begin, 
            ITERATOR end
        ){
            std::vector<typename SV::value_type> tmp(begin,end);
            vec.resize(tmp.size());
            for(size_t i=0; i<tmp.size(); ++i)
                vec[i] = tmp[i];
        }
    };


}
/// \endcond


/// \brief Vector that stores values on the stack if size is smaller than MAX_STACK
/// \tparam T value type
/// \tparam MAX_STACK maximum number of elements kept on the stack
///
/// The member functions resize and clear reduce the size but not the
/// capacity of the vector.
///
/// \ingroup utilities
template<class T, size_t MAX_STACK=inferno::USUAL_MAX_FACTOR_ORDER>
class SmallVector{
public:
    typedef T ValueType;
    typedef T value_type;
    typedef T const * ConstIteratorType;
    typedef T const * const_iterator;
    typedef T* IteratorType;
    typedef T* iterator;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;

    SmallVector( );
    SmallVector(const size_t );
    SmallVector(const size_t , const T & );
    SmallVector(const SmallVector<T, MAX_STACK> &);
    template<class ITER>
    SmallVector(ITER begin, ITER end);

    ~SmallVector( );
    SmallVector<T, MAX_STACK>& operator=(const SmallVector<T, MAX_STACK> &);
    template<class ITERATOR> void assign(ITERATOR , ITERATOR);

    size_t size() const ;
    T const * begin() const;
    T const * end() const;
    T* const begin();
    T* const end();
    T const & operator[](const size_t) const;
    T& operator[](const size_t);
    void push_back(const T &);
    void resize(const size_t );
    void reserve(const size_t );
    void clear();
    bool empty() const;
    const T& front() const;
    const T& back() const;
    T& front();
    T& back();

    iterator erase(iterator pos){
        const difference_type index = std::distance(begin(), pos);
        if(index == size_ - 1){
            --size_;
            // now pointing to end
            return pointerToSequence_ + index;
        }
        else{ //(index != size_ - 1){
            for(size_t i=index+1 ; i<size_; ++i){
                pointerToSequence_[index-1] = pointerToSequence_[index];
            }
            --size_;
            // now pointing to the address
            // the erased element has been
            return pointerToSequence_ + index;
        }
        
        
    }

    iterator erase (iterator first, iterator last){
        const difference_type nErase = std::distance(first, last);
        const difference_type index = std::distance(begin(), first);

        if(index + nErase >= size_){
            size_ -= nErase;
            // now pointing to end
            return pointerToSequence_ + size_;
        }
        else{
            for(size_t i=index+nErase ; i<size_; ++i){
                pointerToSequence_[i-nErase] = pointerToSequence_[i];
            }
            size_ -= nErase;
            // now pointing to the address
            // the erased element has been
            return pointerToSequence_ + index;
        }
    }


    iterator insert (iterator position, const value_type& val);
    void insert (iterator position, size_type n, const value_type& val);
    template <class InputIterator>
    void insert (iterator position, InputIterator first, InputIterator last);

    const T * data()const{
        return pointerToSequence_;
    }
    T * data(){
        return pointerToSequence_;
    }

private:

   size_t size_;
   size_t capacity_;
   T stackSequence_[MAX_STACK];
   T* pointerToSequence_;
};

/// constructor
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK>::SmallVector( )
:  size_(0),
   capacity_(MAX_STACK),
   pointerToSequence_(stackSequence_) {

}

/// constructor
/// \param size length of the sequence
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK>::SmallVector
(
   const size_t size
)
:  size_(size),
   capacity_(size>MAX_STACK?size:MAX_STACK) {
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   if(size_>MAX_STACK) {
      pointerToSequence_ = new T[size];
   }
   else{
      pointerToSequence_=stackSequence_;
   }
}

/// constructor
/// \param size lenght of the sequence
/// \param value initial value
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK>::SmallVector
(
   const size_t size,
   const T & value
)
:  size_(size),
   capacity_(size>MAX_STACK?size:MAX_STACK) {
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   if(size_>MAX_STACK) {
      pointerToSequence_ = new T[size_];
   }
   else{
      pointerToSequence_=stackSequence_;
   }
   std::fill(pointerToSequence_, pointerToSequence_+size_, value);
}


template<class T, size_t MAX_STACK>
template<class ITER>
SmallVector<T, MAX_STACK>::SmallVector
(
    ITER begin,
    ITER end
)
:   size_(),
    capacity_() 
{
    typedef std::iterator_traits<ITER> IterTraits;
    typedef typename IterTraits::iterator_category  IteratorTag;
    typedef detail_small_vector::AssignmentHelper<IteratorTag> Helper;
    Helper::assign(*this, begin, end);
}


/// copy constructor
/// \param other container to copy
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK>::SmallVector
(
   const  SmallVector<T, MAX_STACK> & other
)
:  size_(other.size_),
   capacity_(other.capacity_)
{
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   if(size_>MAX_STACK) {
      pointerToSequence_ = new T[size_];
   }
   else{
      pointerToSequence_=stackSequence_;
   }
   std::copy(other.pointerToSequence_, other.pointerToSequence_+size_, pointerToSequence_);
}

/// destructor
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK>::~SmallVector( ) {
   if(size_>MAX_STACK) {
      INFERNO_ASSERT(pointerToSequence_!=NULL);
      delete[] pointerToSequence_;
   }
}

/// assignment operator
/// \param other container to copy
template<class T, size_t MAX_STACK>
SmallVector<T, MAX_STACK> & SmallVector<T, MAX_STACK>::operator=
(
   const  SmallVector<T, MAX_STACK> & other
)
{
   if(&other != this) {
      // size_=other.size_;
      // capacity_=other.capacity_;
      // INFERNO_ASSERT(size_<=capacity_);
      // INFERNO_ASSERT(capacity_>=MAX_STACK);
      if(other.size_>MAX_STACK) {
         // delete old sequence
         if(size_>MAX_STACK) {
            delete [] pointerToSequence_;
            pointerToSequence_ = new T[other.size_];
         }
         // nothing to delete
         else{
            pointerToSequence_ = new T[other.size_];
         }
         size_=other.size_;
         capacity_=size_;
      }
      else{
         pointerToSequence_=stackSequence_;
         size_=other.size_;
      }
      std::copy(other.pointerToSequence_, other.pointerToSequence_+size_, pointerToSequence_);
   }
   return *this;
}

/// size
template<class T, size_t MAX_STACK>
inline size_t
SmallVector<T, MAX_STACK>::size() const {
   INFERNO_ASSERT(pointerToSequence_!=NULL ||size_== 0 );
   return size_;
}

/// begin iterator
template<class T, size_t MAX_STACK>
inline T const *
SmallVector<T, MAX_STACK>::begin() const{
   INFERNO_ASSERT(pointerToSequence_!=NULL);
   return pointerToSequence_;
}

/// end iterator
template<class T, size_t MAX_STACK>
inline T const *
SmallVector<T, MAX_STACK>::end() const{
   INFERNO_ASSERT(pointerToSequence_!=NULL);
   return pointerToSequence_ + size_;
}

/// begin iterator
template<class T, size_t MAX_STACK>
inline T * const
SmallVector<T, MAX_STACK>::begin() {
   INFERNO_ASSERT(pointerToSequence_!=NULL);
   return pointerToSequence_;
}

/// end iterator
template<class T, size_t MAX_STACK>
inline T * const
SmallVector<T, MAX_STACK>::end() {
   INFERNO_ASSERT(pointerToSequence_!=NULL);
   return pointerToSequence_ + size_;
}

/// access entries
template<class T, size_t MAX_STACK>
inline T const &
SmallVector<T, MAX_STACK>::operator[]
(
   const size_t index
) const {
   INFERNO_ASSERT(pointerToSequence_!=NULL);
   INFERNO_ASSERT(index<size_);
   return pointerToSequence_[index];
}

/// access entries
template<class T, size_t MAX_STACK>
inline T &
SmallVector<T, MAX_STACK>::operator[]
(
   const size_t index
) {
   INFERNO_ASSERT(index<size_);
   return pointerToSequence_[index];
}

/// append a value
/// \param value value to append
template<class T, size_t MAX_STACK>
inline void
SmallVector<T, MAX_STACK>::push_back
(
   const T & value
) {
   INFERNO_ASSERT(capacity_ >= MAX_STACK);
   INFERNO_ASSERT(size_ <= capacity_);
   if(capacity_ == size_) {
      T * tmp=new T[capacity_ * 2];
      std::copy(pointerToSequence_, pointerToSequence_ + size_, tmp);
      if(capacity_ > MAX_STACK) {
         delete[] pointerToSequence_;
      }
      capacity_ *= 2;
      pointerToSequence_ = tmp;
   }
   pointerToSequence_[size_]=value;
   ++size_;
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
}

/// resize the sequence
/// \param size new size of the container
template<class T, size_t MAX_STACK>
inline void
SmallVector<T, MAX_STACK>::resize
(
   const size_t size
) {
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   INFERNO_ASSERT(size_<=capacity_);
   if(size>capacity_) {
      T * tmp=new T[size];
      std::copy(pointerToSequence_, pointerToSequence_+size_, tmp);
      if(capacity_>MAX_STACK) {
         delete[] pointerToSequence_;
      }
      capacity_=size;
      pointerToSequence_=tmp;
   }
   size_=size;
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
}

/// reserve memory
/// \param  size new size of the container
template<class T, size_t MAX_STACK>
inline void
SmallVector<T, MAX_STACK>::reserve
(
   const size_t size
) {
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   INFERNO_ASSERT(size_<=capacity_);
   if(size>capacity_) {
      T * tmp=new T[size];
      std::copy(pointerToSequence_, pointerToSequence_+size_, tmp);
      if(capacity_>MAX_STACK) {
         delete[] pointerToSequence_;
      }
      capacity_=size;
      pointerToSequence_=tmp;
   }
   // size_ = size;
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
}

/// clear the sequence
template<class T, size_t MAX_STACK>
inline void
SmallVector<T, MAX_STACK>::clear() {
   INFERNO_ASSERT(capacity_>=MAX_STACK);
   INFERNO_ASSERT(size_<=capacity_);
   if(capacity_>MAX_STACK) {
      delete[] pointerToSequence_;
   }
   pointerToSequence_=stackSequence_;
   capacity_=MAX_STACK;
   size_=0;
   INFERNO_ASSERT(size_<=capacity_);
   INFERNO_ASSERT(capacity_>=MAX_STACK);
}

/// query if the sequence is empty
template<class T, size_t MAX_STACK>
inline bool
SmallVector<T, MAX_STACK>::empty() const {
   return (size_ == 0);
}

/// assign values
/// \param begin begin iterator
/// \param end end iterator
template<class T, size_t MAX_STACK>
template<class ITERATOR>
inline void
SmallVector<T, MAX_STACK>::assign(ITERATOR begin, ITERATOR end) {
    typedef std::iterator_traits<ITERATOR> IterTraits;
    typedef typename IterTraits::iterator_category  IteratorTag;
    typedef detail_small_vector::AssignmentHelper<IteratorTag> Helper;
    Helper::assign(*this, begin, end);
}

template<class T, size_t MAX_STACK>
typename SmallVector<T, MAX_STACK>::iterator 
SmallVector<T, MAX_STACK>::insert 
(
    iterator position, 
    const value_type& val
){
    if(position==end()){
        resize(size_+1);
        back() = val; 
        return pointerToSequence_ + (size_ -1);
    }
    else{
        resize(size_+1);
        const difference_type index = std::distance(begin(),position);
        for(difference_type i=size_ -2; i>index; --i){
            pointerToSequence_[i+1] = pointerToSequence_[i]; 
        }
        pointerToSequence_ + index;
    }
}

template<class T, size_t MAX_STACK>
void 
SmallVector<T, MAX_STACK>::insert 
(
    iterator position, 
    size_type n, 
    const value_type& val
){
    if(position==end()){
        const difference_type index = std::distance(begin(),position);
        resize(size_+n);
        for(difference_type i=index; i<size_; ++i)
            pointerToSequence_[i] = val;
    }
    else{
        const size_t oldSize = size_;
        const difference_type index = std::distance(begin(),position);
        resize(size_+n);
        for(difference_type i=size_ - 1; i>index; --i){
            pointerToSequence_[i] = pointerToSequence_[i-n]; 
        }
        for(difference_type i=0; i<n; ++i){
            pointerToSequence_[index + i]  = val;
        }
        pointerToSequence_ + index;
    }
}

template<class T, size_t MAX_STACK>
template <class InputIterator>
void 
SmallVector<T, MAX_STACK>::insert 
(
    iterator position, 
    InputIterator first, 
    InputIterator last
){
    // DO THIS ONLY IF IT IS AN TRUE INPUT ITERATOR!!!
    const difference_type index = std::distance(begin(), position);
    std::vector<T> tmp(begin(),end());
    tmp.insert(tmp.begin()+index, first,last);
    resize(tmp.size());
    std::copy(tmp.begin(), tmp.end(), begin());
}


/// reference to the last entry
template<class T, size_t MAX_STACK>
inline const T &
SmallVector<T, MAX_STACK>::back()const{
   return pointerToSequence_[size_-1];
}

/// reference to the last entry
template<class T, size_t MAX_STACK>
inline T &
SmallVector<T, MAX_STACK>::back() {
   return pointerToSequence_[size_-1];
}

/// reference to the first entry
template<class T, size_t MAX_STACK>
inline const T &
SmallVector<T, MAX_STACK>::front()const{
   return pointerToSequence_[0];
}

/// reference to the first entry
template<class T, size_t MAX_STACK>
inline T &
SmallVector<T, MAX_STACK>::front() {
   return pointerToSequence_[0];
}

template<class T, size_t MAX_STACK>
std::ostream& operator <<(std::ostream& stream, const SmallVector<T, MAX_STACK> & vec) {
    stream<<"[";
    for(size_t d=0; d<vec.size(); ++d){
        if(d==vec.size()-1){
            stream<<vec[d];
        }
        else
            stream<<vec[d]<<", ";
    }
    stream<<"]";
    return stream;
}

} // namespace inferno

#endif // INFERNO_UTILITIES_SMALL_VECTOR
