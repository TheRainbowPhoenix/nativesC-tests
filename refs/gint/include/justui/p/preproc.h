//---
// JustUI.util.preproc: Preprocessor utilities
//---

#ifndef _J_UTIL_PREPROC
#define _J_UTIL_PREPROC

/* J_REPEAT: Dispatch a partially-evaluated macro call on each arg in a list
   The call `J_REPEAT(X, C, V1, ..., VN)` where `C = (C1, ..., CM)` (with the
   parentheses) is equivalent to the series of calls

   ```
   X(C1, ..., CM, V1)
   X(C1, ..., CM, V2)
   ...
   X(C1, ..., CM, VN)
   ```

   and is used to iterate on lists. There is a size limit of N≤8. */
#define J_REPEAT1(X, C, V1, ...) \
    J_CALL(X, C, V1) __VA_OPT__(J_REPEAT2(X, C, __VA_ARGS__))
#define J_REPEAT2(X, C, V2, ...) \
    J_CALL(X, C, V2) __VA_OPT__(J_REPEAT3(X, C, __VA_ARGS__))
#define J_REPEAT3(X, C, V3, ...) \
    J_CALL(X, C, V3) __VA_OPT__(J_REPEAT4(X, C, __VA_ARGS__))
#define J_REPEAT4(X, C, V4, ...) \
    J_CALL(X, C, V4) __VA_OPT__(J_REPEAT5(X, C, __VA_ARGS__))
#define J_REPEAT5(X, C, V5, ...) \
    J_CALL(X, C, V5) __VA_OPT__(J_REPEAT6(X, C, __VA_ARGS__))
#define J_REPEAT6(X, C, V6, ...) \
    J_CALL(X, C, V6) __VA_OPT__(J_REPEAT7(X, C, __VA_ARGS__))
#define J_REPEAT7(X, C, V7, ...) \
    J_CALL(X, C, V7) __VA_OPT__(J_REPEAT8(X, C, __VA_ARGS__))
#define J_REPEAT8(X, C, V8, ...) \
    ({ __VA_OPT__(_Static_assert(0, \
        "J_REPEAT: too many macro arguments (maximum 8)");) \
    J_CALL(X, C, V8); })
#define J_REPEAT(X, C, ...) __VA_OPT__(J_REPEAT1(X, C, __VA_ARGS__))

/* J_CALL: Perform a call to a partially evaluated macro
   The call `J_CALL(X, C, A1, ..., AN)` where `C = (C1, ..., CM)` (with the
   parentheses) reduces to `X(C1, ..., CM, A1, ..., AN)`, i.e. it calls the
   already-partially-applied `X(C)` with further arguments. Both M=0 (`C=()`)
   and N=0 (no variadic arguments) are allowed.

   The main difficulty is "unfolding" `C` into the arguments of `X`. This
   problem is dealt with by absorbing the parentheses into an ID-function macro
   call:

   ```
       J_ID C
   ~>  J_ID (C1, ..., CM)
   ~>  , C1, ..., CM
  ```

   From there, the only subtletly is gobbling the commas. We gobble the comma
   before `J_ID C` if M=0 and the comma before the other arguments if N=0. This
   requires a few expansion stages. */
#define J_ID(...) __VA_OPT__(,) __VA_ARGS__
#define J_CALL3(X, ...) X(__VA_ARGS__)
#define J_CALL2(...) J_CALL3(__VA_ARGS__)
#define J_CALL(X, C, ...) J_CALL2(X J_ID C, ##__VA_ARGS__)

#endif /* _J_UTIL_PREPROC */
