/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkThreadedCallbackQueue.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkObjectFactory.h"

#include <chrono>
#include <stdexcept>
#include <tuple>
#include <type_traits>

VTK_ABI_NAMESPACE_BEGIN

//=============================================================================
struct vtkThreadedCallbackQueue::InvokerFutureSharedStateBase
{
  InvokerFutureSharedStateBase()
    : NumberOfPriorSharedFuturesRemaining(0)
    , IsReady(false)
  {
  }

  /**
   *
   */
  void Wait() const;

  /**
   * Number of futures that need to terminate before we can run.
   */
  std::atomic_int NumberOfPriorSharedFuturesRemaining;

  /**
   * When the invoker has terminated, it must set this boolean to true.
   */
  std::atomic_bool IsReady;

  /**
   * List of futures which are depending on us. This is filled by them as they get pushed if we are
   * not done with our task.
   */
  std::vector<SharedFutureBasePointer> DependentSharedFutures;

  /**
   * Constructing is true until we have filled the DependentSharedFutures of the futures we depend
   * on, and until NumberOfPriorSharedFuturesRemaining is not final.
   */
  bool Constructing = true;

  mutable std::mutex Mutex;
  mutable std::condition_variable ConditionVariable;
};

//-----------------------------------------------------------------------------
void vtkThreadedCallbackQueue::InvokerFutureSharedStateBase::Wait() const
{
  if (this->IsReady)
  {
    return;
  }
  std::unique_lock<std::mutex> lock(this->Mutex);
  if (!this->IsReady)
  {
    this->ConditionVariable.wait(lock, [this] { return this->IsReady == true; });
  }
}

//=============================================================================
template <>
struct vtkThreadedCallbackQueue::InvokerFutureSharedState<void>
  : public vtkThreadedCallbackQueue::InvokerFutureSharedStateBase
{
  using ReturnLValueRef = void;
  using ReturnConstLValueRef = void;

  void Set()
  {
    if (this->IsReady)
    {
      throw std::logic_error("Result already set.");
    }
    {
      std::lock_guard<std::mutex> lock(this->Mutex);
      this->IsReady = true;
    }
    this->ConditionVariable.notify_all();
  }

  void Get() { this->Wait(); }
};

//=============================================================================
template <class ReturnT>
struct vtkThreadedCallbackQueue::InvokerFutureSharedState<ReturnT>
  : public vtkThreadedCallbackQueue::InvokerFutureSharedStateBase
{
  using ReturnLValueRef = ReturnT&;
  using ReturnConstLValueRef = const ReturnT&;

  ReturnT& Get()
  {
    this->Wait();
    return this->Value;
  }

  const ReturnT& Get() const
  {
    this->Wait();
    return this->Value;
  }

  void Set(ReturnT&& val)
  {
    if (this->IsReady)
    {
      throw std::logic_error("Result already set.");
    }
    {
      std::lock_guard<std::mutex> lock(this->Mutex);
      this->IsReady = true;
      this->Value = std::move(val);
    }
    this->ConditionVariable.notify_all();
  }

  ReturnT Value;
};

//=============================================================================
struct vtkThreadedCallbackQueue::InvokerImpl
{
  /**
   * Substitute for std::integer_sequence which is C++14
   */
  template <std::size_t... Is>
  struct IntegerSequence;
  template <std::size_t N, std::size_t... Is>
  struct MakeIntegerSequence;

  /**
   * This function is used to discriminate whether you can dereference the template parameter T.
   * It uses SNFINAE and jumps to the std::false_type version if it fails dereferencing T.
   */
  template <class T>
  static decltype(*std::declval<T&>(), std::true_type{}) CanBeDereferenced(std::nullptr_t);
  template <class>
  static std::false_type CanBeDereferenced(...);

  template <class T, class CanBeDereferencedT = decltype(CanBeDereferenced<T>(nullptr))>
  struct DereferenceImpl;

  /**
   * Convenient typedef that use Signature to convert the parameters of function of type FT
   * to a std::tuple.
   */
  template <class FT, std::size_t N = Signature<typename std::decay<FT>::type>::ArgsSize>
  using ArgsTuple = typename Signature<typename std::decay<FT>::type>::ArgsTuple;

  /**
   * Convenient typedef that, given a function of type FT and an index I, returns the type of the
   * Ith parameter of the function.
   */
  template <class FT, std::size_t I>
  using ArgType = typename std::tuple_element<I, ArgsTuple<FT>>::type;

