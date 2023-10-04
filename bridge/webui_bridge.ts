'use-strict' // Force strict mode for transpiled

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
import { AsyncFunction, addRefreshableEventListener } from './utils.ts'

type B64string = string
type JSONValue =
	| string
	| number
	| boolean
	| { [x: string]: JSONValue | undefined }
	| JSONValue[]
	| Uint8Array

class WebuiBridge {
	// WebUI settings
	#token: number
	#port: number
	#winNum: number
	#bindList: unknown[] = []
	#log: boolean
	#winX: number
	#winY: number
	#winW: number
	#winH: number

	// Internals
	#ws: WebSocket
	#wsStatus = false
	#wsStatusOnce = false
	#closeReason = 0
	#closeValue: string
	#hasEvents = false
	#callPromiseID = new Uint16Array(1)
	#callPromiseResolve: (((data: string) => unknown) | undefined)[] = []
	#allowNavigation = false

	// WebUI const
	#WEBUI_SIGNATURE = 221
	#CMD_JS = 254
	#CMD_JS_QUICK = 253
	#CMD_CLICK = 252
	#CMD_NAVIGATION = 251
	#CMD_CLOSE = 250
	#CMD_CALL_FUNC = 249
	#CMD_SEND_RAW = 248
	#CMD_NEW_ID = 247
	#PROTOCOL_SIZE = 8 // Protocol header size in bytes
	#PROTOCOL_SIGN = 0 // Protocol byte position: Signature (1 Byte)
	#PROTOCOL_TOKEN = 1 // Protocol byte position: Token (4 Bytes)
	#PROTOCOL_ID = 5 // Protocol byte position: ID (2 Bytes)
	#PROTOCOL_CMD = 7 // Protocol byte position: Command (1 Byte)
	#PROTOCOL_DATA = 8 // Protocol byte position: Data (n Byte)

	#Token = new Uint32Array(1);

