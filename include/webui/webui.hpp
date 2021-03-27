/*
	WebUI Library
	- - - - - - -
	http://webui.me
	https://github.com/alifcommunity/webui
	Licensed under GNU General Public License v3.0.
	Copyright (C)2020 Hassan DRAGA <https://github.com/hassandraga>.
*/

#pragma once
#ifndef WEBUI_H
#define WEBUI_H

// -- C++ Standard -----------------------------
#include <iostream>			//
#include <string>			//
#include <vector>			//
#include <thread>			// Standard threading
#ifdef __linux__
	#include <pthread.h>	// POSIX threading
#endif

namespace webui{

	namespace browser{

		unsigned short chrome	= 1;
		unsigned short firefox	= 2;
		unsigned short edge		= 3;
		unsigned short safari	= 4;
		unsigned short custom	= 99;
	}

	struct event{
	
		unsigned short id = 0;
		std::string element = "";
	};

	struct custom_browser_t {

		std::string app;
		std::string arg;
		bool link = true;
	};

	void ini();
	void loop();
	void exit();
	bool any_is_show();
	void set_custom_browser(const webui::custom_browser_t *b);
	void set_timeout_sec(unsigned short s);

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
            std::string webserver_port = "0";
            std::string websocket_port = "0";
            const std::string * html = nullptr;
        } settings;
        void receive(std::vector<std::uint8_t> &packets_v);
        void send(std::vector<std::uint8_t> &packets_v) const;
        static void event(const std::string& id, const std::string& element);
        void websocket_session_clean();
        unsigned short bind(std::string key_id, void(*function_ref)(webui::event e)) const;
        bool window_show(const std::string * html, unsigned short browser);
		void allow_multi_access(bool status);
        std::string new_server(const std::string * html);
        bool window_is_running() const;
        bool any_window_is_running() const;
        void destroy();
        std::string run(std::string js, unsigned short seconds) const;
	};

	class window{

		private:

			union {

				//_window o_win = _window::_window();
				_window o_win = _window();
			};

			const std::string * p_html = nullptr;

		public:

			window(){

				webui::ini();
			}

			~window(){

				o_win.~_window();
			}

		// -- Window -----------------------------

		bool is_show() const{

			return o_win.window_is_running();
		}

		bool any_is_show() const{

			return o_win.any_window_is_running();
		}

		bool show(const std::string * html){

			// if(o_win.window_is_running())
			// 	return true;

			this->p_html = html;
			return o_win.window_show(this->p_html, 0);
		}

		bool show(const std::string * html, unsigned short browser){

			// if(o_win.window_is_running())
			// 	return true;
			
			this->p_html = html;
			return o_win.window_show(this->p_html, browser);
		}

		bool show(const std::string html){

			std::string * _html = new std::string(html);
			
			//return this->show(_html, 0);
			return o_win.window_show(_html, 0);
		}

		bool show(const std::string * html, const webui::custom_browser_t *b){

			// if(o_win.window_is_running())
			// 	return true;
			
			// Update custom browser reference
			webui::set_custom_browser(b);

			this->p_html = html;
			return o_win.window_show(this->p_html, webui::browser::custom);
		}

		unsigned short bind(const std::string& id, void (* func_ref)(webui::event e)) const{

			return o_win.bind(id, func_ref);
		}

		void close(){

			o_win.run(" window.close(); ", 2);

			if(o_win.window_is_running())
				o_win.destroy();
		}

		void allow_multi_serving(bool b){

			o_win.allow_multi_access(b);
		}

		std::string new_server(const std::string * html){

			return o_win.new_server(html);
		}

		// -- JavaScript ------------------------------------------

		std::string run_js(const std::string& js) const{

			if(!o_win.window_is_running())
				return "";

			return o_win.run(js, 1);
		}

		std::string run_js(const std::string& js, unsigned short s) const{

			if(!o_win.window_is_running())
				return "";

			return o_win.run(js, s);
		}

		std::string get_value(const std::string& id) const{

			if(!o_win.window_is_running())
				return "";

            std::string buf;
            buf.append(" return String(document.getElementById('");
            buf.append(id);
            buf.append("').value); ");

			return o_win.run(buf, 1);
		}

		std::string set_value(const std::string& id, const std::string& data){

			if(!o_win.window_is_running())
				return "";

            std::string buf;
            buf.append(" const rawdata = String.raw`");
            buf.append(data);
            buf.append("`; document.getElementById('");
            buf.append(id);
            buf.append("').value = `${rawdata}`; ");

			return o_win.run(buf, 1);
		}
	};
}

#endif