
#include <memory>
namespace ccy{

class HttpData: public std::enable_shared_from_this<HttpData>{
public:
    void handleClose();
    void linkTimer(std::shared_ptr<TimeNode> mtimer) {
    // shared_ptr重载了bool, 但weak_ptr没有
    timer_ = mtimer;
  }
private:
    std::weak_ptr<TimeNode> timer_;

};

}