	constructor({
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
		token: number
		port: number
		winNum: number
		bindList: unknown[]
		log?: boolean
		winX: number
		winY: number
		winW: number
		winH: number
	}) {
		// Constructor arguments are injected by webui.c
		this.#token = token
		this.#port = port
		this.#winNum = winNum
		this.#bindList = bindList
		this.#log = log
		this.#winX = winX
		this.#winY = winY
		this.#winW = winW
		this.#winH = winH

		// Token
		this.#Token[0] = this.#token;

		// Instance
		if ('webui' in globalThis) {
			throw new Error(
				'Sorry. WebUI is already defined, only one instance is allowed.'
			)
		}

		// Positioning the current window
		if (this.#winX !== undefined && this.#winY !== undefined) {
			window.moveTo(this.#winX, this.#winY)
		}

		// Resize the current window
		if (this.#winW !== undefined && this.#winH !== undefined) {
			window.resizeTo(this.#winW, this.#winH)
		}

		// WebSocket
		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket is not supported by your web browser.')
			if (!this.#log) globalThis.close()
		}

		// Connect to the backend application
		this.#start()

		// Handle navigation server side
		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				if(!this.#allowNavigation) {
					event.preventDefault()
					const url = new URL(event.destination.url)
					if (this.#hasEvents) {
						if (this.#log) console.log(`WebUI -> DOM -> Navigation Event [${url.href}]`)
						this.#sendEventNavigation(url.href)
					}
					else {
						this.#close(this.#CMD_NAVIGATION, url.href)
					}
				}
			})
		} else {
			// Handle all link click to prevent natural navigation
			// Rebind listener if user inject new html
			addRefreshableEventListener(
				document.body,
				'a',
				'click',
				(event) => {
					if(!this.#allowNavigation) {
						event.preventDefault()
						const { href } = event.target as HTMLAnchorElement
						if (this.#hasEvents) {
							if (this.#log) console.log(`WebUI -> DOM -> Navigation Click Event [${href}]`)
							// if (this.#isExternalLink(href)) {
							// 	this.#close(this.#CMD_NAVIGATION, href)
							// } else {
							// 	this.#sendEventNavigation(href)
							// }
							this.#sendEventNavigation(href)
						}
						else {
							this.#close(this.#CMD_NAVIGATION, href)
						}
					}
				}
			)
		}

		// Prevent F5 refresh
		document.addEventListener('keydown', (event) => {
			// Disable F5
			if (this.#log) return
			if (event.key === 'F5') event.preventDefault()
		})

		onbeforeunload = () => {
			this.#close()
		}

		setTimeout(() => {
			if (!this.#wsStatusOnce) {
				this.#freezeUi()
				alert(
					'Sorry. WebUI failed to connect to the backend application. Please try again.'
				)
				if (!this.#log) globalThis.close()
			}
		}, 1500)
	}

	#close(reason = 0, value = '') {
		// if (reason === this.#CMD_NAVIGATION) this.#sendEventNavigation(value)
		this.#wsStatus = false
		this.#closeReason = reason
		this.#closeValue = value
		this.#ws.close()
		if (reason === this.#CMD_NAVIGATION) {
			if (this.#log) {
				console.log(
					`WebUI -> Close -> Navigation to [${value}]`
				)
			}
			this.#allowNavigation = true
			globalThis.location.replace(this.#closeValue)
		}
		else {
			if (this.#log) {
				console.log(
					`WebUI -> Close.`
				)
			}
		}
	}

	#freezeUi() {
		document.body.style.filter = 'contrast(1%)'
	}

	#isTextBasedCommand(cmd: number): Boolean {
		if(cmd !== this.#CMD_SEND_RAW)
			return true;
		return false;
	}

	#getDataStrFromPacket(buffer: Uint8Array, startIndex: number): string {
		let stringBytes: number[] = [];
		for (let i = startIndex; i < buffer.length; i++) {
			if (buffer[i] === 0) { // Check for null byte
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
		if (value < 0 || value > 0xFFFFFFFF) {
			throw new Error('Number is out of the range for 4 bytes representation.');
		}
	
		if (index < 0 || index > buffer.length - 4) {
			throw new Error('Index out of bounds or insufficient space in buffer.');
		}
	
		// WebUI expect Little-endian (Work for Little/Big endian platforms)
		buffer[index] = value & 0xFF;
		buffer[index + 1] = (value >>> 8) & 0xFF;
		buffer[index + 2] = (value >>> 16) & 0xFF;
		buffer[index + 3] = (value >>> 24) & 0xFF;
	}

	#addID(buffer: Uint8Array, value: number, index: number): void {
		if (value < 0 || value > 0xFFFF) {
			throw new Error('Number is out of the range for 2 bytes representation.');
		}
	
		if (index < 0 || index > buffer.length - 2) {
			throw new Error('Index out of bounds or insufficient space in buffer.');
		}
	
		// WebUI expect Little-endian (Work for Little/Big endian platforms)
		buffer[index] = value & 0xFF; // Least significant byte
		buffer[index + 1] = (value >>> 8) & 0xFF; // Most significant byte
	}

	#start() {
		this.#callPromiseID[0] = 0

		if (this.#bindList.includes(this.#winNum + '/')) {
			this.#hasEvents = true
		}

		this.#ws = new WebSocket(
			`ws://localhost:${this.#port}/_webui_ws_connect`
		)
		this.#ws.binaryType = 'arraybuffer'

		this.#ws.onopen = () => {
			this.#wsStatus = true
			this.#wsStatusOnce = true
			if (this.#log) console.log('WebUI -> Connected')
			this.#clicksListener()
		}

		this.#ws.onerror = () => {
			if (this.#log) console.log('WebUI -> Connection Failed')
			this.#freezeUi()
		}

		this.#ws.onclose = (event) => {
			this.#wsStatus = false
			if (this.#closeReason === this.#CMD_NAVIGATION) {
				if (this.#log) {
					console.log(
						`WebUI -> Connection closed du to Navigation to [${this.#closeValue}]`
					)
				}
			} else {
				if (this.#log) {
					console.log(`WebUI -> Connection lost (${event.code})`)
					this.#freezeUi()
				} else {
					this.#closeWindowTimer()
				}
			}
		}

