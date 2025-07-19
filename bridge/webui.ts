'use-strict'; // Force strict mode for transpiled

/*
  WebUI Bridge

  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2025 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.

  Converted from JavaScript to TypeScript
  By Oculi Julien. Copyright (c) 2023.
*/

//@ts-ignore use *.ts import real extension
import { AsyncFunction, addRefreshableEventListener } from './utils.ts';

type DataTypes = string | number | boolean | Uint8Array;

class WebuiBridge {
	// WebUI Settings
	#secure: boolean;
	#token: number;
	#port: number;
	#log: boolean;
	#winX: number;
	#winY: number;
	#winW: number;
	#winH: number;
	// Frameless Dragging
	#isDragging: boolean = false;
	#initialMouseX: number = 0;
	#initialMouseY: number = 0;
	#initialWindowX: number = window.screenX || window.screenLeft;
	#initialWindowY: number = window.screenY || window.screenTop;
	#currentWindowX: number = window.screenX || window.screenLeft;
	#currentWindowY: number = window.screenY || window.screenTop;
	// Internals
	#ws: WebSocket;
	#wsStayAlive: boolean = true;
	#wsStayAliveTimeout: number = 500;
	#wsWasConnected: boolean = false;
	#TokenAccepted: boolean = false;
	#closeReason: number = 0;
	#closeValue: string;
	#AllEvents: boolean = false;
	#callPromiseID: Uint16Array = new Uint16Array(1);
	#callPromiseResolve: (((data: string) => unknown) | undefined)[] = [];
	#allowNavigation: boolean = true;
	#sendQueue: Uint8Array[] = [];
	#isSending: boolean = false;
	#bindsList: string[];
	// WebUI Const
	#WEBUI_SIGNATURE: number = 221;
	#CMD_JS: number = 254;
	#CMD_JS_QUICK: number = 253;
	#CMD_CLICK: number = 252;
	#CMD_NAVIGATION: number = 251;
	#CMD_CLOSE: number = 250;
	#CMD_CALL_FUNC: number = 249;
	#CMD_SEND_RAW: number = 248;
	#CMD_NEW_ID: number = 247;
	#CMD_MULTI: number = 246;
	#CMD_CHECK_TK: number = 245;
	#CMD_WINDOW_DRAG: number = 244;
	#CMD_WINDOW_RESIZED: number = 243;
	#MULTI_CHUNK_SIZE: number = 65500;
	#PROTOCOL_SIZE: number = 8; // Protocol header size in bytes
	#PROTOCOL_SIGN: number = 0; // Protocol byte position: Signature (1 Byte)
	#PROTOCOL_TOKEN: number = 1; // Protocol byte position: Token (4 Bytes)
	#PROTOCOL_ID: number = 5; // Protocol byte position: ID (2 Bytes)
	#PROTOCOL_CMD: number = 7; // Protocol byte position: Command (1 Byte)
	#PROTOCOL_DATA: number = 8; // Protocol byte position: Data (n Byte)
	#Token: Uint32Array = new Uint32Array(1);
	#Ping: Boolean = true;
	// Events
	#eventsCallback: ((event: number) => void) | null = null;
	event = {
		// TODO: Make `event` static and solve the ESBUILD `_WebuiBridge` issue.
        CONNECTED: 0,
        DISCONNECTED: 1,
    };
	// Constructor
	constructor({
		secure = false,
		token = 0,
		port = 0,
		log = false,
		winX = 0,
		winY = 0,
		winW = 0,
		winH = 0,
	}: {
		secure: boolean;
		token: number;
		port: number;
		log?: boolean;
		winX: number;
		winY: number;
		winW: number;
		winH: number;
	}) {
		// Constructor arguments are injected by webui.c
		this.#secure = secure;
		this.#token = token;
		this.#port = port;
		this.#log = log;
		this.#winX = winX;
		this.#winY = winY;
		this.#winW = winW;
		this.#winH = winH;
		// Token
		this.#Token[0] = this.#token;
		// Instance
		if ('webui' in globalThis) {
			throw new Error('Sorry. WebUI is already defined, only one instance is allowed.');
		}
		// Positioning the current window
		if (this.#winX !== undefined && this.#winY !== undefined) {
			// window.moveTo(this.#winX, this.#winY);
		}
		// Resize the current window
		if (this.#winW !== undefined && this.#winH !== undefined) {
			// window.resizeTo(this.#winW, this.#winH);
		}
		// WebSocket
		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket is not supported by your web browser.');
			if (!this.#log) globalThis.close();
		}
		// Connect to the backend application
		this.#start();
		// Navigation event listener
		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				if (!this.#allowNavigation) {
					if (this.#AllEvents && (this.#wsIsConnected())) {
						event.preventDefault();
						const url = new URL(event.destination.url);
						if (this.#log) console.log(`WebUI -> DOM -> Navigation Event [${url.href}]`);
						this.#sendEventNavigation(url.href);
					}
				}
			});
		} else {
			// Click navigation event listener
			addRefreshableEventListener(document.body, 'a', 'click', (event) => {
				if (!this.#allowNavigation) {
					if (this.#AllEvents && (this.#wsIsConnected())) {
						event.preventDefault();
						const { href } = event.target as HTMLAnchorElement;
						if (this.#log) console.log(`WebUI -> DOM -> Navigation Click Event [${href}]`);
						this.#sendEventNavigation(href);
					}
				}
			});
		}
		// Prevent F5 refresh
		document.addEventListener('keydown', (event) => {
			if (this.#log) return; // Allowed in debug mode
			if (event.key === 'F5') event.preventDefault();
		});
		// Frameless Dragging
		document.addEventListener("mousemove", (e) => {
			// WebUI Webkit App-Region Custom Implementation
			if (e.buttons !== 1) {
				this.#isDragging = false;
				return;
			}
			if (!this.#isDragging) {
				let target = e.target;
				while (target) {
					let computedStyle = window.getComputedStyle(target);
					let webkitComputed = computedStyle.getPropertyValue("-webkit-app-region").trim();
					let webuiComputed = computedStyle.getPropertyValue("--webui-app-region").trim();
					if (webkitComputed === "drag" || webuiComputed === "drag") {
						this.#initialMouseX = e.screenX;
						this.#initialMouseY = e.screenY;
						this.#initialWindowX = this.#currentWindowX;
						this.#initialWindowY = this.#currentWindowY;
						this.#isDragging = true;
						break;
					}
					target = target.parentElement;
				}
				return;
			}
			// Calculate window position relative to cursor movement
			const deltaX = e.screenX - this.#initialMouseX;
			const deltaY = e.screenY - this.#initialMouseY;
			let newX = this.#initialWindowX + deltaX;
			let newY = this.#initialWindowY + deltaY;
			// Fix out of screen
			if (newX < 0) newX = 0;
			if (newY < 0) newY = 0;
			// Move the window
			this.#sendDrag(newX, newY);
			// Update the last window position
			this.#currentWindowX = newX;
			this.#currentWindowY = newY;
		});
		// Stop frameless dragging on mouse release
		document.addEventListener("mouseup", () => {
			this.#isDragging = false;
		});
		onbeforeunload = () => {
			this.#close();
		};
		setTimeout(() => {
			if (!this.#wsWasConnected) {
				alert('Sorry. WebUI failed to connect to the backend application. Please try again.');
			}
		}, 1500);
	}
	// Methods
	#close(reason = 0, value = '') {
		this.#closeReason = reason;
		this.#closeValue = value;
		if (this.#wsIsConnected()) {
			this.#ws.close();
		}
	}
	#freezeUi() {
		setTimeout(() => {
			if (!this.#wsIsConnected()) {
				if (document.getElementById('webui-error-connection-lost')) return;
				const div = document.createElement('div');
				div.id = 'webui-error-connection-lost';
				Object.assign(div.style, {
					position: 'relative',
					top: '0',
					left: '0',
					width: '100%',
					backgroundColor: '#ff4d4d',
					color: '#fff',
					textAlign: 'center',
					padding: '2px 0',
					fontFamily: 'Arial, sans-serif',
					fontSize: '14px',
					zIndex: '1000',
					lineHeight: '1'
				});
				div.innerText = 'WebUI Error: Connection with the backend is lost.';
				document.body.insertBefore(div, document.body.firstChild);
			}
		}, 1000);
	}
	#unfreezeUI() {
		const div = document.getElementById('webui-error-connection-lost');
		if (div) {
			div.remove();
		}
	}
	#isTextBasedCommand(cmd: number): Boolean {
		if (cmd !== this.#CMD_SEND_RAW) return true;
		return false;
	}
	#parseDimensions(input: string): {x: number; y: number; width: number; height: number} {
		try {
			const parts = input.split(',');
			if (parts.length !== 4) return {x: 0, y: 0, width: 0, height: 0};
			const x = parseFloat(parts[0]),
				y = parseFloat(parts[1]),
				width = parseFloat(parts[2]),
				height = parseFloat(parts[3]);
			return [x, y, width, height].some(isNaN) ? {x: 0, y: 0, width: 0, height: 0} : {x, y, width, height};
		} catch {
			return {x: 0, y: 0, width: 0, height: 0};
		}
	}
	#getDataStrFromPacket(buffer: Uint8Array, startIndex: number): string {
		let stringBytes: number[] = [];
		for (let i = startIndex; i < buffer.length; i++) {
			if (buffer[i] === 0) {
				// Check for null byte
				break;
			}
			stringBytes.push(buffer[i]);
		}
		// Convert the array of bytes to a string
		const stringText = new TextDecoder().decode(new Uint8Array(stringBytes));
		return stringText;
	}
	#getID(buffer: Uint8Array, index: number): number {
		if (index < 0 || index >= buffer.length - 1) {
			throw new Error('Index out of bounds or insufficient data.');
		}
		const firstByte = buffer[index];
		const secondByte = buffer[index + 1];
		const combined = (secondByte << 8) | firstByte; // Works only for little-endian
		return combined;
	}
	#addToken(buffer: Uint8Array, value: number, index: number): void {
		if (value < 0 || value > 0xffffffff) {
			throw new Error('Number is out of the range for 4 bytes representation.');
		}
		if (index < 0 || index > buffer.length - 4) {
			throw new Error('Index out of bounds or insufficient space in buffer.');
		}
		// WebUI expect Little-endian (Work for Little/Big endian platforms)
		buffer[index] = value & 0xff; // Least significant byte
		buffer[index + 1] = (value >>> 8) & 0xff;
		buffer[index + 2] = (value >>> 16) & 0xff;
		buffer[index + 3] = (value >>> 24) & 0xff; // Most significant byte
	}
	#addID(buffer: Uint8Array, value: number, index: number): void {
		if (value < 0 || value > 0xffff) {
			throw new Error('Number is out of the range for 2 bytes representation.');
		}
		if (index < 0 || index > buffer.length - 2) {
			throw new Error('Index out of bounds or insufficient space in buffer.');
		}
		// WebUI expect Little-endian (Work for Little/Big endian platforms)
		buffer[index] = value & 0xff; // Least significant byte
		buffer[index + 1] = (value >>> 8) & 0xff; // Most significant byte
	}
	#start() {
		this.#keepAlive();
		this.#callPromiseID[0] = 0;
		// Connect to the backend application
		this.#wsConnect();
	}
	#keepAlive = async () => {
		while (true) {
			if (this.#Ping) {
				// Some web browsers may close the connection
				// let's send a void message to keep WS open
				this.#sendData(new TextEncoder().encode('ping'));
			} else {
				// There is an active communication
				this.#Ping = true;
			}
			await new Promise((resolve) => setTimeout(resolve, 20000));
		}
	};
	#clicksListener() {
		document.querySelectorAll<HTMLElement>("[id]").forEach(e => {
			if (this.#AllEvents || ((e.id !== '') && (this.#bindsList.includes(e.id)))) {
				if (e.id && !e.dataset.webui_click_is_set) {
					e.dataset.webui_click_is_set = "true";
					e.addEventListener("click", () => this.#sendClick(e.id));
				}
			}
		});
	}
	async #sendData(packet: Uint8Array) {
		this.#Ping = false;
		if ((!this.#wsIsConnected()) || packet === undefined) return;
		// Enqueue the packet
		this.#sendQueue.push(packet);
		if (this.#isSending) return;
		this.#isSending = true;
		while (this.#sendQueue.length > 0) {
			const currentPacket = this.#sendQueue.shift()!;
			if (currentPacket.length < this.#MULTI_CHUNK_SIZE) {
				this.#ws.send(currentPacket.buffer);
			} else {
				// Pre-packet to let WebUI be ready for multi packet
				const pre_packet = Uint8Array.of(
					this.#WEBUI_SIGNATURE,
					0,
					0,
					0,
					0, // Token (4 Bytes)
					0,
					0, // ID (2 Bytes)
					this.#CMD_MULTI,
					...new TextEncoder().encode(currentPacket.length.toString()),
					0,
				);
				this.#ws.send(pre_packet.buffer);
				// Send chunks
				let offset = 0;
				const sendChunk = async () => {
					if (offset < currentPacket.length) {
						const chunkSize = Math.min(this.#MULTI_CHUNK_SIZE, currentPacket.length - offset);
						const chunk = currentPacket.subarray(offset, offset + chunkSize);
						this.#ws.send(chunk);
						offset += chunkSize;
						await sendChunk();
					}
				};
				await sendChunk();
			}
		}
		this.#isSending = false;
	}
	#sendClick(elem: string) {
		if (this.#wsIsConnected()) {
			// Protocol
			// 0: [SIGNATURE]
			// 1: [TOKEN]
			// 2: [ID]
			// 3: [CMD]
			// 4: [Element]
			const packet =
				elem !== ''
					? Uint8Array.of(
							this.#WEBUI_SIGNATURE,
							0,
							0,
							0,
							0, // Token (4 Bytes)
							0,
							0, // ID (2 Bytes)
							this.#CMD_CLICK,
							...new TextEncoder().encode(elem),
							0,
					  )
					: Uint8Array.of(
							this.#WEBUI_SIGNATURE,
							0,
							0,
							0,
							0, // Token (4 Bytes)
							0,
							0, // ID (2 Bytes)
							this.#CMD_CLICK,
							0,
					  );
			this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
			// this.#addID(packet, 0, this.#PROTOCOL_ID)
			this.#sendData(packet);
			if (this.#log) console.log(`WebUI -> Send Click [${elem}]`);
		}
	}
	#checkToken() {
		if (this.#wsIsConnected()) {
			// Protocol
			// 0: [SIGNATURE]
			// 1: [TOKEN]
			// 2: [ID]
			// 3: [CMD]
			const packet =
				Uint8Array.of(
					this.#WEBUI_SIGNATURE,
					0,
					0,
					0,
					0, // Token (4 Bytes)
					0,
					0, // ID (2 Bytes)
					this.#CMD_CHECK_TK,
					0,
				);
			this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
			// this.#addID(packet, 0, this.#PROTOCOL_ID)
			this.#sendData(packet);
			if (this.#log) console.log(`WebUI -> Send Token [0x${this.#token.toString(16).padStart(8, '0')}]`);
		}
	}
	#sendEventNavigation(url: string) {
		if (url !== '') {
			if (this.#wsIsConnected()) {
				if (this.#log) console.log(`WebUI -> Send Navigation Event [${url}]`);
				const packet = Uint8Array.of(
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [URL]
					this.#WEBUI_SIGNATURE,
					0,
					0,
					0,
					0, // Token (4 Bytes)
					0,
					0, // ID (2 Bytes)
					this.#CMD_NAVIGATION,
					...new TextEncoder().encode(url),
				);
				this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
				// this.#addID(packet, 0, this.#PROTOCOL_ID)
				this.#sendData(packet);
			}
		}
	}
	#sendDrag(x: number, y: number) {
		if (this.#wsIsConnected()) {
			if (this.#log) console.log(`WebUI -> Send Drag Event [${x}, ${y}]`);
			const packet = Uint8Array.of(
				// Protocol
				// 0: [SIGNATURE]
				// 1: [TOKEN]
				// 2: [ID]
				// 3: [CMD]
				// 4: [X]
				// 4: [Y]
				this.#WEBUI_SIGNATURE,
				0,
				0,
				0,
				0, // Token (4 Bytes)
				0,
				0, // ID (2 Bytes)
				this.#CMD_WINDOW_DRAG,
				...new Uint8Array(new Int32Array([x]).buffer), // X (4 Bytes)
				...new Uint8Array(new Int32Array([y]).buffer), // Y (4 Bytes)
			);
			this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
			// this.#addID(packet, 0, this.#PROTOCOL_ID)
			this.#sendData(packet);
		}
	}
	#closeWindowTimer() {
		setTimeout(function () {
			globalThis.close();
		}, 1000);
	}
	#updateBindsList() {
		if (this.#bindsList.includes('')) {
			this.#AllEvents = true;
			this.#allowNavigation = false;
		}
		this.#generateCallObjects();
		this.#clicksListener();
	}
	#toUint16(value: number): number {
		return value & 0xffff;
	}
	#generateCallObjects() {
		for (const bind of this.#bindsList) {
			if (bind.trim()) {
				const fn = bind;
				if (fn.trim()) {
					if (fn !== '__webui_core_api__') {
						if (typeof (window as any)[fn] === 'undefined') {
							this[fn] = (...args: DataTypes[]) => this.call(fn, ...args);
							(window as any)[fn] = (...args: string[]) => this.call(fn, ...args);
							if (this.#log) console.log(`WebUI -> Binding backend function [${fn}]`);
						}
					}
				}
			}
		}
	}
	#getScriptUrl() {
		const scripts = Array.from(document.scripts)
			.filter(s => s.src !== null && s.src.startsWith('http') && s.src.endsWith('webui.js'))
			.map(s => s.src);
		return scripts.length === 0 ? null : new URL(scripts[0]);
	}
	#callPromise(fn: string, ...args: DataTypes[]) {
		--this.#callPromiseID[0];
		const callId = this.#toUint16(this.#callPromiseID[0]);
		// Combine lengths
		let argsLengths = args
			.map((arg) => {
				if (typeof arg === 'object') {
					// Uint8Array
					return arg.length;
				} else {
					// string, number, boolean
					return new TextEncoder().encode(arg.toString()).length;
				}
			})
			.join(';');
		// Combine values
		let argsValues: Uint8Array = new Uint8Array();
		for (const arg of args) {
			let buffer: Uint8Array;
			if (typeof arg === 'object') {
				buffer = arg; // Uint8Array
			} else {
				// string, number, boolean
				buffer = new TextEncoder().encode(arg.toString());
			}
			const temp = new Uint8Array(argsValues.length + buffer.length + 1);
			temp.set(argsValues, 0);
			temp.set(buffer, argsValues.length);
			temp[argsValues.length + buffer.length] = 0x00;
			argsValues = temp;
		}
		// Protocol
		// 0: [SIGNATURE]
		// 1: [TOKEN]
		// 2: [ID]
		// 3: [CMD]
		// 4: [Fn, Null, {LenLen...}, Null, {Data,Null,Data,Null...}]
		let packet = new Uint8Array(0);
		const packetPush = (data: Uint8Array) => {
			const newPacket = new Uint8Array(packet.length + data.length);
			newPacket.set(packet);
			newPacket.set(data, packet.length);
			packet = newPacket;
		};
		packetPush(new Uint8Array([this.#WEBUI_SIGNATURE]));
		packetPush(new Uint8Array([0, 0, 0, 0])); // Token (4 Bytes)
		packetPush(new Uint8Array([0, 0])); // ID (2 Bytes)
		packetPush(new Uint8Array([this.#CMD_CALL_FUNC]));
		packetPush(new TextEncoder().encode(fn));
		packetPush(new Uint8Array([0]));
		packetPush(new TextEncoder().encode(argsLengths));
		packetPush(new Uint8Array([0]));
		packetPush(new Uint8Array(argsValues));
		this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
		this.#addID(packet, callId, this.#PROTOCOL_ID);
		return new Promise((resolve) => {
			this.#callPromiseResolve[callId] = resolve;
			this.#sendData(packet);
		});
	}
	async callCore(fn: string, ...args: DataTypes[]): Promise<DataTypes> {
		return this.call('__webui_core_api__', fn, ...args);
	}
	// -- WebSocket ----------------------------
	#wsIsConnected(): boolean {
		return ((this.#ws) && (this.#ws.readyState === WebSocket.OPEN));
	}
	#wsConnect(): void {
		if (this.#wsIsConnected()) {
			this.#ws.close();
		}
		this.#TokenAccepted = false;
		const scriptUrl = this.#getScriptUrl();
		const host = scriptUrl !== null ? scriptUrl.hostname : window.location.hostname;
		const url = this.#secure ? ('wss://' + host) : ('ws://' + host);
		this.#ws = new WebSocket(`${url}:${this.#port}/_webui_ws_connect`);
		this.#ws.binaryType = 'arraybuffer';
		this.#ws.onopen = this.#wsOnOpen.bind(this);
		this.#ws.onmessage = this.#wsOnMessage.bind(this);
		this.#ws.onclose = this.#wsOnClose.bind(this);
		this.#ws.onerror = this.#wsOnError.bind(this);
	}
	#wsOnOpen = (event: Event) => {
		this.#wsWasConnected = true;
		this.#unfreezeUI();
		if (this.#log) console.log('WebUI -> Connected');
		this.#checkToken();
	};
	#wsOnError = (event: Event) => {
		if (this.#log) console.log(`WebUI -> Connection failed.`);
	};
	#wsOnClose = (event: CloseEvent) => {
		if (this.#closeReason === this.#CMD_NAVIGATION) {
			this.#closeReason = 0;
			if (this.#log) console.log(`WebUI -> Connection lost. Navigation to [${this.#closeValue}]`);
			this.#allowNavigation = true;
			globalThis.location.replace(this.#closeValue);
		} else {
			if (this.#wsStayAlive) {
				// Re-connect
				if (this.#log) console.log(`WebUI -> Connection lost (${event.code}). Reconnecting...`);
				this.#freezeUi();
				setTimeout(() => this.#wsConnect(), this.#wsStayAliveTimeout);
			}
			else if (this.#log) {
				// Debug close
				console.log(`WebUI -> Connection lost (${event.code})`);
				this.#freezeUi();
			} else {
				// Release close
				this.#closeWindowTimer();
			}
		}
		// Event Callback
		if (this.#eventsCallback) {
			this.#eventsCallback(this.event.DISCONNECTED);
		}
	};
	#wsOnMessage = async (event: MessageEvent) => {
		const buffer8 = new Uint8Array(event.data);
		if (buffer8.length < this.#PROTOCOL_SIZE) return;
		if (buffer8[this.#PROTOCOL_SIGN] !== this.#WEBUI_SIGNATURE) return;
		if (this.#isTextBasedCommand(buffer8[this.#PROTOCOL_CMD])) {
			// UTF8 Text based commands
			const callId = this.#getID(buffer8, this.#PROTOCOL_ID);
			// Process Command
			switch (buffer8[this.#PROTOCOL_CMD]) {
				case this.#CMD_JS_QUICK:
				case this.#CMD_JS:
					{
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [Script]
						const script: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
						const scriptSanitize = script.replace(/(?:\r\n|\r|\n)/g, '\n');
						if (this.#log) console.log(`WebUI -> CMD -> JS [${scriptSanitize}]`);
						// Get callback result
						let FunReturn: string | Uint8Array = 'undefined';
						let FunError = false;
						let isBinaryReturn = false;
						try {
							const result = await AsyncFunction(scriptSanitize)();
							if (result instanceof Uint8Array) {
								FunReturn = result;
								isBinaryReturn = true;
							} else {
								FunReturn = String(result);
							}
						} catch (e) {
							FunError = true;
							FunReturn = e.message;
						}
						// Stop if this is a quick call
						if (buffer8[this.#PROTOCOL_CMD] === this.#CMD_JS_QUICK) return;
						// Get the call return
						if (FunReturn === undefined) {
							FunReturn = 'undefined';
						}
						// Logging
						if (this.#log && !FunError) {
							if (isBinaryReturn) {
								const binaryData = FunReturn as Uint8Array;
								const hexPreview = Array.from(binaryData.slice(0, 64)).map(b => `0x${b.toString(16).padStart(2, '0')}`).join(', ');
								console.log(`WebUI -> CMD -> JS -> Return Success (${binaryData.length} Bytes) [${hexPreview}${binaryData.length > 64 ? '...' : ''}]`);
							} else {
								const stringData = String(FunReturn);
								console.log(`WebUI -> CMD -> JS -> Return Success (${new TextEncoder().encode(stringData).length} Bytes) [${stringData.substring(0, 64)}${stringData.length > 64 ? '...' : ''}]`);
							}
						}
						else if (this.#log && FunError) {
							const errorString = String(FunReturn);
							console.log(`WebUI -> CMD -> JS -> Return Error [${errorString.substring(0, 64)}${errorString.length > 64 ? '...' : ''}]`);
						}
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [Error, Script Response]
						let packet = new Uint8Array(0);
						const packetPush = (data: Uint8Array) => {
							const newPacket = new Uint8Array(packet.length + data.length);
							newPacket.set(packet);
							newPacket.set(data, packet.length);
							packet = newPacket;
						};
						const packetPushStr = (data: string) => {
							const chunkSize = 1024 * 8;
							if (data.length > chunkSize) {
								const encoder = new TextEncoder();
								for (let i = 0; i < data.length; i += chunkSize) {
									const chunk = data.substring(i, Math.min(i + chunkSize, data.length));
									const encodedChunk = encoder.encode(chunk);
									packetPush(encodedChunk);
								}
							} else {
								packetPush(new TextEncoder().encode(data));
							}
						};
						packetPush(new Uint8Array([this.#WEBUI_SIGNATURE]));
						packetPush(new Uint8Array([0, 0, 0, 0])); // Token (4 Bytes)
						packetPush(new Uint8Array([0, 0])); // ID (2 Bytes)
						packetPush(new Uint8Array([this.#CMD_JS]));
						packetPush(new Uint8Array(FunError ? [1] : [0]));
						if (isBinaryReturn) {
							packetPush(FunReturn as Uint8Array);
						} else {
							packetPushStr(FunReturn as string);
						}
						packetPush(new Uint8Array([0]));
						this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN);
						this.#addID(packet, callId, this.#PROTOCOL_ID);
						this.#sendData(packet);
					}
					break;
				case this.#CMD_CALL_FUNC:
					{
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [Call Response]
						const callResponse: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
						if (this.#log) {
							console.log(`WebUI -> CMD -> Call Response [${callResponse}]`);
						}
						if (this.#callPromiseResolve[callId]) {
							if (this.#log) {
								console.log(`WebUI -> CMD -> Resolving Response #${callId}...`);
							}
							this.#callPromiseResolve[callId]?.(callResponse);
							this.#callPromiseResolve[callId] = undefined;
						}
					}
					break;
				case this.#CMD_NAVIGATION:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [URL]
					const url: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
					if (this.#log) console.log(`WebUI -> CMD -> Navigation [${url}]`);
					this.#close(this.#CMD_NAVIGATION, url);
					break;
				case this.#CMD_WINDOW_RESIZED:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [x,y,width,height]
					const widthAndHeight: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
					const { x, y, width, height } = this.#parseDimensions(widthAndHeight);
					// Update drag positions
					this.#currentWindowX = x;
					this.#currentWindowY = y;
					if (this.#log) console.log(`WebUI -> CMD -> Window Resized [x: ${x}, y: ${y}, width: ${width}, height: ${height}]`);
					break;
				case this.#CMD_NEW_ID:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [New Element]
					const newElement: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
					if (this.#log) console.log(`WebUI -> CMD -> New Bind ID [${newElement}]`);
					if (!this.#bindsList.includes(newElement)) this.#bindsList.push(newElement);
					// Generate objects
					this.#updateBindsList();
					break;
				case this.#CMD_CLOSE:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					if (this.#log) {
						// Debug Close
						console.log(`WebUI -> CMD -> Close`);
						if (this.#wsIsConnected()) {
							this.#wsStayAlive = false;
							this.#ws.close();
						}
					}
					else {
						// Release Close
						globalThis.close();
					}
					break;
				case this.#CMD_CHECK_TK:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [Status]
					// 5: [BindsList]
					const status = (buffer8[this.#PROTOCOL_DATA] == 0 ? false : true);
					const tokenHex = `0x${this.#token.toString(16).padStart(8, '0')}`;
					if (status) {
						if (this.#log) console.log(`WebUI -> CMD -> Token [${tokenHex}] Accepted`);
						this.#TokenAccepted = true;
						// Get binds list (CSV)
						let csv: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA + 1);
						csv = csv.endsWith(',') ? csv.slice(0, -1) : csv;
						this.#bindsList = csv.split(',');
						// Generate objects
						this.#updateBindsList();
						// User event callback
						if (this.#eventsCallback) {
							this.#eventsCallback(this.event.CONNECTED);
						}
					}
					else {
						if (this.#log) console.log(`WebUI -> CMD -> Token [${tokenHex}] Not Accepted. Reload page...`);
						// Refresh the page to get a new token
						this.#allowNavigation = true;
						this.#wsStayAlive = false;
						globalThis.location.reload();
					}
					break;
			}
		} else {
			// Raw-binary based commands
			switch (buffer8[this.#PROTOCOL_CMD]) {
				case this.#CMD_SEND_RAW:
					// Protocol
					// 0: [SIGNATURE]
					// 1: [TOKEN]
					// 2: [ID]
					// 3: [CMD]
					// 4: [Function,Null,Raw Data]
					// Get function name
					const functionName: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
					// Get the raw data
					const rawDataIndex: number = this.#PROTOCOL_DATA + functionName.length + 1;
					const rawDataSize: number = (buffer8.length - rawDataIndex) - 1;
					const userRawData = buffer8.subarray(rawDataIndex, (rawDataIndex + rawDataSize));
					if (this.#log) console.log(`WebUI -> CMD -> Received Raw ${rawDataSize} bytes for [${functionName}()]`);
					// Call the user function, and pass the raw data
					if (typeof window[functionName] === 'function') window[functionName](userRawData);
					else await AsyncFunction(functionName + '(userRawData)')();
					break;
			}
		}
	};
	// -- Public APIs --------------------------
	/**
	 * Call a backend function
	 *
	 * @param fn - binding name
	 * @param data - data to be send to the backend function
	 * @return - Response of the backend callback string
	 * @example - const res = await webui.call("myID", 123, true, "Hi", new Uint8Array([0x42, 0x43, 0x44]))
	 */
	async call(fn: string, ...args: DataTypes[]): Promise<DataTypes> {
		if (!fn) return Promise.reject(new SyntaxError('No binding name is provided'));

		if (!this.#wsIsConnected()) return Promise.reject(new Error('WebSocket is not connected'));

		// Check binding list
		if (fn !== '__webui_core_api__') {
			if (!this.#AllEvents && !this.#bindsList.includes(`${fn}`))
				return Promise.reject(new ReferenceError(`No binding was found for "${fn}"`));
		}

		// Call backend and wait for response
		if (this.#log) console.log(`WebUI -> Calling [${fn}(...)]`);
		const response = (await this.#callPromise(fn, ...args)) as string;

		// WebUI lib accept `DataTypes` but return only string
		if (typeof response !== 'string') return '';

		return response;
	}
	/**
	 * Active or deactivate webui debug logging
	 *
	 * @param status - log status to set
	 */
	setLogging(status: boolean) {
		if (status) {
			console.log('WebUI -> Log Enabled.');
			this.#log = true;
		} else {
			console.log('WebUI -> Log Disabled.');
			this.#log = false;
		}
	}
	/**
	 * Encode text into base64 string
	 *
	 * @param data - text string
	 */
	encode(data: string): string {
		return btoa(data);
	}
	/**
	 * Decode base64 string into text
	 *
	 * @param data - base64 string
	 */
	decode(data: string): string {
		return atob(data);
	}
	/**
	 * Set a callback to receive events like connect/disconnect
	 *
	 * @param callback - callback function `myCallback(e)`
	 * @example - webui.setEventCallback((e) => {if(e == webui.event.CONNECTED){ ... }});
	 */
	setEventCallback(callback: (e: number) => void): void {
        this.#eventsCallback = callback;
    }
	/**
	 * Check if UI is connected to the back-end. The connection
	 * is done by including `webui.js` virtual file in the HTML.
	 *
	 * @return - Boolean `true` if connected
	 */
	isConnected(): boolean {
		return ((this.#wsIsConnected()) && (this.#TokenAccepted));
	}
	/**
	 * Get OS high contrast preference.
	 *
	 * @return - Boolean `True` if OS is using high contrast theme
	 */
	async isHighContrast(): Promise<boolean> {
		// Call a core function and wait for response
		const response = await this.callCore("high_contrast") as boolean;
		if (this.#log) console.log(`Core Response: [${response}]`);
		return response;
	}
	/**
	 * When binding all events on the backend, WebUI blocks all navigation events
	 * and sends them to the backend. This API allows you to control that behavior.
	 *
	 * @param status - Boolean `True` means WebUI will allow navigations
	 * @example - webui.allowNavigation(true); // Allow navigation
	 * window.location.replace('www.test.com'); // This will now proceed as usual
	 */
	allowNavigation(status: boolean): void {
        this.#allowNavigation = status;
    }
}
// Export
type webui = WebuiBridge;
export default webui;
export type { WebuiBridge };
// Wait for the html to be parsed
addEventListener('load', () => {
	document.body.addEventListener('contextmenu', (event) => event.preventDefault());
	addRefreshableEventListener(document.body, 'input', 'contextmenu', (event) => event.stopPropagation());
});
