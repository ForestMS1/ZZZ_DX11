#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <format>

#include <memory>

#include <thread>

#include <mutex>
#include <boost/version.hpp>
#include <boost/asio.hpp>

#include "Server_Defines.h"
#include "../../Common/CommonStruct.h"


using namespace std;
using namespace boost::asio;
using ip::tcp;
using ip::udp;