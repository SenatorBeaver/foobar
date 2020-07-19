
#include <assert.h>
#include <stddef.h>

const int STATUS_OK = 0;
const int STATUS_INVALID_RESPONSE = -1;
const int STATUS_ERROR = -2;


int get_QRXFTM(const char* buf, size_t size)
{
  return 0;
}


int main()
{
  const char test_response_OK[] = "+QRXFTM: -176,-65\n\nOK\n";
  const char test_response_ERROR[] = "+QRWFTM: -176,-65\n\nERROR\n";
  const char test_response_INVALID[] = "+XRWFTM: -176,-65\n\nOK\n";
  assert(get_QRXFTM(test_response_OK, sizeof(test_response_OK)) == STATUS_OK);
  assert(get_QRXFTM(test_response_ERROR, sizeof(test_response_ERROR)) == STATUS_ERROR);
  assert(get_QRXFTM(test_response_INVALID, sizeof(test_response_INVALID)) == STATUS_INVALID_RESPONSE);

  return 0;
}