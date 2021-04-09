/*
	WebUI Library
	- - - - - - -
	http://webui.me
	https://github.com/alifcommunity/webui
	Licensed under GNU General Public License v3.0.
	Copyright (C)2020 Hassan DRAGA <https://github.com/hassandraga>.
*/

#define MINIMUM_PACKETS_SIZE (1)
#define TYPE_SIGNATURE 		0xFF	// 255
#define TYPE_RUN_JS 		0xFE	// 254
#define TYPE_EVENT 			0xFD	// 253
#define TYPE_SWITCH 		0xFC	// 252
#define TYPE_CLOSE 			0xFB	// 251

// -- Windows ---------------------------------
#ifdef _WIN32
	//Fix: _WIN32_WINNT warning
	#include <SDKDDKVer.h>
#endif

// -- Linux -----------------------------------
#ifdef __linux__
	#include <pthread.h>	// POSIX threading
#endif

// -- macOS -----------------------------------
// ...

// -- C++ Standard ----------------------------
#include <iostream>		// std::cout / std::cerr
#include <codecvt>		// std::wstring_convert
#include <fstream>		// std::fstream
#include <thread>		// Standard threading

// -- Boost Defines ----------------------------
#ifndef BOOST_ALL_NO_LIB
	#define BOOST_ALL_NO_LIB
#endif

// -- Boost Web Server Headers -----------------
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>

// -- Boost Web Socket Headers -----------------
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

// -- Boost File System ------------------------
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// -- Boost Process ----------------------------
// Workaround for a boost/mingw bug.
// This must occur before the inclusion of the boost/process.hpp header.
// Taken from https://github.com/boostorg/process/issues/96
#ifndef __kernel_entry
    #define __kernel_entry
#endif
#include <boost/process.hpp>
//#include <boost/process/windows.hpp> // windows::hide

// -- JavaScript bridge ------------------------
namespace webui {

	const std::string javascriptbridge = (R"V0G0N(

		// - - - - - - - - - - -
		// WebUI WebSocket
		// - - - - - - - - - - -

		// external const _webui_websocket_port;
		// external const _webui_webserver_port;
		// external const _webui_minimum_data;

		// Log & keep window open
		const _webui_log = false;

		var _webui_ws;
		var _webui_ws_status = false;
		var _webui_action8 = new Uint8Array(1);
		var _webui_action_val;
		var _webui_allow_close = false;

		function _webui_close(vbyte, v){

			_webui_ws_status = false;
			_webui_action8[0] = vbyte;
			_webui_action_val = v;
			_webui_ws.close();
		}

		function _webui_freez_ui(){

			document.body.style.filter = "contrast(1%)";
		}

		function webui_close_window(){

			_webui_freez_ui();
			_webui_allow_close = true;

			if(_webui_ws_status)
				_webui_close(0xFF, '');
			else
				window.close();
		}

		function _webui_start(){
		
			if ('WebSocket' in window){
			
				_webui_ws = new WebSocket('ws://127.0.0.1:' + _webui_websocket_port);
				_webui_ws.binaryType = 'blob';
			
				_webui_ws.onopen = function(){

					_webui_ws.binaryType = 'blob';
					_webui_ws_status = true;					

					if(_webui_log)
						console.log('WebUI -> Connected');
				};

				_webui_ws.onerror = function(){

					if(_webui_log)
						console.log('WebUI -> Connection error');

					_webui_close(0xFF, '');
				};
			
				_webui_ws.onclose = function(){

					_webui_ws_status = false;

					if(_webui_action8[0] == 0xFC){

						if(_webui_log)
							console.log('WebUI -> Switch URL');

						// Switch URL
						window.location.replace(_webui_action_val);
					}
					else {

						// App has exit.
						// WS Error.
						// Window close.

						if(_webui_log)
							console.log('WebUI -> Connection lost');

						if(!_webui_log)
							window.close();
						else
							_webui_freez_ui();
					}
				};

				_webui_ws.onmessage = function (evt){

					var reader;
					var buffer8;

					reader = new FileReader();
					reader.readAsArrayBuffer(evt.data);
					reader.addEventListener("loadend", function(e){

						buffer8 = new Uint8Array(e.target.result);

						if(buffer8.length < _webui_minimum_data)
							return;
						
						if(buffer8[0] !== 0xFF){

							if(_webui_log)
								console.log('WebUI -> Invalid flag -> 0x' + buffer8[0] + ' 0x' + buffer8[1] + ' 0x' + buffer8[2]);

							return;
						}

						if(_webui_log)
							console.log('WebUI -> Flag -> 0x' + buffer8[0] + ' 0x' + buffer8[1] + ' 0x' + buffer8[2]);

						var len = buffer8.length - 3;

						data8 = new Uint8Array(len);
						for(i = 0; i < len; i++)
							data8[i] = buffer8[i+3];
						
						var data8utf8 = new TextDecoder("utf-8").decode(data8);

						if(buffer8[1] === 0xFC){

							// 0xFC (252): Switch the URL
							_webui_close(0xFC, data8utf8);
						}
						else if(buffer8[1] === 0xFB){

							// 0xFB (251): Close connection
							_webui_close(0xFB, '');
						}
						else if(buffer8[1] === 0xFE){

							if(_webui_log)
								console.log('WebUI -> JS -> Run -> ' + data8utf8);

							var fun = new Function (data8utf8);
							var FunReturn;
							FunReturn = String(fun());

							if (FunReturn === undefined)
								return;

							if(_webui_log)
								console.log('WebUI -> JS -> Return -> ' + FunReturn);

							var FunReturn8 = new TextEncoder("utf-8").encode(FunReturn);
							var Return8 = new Uint8Array(3 + FunReturn8.length);
							Return8[0] = 0xFF;
							Return8[1] = 0xFE;
							Return8[2] = buffer8[2];

							var p = -1;
							for(i = 3; i < FunReturn8.length + 3; i++)
								Return8[i] = FunReturn8[++p];
							
							if(Return8[0] !== 0xFF){

								if(_webui_log)
									console.log('WebUI -> JS -> Invalid respons -> 0x' + buffer8[0] + ' 0x' + buffer8[1] + ' 0x' + buffer8[2]);
								return;
							}

							if(_webui_ws_status)
								_webui_ws.send(Return8.buffer);							

							if(_webui_log){

								var buf = '[ ';
								for(i = 0; i < Return8.length; i++)
									buf = buf + '0x' + Return8[i] + ' ';
								buf = buf + ']';
								console.log('WebUI -> JS -> Send respons -> ' + buf);
							}
						}
					});
				};
			}
			else {
				
				alert('Sorry. WebSocket not supported by your Browser.');

				if(!_webui_log)
					webui_close_window();
			}
		}

		function SendEvent(name){

			if(_webui_ws_status && name != ''){

				var Name8 = new TextEncoder("utf-8").encode(name);
				var Event8 = new Uint8Array(3 + Name8.length);
				Event8[0] = 0xFF;
				Event8[1] = 0xFD;
				Event8[2] = 0x0;

				var p = -1;
				for(i = 3; i < Name8.length + 3; i++)
					Event8[i] = Name8[++p];
				
				if(_webui_ws_status)
					_webui_ws.send(Event8.buffer);				

				if(_webui_log){

					var buf = '[ ';
					for(i = 0; i < Event8.length; i++)
						buf = buf + '0x' + Event8[i] + ' ';
					buf = buf + ']';
					console.log('WebUI -> Event -> Send -> ' + buf);
				}
			}
		}

		window.addEventListener("load",function(){

			var elems = document.getElementsByTagName("form");
			for (i = 0; i < elems.length; i++){

				_webui_ws_status = false;
				alert('Incompatible HTML.\n\nYour HTML contain <form> elements, wish is not compatible with WebUI. Please remove all those elements.');

				_webui_close(0xFF, '');
			}
		
			elems = document.getElementsByTagName("button");
			for (i = 0; i < elems.length; i++){

				if(elems[i].id == '')
					continue;

				if(_webui_log)
					console.log('WebUI -> Listen -> Button -> ' + elems[i].id);
				
				elems[i].addEventListener("click", function(){ SendEvent(this.id) });
			}
			
			elems = document.getElementsByTagName("div");
			for (i = 0; i < elems.length; i++){

				if(elems[i].id == '')
					continue;

				if(_webui_log)
					console.log('WebUI -> Listen -> Div -> ' + elems[i].id);
				
				elems[i].addEventListener("click", function(){ SendEvent(this.id) });
			}
		});	

		_webui_start();

		setTimeout(function(){

			if(!_webui_ws_status){

				//document.body.style.filter = "brightness(1%)";
				document.body.style.filter = "contrast(1%)";
				alert('WebUI failed to find the background application.');

				if(!_webui_log)
					webui_close_window();
			}
		}, 1000);

		// Reload
		document.addEventListener('keydown', function (e){
			if (e.keyCode === 116) {
				e.preventDefault();
				e.returnValue = false;
				e.keyCode = 0;
				return false;
			}		
		});

		// Unload
		window.addEventListener('beforeunload', function (e) {

			// console.log('WebUI -> CLOSE -> START');
			// if(_webui_ws_status && !_webui_allow_close){
			// 	console.log('WebUI -> CLOSE -> TRY TO PROVENT');
			// 	e.preventDefault();
			// 	e.returnValue = '';
			// 	console.log('WebUI -> CLOSE -> RETURNED VOID');
			// }
			// console.log('WebUI -> CLOSE -> FUNC END');
			_webui_close(0xFF, '');
		});

		// window.onbeforeunload = function (){
		// 	_webui_close(0xFF, '');
		// };

		// Right click
		document.addEventListener('contextmenu', function (e) {
		  //e.preventDefault();
		  //e.returnValue = '';
		});

	)V0G0N");
}

