/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/rpchub
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/commands/errors.h"


namespace common {

    namespace cmd {

        grpc::Status errorToGrpcError( Error err){
            grpc::Status status;
            switch (err) {
                case OK:
                    status = grpc::Status::OK;
                    break;

                case CANCELLED:
                    status = grpc::Status::CANCELLED;
                    break;
                case USER_EXISTS:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                      "User does not exist");
                    break;
                case UNKNOWN_ADDRESS:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "Unknown address");
                    break;

                case UNKNOWN_ERROR:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "Unknown error");
                    break;

                case USER_DOES_NOT_EXIST:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "User does not exist");
                    break;

                case GET_ADDRESS_FAILED:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "failed in trying to get the address");
                    break;

                case INVALID_UUID:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "invalid UUID");
                    break;
                case INVALID_CHECKSUM:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "invalid checksum");
                    break;
                case INVALID_ADDRESS:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "invalid address");
                    break;

                case IOTA_NODE_UNAVAILABLE:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "iota node is unavailable");
                    break;

                case ADDRESS_WAS_SPENT:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "address was already spent");
                    break;

                case WITHDRAWAL_CAN_NOT_BE_CANCELLED:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "Withdrawal cannot be cancelled");
                    break;


                case INVALID_AUTHENTICATION:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "Invalid authentication");
                    break;

                case SIGNATURE_FAILED:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "signature failed");

                case WRONG_USER_ADDRESS:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "wrong user address");
                    break;

                case ADDRESS_NOT_KNOWN_TO_NODE:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "address not known to node");
                    break;

                case ADDRESS_BALANCE_ZERO:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "address balance is zero");
                    break;

                case BATCH_AMOUNT_NOT_ZERO:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "batch amount is not zero");
                    break;

                case BATCH_INCONSISTENT:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "batch is inconsistent");
                    break;

                case BATCH_INVALID:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "batch is invalid");
                    break;

                case INSUFFICIENT_BALANCE:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "insufficient balance");
                    break;
                default:
                    status = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "",
                                          "unrecognized error code");
                    break;
            }

            return status;
        }
    }
}

