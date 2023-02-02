// Work around different directory structure for utfcpp when using conan vs fetch content
#if __has_include(<utf8.h>)
#include <utf8.h>
#else
#include <utfcpp/utf8.h>
#endif
