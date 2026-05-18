# preproc

JustUI.util.preproc: Preprocessor utilities


## Macros


### `J_REPEAT1`

and is used to iterate on lists. There is a size limit of N≤8.


```c
#define J_REPEAT1(X, C, V1, ...) \
```


---


### `J_REPEAT2`


```c
#define J_REPEAT2(X, C, V2, ...) \
```


---


### `J_REPEAT3`


```c
#define J_REPEAT3(X, C, V3, ...) \
```


---


### `J_REPEAT4`


```c
#define J_REPEAT4(X, C, V4, ...) \
```


---


### `J_REPEAT5`


```c
#define J_REPEAT5(X, C, V5, ...) \
```


---


### `J_REPEAT6`


```c
#define J_REPEAT6(X, C, V6, ...) \
```


---


### `J_REPEAT7`


```c
#define J_REPEAT7(X, C, V7, ...) \
```


---


### `J_REPEAT8`


```c
#define J_REPEAT8(X, C, V8, ...) \
```


---


### `J_REPEAT`


```c
#define J_REPEAT(X, C, ...) __VA_OPT__(J_REPEAT1(X, C, __VA_ARGS__))
```


---


### `J_ID`

requires a few expansion stages.


```c
#define J_ID(...) __VA_OPT__(,) __VA_ARGS__
```


---


### `J_CALL3`


```c
#define J_CALL3(X, ...) X(__VA_ARGS__)
```


---


### `J_CALL2`


```c
#define J_CALL2(...) J_CALL3(__VA_ARGS__)
```


---


### `J_CALL`


```c
#define J_CALL(X, C, ...) J_CALL2(X J_ID C, ##__VA_ARGS__)
```


---
