/*
g++ main.cpp -lboost_system -lboost_thread -lpthread -o main
*/

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>

#define BUFFER_SIZE 176

union FRAME{
  struct{
    uint32_t time;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  uint8_t data[BUFFER_SIZE];
};

FRAME tx,rx;

void send_something(std::string host, int port, std::string message)
{
	boost::asio::io_service ios;
			
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port);

        boost::asio::ip::tcp::socket socket(ios);

	socket.connect(endpoint);

	// boost::array<char, 128> buf;
    //     std::copy(message.begin(),message.end(),buf.begin());
	boost::system::error_code error;
	socket.write_some(boost::asio::buffer(tx.data, BUFFER_SIZE), error);
    socket.read_some(boost::asio::buffer(rx.data, BUFFER_SIZE), error);
    printf("time: %d\n",rx.values.time);
    socket.close();
}

int main()
{
    send_something("10.42.0.132", 80, "hello flowers team\n\n");
    return 0;
}