#ifndef _STATIC_ASSERT_H_
#define _STATIC_ASSERT_H_

// The 'static_assert' macro is expected to become availabe with C11 compliance
#ifndef static_assert
#define static_assert(assertion, reason /* unused */) \
    extern char compile_assert_array[1/((assertion) != 0)]
// The second parameter is not used, but will ensure that code
// does not need to be changed when the macro becomes available
#endif

#endif /*_STATIC_ASSERT_H_*/