// -- Boost namespaces -------------------------
namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = beast::http;				// from <boost/beast/http.hpp>
namespace websocket = beast::websocket;		// from <boost/beast/websocket.hpp>
namespace net = boost::asio;				// from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>

namespace webui{

	// -- Global ---
	std::atomic<unsigned short> connected_swindow(0);
	
	const std::string html_served = "<html><head><title>Access Denied</title><style>"
									"html{-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}body{background-color:#2F2F2F;font-family:sans-serif;margin:20px;color:#00ff00}a{color:#00ff00}</style></head><body>"
									"<h2>[ ! ] Access Denied</h2><p>You can't access this window<br>because it's already served.<br><br>The security policy is set to<br>deny multiple requests.</p>"
									"<br><a href=\"http://www.webui.me\"><small>WebUI Library<small></a></body></html>";

	const std::string html_res_notavailable = "<html><head><title>Resource Not Available</title><style>"
									"html{-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}body{background-color:#2F2F2F;font-family:sans-serif;margin:20px;color:#00ff00}a{color:#00ff00}</style></head><body>"
									"<h2>[ ! ] Resource Not Available</h2><p>The requested resource is not available.</p>"
									"<br><a href=\"http://www.webui.me\"><small>WebUI Library<small></a></body></html>";

	const std::string def_icon = "<?xml version=\"1.0\" ?><svg height=\"24\" version=\"1.1\" width=\"24\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:cc=\"http://creativecommons.org/ns#\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"><g transform=\"translate(0 -1028.4)\"><path d=\"m3 1030.4c-1.1046 0-2 0.9-2 2v7 2 7c0 1.1 0.8954 2 2 2h9 9c1.105 0 2-0.9 2-2v-7-2-7c0-1.1-0.895-2-2-2h-9-9z\" fill=\"#2c3e50\"/><path d=\"m3 2c-1.1046 0-2 0.8954-2 2v3 3 1 1 1 3 3c0 1.105 0.8954 2 2 2h9 9c1.105 0 2-0.895 2-2v-3-4-2-3-3c0-1.1046-0.895-2-2-2h-9-9z\" fill=\"#34495e\" transform=\"translate(0 1028.4)\"/><path d=\"m4 5.125v1.125l3 1.75-3 1.75v1.125l5-2.875-5-2.875zm5 4.875v1h5v-1h-5z\" fill=\"#ecf0f1\" transform=\"translate(0 1028.4)\"/></g></svg>";

	const std::string def_icon_type = "image/svg+xml";

	struct event{
	
		unsigned short id = 0;
		std::string element = "";
	};

	struct custom_browser_t {

		std::string app;
		std::string arg;
		bool link = true;
	};

	const custom_browser_t *p_custom;
	std::atomic<bool> waitfor_swindow (false);
	std::vector<std::string> key_v;
	std::array<void(*)(webui::event e), 64> key_actions;
	unsigned short port_start = 8080;
	std::vector<unsigned short> port_v = { 0 };
	unsigned short winlast = 1;
	unsigned short startup_timeout = 10; // t = n * 1000 / 500
	bool use_timeout = true;
	std::array<std::string *,		64> nat_data;
	std::array<std::atomic<bool>,	64> nat_data_status;
	std::vector<unsigned short>			nat_id_v = { 0 };

	#ifdef _WIN32
		std::string sep = "\\";
	#else
		std::string sep = "/";
	#endif

	// -- Func ----
	void ini();
	void msgbox(const std::wstring& msg);
	void msgbox(const std::string& msg);
	signed short getkey(const std::string& id);
	unsigned short setkey(const std::string& id);
	unsigned short getport();
	unsigned short getwin();
	// void start_webserver(const char * ip, unsigned short port, const std::string * html, webui::_window * ui);
	// void start_websocket(const char * ip, unsigned short port, webui::_window * ui);
	void set_timeout_sec(unsigned short s);
	void waitforstartup();
	void FreePort();
	void FreeWindow();
	void loop();
	void exit();
	bool any_is_show();
	void set_custom_browser(const webui::custom_browser_t *p);

	// -- Class ----
	class _window {
		public:
        _window();
        ~_window();
        struct {
            std::thread * webserver_thread = nullptr;
            std::thread * websocket_thread = nullptr;
            std::string number_s;
            unsigned short number = 0;
            bool webserver_running = false;
            bool websocket_running = false;
            bool webserver_served = false;
            bool webserver_allow_multi = false;
            bool webserver_local_files = false;
            std::string webserver_local_root;
            std::string webserver_port = "0";
            std::string websocket_port = "0";
            const std::string * html = nullptr;
            const std::string * icon = nullptr;
            std::string icon_type;
        } settings;
        void receive(std::vector<std::uint8_t> &packets_v);
        void send(std::vector<std::uint8_t> &packets_v) const;
        static void event(const std::string& id, const std::string& element);
        void websocket_session_clean();
        unsigned short bind(std::string key_id, void(*function_ref)(webui::event e)) const;
        bool window_show(const std::string * html, unsigned short browser);
        void set_window_icon(const std::string * icon_s, const std::string type_s);
		void allow_multi_access(bool status);
		void set_root_folder(std::string local_path);
        std::string new_server(const std::string * html);
        bool window_is_running() const;
        bool any_window_is_running() const;
        void destroy();
        std::string run(std::string js, unsigned short seconds) const;
	};
}

namespace BoostWebServer{

	// - - - - - - - - - - - - -
	// Boost Web Server [small] + Async local files servings
	// - - - - - - - - - - - - -