  /**
   * This helper function returns a tuple of cherry-picked types from the argument types from
   * the input function or the argument types of the provided input following this criterion:
   * * If the input function expects an lvalue reference, store it in its decaid type.
   * * Otherwise, store it as is (decaid input type). The conversion to the function argument
   *   type will be done upon invoking.
   *
   * This specific casting allows us to permit calling the constructor for rvalue reference inputs
   * when the type differs from the one provided by the user (take a const char* input when the
   * function expects a std::string for instance).
   * We want to keep the input type in all other circumstances in case the user inputs smart
   * pointers and the function only expects a raw pointer. If we casted it to the function argument
   * type, we would not own a reference of the smart pointer.
   *
   * FunctionArgsTupleT is a tuple of the function native argument types (extracted from its
   * signature), and InputArgsTupleT is a tuple of the types provided by the user as input
   * parameters.
   */
  template <class FunctionArgsTupleT, class InputArgsTupleT, std::size_t... Is>
  static std::tuple<typename std::conditional<
    std::is_lvalue_reference<typename std::tuple_element<Is, FunctionArgsTupleT>::type>::value,
    typename std::decay<typename std::tuple_element<Is, FunctionArgsTupleT>::type>::type,
    typename std::decay<typename std::tuple_element<Is, InputArgsTupleT>::type>::type>::type...>
  GetStaticCastArgsTuple(IntegerSequence<Is...>);

  /**
   * Convenient typedef to create a tuple of types allowing to call the constructor of the function
   * argument type when relevant, or hold a copy of the input parameters provided by the user
   * instead.
   */
  template <class FunctionArgsTupleT, class... InputArgsT>
  using StaticCastArgsTuple =
    decltype(GetStaticCastArgsTuple<FunctionArgsTupleT, std::tuple<InputArgsT...>>(
      MakeIntegerSequence<sizeof...(InputArgsT)>()));

  /**
   * This holds the attributes of a function.
   * There are 2 implementations: one for member function pointers, and one for all the others
   * (functors, lambdas, function pointers)
   */
  template <bool IsMemberFunctionPointer, class... ArgsT>
  class InvokerHandle;

  /**
   * Actually invokes the function and sets its future. An specific implementation is needed for
   * void return types.
   */
  template <class ReturnT>
  struct InvokerHelper
  {
    template <class InvokerT>
    static void Invoke(InvokerT&& invoker, InvokerFutureSharedState<ReturnT>* state)
    {
      state->Set(invoker());
    }
  };
};

//=============================================================================
template <>
struct vtkThreadedCallbackQueue::InvokerImpl::InvokerHelper<void>
{
  template <class InvokerT>
  static void Invoke(InvokerT&& invoker, InvokerFutureSharedState<void>* state)
  {
    invoker();
    state->Set();
  }
};

//=============================================================================
// For lamdas or std::function
template <class ReturnT, class... ArgsT>
struct vtkThreadedCallbackQueue::Signature<ReturnT(ArgsT...)>
{
  using ArgsTuple = std::tuple<ArgsT...>;
  using InvokeResult = ReturnT;
  static constexpr std::size_t ArgsSize = sizeof...(ArgsT);
};

//=============================================================================
// For methods inside a class ClassT
template <class ClassT, class ReturnT, class... ArgsT>
struct vtkThreadedCallbackQueue::Signature<ReturnT (ClassT::*)(ArgsT...)>
{
  using ArgsTuple = std::tuple<ArgsT...>;
  using InvokeResult = ReturnT;
  static constexpr std::size_t ArgsSize = sizeof...(ArgsT);
};

//=============================================================================
// For const methods inside a class ClassT
template <class ClassT, class ReturnT, class... ArgsT>
struct vtkThreadedCallbackQueue::Signature<ReturnT (ClassT::*)(ArgsT...) const>
{
  using ArgsTuple = std::tuple<ArgsT...>;
  using InvokeResult = ReturnT;
  static constexpr std::size_t ArgsSize = sizeof...(ArgsT);
};

//=============================================================================
// For function pointers
template <class ReturnT, class... ArgsT>
struct vtkThreadedCallbackQueue::Signature<ReturnT (*)(ArgsT...)>
{
  using ArgsTuple = std::tuple<ArgsT...>;
  using InvokeResult = ReturnT;
  static constexpr std::size_t ArgsSize = sizeof...(ArgsT);
};

//=============================================================================
// For functors
template <class FT>
struct vtkThreadedCallbackQueue::Signature
  : vtkThreadedCallbackQueue::Signature<decltype(&FT::operator())>
{
};

//=============================================================================
template <std::size_t... Is>
struct vtkThreadedCallbackQueue::InvokerImpl::IntegerSequence
{
};

