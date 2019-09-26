/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/hub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/commands/errors.h"

namespace common {

namespace cmd {

static std::unordered_map<Error, std::string> errorToStringMap =
    errorToStringMapCreate();

std::unordered_map<Error, std::string> errorToStringMapCreate() {
  std::unordered_map<Error, std::string> map;
  map[OK] = "Ok";
  map[MISSING_ARGUMENT] = "Missing argument";
  map[WRONG_ARGUMENT_NAME] = "Wrong argument name";
  map[CANCELLED] = "Cancelled";
  map[USER_EXISTS] = "User exist";
  map[USER_DOES_NOT_EXIST] = "User does not exist";
  map[UNKNOWN_ADDRESS] = "Unknown address";
  map[UNKNOWN_ERROR] = "Unknown error";
  map[GET_ADDRESS_FAILED] = "Get address failed";
  map[INVALID_UUID] = "Invalid UUID";
  map[INVALID_CHECKSUM] = "Invalid checksum";
  map[INVALID_ADDRESS] = "Invalid address";
  map[ADDRESS_WAS_SPENT] = "Address was already spent";
  map[IOTA_NODE_UNAVAILABLE] = "IOTA node is unavailable";
  map[WITHDRAWAL_CAN_NOT_BE_CANCELLED] = "Withdrawal can not be cancelled";
  map[INVALID_AUTHENTICATION] = "Invalid authentication";
  map[ADDRESS_NOT_KNOWN_TO_NODE] = "Address is not known to node";
  map[SIGNATURE_FAILED] = "Signing has failed";
  map[WRONG_USER_ADDRESS] = "Wrong user address";
  map[ADDRESS_BALANCE_ZERO] = "Address balance is zero";
  map[BATCH_INVALID] = "Batch is invalid";
  map[BATCH_INCONSISTENT] = "Batch is inconsistent";
  map[BATCH_AMOUNT_NOT_ZERO] = "Batch amount is not zero (unbalanced)";
  map[INSUFFICIENT_BALANCE] = "Insufficient balance";

  return map;
}

std::string getErrorString(Error error) { return errorToStringMap.at(error); }

grpc::Status errorToGrpcError(Error err) {
  grpc::Status status;
  switch (err) {
    case OK:
      status = grpc::Status::OK;
      break;

    case MISSING_ARGUMENT:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(MISSING_ARGUMENT));
      break;

    case WRONG_ARGUMENT_NAME:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(WRONG_ARGUMENT_NAME));
      break;

    case CANCELLED:
      status = grpc::Status::CANCELLED;
      break;
    case USER_EXISTS:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(USER_EXISTS));
      break;
    case UNKNOWN_ADDRESS:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(UNKNOWN_ADDRESS));
      break;

    case UNKNOWN_ERROR:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(UNKNOWN_ERROR));
      break;

    case USER_DOES_NOT_EXIST:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(USER_DOES_NOT_EXIST));
      break;

    case GET_ADDRESS_FAILED:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(GET_ADDRESS_FAILED));
      break;

    case INVALID_UUID:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(INVALID_UUID));
      break;
    case INVALID_CHECKSUM:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(INVALID_CHECKSUM));
      break;
    case INVALID_ADDRESS:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(INVALID_ADDRESS));
      break;

    case IOTA_NODE_UNAVAILABLE:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(IOTA_NODE_UNAVAILABLE));
      break;

    case ADDRESS_WAS_SPENT:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(ADDRESS_WAS_SPENT));
      break;

    case WITHDRAWAL_CAN_NOT_BE_CANCELLED:
      status =
          grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                       errorToStringMap.at(WITHDRAWAL_CAN_NOT_BE_CANCELLED));
      break;

    case INVALID_AUTHENTICATION:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(INVALID_AUTHENTICATION));
      break;

    case SIGNATURE_FAILED:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(SIGNATURE_FAILED));

    case WRONG_USER_ADDRESS:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(WRONG_USER_ADDRESS));
      break;

    case ADDRESS_NOT_KNOWN_TO_NODE:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(ADDRESS_NOT_KNOWN_TO_NODE));
      break;

    case ADDRESS_BALANCE_ZERO:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(ADDRESS_BALANCE_ZERO));
      break;

    case BATCH_AMOUNT_NOT_ZERO:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(BATCH_AMOUNT_NOT_ZERO));
      break;

    case BATCH_INCONSISTENT:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(BATCH_INCONSISTENT));
      break;

    case BATCH_INVALID:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(BATCH_INVALID));
      break;

    case INSUFFICIENT_BALANCE:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            errorToStringMap.at(INSUFFICIENT_BALANCE));
      break;
    default:
      status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                            "unrecognized error code");
      break;
  }

  return status;
}
}  // namespace cmd
}  // namespace common