	// Return a reasonable mime type based on the extension of a file.
    beast::string_view
    mime_type(beast::string_view path, bool *is_html){

        auto const ext = [&path]{

            auto const pos = path.rfind(".");
            if(pos == beast::string_view::npos)
                return beast::string_view{};
			
            return path.substr(pos);
        }();

        if(boost::beast::iequals(ext, ".htm")){ *is_html = true;  return "text/html";}
        if(boost::beast::iequals(ext, ".html")){ *is_html = true; return "text/html";}
        if(boost::beast::iequals(ext, ".php")){ *is_html = true;  return "text/html";}
        if(boost::beast::iequals(ext, ".css"))  return "text/css";
        if(boost::beast::iequals(ext, ".txt"))  return "text/plain";
        if(boost::beast::iequals(ext, ".js"))   return "application/javascript";
        if(boost::beast::iequals(ext, ".json")) return "application/json";
        if(boost::beast::iequals(ext, ".xml"))  return "application/xml";
        if(boost::beast::iequals(ext, ".swf"))  return "application/x-shockwave-flash";
        if(boost::beast::iequals(ext, ".flv"))  return "video/x-flv";
        if(boost::beast::iequals(ext, ".png"))  return "image/png";
        if(boost::beast::iequals(ext, ".jpe"))  return "image/jpeg";
        if(boost::beast::iequals(ext, ".jpeg")) return "image/jpeg";
        if(boost::beast::iequals(ext, ".jpg"))  return "image/jpeg";
        if(boost::beast::iequals(ext, ".gif"))  return "image/gif";
        if(boost::beast::iequals(ext, ".bmp"))  return "image/bmp";
        if(boost::beast::iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
        if(boost::beast::iequals(ext, ".tiff")) return "image/tiff";
        if(boost::beast::iequals(ext, ".tif"))  return "image/tiff";
        if(boost::beast::iequals(ext, ".svg"))  return "image/svg+xml";
        if(boost::beast::iequals(ext, ".svgz")) return "image/svg+xml";

        return "application/text";
    }

	// Append an HTTP rel-path to a local filesystem path.
    // The returned path is normalized for the platform.
    std::string path_cat(beast::string_view base, beast::string_view path){

        if(base.empty())
            return std::string(path);
		
        std::string result(base);

        #ifdef BOOST_MSVC
			char constexpr path_separator = '\\';
			if(result.back() == path_separator)
				result.resize(result.size() - 1);
			result.append(path.data(), path.size());
			for(auto& c : result)
				if(c == '/')
					c = path_separator;
        #else
			char constexpr path_separator = '/';
			if(result.back() == path_separator)
				result.resize(result.size() - 1);
			result.append(path.data(), path.size());
        #endif

        return result;
    }

	class http_connection : public std::enable_shared_from_this<http_connection>{

		public:
		//const std::string * html_main = &webui::html_def;
		webui::_window *p_ui = nullptr;

		http_connection(tcp::socket socket)
			: socket_(std::move(socket))
		{
		}

		// Initiate the asynchronous operations associated with the connection.
		void
		start(webui::_window * ui)
		{
			//this->html_main = html;
			this->p_ui = ui;
			
			read_request();
			check_deadline();
		}

		private:
		// The socket for the currently connected client.
		tcp::socket socket_;

		// The buffer for performing reads.
		beast::flat_buffer buffer_{8192};

		// The request message.
		http::request<http::dynamic_body> request_;

		// The response message.
		http::response<http::dynamic_body> response_;

		// The timer for putting a deadline on connection processing.
		net::steady_timer deadline_{
			socket_.get_executor(), 
			std::chrono::seconds(60)
		};

		// Asynchronously receive a complete request message.
		void
		read_request()
		{
			auto self = shared_from_this();
			
			http::async_read(
				socket_,
				buffer_,
				request_,
				[self](beast::error_code ec,
					std::size_t bytes_transferred)
				{
					boost::ignore_unused(bytes_transferred);
					if(!ec)
						self->process_request();
				});
		}

		// Determine what needs to be done with the request message.
		void
		process_request()
		{
			response_.version(request_.version());
			response_.keep_alive(false);

			switch(request_.method())
			{
				case http::verb::get:
				response_.result(http::status::ok);
				response_.set(http::field::server, "WebUI");
				create_response();
				break;

				default:
				// We return responses indicating an error if
				// we do not recognize the request method.
				response_.result(http::status::bad_request);
				response_.set(http::field::server, "WebUI");
				response_.set(http::field::content_type, "text/plain");
				beast::ostream(response_.body())
					<< "Invalid request-method '"
					<< std::string(request_.method_string())
					<< "'";
				break;
			}

			write_response();
		}

		// Construct a response message based on the program state.
		void
		create_response()
		{
			// Disable browser cache
			response_.set(http::field::cache_control, "no-cache, no-store, must-revalidate");
			response_.set(http::field::pragma, "no-cache");
			response_.set(http::field::expires, "0");

			auto resource_not_available = [&](){

				// Resource Not Available
				response_.result(http::status::not_found);
				response_.set(http::field::content_type, "text/html; charset=utf-8");
				beast::ostream(response_.body()) << webui::html_res_notavailable;
			};

			// Security check - method
			if(	request_.method() != http::verb::get &&
            	request_.method() != http::verb::head){

				resource_not_available();
			}

			// Security check - path
			if(	request_.target().empty() ||
            	request_.target()[0] != '/' ||
            	request_.target().find("..") != boost::beast::string_view::npos){

				resource_not_available();
			}

			if(this->p_ui->settings.webserver_local_files || this->p_ui->settings.html == nullptr){

				// Serve local files

				if(this->p_ui->settings.webserver_local_root == ""){

					// boost::filesystem::path full_path(boost::filesystem::current_path());
					auto cwd = boost::filesystem::current_path();

					this->p_ui->settings.webserver_local_root = cwd.string();
					this->p_ui->settings.webserver_local_root.append(webui::sep);
				}

				// Build the path to the requested file
				std::string path = path_cat(this->p_ui->settings.webserver_local_root, request_.target());
				if(request_.target().back() == '/')
					path.append("index.html");

				// Attempt to open the file
				beast::error_code ec;
				http::file_body::value_type body;
				body.open(path.c_str(), beast::file_mode::scan, ec);

				// Handle the case where the file doesn't exist
				if(ec == beast::errc::no_such_file_or_directory){

					resource_not_available();
					return;
				}

				// Handle an unknown error
        		if(ec)
					resource_not_available();
				
				// Cache the size since we need it after the move
        		//auto const size = body.size();

				// Get file data
				std::ifstream file(path);
  				std::string file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

				// Respond
				bool is_html = false;
				response_.set(http::field::content_type, mime_type(path, &is_html));

				if(is_html){

					beast::ostream(response_.body())	<< file_data
														<< "\n <script type = \"text/javascript\"> \n const _webui_websocket_port = " 
														<< this->p_ui->settings.websocket_port
														<< "; \n "
														<< " const _webui_webserver_port = " 
														<< this->p_ui->settings.webserver_port
														<< "; \n "
														<< " const _webui_minimum_data = " 
														<< MINIMUM_PACKETS_SIZE
														<< "; \n "
														<< webui::javascriptbridge
														<< " \n </script>";
				}
				else
					beast::ostream(response_.body()) << file_data;
			}
			else if(request_.target() == "/"){

				if(!this->p_ui->settings.webserver_allow_multi && this->p_ui->settings.webserver_served){

					// Main HTML already served.
					response_.set(http::field::content_type, "text/html; charset=utf-8");
					beast::ostream(response_.body()) << webui::html_served;

					return;
				}

				// Send main HTML
				response_.set(http::field::content_type, "text/html; charset=utf-8");
				beast::ostream(response_.body())	<< *this->p_ui->settings.html
													<< "\n <script type = \"text/javascript\"> \n const _webui_websocket_port = " 
													<< this->p_ui->settings.websocket_port
													<< "; \n "
													<< " const _webui_webserver_port = " 
													<< this->p_ui->settings.webserver_port
													<< "; \n "
													<< " const _webui_minimum_data = " 
													<< MINIMUM_PACKETS_SIZE
													<< "; \n "
													<< webui::javascriptbridge
													<< " \n </script>";

				this->p_ui->settings.webserver_served = true;
			}
			else if(request_.target() == "/favicon.ico" ||
					request_.target() == "/favicon.svg" ||
					request_.target() == "/favicon.png"){

				// Send favicon
				if(this->p_ui->settings.icon){
					
					response_.set(http::field::content_type, this->p_ui->settings.icon_type);
					beast::ostream(response_.body()) << *this->p_ui->settings.icon;
				}
				else {
					
					response_.set(http::field::content_type, webui::def_icon_type);
					beast::ostream(response_.body()) << webui::def_icon;
				}
			}
			else {

				resource_not_available();
			}
		}

		// Asynchronously transmit the response message.
		void
		write_response()
		{
			auto self = shared_from_this();

			response_.content_length(response_.body().size());

			http::async_write(
				socket_,
				response_,
				[self](beast::error_code ec, std::size_t)
				{
					self->socket_.shutdown(tcp::socket::shutdown_send, ec);
					self->deadline_.cancel();
				}
			);

		}

		// Check whether we have spent enough time on this connection.
		void
		check_deadline()
		{
			auto self = shared_from_this();

			deadline_.async_wait(
				[self](beast::error_code ec)
				{
					if(!ec)
					{
						// Close socket to cancel any outstanding operation.
						self->socket_.close(ec);
					}
				});
		}
	};

	// -- HTTP loop ------------------------------------------
	class http_loop {

		public:
		//const std::string * html_def = &webui::html_def;
		webui::_window *p_ui;

		void
		http_server(tcp::acceptor& acceptor, tcp::socket& socket){
			
			// if(this->p_ui->settings.webserver_served)
			// 	return;

			acceptor.async_accept(socket,
			[&](beast::error_code ec)
			{
				if(!ec)
					std::make_shared<http_connection>(std::move(socket))->start(this->p_ui);

				http_server(acceptor, socket);
			});
		}
	};
}

namespace BoostWebSocket{

