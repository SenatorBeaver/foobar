
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const int STATUS_OK = 0;
const int STATUS_INVALID_RESPONSE = -1;
const int STATUS_ERROR = -2;
const int STATUS_INVALID_PARAMETER = -3;

typedef struct
{
  int16_t agc_val;
  int16_t agc_to_pwr;
} QRXFTM_response;



const char QRXTRM_PREFIX[] = "+QRXFTM:";
const char QRXTRM_OK[] = "OK";
const char QRXTRM_ERROR[] = "ERROR";

int get_value_from_buf(long int* value, const char* buf, char** end)
{
  if (value == NULL) {
    return STATUS_INVALID_PARAMETER;
  }
  if (buf == NULL) {
    return STATUS_INVALID_PARAMETER;
  }

  const int BASE_10 = 10;
  *value = strtol(buf, end, BASE_10);
  if (*end <= buf || *value == LONG_MIN || *value == LONG_MAX)
  {
    return STATUS_INVALID_RESPONSE;
  }
  return STATUS_OK;
}

int get_QRXFTM(QRXFTM_response* response, const char* buf, int size)
{
  if (response == NULL)
  {
    return STATUS_INVALID_PARAMETER;
  }
  if (buf == NULL)
  {
    return STATUS_INVALID_PARAMETER;
  }
  const size_t len_of_prefix = sizeof(QRXTRM_PREFIX) - 1;
  if (strncmp(QRXTRM_PREFIX, buf, len_of_prefix) != 0)
  {
    return STATUS_INVALID_RESPONSE;
  }
  buf += len_of_prefix;
  size -= len_of_prefix;


  if (strstr(buf, QRXTRM_ERROR) != NULL)
  {
    return STATUS_ERROR;
  }

  if (strstr(buf, QRXTRM_OK) == NULL)
  {
    return STATUS_INVALID_RESPONSE;
  }

  const char* comma_ptr = strchr(buf, ',');
  char* end = NULL;
  long int value;

  int retcode = get_value_from_buf(&value, buf, &end);
  if (retcode != STATUS_OK)
  {
    return retcode;
  }
  if (end != comma_ptr) // example: +QRXFTM: -176foobar,-32 
  {
    return STATUS_INVALID_RESPONSE;
  }
  response->agc_val = (int16_t)value;

  unsigned ptr_diff = end - buf;
  const unsigned COMMA_SIZE = 1;
  buf += ptr_diff + COMMA_SIZE;
  size -= ptr_diff + COMMA_SIZE;

  const char* lf_ptr = strchr(buf, '\n');
  retcode = get_value_from_buf(&value, buf, &end);
  if (retcode != STATUS_OK)
  {
    return retcode;
  }
  if (end != lf_ptr) // example: +QRXFTM: -176,-32foobar\n 
  {
    return STATUS_INVALID_RESPONSE;
  }

  response->agc_to_pwr = (int16_t)value;

  return STATUS_OK;
}


int main()
{
  QRXFTM_response response;
  const char test_response_OK[] = "+QRXFTM: -176,-65\n\nOK\n";
  const char test_response_ERROR[] = "+QRXFTM: ERROR\n";
  const char test_response_INVALID[] = "+XRWFTM: -176,-65\n\nOK\n";
  const char test_response_INVALID2[] = "+QRXFTM: x,-65\n\nOK\n";
  assert(get_QRXFTM(&response, test_response_INVALID2, sizeof(test_response_INVALID2)) == STATUS_INVALID_RESPONSE);
  assert(get_QRXFTM(&response, test_response_OK, sizeof(test_response_OK)) == STATUS_OK);
  assert(response.agc_val == -176);
  assert(response.agc_to_pwr == -65);
  assert(get_QRXFTM(&response, test_response_ERROR, sizeof(test_response_ERROR)) == STATUS_ERROR);
  assert(get_QRXFTM(&response, test_response_INVALID, sizeof(test_response_INVALID)) == STATUS_INVALID_RESPONSE);
  assert(get_QRXFTM(NULL, test_response_INVALID, sizeof(test_response_INVALID)) == STATUS_INVALID_PARAMETER);
  assert(get_QRXFTM(&response, NULL, sizeof(test_response_INVALID)) == STATUS_INVALID_PARAMETER);

  return 0;
}