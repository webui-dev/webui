/*
	WebUI Library
	- - - - - - -
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

	void ini();
	void loop();
	void exit();

	namespace browser{

		unsigned short chrome	= 1;
		unsigned short firefox	= 2;
		unsigned short edge		= 3;
		unsigned short safari	= 4;
	}		

	// -- Class ----
	class _window {
		public:
			_window();
			~_window();			
			struct {
				std::thread * t_ws;
				std::thread * t_wss;
				std::string number_s;
				unsigned short number;
				bool ws_running = false;
				bool wss_running = false;
				bool ws_served = false;
				std::string ws_port = "0";
				std::string wss_port = "0";
				const std::string * html;
			} settings;
			void receive(std::vector<std::uint8_t> &packets_v);
			void send(std::vector<std::uint8_t> &packets_v);
			void event(std::string id);
			void websocket_session_clean();
			void bind(std::string key_id, void(*new_action)());
			bool window_show(const std::string * html, unsigned short browser);
			bool window_is_running();
			void destroy();
			std::string run(std::string js, unsigned short seconds);
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

		bool is_show(){

			return o_win.window_is_running();
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

		void bind(std::string id, void (* func_ref)()){

			o_win.bind(id, func_ref);
		}

		void close(){

			o_win.destroy();
		}

		// -- JavaScript ------------------------------------------

		std::string run_js(const std::string js){

			if(!o_win.window_is_running())
				return "";

			return o_win.run(js, 1);
		}

		std::string run_js(const std::string js, unsigned short s){

			if(!o_win.window_is_running())
				return "";

			return o_win.run(js, s);
		}

		std::string get_value(const std::string id){

			if(!o_win.window_is_running())
				return "";

			return o_win.run(" return String(document.getElementById('" + id + "').value); ", 1);
		}

		std::string set_value(const std::string id, const std::string data){

			if(!o_win.window_is_running())
				return "";

			return o_win.run(" const rawdata = String.raw`" + data + "`; document.getElementById('" + id + "').value = `${rawdata}`; ", 1);
		}
	};
}

#endif