	// - - - - - - - - - - - - - - - -
	// Boost Web Socket Server [async]
	// - - - - - - - - - - - - - - - -

	class session : public std::enable_shared_from_this<session>{

		websocket::stream<beast::tcp_stream> ws_;
		beast::flat_buffer buffer_;

		private:

		void fail(beast::error_code ec, char const* what){

			std::cerr << "[!] WebUI -> Websocket -> Session failed on port " + this->p_ui->settings.websocket_port + " -> " << what << ": " << ec.message() << "\n";
		}

		public:

		webui::_window *p_ui = nullptr;
		std::vector<std::uint8_t> packets_v;

		~session()= default;

		// Take ownership of the socket
		explicit session(tcp::socket&& socket) : ws_(std::move(socket)){

			ws_.binary(true);
		}

		// Get on the correct executor
		void run(webui::_window * ui);

		// Start the asynchronous operation
		void
		on_run()
		{
			ws_.binary(true);

			// Set suggested timeout settings for the websocket
			ws_.set_option(
				websocket::stream_base::timeout::suggested(
					beast::role_type::server));

			// Set a decorator to change the Server of the handshake
			ws_.set_option(websocket::stream_base::decorator(
				[](websocket::response_type& res)
				{
					// res.set(http::field::server,
					// 	const std::string * (BOOST_BEAST_VERSION_STRING) +
					// 		" websocket-server-async");
					res.set(http::field::server,
						"WebUI");
				}));

			// Accept the websocket handshake
			ws_.async_accept(
				beast::bind_front_handler(
					&session::on_accept,
					shared_from_this()));
		}

		void
		on_accept(beast::error_code ec)
		{
			if(ec)
				return fail(ec, "accept");

			// Read a message
			do_read();
		}

		void
		do_read()
		{
			// Read a message into our buffer
			ws_.async_read(
				buffer_,
				beast::bind_front_handler(
					&session::on_read,
					shared_from_this()));
		}

		//void receive_task(std::vector<std::uint8_t> &packets_v){
		//	this->p_ui->receive(packets_v);
		//}

		void receive_task(){

			this->p_ui->receive(this->packets_v);
		}

		void
		on_read(
			beast::error_code ec,
			std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);

			// This indicates that the session was closed
			if(ec == websocket::error::closed)
				return;

			if(ec)
				fail(ec, "read");

			if (ws_.got_binary()){
				
				//std::vector<std::uint8_t> packets_v;
				
				// Loop cast way
				// packets_v.reserve(buffer_.data().size());
				// net::const_buffer cb = buffer_.data();
				// for(std::size_t i = 0; i < cb.size(); ++i){
				// 	//packets_v.push_back( reinterpret_cast<unsigned char const*>(cb.data())[i] );
				// 	packets_v.push_back( reinterpret_cast<std::uint8_t const*>(cb.data())[i] );
				// }

				// Net buffer copy way
				auto cb = buffer_.data();
				packets_v.resize(beast::buffer_bytes(cb));
				buffer_.consume(net::buffer_copy(net::buffer(packets_v), cb));

				if(packets_v[0] != TYPE_SIGNATURE)
					return;
				
				if(packets_v.size() < MINIMUM_PACKETS_SIZE)
					return;
				
				// -- Process message --------------------------------------------
				//std::thread receive_job(&session::receive_task, this, packets_v);
				std::thread receive_job(&session::receive_task, this);
				receive_job.detach();
				// ---------------------------------------------------------------
			}

			// Clear buffer
			buffer_.consume(buffer_.size());

			// Read again
			ws_.async_read(
				buffer_,
				beast::bind_front_handler(
					&session::on_read,
					shared_from_this()
				)
			);

			// ws_.async_write(
			// 	buffer_.data(),
			// 	beast::bind_front_handler(
			// 		&session::on_write,
			// 		shared_from_this()
			// 	)
			// );
		}

		void
		on_write(
			beast::error_code ec,
			std::size_t bytes_transferred)
		{
			boost::ignore_unused(bytes_transferred);

			if(ec)
				return fail(ec, "write");

			// Clear the buffer
			buffer_.consume(buffer_.size());

			// Do another read
			//do_read();
		}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		void send(std::vector<std::uint8_t> &data){

			if(data.size() < MINIMUM_PACKETS_SIZE)
				return;

			ws_.write(boost::asio::buffer(data, data.size()));
			
			// Did we need to clear the buffer?
			//buffer_.consume(buffer_.size());
		}

		void on_close(beast::error_code ec){

			if(ec)
				return fail(ec, "close");
		}
	};

	std::vector<BoostWebSocket::session> socket_session_v;

	void BoostWebSocket::session::run(webui::_window * ui){

		this->p_ui = ui;

		// We need to be executing within a strand to perform async operations
		// on the I/O objects in this session. Although not strictly necessary
		// for single-threaded contexts, this example code is written to be
		// thread-safe by default.
		boost::asio::dispatch(
			ws_.get_executor(),
			beast::bind_front_handler(
				&session::on_run,
				shared_from_this()
			)
		);
	}

	// Accepts incoming connections 
	// and launches the sessions
	class listener : public std::enable_shared_from_this<listener>
	{
		net::io_context& ioc_;
		tcp::acceptor acceptor_;

		private:

		void fail(beast::error_code ec, char const* what){

			std::cerr << "[!] WebUI -> Websocket -> Listener failed on port " + this->p_ui->settings.websocket_port + " -> " << what << ": " << ec.message() << "\n";
			
			// Close the IOC service
			ioc_.stop();
		}

		public:
		bool *p_WindowWasConnected = nullptr;
		webui::_window *p_ui = nullptr;

		listener(
			net::io_context& ioc,
			tcp::endpoint endpoint)
			: ioc_(ioc)
			, acceptor_(ioc)
		{
			beast::error_code ec;

			// Open the acceptor
			acceptor_.open(endpoint.protocol(), ec);
			if(ec)
			{
				fail(ec, "open");
				return;
			}

			// Allow address reuse
			acceptor_.set_option(net::socket_base::reuse_address(false), ec);
			if(ec)
			{
				fail(ec, "set_option");
				return;
			}

			// bind to the server address
			acceptor_.bind(endpoint, ec);
			if(ec)
			{
				fail(ec, "bind");
				return;
			}

			// Start listening for connections
			acceptor_.listen(
				net::socket_base::max_listen_connections, ec);
			if(ec)
			{
				fail(ec, "listen");
				return;
			}
		}

		// Start accepting incoming connections
		void
		run(bool * WindowWasConnected, webui::_window * ui)
		{
			this->p_WindowWasConnected = WindowWasConnected;
			this->p_ui = ui;

			do_accept();
		}

		private:
		void
		do_accept()
		{
			// The new connection gets its own strand
			acceptor_.async_accept(
				net::make_strand(ioc_),
				beast::bind_front_handler(
					&listener::on_accept,
					shared_from_this()
				)
			);
		}

		void on_accept(beast::error_code ec, tcp::socket socket);
	};
}

namespace webui{

	std::array<std::shared_ptr<BoostWebSocket::session>, 64> session_actions;	
}

namespace BoostWebSocket{

	void listener::on_accept(beast::error_code ec, tcp::socket socket){

			if(ec){

				fail(ec, "accept");
			}
			else {

				webui::connected_swindow++;
				this->p_ui->settings.websocket_running = true;
				*this->p_WindowWasConnected = true;

				// Create the session and run it
				//std::make_shared<session>(std::move(socket) )-> run(this->p_ui);

				std::shared_ptr<BoostWebSocket::session> p_session = std::make_shared<BoostWebSocket::session>(std::move(socket));
				p_session->run(this->p_ui);

				webui::session_actions[this->p_ui->settings.number] = p_session;
			}

			if(this->p_ui->settings.webserver_allow_multi){

				// Accept another connection
				do_accept();
			}
	}
}

namespace webui{

	namespace browser{

		#define chrome		(1)
		#define firefox		(2)
		#define edge		(3)
		#define safari		(4)
		#define chromium	(5)
		#define custom		(99)

		unsigned short CurrentBrowser = 0;
		std::string browser_path;
		std::string frofile_path;

		int command(std::string cmd){

			//boost::process::environment env = ::boost::this_process::environment();
			//boost::process::child c(cmd, browser::env, boost::process::windows::hide);
			boost::process::child c(cmd);

			c.wait();
			return c.exit_code();
		}

		int command_browser(std::string cmd){

			boost::process::child c(cmd);
			c.detach();
			return 0;
		}

		void clean(){

			// - Clean web server.
			// All WebSocket are closed but not web servers
			// every window has 1 web server still runing.
			// right now, we let compiler destroy every obj!
			
			// - Clean profile folders
			// Remove browser profile folders if needed
			// or keep it to save window positions for
			// future run.
		}

		bool browser_exist(unsigned short browser){

			// Check if a browser exist

			if(browser == custom){

				// Custom Browser

				if(p_custom == nullptr)
					return false;

				return true;
			}

			#ifdef _WIN32
				// Resolve SystemDrive
				#ifdef _MSC_VER
					char* buf = nullptr;
					size_t sz = 0;
					if (_dupenv_s(&buf, &sz, "SystemDrive") != 0 || buf == nullptr)
						return false;
					std::string drive = buf;
				#else
					char const* p_drive = std::getenv("SystemDrive"); // _dupenv_s
					if(p_drive == nullptr)
						return false;
					std::string drive = p_drive;
				#endif
				std::string programs_folder32 = drive + webui::sep + "Program Files (x86)";
				std::string programs_folder64 = drive + webui::sep + "Program Files";
			#endif

			if(browser == firefox){

				// Firefox
			  
                #ifdef _WIN32
			  
					// Firefox 32/64 on Windows
					// TODO: Add support for C:\Program Files\Firefox Nightly\firefox.exe
					std::string fullpath32 = programs_folder32 + webui::sep + "Mozilla Firefox\\firefox.exe";
					std::string fullpath64 = programs_folder64 + webui::sep + "Mozilla Firefox\\firefox.exe";

					if(boost::filesystem::is_regular_file(fullpath64)){
						
						browser_path = "\"" + fullpath64 + "\"";
						return true;
					}
					else if(boost::filesystem::is_regular_file(fullpath32)){

						browser_path = "\"" + fullpath32 + "\"";
						return true;
					}
					else
						return false;

				#elif __APPLE__
					
					// Firefox on macOS
					if (system("open -R -a \"firefox\"") == 0){

						browser_path  = "/Applications/Firefox.app/Contents/MacOS/firefox";
						return true;
					}
					else
						return false;
               #else

					// Firefox on Linux

					if(system("firefox -v >>/dev/null 2>>/dev/null") == 0){

						browser_path = "firefox";
						return true;
					}
					else
						return false;

				#endif

			}
			else if(browser == chrome){

				// Chrome

				#ifdef _WIN32

					// Chrome on Windows

					std::string fullpath32 = programs_folder32 + webui::sep + "Google\\Chrome\\Application\\chrome.exe";
					std::string fullpath64 = programs_folder64 + webui::sep + "Google\\Chrome\\Application\\chrome.exe";

					if(boost::filesystem::is_regular_file(fullpath64)){

						browser_path = "\"" + fullpath64 + "\"";
						return true;
					}
					else if(boost::filesystem::is_regular_file(fullpath32)){

						browser_path = "\"" + fullpath32 + "\"";
						return true;
					}
					else return false;

				#elif __APPLE__

					// Chrome on macOS
					if (system("open -R -a \"Google Chrome\"") == 0){

					    browser_path  = "/Applications/Google\\ Chrome.app/Contents/MacOS/Google\\ Chrome";
					    return true;
					}
					else
						return false;
				#else

					// Chrome on Linux
					if(system("google-chrome --version >>/dev/null 2>>/dev/null") == 0){

						browser_path = "google-chrome";
						return true;
					}
					else
						return false;

				#endif
			}
			else if(browser == edge){

				// Edge

				#ifdef _WIN32

					// Edge on Windows

					std::string fullpath32 = programs_folder32 + webui::sep + "Microsoft\\Edge\\Application\\msedge.exe";
					std::string fullpath64 = programs_folder64 + webui::sep + "Microsoft\\Edge\\Application\\msedge.exe";

					if(boost::filesystem::is_regular_file(fullpath64)){

						browser_path = "\"" + fullpath64 + "\"";
						return true;
					}
					else if(boost::filesystem::is_regular_file(fullpath32)){

						browser_path = "\"" + fullpath32 + "\"";
						return true;
					}
					else return false;

				#elif __APPLE__

					// Edge on macOS
					return false;

				#else

					// Edge on Linux
					return false;

				#endif
			}

			return false;
		}