//=============================================================================
template <std::size_t N, std::size_t... Is>
struct vtkThreadedCallbackQueue::InvokerImpl::MakeIntegerSequence
  : vtkThreadedCallbackQueue::InvokerImpl::MakeIntegerSequence<N - 1, N - 1, Is...>
{
};

//=============================================================================
template <std::size_t... Is>
struct vtkThreadedCallbackQueue::InvokerImpl::MakeIntegerSequence<0, Is...>
  : vtkThreadedCallbackQueue::InvokerImpl::IntegerSequence<Is...>
{
};

//=============================================================================
template <class T>
struct vtkThreadedCallbackQueue::InvokerImpl::DereferenceImpl<T,
  std::true_type /* CanBeDereferencedT */>
{
  using Type = decltype(*std::declval<T>());
  static Type& Get(T& instance) { return *instance; }
};

//=============================================================================
template <class T>
struct vtkThreadedCallbackQueue::InvokerImpl::DereferenceImpl<T,
  std::false_type /* CanBeDereferencedT */>
{
  using Type = T;
  static Type& Get(T& instance) { return instance; }
};

//=============================================================================
template <class T>
struct vtkThreadedCallbackQueue::Dereference<T, std::nullptr_t>
{
  using Type = typename InvokerImpl::DereferenceImpl<T>::Type;
};

//=============================================================================
template <class FT, class ObjectT, class... ArgsT>
class vtkThreadedCallbackQueue::InvokerImpl::InvokerHandle<true /* IsMemberFunctionPointer */, FT,
  ObjectT, ArgsT...>
{
public:
  template <class FTT, class ObjectTT, class... ArgsTT>
  InvokerHandle(FTT&& f, ObjectTT&& instance, ArgsTT&&... args)
    : Function(std::forward<FTT>(f))
    , Instance(std::forward<ObjectTT>(instance))
    , Args(std::forward<ArgsTT>(args)...)
  {
  }

  InvokeResult<FT> operator()() { this->Invoke(MakeIntegerSequence<sizeof...(ArgsT)>()); }

private:
  template <std::size_t... Is>
  InvokeResult<FT> Invoke(IntegerSequence<Is...>)
  {
    // If the input object is wrapped inside a pointer (could be shared_ptr, vtkSmartPointer),
    // we need to dereference the object before invoking it.
    auto& deref = DereferenceImpl<ObjectT>::Get(this->Instance);

    // The static_cast to ArgType forces casts to the correct types of the function.
    // There are conflicts with rvalue references not being able to be converted to lvalue
    // references if this static_cast is not performed
    return (deref.*Function)(static_cast<ArgType<FT, Is>>(std::get<Is>(this->Args))...);
  }

  FT Function;

  // We DO NOT want to hold lvalue references! They could be destroyed before we execute them.
  // This forces to call the copy constructor on lvalue references inputs.
  typename std::decay<ObjectT>::type Instance;

  // We want to hold an instance of the arguments in the type expected by the function rather than
  // the types provided by the user when the function expects a lvalue reference.
  // This way, if there is a conversion to be done, it can be done
  // in the constructor of each type.
  //
  // Example: The user provides a string as "example", but the function expects a std::string&.
  // We can directly store this argument as a std::string and allow to pass it to the function as a
  // std::string.
  StaticCastArgsTuple<ArgsTuple<FT>, ArgsT...> Args;
};

//=============================================================================
template <class FT, class... ArgsT>
class vtkThreadedCallbackQueue::InvokerImpl::InvokerHandle<false /* IsMemberFunctionPointer */, FT,
  ArgsT...>
{
public:
  template <class FTT, class... ArgsTT>
  InvokerHandle(FTT&& f, ArgsTT&&... args)
    : Function(std::forward<FTT>(f))
    , Args(std::forward<ArgsTT>(args)...)
  {
  }

  InvokeResult<FT> operator()() { return this->Invoke(MakeIntegerSequence<sizeof...(ArgsT)>()); }

private:
  template <std::size_t... Is>
  InvokeResult<FT> Invoke(IntegerSequence<Is...>)
  {
    // If the input is a functor and is wrapped inside a pointer (could be shared_ptr),
    // we need to dereference the functor before invoking it.
    auto& f = DereferenceImpl<FT>::Get(this->Function);

    // The static_cast to ArgType forces casts to the correct types of the function.
    // There are conflicts with rvalue references not being able to be converted to lvalue
    // references if this static_cast is not performed
    return f(static_cast<ArgType<decltype(f), Is>>(std::get<Is>(this->Args))...);
  }

  // We DO NOT want to hold lvalue references! They could be destroyed before we execute them.
  // This forces to call the copy constructor on lvalue references inputs.
  typename std::decay<FT>::type Function;

  // We want to hold an instance of the arguments in the type expected by the function rather than
  // the types provided by the user when the function expects a lvalue reference.
  // This way, if there is a conversion to be done, it can be done
  // in the constructor of each type.
  //
  // Example: The user provides a string as "example", but the function expects a std::string&.
  // We can directly store this argument as a std::string and allow to pass it to the function as a
  // std::string.
  StaticCastArgsTuple<ArgsTuple<typename Dereference<FT>::Type>, ArgsT...> Args;
};

