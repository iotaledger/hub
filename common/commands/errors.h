/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef COMMON_ERRORS_H_
#define COMMON_ERRORS_H_

#include <string>
#include <unordered_map>

#include <grpc++/support/sync_stream.h>
#include "grpc++/grpc++.h"

namespace common {

namespace cmd {

typedef enum {

  OK = 0,
  MISSING_ARGUMENT,
  WRONG_ARGUMENT_NAME,
  CANCELLED,
  USER_EXISTS,
  USER_DOES_NOT_EXIST,
  UNKNOWN_ADDRESS,
  UNKNOWN_ERROR,
  GET_ADDRESS_FAILED,
  INVALID_UUID,
  INVALID_CHECKSUM,
  INVALID_ADDRESS,
  ADDRESS_WAS_SPENT,
  IOTA_NODE_UNAVAILABLE,
  WITHDRAWAL_CAN_NOT_BE_CANCELLED,
  INVALID_AUTHENTICATION,
  SIGNATURE_FAILED,
  WRONG_USER_ADDRESS,
  ADDRESS_NOT_KNOWN_TO_NODE,
  ADDRESS_BALANCE_ZERO,
  BATCH_INVALID,
  BATCH_INCONSISTENT,
  BATCH_AMOUNT_NOT_ZERO,
  INSUFFICIENT_BALANCE,

} Error;

std::unordered_map<Error, std::string> errorToStringMapCreate();

grpc::Status errorToGrpcError(Error err);

std::string getErrorString(Error);

}  // namespace cmd
}  // namespace common

#endif  // COMMON_ERRORS_H_