		std::string get_temp_path(unsigned short browser){

			boost::filesystem::path p(boost::filesystem::temp_directory_path());
			//return p.string();

			#ifdef _WIN32
				// Resolve %USERPROFILE%
				#ifdef _MSC_VER
					char* p_drive = nullptr;
					size_t sz = 0;
					if (_dupenv_s(&p_drive, &sz, "USERPROFILE") != 0 || p_drive == nullptr)
						return p.string();
					std::string WinUserProfile = p_drive;
				#else
					char* p_drive = std::getenv("USERPROFILE"); // _dupenv_s
					if(p_drive == nullptr)
						return p.string();
					std::string WinUserProfile = p_drive;
				#endif

			#endif

			if(browser == chrome){

				#ifdef _WIN32
					return WinUserProfile;
				#elif __APPLE__
					char* tmpdir = std::getenv("TMPDIR"); 
					return tmpdir;
				#else
					return "/var/tmp";
				#endif
			}
			else if(browser == firefox){

				#ifdef _WIN32
					return WinUserProfile;
				#elif __APPLE__
					char* tmpdir = std::getenv("TMPDIR"); 
					return tmpdir;
				#else
					return "/var/tmp";
				#endif
			}
			else if(browser == edge){
				
				#ifdef _WIN32
					return WinUserProfile;
				#elif __APPLE__
					char* tmpdir = std::getenv("TMPDIR"); 
					return tmpdir;
                #else
					return "/var/tmp";
				#endif
			}
			
			std::cerr << "[!] WebUI -> Browser -> Unknow browser ID\n";
			webui::exit();
			return "";
		}

		bool folder_exist(std::string folder){

			boost::filesystem::path p(folder);
			return boost::filesystem::is_directory(p);
		}