		this.#ws.onmessage = async (event) => {
			const buffer8 = new Uint8Array(event.data)
			if (buffer8.length < this.#PROTOCOL_SIZE) return
			if (buffer8[this.#PROTOCOL_SIGN] !== this.#WEBUI_SIGNATURE) return

			if(this.#isTextBasedCommand(buffer8[this.#PROTOCOL_CMD])) {
				// UTF8 Text based commands

				const callId = this.#getID(buffer8, this.#PROTOCOL_ID)

				// Process Command
				switch (buffer8[this.#PROTOCOL_CMD]) {
					case this.#CMD_CALL_FUNC:
						{
							// Protocol
							// 0: [SIGNATURE]
							// 1: [TOKEN]
							// 2: [ID]
							// 3: [CMD]
							// 4: [Call Response]

							const callResponse = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA)
							
							if (this.#log) {
								console.log(`WebUI -> CMD -> Call Response [${callResponse}]`)
							}
							if (this.#callPromiseResolve[callId]) {
								if (this.#log) {
									console.log(
										`WebUI -> CMD -> Resolving Response #${callId}...`
									)
								}
								this.#callPromiseResolve[callId]?.(callResponse)
								this.#callPromiseResolve[callId] = undefined
							}
						}
						break
					case this.#CMD_NAVIGATION:
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [URL]

						const url = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA)
						console.log(`WebUI -> CMD -> Navigation [${url}]`)
						this.#close(this.#CMD_NAVIGATION, url)
						break
					case this.#CMD_NEW_ID:
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [New Element]

						const newElement = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA)
						console.log(`WebUI -> CMD -> New Bind ID [${newElement}]`)
						if(!this.#bindList.includes(newElement))
							this.#bindList.push(newElement)
						break
					case this.#CMD_JS_QUICK:
					case this.#CMD_JS:
						{
							// Protocol
							// 0: [SIGNATURE]
							// 1: [TOKEN]
							// 2: [ID]
							// 3: [CMD]
							// 4: [Script]

							const script = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA)
							const scriptSanitize = script.replace(
								/(?:\r\n|\r|\n)/g,
								'\n'
							)

							if (this.#log)
								console.log(`WebUI -> CMD -> JS [${scriptSanitize}]`)

							// Get callback result
							let FunReturn = 'undefined'
							let FunError = false
							try {
								FunReturn = await AsyncFunction(scriptSanitize)()
							} catch (e) {
								FunError = true
								FunReturn = e.message
							}

							// Stop if this is a quick call
							if (buffer8[this.#PROTOCOL_CMD] === this.#CMD_JS_QUICK) return

							// Get the call return
							if (FunReturn === undefined) {
								FunReturn = 'undefined'
							}

							// Logging
							if (this.#log && !FunError)
								console.log(`WebUI -> CMD -> JS -> Return Success [${FunReturn}]`)
							if (this.#log && FunError)
								console.log(`WebUI -> CMD -> JS -> Return Error [${FunReturn}]`)

							// Protocol
							// 0: [SIGNATURE]
							// 1: [TOKEN]
							// 2: [ID]
							// 3: [CMD]
							// 4: [Error, Script Response]

							const Return8 = Uint8Array.of(
								this.#WEBUI_SIGNATURE,
								0, 0, 0, 0, // Token (4 Bytes)
								0, 0, // ID (2 Bytes)
								this.#CMD_JS,
								FunError ? 1 : 0,
								...new TextEncoder().encode(FunReturn)
							)

							this.#addToken(Return8, this.#token, this.#PROTOCOL_TOKEN)
							this.#addID(Return8, callId, this.#PROTOCOL_ID)

							if (this.#wsStatus) this.#ws.send(Return8.buffer)
						}
						break
					case this.#CMD_CLOSE:
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]

						if (!this.#log)
							globalThis.close()
						else {
							console.log(`WebUI -> CMD -> Close`)
							this.#ws.close()
						}
						break
				}
			}
			else {
				// Raw binary commands

				// Process Command
				switch (buffer8[this.#PROTOCOL_CMD]) {
					case this.#CMD_SEND_RAW:
						{
							// Protocol
							// 0: [SIGNATURE]
							// 1: [TOKEN]
							// 2: [ID]
							// 3: [CMD]
							// 4: [Function,Null,Raw Data]

							// Get function name
							const functionName: string = this.#getDataStrFromPacket(buffer8, this.#PROTOCOL_DATA)
							
							// Get the raw data
							const rawDataIndex: number = 2 + functionName.length + 1
							const userRawData = buffer8.subarray(rawDataIndex);

							if (this.#log)
								console.log(`WebUI -> CMD -> Send Raw ${buffer8.length} bytes to [${functionName}()]`)
							
							// Call the user function, and pass the raw data
							if (typeof window[functionName] === 'function')
								window[functionName](userRawData);
							else
								await AsyncFunction(functionName + '(userRawData);')()
						}
						break
				}
			}
		}
	}

	#clicksListener() {
		Object.keys(window).forEach((key) => {
			if (/^on(click)/.test(key)) {
				globalThis.addEventListener(key.slice(2), (event) => {
					if (!(event.target instanceof HTMLElement)) return
					if (
						this.#hasEvents ||
						(event.target.id !== '' &&
							this.#bindList.includes(
								this.#winNum + '/' + event.target?.id
							))
					) {
						this.#sendClick(event.target.id)
					}
				})
			}
		})
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
							0, 0, 0, 0, // Token (4 Bytes)
							0, 0, // ID (2 Bytes)
							this.#CMD_CLICK,
							...new TextEncoder().encode(elem),
							0
					  )
					: Uint8Array.of(
							this.#WEBUI_SIGNATURE,
							0, 0, 0, 0, // Token (4 Bytes)
							0, 0, // ID (2 Bytes)
							this.#CMD_CLICK,
							0
					  )
			this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN)
			// this.#addID(packet, 0, this.#PROTOCOL_ID)
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`WebUI -> Send Click [${elem}]`)
		}
	}

	#sendEventNavigation(url: string) {
		if(url !== '') {
			if(this.#hasEvents) {
				if (this.#log) console.log(`WebUI -> Send Navigation Event [${url}]`)			
				if (this.#wsStatus && this.#hasEvents) {
					const packet = Uint8Array.of(
						// Protocol
						// 0: [SIGNATURE]
						// 1: [TOKEN]
						// 2: [ID]
						// 3: [CMD]
						// 4: [URL]
						this.#WEBUI_SIGNATURE,
						0, 0, 0, 0, // Token (4 Bytes)
						0, 0, // ID (2 Bytes)
						this.#CMD_NAVIGATION,
						...new TextEncoder().encode(url)
					)

					this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN)
					// this.#addID(packet, 0, this.#PROTOCOL_ID)

					this.#ws.send(packet.buffer)
				}
			}
			else {
				if (this.#log) console.log(`WebUI -> Navigation To [${url}]`)	
				this.#allowNavigation = true
				globalThis.location.replace(url)
			}
		}
	}

	#isExternalLink(url: string) {
		return new URL(url).host === globalThis.location.host
	}

