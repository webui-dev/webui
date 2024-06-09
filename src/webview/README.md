Changes currently required when updating the submodule to ensure full direct interoperability for wrappers:

- Prevent warnings.

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

  /* this ALWAYS GENERATED file contains the definitions for the interfaces */
  ```
