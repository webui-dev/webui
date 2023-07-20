'use-strict' //force strict mode for transpilled
import { AsyncFunction, addRefreshableEventListener } from './utils.js'

type B64string = string
type JSONValue =
	| string
	| number
	| boolean
	| { [x: string]: JSONValue | undefined }
	| JSONValue[]

class WebUiClient {
	//webui settings
	#port: number
	#winNum: number
	#log: boolean
	#bindList: unknown[] = []

	//Internals
	#ws: WebSocket
	#wsStatus = false
	#wsStatusOnce = false
	#closeReason = 0
	#closeValue: string
	#hasEvents = false
	#fnId = 1
	#fnPromiseResolve: (((data: string) => unknown) | undefined)[] = []

	//webui const
	#HEADER_SIGNATURE = 221
	#HEADER_JS = 254
	#HEADER_JS_QUICK = 253
	#HEADER_CLICK = 252
	#HEADER_SWITCH = 251
	#HEADER_CLOSE = 250
	#HEADER_CALL_FUNC = 249

	constructor({
		port,
		winNum,
		bindList,
		log = false,
	}: {
		port: number
		winNum: number
		bindList: unknown[]
		log?: boolean
	}) {
		// constructor arguments are injected by webui.c
		this.#port = port
		this.#winNum = winNum
		this.#bindList = bindList
		this.#log = log

		if ('webui' in globalThis) {
			throw new Error(
				'webui is already defined, only one instance is allowed'
			)
		}

		if (!('WebSocket' in window)) {
			alert('Sorry. WebSocket not supported by your Browser.')
			if (!this.#log) globalThis.close()
		}

		this.#start()

		// Handle navigation server side
		if ('navigation' in globalThis) {
			globalThis.navigation.addEventListener('navigate', (event) => {
				const url = new URL(event.destination.url)
				this.#sendEventNavigation(url.href)
			})
		} else {
			// Handle all link click to prevent natural navigation
			// Rebind listener if user inject new html
			addRefreshableEventListener(
				document.body,
				'a',
				'click',
				(event) => {
					event.preventDefault()
					const { href } = event.target as HTMLAnchorElement
					if (this.#isExternalLink(href)) {
						this.#close(this.#HEADER_SWITCH, href)
					} else {
						this.#sendEventNavigation(href)
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
					'WebUI failed to connect to the background application. Please try again.'
				)
				if (!this.#log) globalThis.close()
			}
		}, 1500)
	}

	#close(reason = 0, value = '') {
		if (reason === this.#HEADER_SWITCH) this.#sendEventNavigation(value)
		this.#wsStatus = false
		this.#closeReason = reason
		this.#closeValue = value
		this.#ws.close()
	}

	#freezeUi() {
		document.body.style.filter = 'contrast(1%)'
	}

	#start() {
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
			this.#fnId = 1
			if (this.#log) console.log('WebUI -> Connected')
			this.#clicksListener()
		}

		this.#ws.onerror = () => {
			if (this.#log) console.log('WebUI -> Connection Failed')
			this.#freezeUi()
		}

		this.#ws.onclose = (event) => {
			this.#wsStatus = false
			if (this.#closeReason === this.#HEADER_SWITCH) {
				if (this.#log) {
					console.log(
						`WebUI -> Connection lost -> Navigation to [${
							this.#closeValue
						}]`
					)
				}
				globalThis.location.replace(this.#closeValue)
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
			if (buffer8.length < 4) return
			if (buffer8[0] !== this.#HEADER_SIGNATURE) return
			const data8 =
				buffer8[buffer8.length - 1] === 0
					? buffer8.slice(3, -1)
					: buffer8.slice(3) // Null byte (0x00) can break eval()
			const data8utf8 = new TextDecoder().decode(data8)

			// Process Command
			switch (buffer8[1]) {
				case this.#HEADER_CALL_FUNC:
					{
						const callId = buffer8[2]
						if (this.#log) {
							console.log(`WebUI -> Func Reponse [${data8utf8}]`)
						}
						if (this.#fnPromiseResolve[callId]) {
							if (this.#log) {
								console.log(
									`WebUI -> Resolving reponse #${callId}...`
								)
							}
							this.#fnPromiseResolve[callId]?.(data8utf8)
							this.#fnPromiseResolve[callId] = undefined
						}
					}
					break
				case this.#HEADER_SWITCH:
					this.#close(this.#HEADER_SWITCH, data8utf8)
					break
				case this.#HEADER_CLOSE:
					globalThis.close()
					break
				case this.#HEADER_JS_QUICK:
				case this.#HEADER_JS:
					{
						const data8utf8sanitize = data8utf8.replace(
							/(?:\r\n|\r|\n)/g,
							'\n'
						)
						if (this.#log)
							console.log(`WebUI -> JS [${data8utf8sanitize}]`)

						// Get callback result
						let FunReturn = 'undefined'
						let FunError = false
						try {
							FunReturn = await AsyncFunction(data8utf8sanitize)()
						} catch (e) {
							FunError = true
							FunReturn = e.message
						}
						if (buffer8[1] === this.#HEADER_JS_QUICK) return
						if (FunReturn === undefined) {
							FunReturn = 'undefined'
						}

						// Logging
						if (this.#log && !FunError)
							console.log(`WebUI -> JS -> Return [${FunReturn}]`)
						if (this.#log && FunError)
							console.log(`WebUI -> JS -> Error [${FunReturn}]`)

						// Format ws response
						const Return8 = Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_JS,
							buffer8[2],
							FunError ? 0 : 1,
							...new TextEncoder().encode(FunReturn)
						)

						if (this.#wsStatus) this.#ws.send(Return8.buffer)
					}
					break
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
			const packet =
				elem !== ''
					? Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							0,
							...new TextEncoder().encode(elem)
					  )
					: Uint8Array.of(
							this.#HEADER_SIGNATURE,
							this.#HEADER_CLICK,
							0,
							0
					  )
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`WebUI -> Click [${elem}]`)
		}
	}

	#sendEventNavigation(url: string) {
		if (this.#hasEvents && this.#wsStatus && url !== '') {
			const packet = Uint8Array.of(
				this.#HEADER_SIGNATURE,
				this.#HEADER_SWITCH,
				...new TextEncoder().encode(url)
			)
			this.#ws.send(packet.buffer)
			if (this.#log) console.log(`WebUI -> Navigation [${url}]`)
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
	#fnPromise(fn: string, value: string) {
		if (this.#log) console.log(`WebUI -> Func [${fn}](${value})`)
		const callId = this.#fnId++

		const packet = Uint8Array.of(
			this.#HEADER_SIGNATURE,
			this.#HEADER_CALL_FUNC,
			callId,
			...new TextEncoder().encode(fn),
			0,
			...new TextEncoder().encode(value),
			0
		)

		return new Promise((resolve) => {
			this.#fnPromiseResolve[callId] = resolve
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
	 *  .then(() => console.log("file writed"))
	 *  .catch(() => console.error("can't write the file"))
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
			return Promise.reject(new SyntaxError('no binding name provided'))

		if (!this.#wsStatus)
			return Promise.reject(new Error('websocket is not connected'))
		//Check binding list
		if (
			!this.#hasEvents &&
			!this.#bindList.includes(`${this.#winNum}/${bindingName}`)
		)
			return Promise.reject(
				new ReferenceError(`no binding was found for "${bindingName}"`)
			)

		//Get the binding response
		const response = (await this.#fnPromise(
			bindingName,
			payload === undefined
				? ''
				: typeof payload === 'string'
				? payload
				: JSON.stringify(payload)
		)) as string | void

		//Handle response type (void, string or JSON value)
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

export type webui = InstanceType<typeof WebUiClient>

//wait for the html to be parsed
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
