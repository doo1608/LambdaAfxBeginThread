#pragma once

#include <afxwin.h>
#include <tuple>
#include <type_traits>

template<class _Fn, class... _Args>
class MThreadFunction
{
public:
	/// <summary> </summary>
	/// <param name = "_Fx"> Function</param>
	/// <param name = "_Ax"> Argument list</param>
	MThreadFunction(_Fn&& _Fx, _Args&&... _Ax) : Function(std::forward<_Fn>(_Fx)), Argument((std::decay_t<_Args>...)(std::forward<_Args>(_Ax))...) { }

	/// <summary> Run Function </summary>
	decltype(auto) Run() { return apply(Function, Argument); }

private:

	_Fn Function;
	std::tuple<std::decay_t<_Args>...> Argument;

	//https://msdn.microsoft.com/ko-kr/library/mt125500.aspx
	//tuple_size::value = Tuple member count return
	//make_index_sequence<size> = index_sequence
	template<typename F, typename Tuple = tuple<T...>>
	decltype(auto) apply(F&& f, Tuple&& args)
	{
		using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value >;
		return apply_(std::forward<F>(f), std::forward<Tuple>(args), Indices());
	}

	//Repeat get<I>(Arg) operation as much as tuple_size.  tuple_size 만큼 get<I>(Arg) 동작을 반복함.
	//If tuple_size is 3, it becomes f(get<0>(args), get<1>(args), get<2>(args)) . tuple_size가 3이라면, f(get<0>(args), get<1>(args), get<2>(args)) 가 됨
	template<typename F, typename Tuple = tuple<T...>, size_t... I>
	decltype(auto) apply_(F&& f, Tuple&& args, std::index_sequence<I...>)
	{
		//Argument is used as RValue, Argument는 RValue로 사용한다, 
		//LValue becomes a problem when it is destroyed outside. LValue를 사용하면 안되는게 바깥에 있던 변수가 죽어버리면 답이 없다.
		return std::forward<F>(f)(std::get<I>(std::move<Tuple>(args))...);
	}
};


/// <summary> Wrap it using the AfxBeginThread function. </summary>
/// <param name = "func"> Function, Don't use L Value (Ref Value)!!!  </param>
/// <param name = "args"> Argument list</param>
template<class _Fn, class... _Args>
CWinThread* _AfxBeginThread(_Fn&& func, _Args&&... args)
{
	return AfxBeginThread([](LPVOID pParam) -> UINT
	{
		MThreadFunction<_Fn, _Args... > *pFunc = ((MThreadFunction<_Fn, _Args...>*)pParam);
		pFunc->Run();
		delete pFunc;  
		return 0;
	}, (LPVOID)(new MThreadFunction<_Fn, _Args...>(std::forward<_Fn>(func), std::forward<_Args>(args)...)));
}