//=============================================================================
struct vtkThreadedCallbackQueue::InvokerBase
{
  virtual ~InvokerBase() = default;
  virtual void operator()() = 0;

  virtual std::shared_ptr<InvokerFutureSharedStateBase> GetSharedState() = 0;
  virtual std::shared_ptr<const InvokerFutureSharedStateBase> GetSharedState() const = 0;
};

//=============================================================================
template <class FT, class... ArgsT>
struct vtkThreadedCallbackQueue::Invoker : public vtkThreadedCallbackQueue::InvokerBase
{
  template <class... ArgsTT>
  Invoker(
    std::shared_ptr<InvokerFutureSharedState<InvokeResult<FT>>>& sharedState, ArgsTT&&... args)
    : SharedState(sharedState)
    , Impl(std::forward<ArgsTT>(args)...)
  {
  }

  void operator()() override
  {
    InvokerImpl::InvokerHelper<InvokeResult<FT>>::Invoke(this->Impl, this->SharedState.get());
  }

  std::shared_ptr<InvokerFutureSharedStateBase> GetSharedState() override
  {
    return this->SharedState;
  }

  std::shared_ptr<const InvokerFutureSharedStateBase> GetSharedState() const override
  {
    return this->SharedState;
  }

  std::shared_ptr<InvokerFutureSharedState<InvokeResult<FT>>> SharedState;
  InvokerImpl::InvokerHandle<std::is_member_function_pointer<FT>::value, FT, ArgsT...> Impl;
};

//-----------------------------------------------------------------------------
template <class ReturnT>
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>*
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::New()
{
  VTK_STANDARD_NEW_BODY(vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>);
}

//-----------------------------------------------------------------------------
template <class ReturnT>
vtkThreadedCallbackQueue::SharedFutureBasePointer
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::Clone() const
{
  vtkSharedFuture<ReturnT>* result = vtkSharedFuture<ReturnT>::New();
  result->SharedState = this->SharedState;
  return SharedFutureBasePointer::Take(result);
}

//-----------------------------------------------------------------------------
template <class ReturnT>
void vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::Wait() const
{
  this->SharedState->Wait();
}

//-----------------------------------------------------------------------------
template <class ReturnT>
std::shared_ptr<vtkThreadedCallbackQueue::InvokerFutureSharedStateBase>
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::GetSharedState()
{
  return this->SharedState;
}

//-----------------------------------------------------------------------------
template <class ReturnT>
typename vtkThreadedCallbackQueue::InvokerFutureSharedState<ReturnT>::ReturnLValueRef
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::Get()
{
  return this->SharedState->Get();
}

//-----------------------------------------------------------------------------
template <class ReturnT>
typename vtkThreadedCallbackQueue::InvokerFutureSharedState<ReturnT>::ReturnConstLValueRef
vtkThreadedCallbackQueue::vtkSharedFuture<ReturnT>::Get() const
{
  return this->SharedState->Get();
}

//-----------------------------------------------------------------------------
template <class FT, class... ArgsT>
std::pair<vtkThreadedCallbackQueue::InvokerPointer<FT, ArgsT...>,
  vtkThreadedCallbackQueue::SharedFuturePointer<vtkThreadedCallbackQueue::InvokeResult<FT>>>
vtkThreadedCallbackQueue::CreateInvokerAndSharedFuture(FT&& f, ArgsT&&... args)
{
  auto future = SharedFuturePointer<InvokeResult<FT>>::New();
  future->SharedState = std::make_shared<InvokerFutureSharedState<InvokeResult<FT>>>();
  auto invoker = InvokerPointer<FT, ArgsT...>(new Invoker<FT, ArgsT...>(
    future->SharedState, std::forward<FT>(f), std::forward<ArgsT>(args)...));

  return std::make_pair(std::move(invoker), std::move(future));
}

