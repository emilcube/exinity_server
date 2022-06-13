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
#include <boost/unordered_set.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

#if defined (_MSC_VER)  // Visual studio
#define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
#define thread_local __thread
#endif

//#include <random>
//#include <time.h>
//#include <thread>

//using namespace std;

/* Thread-safe function that returns a random number between min and max (inclusive).
This function takes ~142% the time that calling rand() would take. For this extra
cost you get a better uniform distribution and thread-safety. */
int intRand(const int& min, const int& max) {
	static thread_local std::mt19937* generator = nullptr;
	if (!generator) generator = new std::mt19937(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
	//std::hash<std::thread::id>()(std::this_thread::get_id())
	//this_thread::get_id().hash()
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(*generator);
}

std::unordered_set<int> num;
unsigned long last_calculated = 0;
double result;
const int N = 5;
std::mutex mtx;

//boost::unordered_set<int> my_s;
static std::atomic<int> v2{ 0 };
std::atomic<bool> stopped{ false };

void printContainer() {
	//v2.fetch_add(1);
	//stopped.store(true, std::memory_order_relaxed);
	//if (stopped) {

	//}

	//if (stopped.load(std::memory_order_relaxed)) {
	//}


	//for (auto& x : num) {
	//	std::cout << x << " ";
	//}
	//std::cout << "| result = " << result;
	//std::cout << std::endl;
	std::cout << "In_container: " << num.size() << std::endl;
}

void dumpFile() {
	// + add logger
	std::ofstream fs;// ("dump.bin", std::ios::out | std::ios::binary | std::ios::app); //
	std::string namefile = "dump.bin";
	std::ios_base::openmode mode = std::ios::out | std::ios::binary | std::ios::app;
	unsigned int old_size = 0;

	while (true) {
		//std::lock_guard<std::mutex> guard(mtx);

		mtx.lock();
		unsigned int new_size = num.size();
		if (new_size != old_size) { //(num.size() != 0) {
			fs.open(namefile, mode);
			if (!fs.is_open()) {
				std::cout << "error while opening the file\n";
				exit(-2);
			}
			for (auto x : num) {
				//fres  << x << " ";
				//fs.write(reinterpret_cast<const char*>(x), sizeof x);
				//temp_str += std::to_string(x) + " ";

				std::string tmp = std::to_string(x);
				char const* pchar = tmp.c_str(); 
				fs.write(pchar, sizeof pchar);
				
				//char* rez;
				//_itoa_s(x, rez, 10);
				//fs.write(rez, sizeof rez);

				//fs.write((const char*)&x, sizeof(x));
			}
			fs.close();
			old_size = new_size;
		}
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(N));
	}
}

class EchoWebsocket : public std::enable_shared_from_this<EchoWebsocket>
{
	websocket::stream<beast::tcp_stream> ws;
	beast::flat_buffer buffer;
	int sil_;
	std::set<int> num_;

public:
	EchoWebsocket(tcp::socket&& socket) : ws(std::move(socket)) {}

	void run() {
		ws.async_accept([self{ shared_from_this() }](beast::error_code ec) {
			if (ec) { std::cout << ec.message() << std::endl; return; }
			self->echo();
		});
	}

	void silence() {
		sil_ = 1;
		num_.insert(1);
	}

	void echo() {
		ws.async_read(buffer, [self{ shared_from_this() },this](beast::error_code ec, std::size_t bytes_transferred)
		{
			if (ec == websocket::error::closed) return;
			if (ec) { std::cout << ec.message() << std::endl; return; }
			auto out = beast::buffers_to_string(self->buffer.cdata());
			//std::cout << out << std::endl;
			
			//sil_ = 2;
			//num_.clear();
			std::cout << ">>> " << out << std::endl;

			//~EchoWebsocket();
			//std::lock_guard<std::mutex> guard(mtx);
			
			int new_el = -1;
			try {
				new_el = std::stoi(out.c_str());
			}
			catch (...) {
				std::cout << out.c_str() << " isn't integer\n";
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
				}
			}

			printContainer();
			
			//auto v = self->buffer.data();

			const std::string val = std::to_string(result);
			//const size_t len = std::min(boost::asio::buffer_size(v), test.length());
			//memcpy(boost::asio::buffer_cast<void*>(v),
			//	test.c_str(),
			//	len);

			self->ws.async_write(boost::asio::buffer(val.c_str(), val.size()), [self](beast::error_code ec, std::size_t bytes_transferred) {
				if (ec) { std::cout << ec.message() << std::endl; return; }
				self->buffer.consume(self->buffer.size()); // 
				self->echo();
				});


			//self->ws.async_write(self->buffer.data(), [self](beast::error_code ec, std::size_t bytes_transferred) {
			//	if (ec) { std::cout << ec.message() << std::endl; return; }
			//	self->buffer.consume(self->buffer.size()); // 
			//	self->echo();
			//	});

			//self->echo();

		});
	}

};

class Listener : public std::enable_shared_from_this<Listener>
{
	net::io_context& ioc;
	//tcp::endpoint the_endpoint;// (tcp::v4(), 9999);
	tcp::acceptor acceptor;

public:
	Listener(net::io_context& ioc,
		unsigned short port) : ioc(ioc),
		//the_endpoint(net::ip::make_address("127.0.0.1"), port),
		//acceptor(ioc, the_endpoint){}
		acceptor(ioc, { net::ip::make_address("127.0.0.1"), port }) {}

	void asyncAccept()
	{
		acceptor.async_accept(ioc, [self{ shared_from_this() }](boost::system::error_code ec,
			tcp::socket socket){
			std::make_shared<EchoWebsocket>(std::move(socket))->run();
			std::cout << "connection accepted\n";
			self->asyncAccept();
		});
	}


};

int main(int argc, char* argv[])
{
	//num.insert(34);
	//num.insert(111);
	//dumpFile();

	//return 0;

	//long long sum = 0;
	//long long res_ = 0;
	//int count = 0;
	//std::set<int> ggg;
	//for (int i = 0; i <= 1023; ++i) {
	//	ggg.insert(i);
	//	sum += i * i;
	//}
	//std::cout << ggg.size() << std::endl;
	//std::cout << sum << std::endl; // 349014
	//std::cout << double(sum / double(1024)) << std::endl; // 349014
	//return 0;

	auto const port = 8083;
	net::io_context ioc{};
	std::thread th([]() {
		dumpFile();
		});
	th.detach();
	std::make_shared<Listener>(ioc, port)->asyncAccept();
	ioc.run();

	return 0;

}