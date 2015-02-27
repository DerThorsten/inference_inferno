/*
    (c) Sergey Ryazanov (http://home.onego.ru/~ryazanov)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

For details, see "The Impossibly Fast C++ Delegates" at
http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
*/

#if INFERNO_DELEGATE_PARAM_COUNT > 0
#define INFERNO_DELEGATE_SEPARATOR ,
#else
#define INFERNO_DELEGATE_SEPARATOR
#endif

// see BOOST_JOIN for explanation
#define INFERNO_DELEGATE_JOIN_MACRO( X, Y) INFERNO_DELEGATE_DO_JOIN( X, Y )
#define INFERNO_DELEGATE_DO_JOIN( X, Y ) INFERNO_DELEGATE_DO_JOIN2(X,Y)
#define INFERNO_DELEGATE_DO_JOIN2( X, Y ) X##Y

namespace inferno
{
#ifdef INFERNO_DELEGATE_PREFERRED_SYNTAX
#define INFERNO_DELEGATE_CLASS_NAME delegate
#define INFERNO_DELEGATE_INVOKER_CLASS_NAME delegate_invoker
#else
#define INFERNO_DELEGATE_CLASS_NAME INFERNO_DELEGATE_JOIN_MACRO(delegate,INFERNO_DELEGATE_PARAM_COUNT)
#define INFERNO_DELEGATE_INVOKER_CLASS_NAME INFERNO_DELEGATE_JOIN_MACRO(delegate_invoker,INFERNO_DELEGATE_PARAM_COUNT)
	template <typename R INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_TEMPLATE_PARAMS>
	class INFERNO_DELEGATE_INVOKER_CLASS_NAME;
#endif

	template <typename R INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_TEMPLATE_PARAMS>
#ifdef INFERNO_DELEGATE_PREFERRED_SYNTAX
	class INFERNO_DELEGATE_CLASS_NAME<R (INFERNO_DELEGATE_TEMPLATE_ARGS)>
#else
	class INFERNO_DELEGATE_CLASS_NAME
#endif
	{
	public:
		typedef R return_type;
#ifdef INFERNO_DELEGATE_PREFERRED_SYNTAX
		typedef return_type (INFERNO_DELEGATE_CALLTYPE *signature_type)(INFERNO_DELEGATE_TEMPLATE_ARGS);
		typedef INFERNO_DELEGATE_INVOKER_CLASS_NAME<signature_type> invoker_type;
#else
		typedef INFERNO_DELEGATE_INVOKER_CLASS_NAME<R INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_TEMPLATE_ARGS> invoker_type;
#endif

		INFERNO_DELEGATE_CLASS_NAME()
			: object_ptr(0)
			, stub_ptr(0)
		{}

		template <return_type (*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS)>
		static INFERNO_DELEGATE_CLASS_NAME from_function()
		{
			return from_stub(0, &function_stub<TMethod>);
		}

		template <class T, return_type (T::*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS)>
		static INFERNO_DELEGATE_CLASS_NAME from_method(T* object_ptr)
		{
			return from_stub(object_ptr, &method_stub<T, TMethod>);
		}

		template <class T, return_type (T::*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS) const>
		static INFERNO_DELEGATE_CLASS_NAME from_const_method(T const* object_ptr)
		{
			return from_stub(const_cast<T*>(object_ptr), &const_method_stub<T, TMethod>);
		}

		return_type operator()(INFERNO_DELEGATE_PARAMS) const
		{
			return (*stub_ptr)(object_ptr INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_ARGS);
		}

		operator bool () const
		{
			return stub_ptr != 0;
		}

		bool operator!() const
		{
			return !(operator bool());
		}

	private:
		
		typedef return_type (INFERNO_DELEGATE_CALLTYPE *stub_type)(void* object_ptr INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_PARAMS);

		void* object_ptr;
		stub_type stub_ptr;

		static INFERNO_DELEGATE_CLASS_NAME from_stub(void* object_ptr, stub_type stub_ptr)
		{
			INFERNO_DELEGATE_CLASS_NAME d;
			d.object_ptr = object_ptr;
			d.stub_ptr = stub_ptr;
			return d;
		}

		template <return_type (*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS)>
		static return_type INFERNO_DELEGATE_CALLTYPE function_stub(void* INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_PARAMS)
		{
			return (TMethod)(INFERNO_DELEGATE_ARGS);
		}

		template <class T, return_type (T::*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS)>
		static return_type INFERNO_DELEGATE_CALLTYPE method_stub(void* object_ptr INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_PARAMS)
		{
			T* p = static_cast<T*>(object_ptr);
			return (p->*TMethod)(INFERNO_DELEGATE_ARGS);
		}

		template <class T, return_type (T::*TMethod)(INFERNO_DELEGATE_TEMPLATE_ARGS) const>
		static return_type INFERNO_DELEGATE_CALLTYPE const_method_stub(void* object_ptr INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_PARAMS)
		{
			T const* p = static_cast<T*>(object_ptr);
			return (p->*TMethod)(INFERNO_DELEGATE_ARGS);
		}
	};

	template <typename R INFERNO_DELEGATE_SEPARATOR INFERNO_DELEGATE_TEMPLATE_PARAMS>
#ifdef INFERNO_DELEGATE_PREFERRED_SYNTAX
	class INFERNO_DELEGATE_INVOKER_CLASS_NAME<R (INFERNO_DELEGATE_TEMPLATE_ARGS)>
#else
	class INFERNO_DELEGATE_INVOKER_CLASS_NAME
#endif
	{
		INFERNO_DELEGATE_INVOKER_DATA

	public:
		INFERNO_DELEGATE_INVOKER_CLASS_NAME(INFERNO_DELEGATE_PARAMS)
#if INFERNO_DELEGATE_PARAM_COUNT > 0
			:
#endif
			INFERNO_DELEGATE_INVOKER_INITIALIZATION_LIST
		{
		}

		template <class TDelegate>
		R operator()(TDelegate d) const
		{
			return d(INFERNO_DELEGATE_ARGS);
		}
	};
}

#undef INFERNO_DELEGATE_CLASS_NAME
#undef INFERNO_DELEGATE_SEPARATOR
#undef INFERNO_DELEGATE_JOIN_MACRO
#undef INFERNO_DELEGATE_DO_JOIN
#undef INFERNO_DELEGATE_DO_JOIN2
