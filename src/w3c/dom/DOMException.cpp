#include <w3c/dom/DOMException.hpp>

namespace dom {

static const char * dom_error_strings[] = {
  "",
  "INDEX_SIZE_ERR",
  "DOMSTRING_SIZE_ERR",
  "HIERARCHY_REQUEST_ERR",
  "WRONG_DOCUMENT_ERR",
  "INVALID_CHARACTER_ERR",
  "NO_DATA_ALLOWED_ERR",
  "NO_MODIFICATION_ALLOWED_ERR",
  "NOT_FOUND_ERR",
  "NOT_SUPPORTED_ERR",
  "INUSE_ATTRIBUTE_ERR",
    // Introduced in DOM Level 2:
  "INVALID_STATE_ERR",
    // Introduced in DOM Level 2:
  "SYNTAX_ERR",
    // Introduced in DOM Level 2:
  "INVALID_MODIFICATION_ERR",
    // Introduced in DOM Level 2:
  "NAMESPACE_ERR",
    // Introduced in DOM Level 2:
  "INVALID_ACCESS_ERR",
};

const char *
DOMException::what() const throw()
{
  return dom_error_strings[code];
}
}
