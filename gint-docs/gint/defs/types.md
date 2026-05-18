# types

gint:defs:types - Type definitions


## Macros


### `pad_nam2`

Giving a type to padding bytes is misguiding, let's hide it in a macro


```c
#define pad_nam2(c) _ ## c
```


---


### `pad_name`


```c
#define pad_name(c) pad_nam2(c)
```


---


### `pad`


```c
#define pad(bytes) uint8_t pad_name(__COUNTER__)[bytes]
```


---


### `byte_union`

byte_union() - union between an uint8_t 'byte' element and a bit field


```c
#define byte_union(name, fields) \
```


---


### `word_union`

word_union() - union between an uint16_t 'word' element and a bit field


```c
#define word_union(name, fields) \
```


---


### `lword_union`

lword_union() - union between an uint32_t 'lword' element and a bit field


```c
#define lword_union(name, fields) \
```


---
