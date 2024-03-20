'use-strict'; // Force strict mode for transpiled

/*
  WebUI Bridge

  http://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2023 Hassan Draga.
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
	#winNum: number;
	#bindList: string[] = [];
	#log: boolean;
	#winX: number;
	#winY: number;
	#winW: number;
	#winH: number;
	// Internals
	#ws: WebSocket;
	#wsStatus = false;
	#wsStatusOnce = false;
	#closeReason = 0;
	#closeValue: string;
	#hasEvents = false;
	#callPromiseID = new Uint16Array(1);
	#callPromiseResolve: (((data: string) => unknown) | undefined)[] = [];
	#allowNavigation = false;
	#sendQueue: Uint8Array[] = [];
	#isSending = false;
	// WebUI Const
	#WEBUI_SIGNATURE = 221;
	#CMD_JS = 254;
	#CMD_JS_QUICK = 253;
	#CMD_CLICK = 252;
	#CMD_NAVIGATION = 251;
	#CMD_CLOSE = 250;
	#CMD_CALL_FUNC = 249;
	#CMD_SEND_RAW = 248;
	#CMD_NEW_ID = 247;
	#CMD_MULTI = 246;
	#MULTI_CHUNK_SIZE = 65500;
	#PROTOCOL_SIZE = 8; // Protocol header size in bytes
	#PROTOCOL_SIGN = 0; // Protocol byte position: Signature (1 Byte)
	#PROTOCOL_TOKEN = 1; // Protocol byte position: Token (4 Bytes)
	#PROTOCOL_ID = 5; // Protocol byte position: ID (2 Bytes)
	#PROTOCOL_CMD = 7; // Protocol byte position: Command (1 Byte)
	#PROTOCOL_DATA = 8; // Protocol byte position: Data (n Byte)
	#Token = new Uint32Array(1);
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
		secure,
		token,
		port,
		winNum,
		bindList,
		log = false,
		winX,
		winY,
		winW,
		winH,
	}: {
		secure: boolean;
		token: number;
		port: number;
		winNum: number;
		bindList: string[];
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
		this.#winNum = winNum;
		this.#bindList = bindList;
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
			window.moveTo(this.#winX, this.#winY);
		}
		// Resize the current window
		if (this.#winW !== undefined && this.#winH !== undefined) {
			window.resizeTo(this.#winW, this.#winH);
		}
		// WebSocket
		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket is not supported by your web browser.');
			if (!this.#log) globalThis.close();
		}
		// Connect to the backend application
		this.#start();
		// Handle navigation server side
		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				if (!this.#allowNavigation) {
					event.preventDefault();
					const url = new URL(event.destination.url);
					if (this.#hasEvents) {
						if (this.#log) console.log(`WebUI -> DOM -> Navigation Event [${url.href}]`);
						this.#sendEventNavigation(url.href);
					} else {
						this.#close(this.#CMD_NAVIGATION, url.href);
					}
				}
			});
		} else {
			// Handle all link click to prevent natural navigation
			// Rebind listener if user inject new html
			addRefreshableEventListener(document.body, 'a', 'click', (event) => {
				if (!this.#allowNavigation) {
					event.preventDefault();
					const { href } = event.target as HTMLAnchorElement;
					if (this.#hasEvents) {
						if (this.#log) console.log(`WebUI -> DOM -> Navigation Click Event [${href}]`);
						this.#sendEventNavigation(href);
					} else {
						this.#close(this.#CMD_NAVIGATION, href);
					}
				}
			});
		}
		// Prevent F5 refresh
		document.addEventListener('keydown', (event) => {
			if (this.#log) return; // Allowed in debug mode
			if (event.key === 'F5') event.preventDefault();
		});
		onbeforeunload = () => {
			this.#close();
		};
		setTimeout(() => {
			if (!this.#wsStatusOnce) {
				this.#freezeUi();
				alert('Sorry. WebUI failed to connect to the backend application. Please try again.');
				if (!this.#log) globalThis.close();
			}
		}, 1500);
	}
	// Methods
	#close(reason = 0, value = '') {
		this.#wsStatus = false;
		this.#closeReason = reason;
		this.#closeValue = value;
		this.#ws.close();
		if (reason === this.#CMD_NAVIGATION) {
			if (this.#log) {
				console.log(`WebUI -> Close -> Navigation to [${value}]`);
			}
			this.#allowNavigation = true;
			globalThis.location.replace(this.#closeValue);
		} else {
			if (this.#log) {
				console.log(`WebUI -> Close.`);
			}
		}
	}
	#freezeUi() {
		document.body.style.filter = 'contrast(1%)';
	}
	#isTextBasedCommand(cmd: number): Boolean {
		if (cmd !== this.#CMD_SEND_RAW) return true;
		return false;
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
		this.#generateCallObjects();
		this.#keepAlive();
		this.#callPromiseID[0] = 0;
		if (this.#bindList.includes(this.#winNum + '/')) {
			this.#hasEvents = true;
		}
		const host = window.location.hostname;
		const url = this.#secure ? ('wss://' + host) : ('ws://' + host);
		this.#ws = new WebSocket(`${url}:${this.#port}/_webui_ws_connect`);
		this.#ws.binaryType = 'arraybuffer';
		this.#ws.onopen = () => {
			this.#wsStatus = true;
			this.#wsStatusOnce = true;
			if (this.#log) console.log('WebUI -> Connected');
			this.#clicksListener();
			if (this.#eventsCallback) {
				this.#eventsCallback(this.event.CONNECTED);
			}
		};
		this.#ws.onerror = () => {
			if (this.#log) console.log('WebUI -> Connection Failed');
			this.#freezeUi();
		};
		this.#ws.onclose = (event) => {
			this.#wsStatus = false;
			if (this.#closeReason === this.#CMD_NAVIGATION) {
				if (this.#log) {
					console.log(`WebUI -> Connection closed du to Navigation to [${this.#closeValue}]`);
				}
			} else {
				if (this.#log) {
					console.log(`WebUI -> Connection lost (${event.code})`);
					this.#freezeUi();
				} else {
					this.#closeWindowTimer();
				}
			}
			if (this.#eventsCallback) {
				this.#eventsCallback(this.event.DISCONNECTED);
			}
		};
		this.#ws.onmessage = async (event) => {
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
							const script = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
							const scriptSanitize = script.replace(/(?:\r\n|\r|\n)/g, '\n');
							if (this.#log) console.log(`WebUI -> CMD -> JS [${scriptSanitize}]`);
							// Get callback result
							let FunReturn = 'undefined';
							let FunError = false;
							try {
								FunReturn = await AsyncFunction(scriptSanitize)();
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
							if (this.#log && !FunError) console.log(`WebUI -> CMD -> JS -> Return Success [${FunReturn}]`);
							if (this.#log && FunError) console.log(`WebUI -> CMD -> JS -> Return Error [${FunReturn}]`);
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
							packetPushStr(FunReturn);
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
							const callResponse = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
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
						const url = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
						console.log(`WebUI -> CMD -> Navigation [${url}]`);
						this.#close(this.#CMD_NAVIGATION, url);
						break;
					case this.#CMD_NEW_ID:
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [New Element]
						const newElement = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA);
						console.log(`WebUI -> CMD -> New Bind ID [${newElement}]`);
						if (!this.#bindList.includes(newElement)) this.#bindList.push(newElement);
						break;
					case this.#CMD_CLOSE:
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						if (!this.#log) globalThis.close();
						else {
							console.log(`WebUI -> CMD -> Close`);
							this.#ws.close();
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
						const rawDataIndex: number = 2 + functionName.length + 1;
						const userRawData = buffer8.subarray(rawDataIndex);
						if (this.#log) console.log(`WebUI -> CMD -> Send Raw ${buffer8.length} bytes to [${functionName}()]`);
						// Call the user function, and pass the raw data
						if (typeof window[functionName] === 'function') window[functionName](userRawData);
						else await AsyncFunction(functionName + '(userRawData)')();
						break;
				}
			}
		};
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
		Object.keys(window).forEach((key) => {
			if (/^on(click)/.test(key)) {
				globalThis.addEventListener(key.slice(2), (event) => {
					if (!(event.target instanceof HTMLElement)) return;
					if (
						this.#hasEvents ||
						(event.target.id !== '' && this.#bindList.includes(this.#winNum + '/' + event.target?.id))
					) {
						this.#sendClick(event.target.id);
					}
				}, {passive:true});
			}
		});
	}
	async #sendData(packet: Uint8Array) {
		this.#Ping = false;
		if (!this.#wsStatus || packet === undefined) return;
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
		if (this.#wsStatus) {
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
	#sendEventNavigation(url: string) {
		if (url !== '') {
			if (this.#hasEvents) {
				if (this.#log) console.log(`WebUI -> Send Navigation Event [${url}]`);
				if (this.#wsStatus && this.#hasEvents) {
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
			} else {
				if (this.#log) console.log(`WebUI -> Navigation To [${url}]`);
				this.#allowNavigation = true;
				globalThis.location.replace(url);
			}
		}
	}
	#closeWindowTimer() {
		setTimeout(function () {
			globalThis.close();
		}, 1000);
	}
	#toUint16(value: number): number {
		return value & 0xffff;
	}
	#generateCallObjects() {
		for (const bind of this.#bindList) {
			if (bind.trim()) {
				const fn = bind.replace(`${this.#winNum}/`, '');
				if (fn.trim()) {
					this[fn] = (...args: DataTypes[]) => this.call(fn, ...args);
					if (typeof (window as any)[fn] === 'undefined') {
						(window as any)[fn] = (...args: string[]) => this.call(fn, ...args);
					}
				}
			}
		}
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

		if (!this.#wsStatus) return Promise.reject(new Error('WebSocket is not connected'));

		// Check binding list
		if (!this.#hasEvents && !this.#bindList.includes(`${this.#winNum}/${fn}`))
			return Promise.reject(new ReferenceError(`No binding was found for "${fn}"`));

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
