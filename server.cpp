
#include "httplib.h"

httplib::Server svr;

int main(){
  std::cout << "started"<<std::endl<<std::flush;
  svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000) );
    res.set_content("Hello World!", "text/plain");
  });

  svr.listen("0.0.0.0", 8080);
}