//-----------------------------------------------------------------------------
template <class SharedFutureContainerT, class InvokerT, class SharedFutureT>
void vtkThreadedCallbackQueue::HandleDependentInvoker(
  SharedFutureContainerT&& priorSharedFutures, InvokerT&& invoker, SharedFutureT&& future)
{
  auto& futureState = future->SharedState;

  // We look at all the dependent futures. Each time we find one, we notify the corresponding
  // invoker that we are waiting on it through its SharedState.
  // We count the number of dependents that are not done yet, and we put this counter bundled
  // with the new invoker inside InvokersOnHold.
  // When the signaled invokers terminate, the counter will be decreased, and when it reaches
  // zero, this invoker will be ready to run.
  if (!priorSharedFutures.empty())
  {
    for (const auto& prior : priorSharedFutures)
    {
      // For each prior, we create a clone of the shared staste so there is no race condition
      // on the same shared_ptr.
      auto state = prior->GetSharedState();

      // We can do a quick check to avoid locking if possible. If the prior shared future is ready,
      // we can just move on.
      if (state->IsReady)
      {
        continue;
      }

      // We need to lock the clone (so we block the invoker side).
      // This way, we can make sure that if the invoker is still running, we notify it that we
      // depend on it before it checks its dependents in SignalDependentSharedFutures
      std::unique_lock<std::mutex> lock(state->Mutex);
      if (!state->IsReady)
      {
        // We notify the invoker we depend on by adding ourselves in DependentSharedFutures.
        state->DependentSharedFutures.emplace_back(future);

        // This does not need to be locked because the shared state of this future is not done
        // constructing yet, so the invoker in SignalDependentSharedFutures will never try do
        // anything with it. And it is okay, because at the end of the day, we increment, the
        // invoker decrements, and if we end up with 0 remaining prior futures, we execute the
        // invoker anyway, so the invoker side has nothing to do.
        lock.unlock();
        ++futureState->NumberOfPriorSharedFuturesRemaining;
      }
    }
  }
  // We notify every invokers we depend on that our SharedState is done constructing. This means
  // that we inserted ourselves in InvokersOnHold if necessary and that if the invoker finds no
  // remaining prior futures in this dependent, it can move the invoker associated with us to the
  // runnin queue.
  std::unique_lock<std::mutex> lock(futureState->Mutex);
  futureState->Constructing = false;
  if (futureState->NumberOfPriorSharedFuturesRemaining)
  {
    std::lock_guard<std::mutex> onHoldLock(this->OnHoldMutex);
    this->InvokersOnHold.emplace(future, std::move(invoker));
  }
  else
  {
    // We can unlock the future lock because we're done touching sensitive data. There's no prior
    // future we're waiting for, so we can just run the invoker and signal our potential dependents.
    lock.unlock();
    (*invoker)();
    this->SignalDependentSharedFutures(invoker.get());
  }
}

//-----------------------------------------------------------------------------
template <class SharedFutureContainerT, class FT, class... ArgsT>
vtkThreadedCallbackQueue::SharedFuturePointer<vtkThreadedCallbackQueue::InvokeResult<FT>>
vtkThreadedCallbackQueue::PushDependent(
  SharedFutureContainerT&& priorSharedFutures, FT&& f, ArgsT&&... args)
{
  using InvokerPointerType = InvokerPointer<FT, ArgsT...>;
  using SharedFuturePointerType = SharedFuturePointer<InvokeResult<FT>>;

  auto pair = this->CreateInvokerAndSharedFuture(std::forward<FT>(f), std::forward<ArgsT>(args)...);

  this->Push(&vtkThreadedCallbackQueue::HandleDependentInvoker<SharedFutureContainerT,
               InvokerPointerType, SharedFuturePointerType>,
    this, std::forward<SharedFutureContainerT>(priorSharedFutures), std::move(pair.first),
    pair.second);

  return pair.second;
}

//-----------------------------------------------------------------------------
template <class FT, class... ArgsT>
vtkThreadedCallbackQueue::SharedFuturePointer<vtkThreadedCallbackQueue::InvokeResult<FT>>
vtkThreadedCallbackQueue::Push(FT&& f, ArgsT&&... args)
{
  auto pair = this->CreateInvokerAndSharedFuture(std::forward<FT>(f), std::forward<ArgsT>(args)...);

  {
    std::lock_guard<std::mutex> lock(this->Mutex);
    this->InvokerQueue.emplace_back(std::move(pair.first));
    this->Empty = false;
  }

  this->ConditionVariable.notify_one();

  return pair.second;
}

VTK_ABI_NAMESPACE_END
