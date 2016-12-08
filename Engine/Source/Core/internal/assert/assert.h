#pragma once

#ifndef CONTRACTS_H
#define CONTRACTS_H

#include <exception>
#include <stdexcept>

//
// There are three configuration options for this implementation's behavior
// when pre/post conditions on the types are violated:
//
// 1. TERMINATE_ON_CONTRACT_VIOLATION: std::terminate will be called (default)
// 2. THROW_ON_CONTRACT_VIOLATION: a gsl::fail_fast exception will be thrown
// 3. UNENFORCED_ON_CONTRACT_VIOLATION: nothing happens  
//
#define THROW_ON_CONTRACT_VIOLATION

#if !(defined(THROW_ON_CONTRACT_VIOLATION) ^ defined(TERMINATE_ON_CONTRACT_VIOLATION) ^ defined(UNENFORCED_ON_CONTRACT_VIOLATION))
#define TERMINATE_ON_CONTRACT_VIOLATION 
#endif


#define STRINGIFY_DETAIL(x) #x
#define STRINGIFY(x) STRINGIFY_DETAIL(x)

#if defined(THROW_ON_CONTRACT_VIOLATION)

#define Expects(cond)  if (!(cond)) \
    throw std::runtime_error("Precondition failure at " __FILE__ ": " STRINGIFY(__LINE__))
#define Ensures(cond)  if (!(cond)) \
    throw std::runtime_error("Postcondition failure at " __FILE__ ": " STRINGIFY(__LINE__))


#elif defined(TERMINATE_ON_CONTRACT_VIOLATION)


#define Expects(cond)           if (!(cond)) std::terminate()
#define Ensures(cond)           if (!(cond)) std::terminate()


#elif defined(UNENFORCED_ON_CONTRACT_VIOLATION)

#define Expects(cond)           
#define Ensures(cond)           

#endif 


#endif // CONTRACTS_H