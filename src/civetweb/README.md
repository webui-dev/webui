Changes currently required when updating the submodule to ensure full direct interoperability for wrappers:

- Prevent warnings for e.g. `malloc` and `snprintf` usage outside civetweb.

  ```diff
  + #ifndef NDEBUG
  #define malloc DO_NOT_USE_THIS_FUNCTION__USE_mg_malloc
  #define calloc DO_NOT_USE_THIS_FUNCTION__USE_mg_calloc
  #define realloc DO_NOT_USE_THIS_FUNCTION__USE_mg_realloc
  #define free DO_NOT_USE_THIS_FUNCTION__USE_mg_free
  #define snprintf DO_NOT_USE_THIS_FUNCTION__USE_mg_snprintf
  + #endif
  ```
