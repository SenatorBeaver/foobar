
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

const int STATUS_OK = 0;
const int STATUS_INVALID_RESPONSE = -1;
const int STATUS_ERROR = -2;
const int STATUS_INVALID_PARAMETER = -3;

typedef struct 
{
  int16_t agc_val;
  int16_t agc_to_pwr;
} QRXFTM_response;

int get_QRXFTM(QRXFTM_response* response, const char* buf, size_t size)
{
  if (response != NULL)
  {
    return STATUS_INVALID_PARAMETER;
  }
  return 0;
}


int main()
{
  QRXFTM_response response;
  const char test_response_OK[] = "+QRXFTM: -176,-65\n\nOK\n";
  const char test_response_ERROR[] = "+QRWFTM: -176,-65\n\nERROR\n";
  const char test_response_INVALID[] = "+XRWFTM: -176,-65\n\nOK\n";
  assert(get_QRXFTM(&response, test_response_OK, sizeof(test_response_OK)) == STATUS_OK);
  assert(response.agc_val == -176);
  assert(response.agc_to_pwr == -65);
  assert(get_QRXFTM(&response, test_response_ERROR, sizeof(test_response_ERROR)) == STATUS_ERROR);
  assert(get_QRXFTM(&response, test_response_INVALID, sizeof(test_response_INVALID)) == STATUS_INVALID_RESPONSE);

  return 0;
}