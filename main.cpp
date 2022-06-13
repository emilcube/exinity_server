#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <set>
#include <mutex>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include "Logger.hpp"
#include <boost/algorithm/string.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

std::unordered_set<int> num;
unsigned long last_calculated = 0;
double result  = 0;
std::mutex mtx;
std::atomic<bool> stopped{ false };


class Websocket : public std::enable_shared_from_this<Websocket>
{
	websocket::stream<beast::tcp_stream> ws;
	beast::flat_buffer buffer;

public:
	Websocket(tcp::socket&& socket) : ws(std::move(socket)) {}

	void run() {
		ws.async_accept([self{ shared_from_this() }](beast::error_code ec) {
			if (ec) { std::cout << ec.message() << std::endl; return; }
			self->communicator();
		});
	}

	static void dumpFile() {
		Logger::dumplog("start");
		std::cout << "DumpWriterLogs: start\n";
		std::ofstream fs; // ("dump.bin", std::ios::out | std::ios::binary | std::ios::app); //std::string dumpNamefile = "dump.bin";
		std::ios_base::openmode mode = std::ios::out | std::ios::binary | std::ios::app;
		unsigned int old_size = 0;

		while (true) {
			//std::lock_guard<std::mutex> guard(mtx);
			if (stopped.load(std::memory_order_relaxed)) {
				Logger::dumplog("closed");
				std::cout << "DumpWriterLogs: closed\n";
				exit(-2);
			}
			mtx.lock();
			unsigned int new_size = num.size();
			if (new_size != old_size) {
				fs.open(basePath + dumpNamefile, mode);
				if (!fs.is_open()) {
					Logger::dumplog("opening file corruption");
					std::cout << "DumpWriterLogs: error while opening the file\n";
					exit(-2);
				}
				for (auto& x : num) {
					std::string tmp = std::to_string(x);
					char const* pchar = tmp.c_str();
					fs.write(pchar, sizeof pchar);

					//fres  << x << " ";
					//fs.write(reinterpret_cast<const char*>(x), sizeof x);
					//temp_str += std::to_string(x) + " ";

					//char* rez;
					//_itoa_s(x, rez, 10);
					//fs.write(rez, sizeof rez);
					//fs.write((const char*)&x, sizeof(x));
				}
				fs.close();

				Logger::dumplog({ "added elements",std::to_string(new_size - old_size) });
				std::cout << "DumpWriterLogs: " << "added " << std::to_string(new_size - old_size) << " elements\n";
				old_size = new_size;
			}
			mtx.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(N));
		}
	}

	double valueHandler(const std::string& not_trimmed_out) {
		std::string out = not_trimmed_out;
		boost::algorithm::trim(out);

		std::cout << ">>> " << out << std::endl;

		if (out == std::string("close")) {
			stopped.store(true, std::memory_order_relaxed);
			Logger::log("closed");
			std::cout << "server closed\n"; //std::this_thread::sleep_for(std::chrono::seconds(N)); // call destructor and exit;
			exit(-3);
		}

		//std::lock_guard<std::mutex> guard(mtx);
		int new_el;
		try {
			new_el = std::stoi(out.c_str());
		}
		catch (...) {
			Logger::log({ "value isnt integer", out });
			std::cout << out << " isn't integer\n";
			return result;
		}

		if (new_el >= 0 && new_el <= 1023) {
			mtx.lock();
			auto last_size = num.size();
			num.insert(new_el);
			auto new_size = num.size();
			mtx.unlock();
			if (new_size != last_size) {
				unsigned long sum = last_calculated + new_el * new_el;
				result = sum / static_cast<double>(new_size);
				last_calculated = sum;
				Logger::log({ "added value", out, std::to_string(result) });
				std::cout << "added value: " << out << "\n";
			}
			else {
				Logger::log({ "value already exist", out });
				std::cout << "value already exist: " << out << "\n";
			}
		}
		else {
			Logger::log({ "value not in range", out });
			std::cout << "value not in range: " << out << "\n";
		}
		std::cout << "result: " << std::to_string(result) << std::endl;
		return result;
	}

	void communicator() {
		ws.async_read(buffer, [self{ shared_from_this() },this](beast::error_code ec, std::size_t bytes_transferred)
		{
			if (ec == websocket::error::closed) return;
			if (ec) { std::cout << ec.message() << std::endl; return; }
			auto out = beast::buffers_to_string(self->buffer.cdata());

			double res = valueHandler(out);
			const std::string val = std::to_string(res);

			self->ws.async_write(boost::asio::buffer(val.c_str(), val.size()), [self](beast::error_code ec, std::size_t bytes_transferred) {
				if (ec) { std::cout << ec.message() << std::endl; return; }
				self->buffer.consume(self->buffer.size()); //
				self->communicator();
				});

			//const size_t len = std::min(boost::asio::buffer_size(v), test.length());
			//memcpy(boost::asio::buffer_cast<void*>(v),test.c_str(),len);
			//self->ws.async_write(self->buffer.data(), [self](beast::error_code ec, std::size_t bytes_transferred) {
			//	if (ec) { std::cout << ec.message() << std::endl; return; }
			//	self->buffer.consume(self->buffer.size()); //
			//	self->communicator();
			//	});
			//self->communicator();

		});
	}
	~Websocket()
	{

	}
};

class Listener : public std::enable_shared_from_this<Listener>
{
	net::io_context& ioc; //tcp::endpoint the_endpoint;// (tcp::v4(), 9999);
	tcp::acceptor acceptor;

public:
	Listener(net::io_context& ioc,
		unsigned short port) : ioc(ioc), //the_endpoint(net::ip::make_address("127.0.0.1"), port), //acceptor(ioc, the_endpoint){}
		acceptor(ioc, { net::ip::make_address("127.0.0.1"), port }) {}

	void asyncAccept()
	{
		acceptor.async_accept(ioc, [self{ shared_from_this() }](boost::system::error_code ec,
			tcp::socket socket){
			std::make_shared<Websocket>(std::move(socket))->run();
			std::cout << "new connection\n";
			Logger::log("new connection");
			self->asyncAccept();
		});
	}
};

int main(int argc, char* argv[])
{
	Logger::initializeLog();
	auto const port = 8083;
	net::io_context ioc{};
	std::thread th([]() {Websocket::dumpFile();});
	th.detach();
	Logger::log("start");
	std::cout << "server: start\n";
	std::make_shared<Listener>(ioc, port)->asyncAccept();
	ioc.run();

	return 0;
}
