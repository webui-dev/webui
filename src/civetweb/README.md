Changes currently required when updating the submodule to ensure full direct interoperability for wrappers:

- Prevent warnings for e.g. `malloc` and `snprintf` usage outside civetweb.

  ```diff
  + // Disable All Warnings
  + #ifdef _MSC_VER
  + #pragma warning(push, 0)
  + #pragma warning(disable: 4996)
  + #elif defined(__clang__)
  + #pragma clang system_header
  + #elif defined(__GNUC__)
  + #pragma GCC system_header
  + #endif

  /* Copyright (c) 2013-2024 the Civetweb developers
  ```

  ```diff
  + #ifndef NDEBUG
  #define malloc DO_NOT_USE_THIS_FUNCTION__USE_mg_malloc
  #define calloc DO_NOT_USE_THIS_FUNCTION__USE_mg_calloc
  #define realloc DO_NOT_USE_THIS_FUNCTION__USE_mg_realloc
  #define free DO_NOT_USE_THIS_FUNCTION__USE_mg_free
  #define snprintf DO_NOT_USE_THIS_FUNCTION__USE_mg_snprintf
  + #endif
  ```

- Allow compiling with swift compiler chaning an incompatible structname

  Rename struct `ah` to `auth_header` [civetweb.c#L8715](https://github.com/webui-dev/webui/blob/ea5540c833b3e4ae38cc8dc7d62965a8507a78ee/src/civetweb/civetweb.c#L8715)
  In its related code there is also a variable name `auth_header`, make sure to rename it first to prevent conflicts.
