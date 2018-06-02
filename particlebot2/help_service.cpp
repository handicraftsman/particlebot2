#include "help_service.hpp"

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

pb2::help_service::help_service() {}
pb2::help_service::~help_service() {}