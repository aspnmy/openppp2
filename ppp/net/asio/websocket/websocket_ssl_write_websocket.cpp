#include <ppp/net/asio/websocket/websocket_async_sslv_websocket.h>
#include <ppp/net/asio/websocket/websocket_accept_sslv_websocket.h>

namespace ppp {
    namespace net {
        namespace asio {
            bool sslwebsocket::Write(const void* buffer, int offset, int length, const AsynchronousWriteCallback& cb) noexcept {
                if (NULL == buffer || offset < 0 || length < 1) {
                    return false;
                }

                if (NULL == cb) {
                    return false;
                }

                if (IsDisposed()) {
                    return false;
                }

                const std::shared_ptr<SslvWebSocket> ssl_websocket = ssl_websocket_;
                if (NULL == ssl_websocket || !ssl_websocket->is_open()) {
                    return false;
                }

                const std::shared_ptr<sslwebsocket> self = shared_from_this();
                auto complete_do_async_write_callback = [self, this, cb, buffer, offset, length, ssl_websocket, context = context_, strand = strand_]() noexcept {
                    ssl_websocket->async_write(boost::asio::buffer((Byte*)buffer + offset, length),
                        [self, this, cb](const boost::system::error_code& ec, size_t sz) noexcept {
                            bool ok = ec == boost::system::errc::success;
                            if (cb) {
                                cb(ok); /* b is boost::system::errc::success. */
                            }
                        });
                };

                return ppp::threading::Executors::Post(context_, strand_, complete_do_async_write_callback);
            }
        }
    }
}