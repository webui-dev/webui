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