	#closeWindowTimer() {
		setTimeout(function () {
			globalThis.close()
		}, 1000)
	}

	#toUint16(value: number): number {
		return value & 0xFFFF;
	}

	#callPromise(fn: string, value: any) {
		--this.#callPromiseID[0]
		const callId = this.#toUint16(this.#callPromiseID[0])
		if (this.#log) console.log(`WebUI -> Call [${fn}](${value}) (ID:${this.#callPromiseID[0]})`)

		// Protocol
		// 0: [SIGNATURE]
		// 1: [TOKEN]
		// 2: [ID]
		// 3: [CMD]
        // 4: [Element ID, Null, Len, Null, Data, Null]

		const packet = Uint8Array.of(
			this.#WEBUI_SIGNATURE,
			0, 0, 0, 0, // Token (4 Bytes)
			0, 0, // ID (2 Bytes)
			this.#CMD_CALL_FUNC,
			...new TextEncoder().encode(fn),
			0,
			...new TextEncoder().encode(String(value.length)),
			0,
			...typeof value === 'object' ? value : new TextEncoder().encode(value),
			0
		)

		this.#addToken(packet, this.#token, this.#PROTOCOL_TOKEN)
		this.#addID(packet, callId, this.#PROTOCOL_ID)

		return new Promise((resolve) => {
			this.#callPromiseResolve[callId] = resolve
			this.#ws.send(packet.buffer)
		})
	}

	// -- APIs --------------------------

	/**
	 * Call a backend binding from the frontend.
	 * @param bindingName - Backend bind name.
	 * @param payload - Payload to send to the binding, accept any JSON valid data
	 * (string, number, array, object, boolean, undefined).
	 * @return - Response of the backend callback as JSON compatible value.
	 * @example
	 * __Backend (C)__
	 * ```c
	 * webui_bind(window, "get_cwd", get_current_working_directory);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * const cwd = await webui.call("get_cwd");
	 * ```
	 * @example
	 * __Backend (C)__
	 * ```c
	 * webui_bind(window, "write_file", write_file);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * webui.call("write_file", "content to write")
	 *  .then(() => console.log("Success"))
	 *  .catch(() => console.error("Can't write the file"))
	 * ```
	 * @example
	 * __Backend (C)__
	 * ```c
	 * //complex_datas() returns the following JSON string
	 * //'{ "count": 1, "list": [ 1, 2, 3 ], "isGood": true }'
	 * webui_bind(window, "complex_datas", complex_datas);
	 * ```
	 * __Frontend (JS)__
	 * ```js
	 * type ComplexDatas = {
	 *  count: number;
	 *  list: number[];
	 *  isGood: boolean;
	 * }
	 *
	 * const { count, list, isGood } = await webui
	 *  .call<ComplexDatas>("complex_datas");
	 *
	 * //count = 1;
	 * //list = [ 1, 2, 3 ];
	 * //isGood = true;
	 * ```
	 */
	async call<
		Response extends JSONValue = string,
		Payload extends JSONValue = JSONValue
	>(bindingName: string, payload?: Payload): Promise<Response | void> {
		if (!bindingName)
			return Promise.reject(new SyntaxError('No binding name is provided'))

		if (!this.#wsStatus)
			return Promise.reject(new Error('WebSocket is not connected'))
		// Check binding list
		if (
			!this.#hasEvents &&
			!this.#bindList.includes(`${this.#winNum}/${bindingName}`)
		)
			return Promise.reject(
				new ReferenceError(`No binding was found for "${bindingName}"`)
			)

		// Get the binding response
		const response = (await this.#callPromise(
			bindingName,
			payload === undefined
				? ''
				: typeof payload === 'string'	// String()
				? payload
				: typeof payload === 'object'	// Uint8Array()
				? payload
				: JSON.stringify(payload)		// JSON String
		)) as string | void

		// Handle response type (void, string or JSON value)
		if (response === undefined) return

		try {
			return JSON.parse(response)
		} catch {
			//@ts-ignore string is a valid JSON value
			return response
		}
	}

	/**
	 * Active or deactivate webui debug logging.
	 * @param status - log status to set.
	 */
	setLogging(status: boolean) {
		if (status) {
			console.log('WebUI -> Log Enabled.')
			this.#log = true
		} else {
			console.log('WebUI -> Log Disabled.')
			this.#log = false
		}
	}

	/**
	 * Encode datas into base64 string.
	 * @param datas - string or JSON value.
	 */
	encode(datas: JSONValue): B64string {
		if (typeof datas === 'string') return btoa(datas)
		return btoa(JSON.stringify(datas))
	}

	/**
	 * Decode a base64 string into any JSON valid format
	 * (string, number, array, object, boolean).
	 * @param b64 - base64 string to decode.
	 */
	decode<T extends JSONValue>(b64: B64string): T {
		try {
			return JSON.parse(atob(b64))
		} catch {
			//@ts-ignore string a valid JSON value
			return atob(b64)
		}
	}
}

type webui = WebuiBridge
export default webui
export type { WebuiBridge }

// Wait for the html to be parsed
addEventListener('load', () => {
	document.body.addEventListener('contextmenu', (event) =>
		event.preventDefault()
	)
	addRefreshableEventListener(
		document.body,
		'input',
		'contextmenu',
		(event) => event.stopPropagation()
	)
})