		bool create_profile_folder(unsigned short browser){

			if(browser == custom){

				// Custom Browser

				if(p_custom == nullptr)
					return false;

				return true;
			}

			std::string temp = get_temp_path(browser);

			if(browser == chrome){

				frofile_path = temp + webui::sep + "WebUIChromeProfile";
				return true;
			}
			
			if(browser == edge){

				frofile_path = temp + webui::sep + "WebUIEdgeProfile";
				return true;
			}

			std::string profile_name = "";

			if(browser == firefox)
				profile_name = "WebUIFirefoxProfile";

			if(!folder_exist(temp + webui::sep + profile_name)){

				#ifdef _WIN32
					browser::command("cmd /c \"" + browser::browser_path + " -CreateProfile \"WebUI " + temp + webui::sep + profile_name + "\" > nul 2>&1");
				#else
					browser::command("sh -c \"" + browser::browser_path + " -CreateProfile \"WebUI " + temp + webui::sep + profile_name + "\" >>/dev/null 2>>/dev/null\"");
				#endif
				std::string buf;

				for(unsigned short n = 0; n <= 10; n++){

				    buf.clear();
                    buf.append(temp);
                    buf.append(webui::sep);
                    buf.append(profile_name);

					if(folder_exist(buf))
						break;
					
					std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(500));
				}

				if(!folder_exist(temp + webui::sep + profile_name))
					return false;

				// prefs.js
				std::fstream prefs(temp + webui::sep + profile_name + webui::sep + "prefs.js", std::ios::out | std::ios::app);
					if(prefs.fail())
						return false;
					prefs << "user_pref(\"toolkit.legacyUserProfileCustomizations.stylesheets\", true); " << std::endl;
					prefs << "user_pref(\"browser.shell.checkDefaultBrowser\", false); " << std::endl;
					prefs << "user_pref(\"browser.tabs.warnOnClose\", false); " << std::endl;
				prefs.close();

				// userChrome.css
				boost::filesystem::create_directory(temp + webui::sep + profile_name + webui::sep + "chrome" + webui::sep);
				std::fstream userChrome(temp + webui::sep + profile_name + webui::sep + "chrome" + webui::sep + "userChrome.css", std::ios::out | std::ios::app);
				if(prefs.fail())
					return false;

				#ifdef _WIN32
					userChrome << ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]){--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]){z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}" << std::endl;
				#elif __APPLE__
					userChrome << ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]){--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]){z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}" << std::endl;
				#else
					userChrome << ":root{--uc-toolbar-height:32px}:root:not([uidensity=\"compact\"]){--uc-toolbar-height:38px}#TabsToolbar{visibility:collapse!important}:root:not([inFullscreen]) #nav-bar{margin-top:calc(0px - var(--uc-toolbar-height))}#toolbar-menubar{min-height:unset!important;height:var(--uc-toolbar-height)!important;position:relative}#main-menubar{-moz-box-flex:1;background-color:var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor);background-clip:padding-box;border-right:30px solid transparent;border-image:linear-gradient(to left,transparent,var(--toolbar-bgcolor,--toolbar-non-lwt-bgcolor) 30px) 20 / 30px}#toolbar-menubar:not([inactive]){z-index:2}#toolbar-menubar[inactive] > #menubar-items{opacity:0;pointer-events:none;margin-left:var(--uc-window-drag-space-width,0px)}#nav-bar{visibility:collapse}" << std::endl;
				#endif

				userChrome.close();
				frofile_path = temp + webui::sep + profile_name;
			}

			return true;
		}

		bool start_edge(unsigned short port){

			// -- Edge ----------------------

			if(CurrentBrowser != 0 && CurrentBrowser != edge)
				return false;

			if(!browser_exist(edge))
				return false;
			
			if(!create_profile_folder(edge))
				return false;

			std::string arg = " --user-data-dir=\"" + frofile_path + "\" --no-proxy-server --app=http://127.0.0.1:";	
			std::string s_port = std::to_string(port);
			std::string full(browser::browser_path);
			full.append(arg);
			full.append(s_port);

			if(browser::command_browser("cmd /c \"" + full + "\" > nul 2>&1") == 0){

				browser::CurrentBrowser = edge;
				return true;
			}
			else
				return false;
		}

		bool start_firefox(unsigned short port){

			// -- Firefox ----------------------

			if(CurrentBrowser != 0 && CurrentBrowser != firefox)
				return false;

			if(!browser_exist(firefox))
				return false;

			if(!create_profile_folder(firefox))
				return false;

			std::string arg = " -P WebUI -private -no-remote -new-instance http://127.0.0.1:";	
			std::string s_port = std::to_string(port);
			std::string full(browser::browser_path);
			full.append(arg);
			full.append(s_port);

			#ifdef _WIN32
				if(browser::command_browser("cmd /c \"" + full + "\" > nul 2>&1") == 0)
			#else
				if(browser::command_browser("sh -c \"" + full + " >>/dev/null 2>>/dev/null\"") == 0)
			#endif
			{

				browser::CurrentBrowser = firefox;
				return true;
			}
			else
				return false;
		}

		bool start_custom(unsigned short port){

			// -- Custom Browser ----------------------

			if(CurrentBrowser != 0 && CurrentBrowser != custom)
				return false;

			if(!browser_exist(custom))
				return false;
			
			if(!create_profile_folder(custom))
				return false;

			std::string arg = " " + p_custom->arg;

			if(p_custom->link){

				arg.append("http://127.0.0.1:");

				std::string s_port = std::to_string(port);
				arg.append(s_port);
			}

			std::string full(p_custom->app);
			full.append(arg);

			#ifdef _WIN32
				if(browser::command_browser("cmd /c \"" + full + "\" > nul 2>&1") == 0)
			#else
				if(browser::command_browser("sh -c \"" + full + " >>/dev/null 2>>/dev/null\"") == 0)
			#endif
			{

				browser::CurrentBrowser = custom;
				return true;
			}
			else
				return false;
		}

		bool start_chrome(unsigned short port){

			// -- Chrome ----------------------

			if(CurrentBrowser != 0 && CurrentBrowser != chrome)
				return false;

			if(!browser_exist(chrome))
				return false;
			
			if(!create_profile_folder(chrome))
				return false;
			
			std::string arg = " --user-data-dir=\"" + frofile_path + "\" --disable-gpu --disable-software-rasterizer --no-proxy-server --app=http://127.0.0.1:";	
			std::string s_port = std::to_string(port);
			std::string full(browser::browser_path);
			full.append(arg);
			full.append(s_port);

			#ifdef _WIN32
				if(browser::command_browser("cmd /c \"" + full + "\" > nul 2>&1") == 0)
			#else
				if(browser::command_browser("sh -c \"" + full + " >>/dev/null 2>>/dev/null\"") == 0)
			#endif
			{

				browser::CurrentBrowser = chrome;
				return true;
			}
			else
				return false;
		}

		bool start(unsigned short port, unsigned short browser){

			// Non existing browser
			if(browser > 99)
				return false;
			
			if(browser != 0){

				// Specified browser
				if(browser == chrome)
					return start_chrome(port);
				else if(browser == firefox)
					return start_firefox(port);
				else if(browser == edge)
					return start_edge(port);
				else if(browser == custom)
					return start_custom(port);
				else
					return false;
			}
			else if(CurrentBrowser != 0){

				// Already set browser
				if(CurrentBrowser == chrome)
					return start_chrome(port);
				else if(CurrentBrowser == firefox)
					return start_firefox(port);
				else if(CurrentBrowser == edge)
					return start_edge(port);
				else if(CurrentBrowser == custom)
					return start_custom(port);
				else
					return false;
					//webui::exit();
			}
			else {

				// Default system browser

				#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
					// Windows
					if(!start_chrome(port))
						if(!start_firefox(port))
							if(!start_edge(port))
								if(!start_custom(port))
									return false;
									//webui::exit();
				#elif __APPLE__
					// macOS
					if(!start_chrome(port))
						if(!start_firefox(port))
							if(!start_edge(port))
								if(!start_custom(port))
									return false;
									//webui::exit();
				#else
					// Linux
					if(!start_chrome(port))
						if(!start_firefox(port))
							if(!start_edge(port))
								if(!start_custom(port))
									return false;
									//webui::exit();
				#endif
			}

			return true;
		}
	}

	static std::atomic<bool> initialized (false);

	void ini(){

		if(webui::initialized)
			return;
		webui::initialized = true;

		webui::key_v.emplace_back("");
		webui::port_v.clear();

		webui::use_timeout = true;
		webui::startup_timeout = 10; // 10 seconds
	}

	void msgbox(const std::wstring& msg){

		std::wcout << msg << std::endl;
		return;

		#ifdef _WIN32
			LPCWSTR wide_msg;
			wide_msg = msg.c_str();
			MessageBoxW(NULL, wide_msg, L"Message", 0);
		#else
			wprintf(L"%s \n", msg.c_str());
		#endif
	}

	void msgbox(const std::string& msg){

		std::cout << msg << std::endl;

		/*std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(msg);
		msgbox(wide);*/
	}

	void set_custom_browser(const webui::custom_browser_t *p){

		p_custom = p;
	}

	signed short getkey(const std::string& id){

		for (unsigned short i = 0; i < static_cast<unsigned short>(webui::key_v.size()); i++){

			if (webui::key_v[i] == id){

				return i;
			}
		}

		return -1;
	}

	unsigned short setkey(const std::string& id){

		webui::key_v.push_back(id);
		return static_cast<unsigned short>(webui::key_v.size()) - 1;
	}

	void set_timeout_sec(unsigned short s){

		if(s < 1)
			webui::use_timeout = false;
		else {

			webui::use_timeout = true;
			webui::startup_timeout = s;
		}
	}

	void waitforstartup(){

		if(webui::connected_swindow > 0)
			return;

		// Wait for a specific time
		for(unsigned short n = 0; n <= (webui::startup_timeout * 2); n++){

			if(webui::connected_swindow > 0)
				break;
			
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(500));
		}
	}

	void loop(){

		if(webui::use_timeout){

			// Wait for a specific time

			if(browser::CurrentBrowser < 1)
				return;

			if(webui::waitfor_swindow){

				// Wait one time while user 
				// browser is starting!
				webui::waitforstartup();

				while(webui::connected_swindow > 0){
				
					std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
				}
			}
		}
		else {

			// Wait forever!
			while(1)
				std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
		}

		browser::clean();
	}

	void exit(){

		webui::waitfor_swindow = false;
		webui::connected_swindow = 0;
	}

	void FreePort(unsigned short p){

		webui::port_v.erase(std::remove(webui::port_v.begin(), webui::port_v.end(), p), webui::port_v.end());
	}

	void FreeWindow(){

		webui::connected_swindow--;

		if(webui::connected_swindow < 1)
			webui::waitfor_swindow = false;
	}

	bool port_is_used(unsigned short port_num){

		boost::asio::io_service svc;
		boost::asio::ip::tcp::acceptor a(svc);

		boost::system::error_code ec;
		a.open(boost::asio::ip::tcp::v4(), ec) || a.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port_num), ec);

		return ec == boost::asio::error::address_in_use;
	}

	unsigned short getport(){
		
		unsigned short p = webui::port_start;

		for(unsigned short i = 1; i <= 65535; i++){

			if(std::find(webui::port_v.begin(), webui::port_v.end(), p) != webui::port_v.end())
				p++; // Port used by local window
			else {
				if(port_is_used(p))
					p++; // Port used by an external app
				else
					break; // Port ready to use
			}
		}

		webui::port_v.push_back(p);
		return p;
	}

	unsigned short getwin(){
		
		return webui::winlast++;
	}

	void start_webserver(const char * ip, unsigned short port, webui::_window * ui){

		// Initialization
		ui->settings.webserver_running = true;
		const std::string s_port = std::to_string(port);
		ui->settings.webserver_port = s_port;

		try {

			// IO Context
			auto const address = net::ip::make_address(ip);
			net::io_context ioc{1}; // 1 thread
			tcp::acceptor acceptor{ioc, {address, port}};
			tcp::socket socket{ioc};
			BoostWebServer::http_loop Server{};
			Server.p_ui = ui;
			Server.http_server(acceptor, socket);
			ioc.run();
		}
		catch(std::exception const& e){

			webui::msgbox(L"Error.\nWebUI failed to start http web server.");
			std::cerr << "[!] WebUI -> Webserver -> Start failed on port " + ui->settings.websocket_port + " -> " << e.what() << "\n";
		}

		// Clean
		ui->settings.webserver_running = false;
		ui->settings.webserver_served = false;
		webui::FreePort(port);
	}

	void start_websocket(const char * ip, unsigned short port, webui::_window * ui){

		// Initialization
		bool WindowWasConnected = false;
		std::string s_port = std::to_string(port);
		ui->settings.websocket_port = s_port;
	
		// IO Context
		auto const address = net::ip::make_address(ip);
		//auto const threads = std::max<int>(1, 1); // 1 thread
		net::io_context ioc;
		std::make_shared<BoostWebSocket::listener>(ioc, tcp::endpoint{address, port})->run(&WindowWasConnected, ui);
		ioc.run();

		// Clean
		if(WindowWasConnected)
			webui::FreeWindow();
		webui::session_actions[ui->settings.number] = nullptr;
		webui::FreePort(port);
		ui->settings.websocket_running = false;
	}

	bool any_is_show(){

		if(webui::connected_swindow > 0)
			return true;
		
		return false;
	}

	// --- Window ------------------------------------

	_window::_window(){

		this->settings.webserver_thread = nullptr;
		this->settings.websocket_thread = nullptr;
		this->settings.number = webui::getwin();
		this->settings.number_s = std::to_string(this->settings.number);
		this->settings.webserver_running = false;
		this->settings.webserver_served = false;
		this->settings.websocket_running = false;
		this->settings.webserver_port = "0000";
		this->settings.websocket_port = "0000";
	}

	_window::~_window()= default;

	void _window::destroy(){

		// Prepare packets
		std::vector<std::uint8_t> packets_v;
		packets_v.push_back(TYPE_SIGNATURE);	// Signature
		packets_v.push_back(TYPE_CLOSE);		// Type
		packets_v.push_back(0);					// ID
		packets_v.push_back(0);					// Data

		// Send packets
		this->send(packets_v);
	}

	bool _window::window_is_running() const{

		return this->settings.websocket_running;
	}

	bool _window::any_window_is_running() const{

		if(webui::connected_swindow > 0)
			return true;
		
		return false;
	}

	void _window::set_root_folder(std::string local_path){

		this->settings.webserver_local_files = true;
		this->settings.webserver_local_root = local_path;
	}

	unsigned short get_nat_id(){
		
		unsigned short id = 0;

		for(unsigned short i = 0; i <= 1024; i++){

			if(std::find(webui::nat_id_v.begin(), webui::nat_id_v.end(), id) != webui::nat_id_v.end()){

				id++;
			}
			else break;
		}

		webui::nat_id_v.push_back(id);
		
		return id;
	}
	
	void free_nat_id(unsigned short id){

		webui::nat_data[id] = nullptr;
		webui::nat_id_v.erase(std::remove(webui::nat_id_v.begin(), webui::nat_id_v.end(), id), webui::nat_id_v.end());
	}

	void _window::send(std::vector<std::uint8_t> &packets_v) const{

		if(packets_v.size() < MINIMUM_PACKETS_SIZE)
			return;

		if(webui::session_actions[this->settings.number] && webui::session_actions[this->settings.number] != nullptr)
			webui::session_actions[this->settings.number]->send(packets_v); // TODO: make send(const packets_v)

		// TODO: add custom quee system. dont call send() if busy.
	}

	std::string _window::run(std::string js, unsigned short seconds) const{

		if(js.empty())
			return "";

		// Initializing pipe
		std::string buf_pipe;
		const unsigned short id = webui::get_nat_id();
		webui::nat_data_status[id] = false;
		webui::nat_data[id] = &buf_pipe;

		// Prepare packets
		std::vector<std::uint8_t> packets_v;
		packets_v.push_back(TYPE_SIGNATURE);						// Signature
		packets_v.push_back(TYPE_RUN_JS);							// Type
		packets_v.push_back(static_cast<boost::asio::detail::buffered_stream_storage::byte_type>(id)); // ID
		packets_v.insert(packets_v.end(), js.begin(), js.end());	// Data

		// Send packets
		this->send(packets_v);
		
		for(unsigned short n = 0; n <= (seconds * 100); n++){

			if(webui::nat_data_status[id])
				break;
			
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(10));
		}

		// Clean
		webui::free_nat_id(id);
		return buf_pipe;
	}

	void _window::receive(std::vector<std::uint8_t> &packets_v){

		if(packets_v[0] != TYPE_SIGNATURE)
			return;
		
		if(packets_v.size() < MINIMUM_PACKETS_SIZE)
			return;

		std::uint8_t type	= packets_v[1];
		std::uint8_t id		= packets_v[2];

		// Clear first 3 elemets 
		// [Signature][Type][ID][data...]
		packets_v.erase(packets_v.cbegin(), packets_v.cbegin() + 3);

		std::string data(packets_v.begin(), packets_v.end());

		if (type == TYPE_EVENT){

			// Event

			if(!data.empty())
				this->event(this->settings.number_s + "/" + data, data);
		}
		else if (type == TYPE_RUN_JS){

			// JS Result

			if(id < 0)
				return;
			
			if(webui::nat_data[id] == nullptr)
				return;

			*webui::nat_data[id] = data;
			webui::nat_data_status[id] = true;
		}
	}

	void _window::websocket_session_clean(){

		webui::session_actions[this->settings.number] = nullptr;
	}

	void _window::event(const std::string& id, const std::string& element){

		int key_id = webui::getkey(id);

		if(key_id >= 0 && webui::key_actions[key_id]){

			webui::event e;
			e.element = element;
			e.id = key_id;

			// webui::key_actions[key_id](e);
			std::thread t(webui::key_actions[key_id], e);
    		t.detach();
		}
	}

	unsigned short _window::bind(std::string key_id, void(*function_ref)(webui::event e)) const{

		key_id = this->settings.number_s + "/" + key_id;

		signed short id = webui::getkey(key_id);
		
		if(id >= 0){

			// Replace a reference
			webui::key_actions[id] = function_ref;
		}
		else {

			// New reference
			id = setkey(key_id);
			webui::key_actions[id] = function_ref;
		}

		return id;
	}

	std::string _window::new_server(const std::string * html){

		// Start webserver and websocket
		// 100 is a non existing browser!
		// we need only servers now.
		this->window_show(html, 100);

		// Wait for webserver to start
		for(unsigned short n = 0; n < 10; n++){

			if(this->settings.webserver_running)
				break;
			
			std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
		}

		// Get URL
		std::string url = "http://127.0.0.1:" + this->settings.webserver_port;

		return url;
	}

	void _window::allow_multi_access(bool status){

		this->settings.webserver_allow_multi = status;
	}

	void _window::set_window_icon(const std::string * icon_s, const std::string type_s){

		this->settings.icon = icon_s;
		this->settings.icon_type = type_s;
	}

	bool _window::window_show(const std::string * html, unsigned short browser){

		// Initializing
		unsigned short port_ws				= webui::getport();
		this->settings.html					= html;
		this->settings.webserver_served		= false;
		webui::waitfor_swindow				= true;

		auto _start_websocket = [&](){

			// -- Web Socket ----------------------------
			std::thread StartWebSocket_job(	&webui::start_websocket,	// pointer
											"127.0.0.1",				// IP
											port_ws,					// port
											this);						// obj
			this->settings.websocket_thread = &StartWebSocket_job;
			StartWebSocket_job.detach();
			// ------------------------------------------			 
		};

		if(!this->window_is_running()){

			// Start a new window

			unsigned short port_web = webui::getport();

			// -- Web Server ----------------------------
			std::thread StartWebServer_job(	&webui::start_webserver,	// pointer
											"127.0.0.1",				// IP
											port_web,					// port
											this);						// obj
			this->settings.webserver_thread = &StartWebServer_job;
			StartWebServer_job.detach();
			// ------------------------------------------

			_start_websocket();
			//std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(1000));

			if(!browser::start(port_web, browser))
				return false;
		}
		else {

			// Switch on an already runing window

			// New websocket port
			this->settings.websocket_port = std::to_string(port_ws);

			// Start websocket
			_start_websocket();
			//std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(1000));

			// url
			std::string url = "http://127.0.0.1:" + this->settings.webserver_port;

			// Prepare packets
			std::vector<std::uint8_t> packets_v;
			packets_v.push_back(TYPE_SIGNATURE);						// Signature
			packets_v.push_back(TYPE_SWITCH);							// Type
			packets_v.push_back(0);										// ID
			packets_v.insert(packets_v.end(), url.begin(), url.end());	// Data

			// Provent main loop from breaking
			webui::connected_swindow++;

			// Send packets
			this->send(packets_v);

			// Wait for window to switch
			for(unsigned short n = 0; n < 10; n++){

				if(this->settings.webserver_served)
					break;
				
				std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(100));
			}

			// Back to real runing window number
			webui::connected_swindow--;
		}

		return true;
	}
}
