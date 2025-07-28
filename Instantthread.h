/*
 * protothread.h
 *
 *  Created on: Jul 26, 2025
 *      Author: admin
 */

#ifndef CORO_PROTOTHREAD_H_
#define CORO_PROTOTHREAD_H_

#define InstantCoroutine_Panic(...) do { cppCoroutine_State.current = CppCoroutine_State::Final; {__VA_ARGS__;} } while (false)
#include "InstantCoroutine.h"

#define pt_return(...) CoroutineStop(__VA_ARGS__)
#define pt_yield(...) CoroutineYield(__VA_ARGS__)
#define pt_yield_until(condition, ...) CoroutineYieldUntil(condition, __VA_ARGS__)



#endif /* CORO_PROTOTHREAD_H_ */
