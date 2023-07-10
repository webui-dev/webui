var _webui_log = _webui_log ?? false //If webui.c define _webui_log then use it, instead set it to false

class WebUiClient {
	//webui settings
	//@ts-ignore injected by webui.c
	#port: number = _webui_port
	//@ts-ignore injected by webui.c
	#winNum: number = _webui_win_num

	#log = _webui_log
	#ws: WebSocket
	#wsStatus = false
	#wsStatusOnce = false
	#closeReason = 0
	#closeValue
	#hasEvents = false
	#fnId = new Uint8Array(1)
	#fnPromiseResolve: ((data: string) => unknown)[] = []

	#bindList: unknown[] = []

	//webui const
	#HEADER_SIGNATURE = 221
	#HEADER_JS = 254
	#HEADER_JS_QUICK = 253
	#HEADER_CLICK = 252
	#HEADER_SWITCH = 251
	#HEADER_CLOSE = 250
	#HEADER_CALL_FUNC = 249

    constructor() {
        if ('navigation' in globalThis) {
            globalThis.navigation.addEventListener('navigate', (event) => {
                const url = new URL(event.destination.url)
                this.#sendEventNavigation(url.href)
            })
        } else {
            console.error('navigation API not supported, some features may be missing')
        }

        // -- DOM ---------------------------
        document.addEventListener('keydown', (event) => {
            // Disable F5
            if (this.#log) return
            if (event.key === 'F5') event.preventDefault()
        })
        document.addEventListener('click', (event) => {
            const anchor = (event.target as HTMLElement).closest('a')
            if (anchor) {
                const link = anchor.href
                if (this.#isExternalLink(link)) {
                    event.preventDefault()
                    //TODO fic webui.close declaration
                    this.#close(this.#HEADER_SWITCH) //, link)
                }
            }
        })

        // -- Links -------------------------
        onbeforeunload = () => {
            this.#close()
        }
        setTimeout(() => {
            if (!this.#wsStatusOnce) {
                this.#freezeUi()
                alert(
                    'WebUI failed to connect to the background application. Please try again.'
                )
                if (!webui.log) globalThis.close()
            }
        }, 1500)

        addEventListener('load', () => {
            this.#start()
            document.body.addEventListener('contextmenu', function (event) {
                event.preventDefault()
            })
        })
    }

	#close(reason = 0, value = 0) {
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
		if ('WebSocket' in window) {
			if (this.#bindList.includes(this.#winNum + '/'))
				this.#hasEvents = true
			this.#ws = new WebSocket(
				'ws://localhost:' + this.#port + '/_webui_ws_connect'
			)
			this.#ws.binaryType = 'arraybuffer'
			this.#ws.onopen = () => {
				this.#ws.binaryType = 'arraybuffer'
				this.#wsStatus = true
				this.#wsStatusOnce = true
				this.#fnId[0] = 1
				if (this.#log) console.log('WebUI -> Connected')
				this.#clicksListener()
			}
			this.#ws.onerror = () => {
				if (this.#log) console.log('WebUI -> Connection Failed')
				this.#freezeUi()
			}
			this.#ws.onclose = (evt) => {
				this.#wsStatus = false
				if (this.#closeReason === this.#HEADER_SWITCH) {
					if (this.#log)
						console.log(
							'WebUI -> Connection lost -> Navigation to [' +
								this.#closeValue +
								']'
						)
					globalThis.location.replace(this.#closeValue)
				} else {
					if (this.#log) {
						console.log(
							'WebUI -> Connection lost (' + evt.code + ')'
						)
						this.#freezeUi()
					} else this.#closeWindowTimer()
				}
			}
			this.#ws.onmessage = (evt) => {
				const buffer8 = new Uint8Array(evt.data)
				if (buffer8.length < 4) return
				if (buffer8[0] !== this.#HEADER_SIGNATURE) return
				let len = buffer8.length - 3
				if (buffer8[buffer8.length - 1] === 0) len-- // Null byte (0x00) can break eval()
				const data8 = new Uint8Array(len)
				for (let i = 0; i < len; i++) data8[i] = buffer8[i + 3]
				let data8utf8 = new TextDecoder().decode(data8)
				// Process Command
				if (buffer8[1] === this.#HEADER_CALL_FUNC) {
					const callId = buffer8[2]
					if (this.#log)
						console.log('WebUI -> Func Reponse [' + data8utf8 + ']')
					if (this.#fnPromiseResolve[callId]) {
						if (this.#log)
							console.log(
								'WebUI -> Resolving reponse #' + callId + '...'
							)
						this.#fnPromiseResolve[callId](data8utf8)
						//TODO fix null assignation (determine utility)
						// this.#fnPromiseResolve[callId] = null
					}
				} else if (buffer8[1] === this.#HEADER_SWITCH) {
					this.#close(this.#HEADER_SWITCH, data8utf8)
				} else if (buffer8[1] === this.#HEADER_CLOSE) {
					globalThis.close()
				} else if (
					buffer8[1] === this.#HEADER_JS_QUICK ||
					buffer8[1] === this.#HEADER_JS
				) {
					data8utf8 = data8utf8.replace(/(?:\r\n|\r|\n)/g, '\n')
					if (this.#log)
						console.log('WebUI -> JS [' + data8utf8 + ']')
					let FunReturn = 'undefined'
					let FunError = false
					try {
						FunReturn = eval('(() => {' + data8utf8 + '})()')
					} catch (e) {
						FunError = true
						FunReturn = e.message
					}
					if (buffer8[1] === this.#HEADER_JS_QUICK) return
					if (
						typeof FunReturn === 'undefined' ||
						FunReturn === undefined
					)
						FunReturn = 'undefined'
					if (this.#log && !FunError)
						console.log('WebUI -> JS -> Return [' + FunReturn + ']')
					if (this.#log && FunError)
						console.log('WebUI -> JS -> Error [' + FunReturn + ']')
					const FunReturn8 = new TextEncoder().encode(FunReturn)
					const Return8 = new Uint8Array(4 + FunReturn8.length)
					Return8[0] = this.#HEADER_SIGNATURE
					Return8[1] = this.#HEADER_JS
					Return8[2] = buffer8[2]
					if (FunError) Return8[3] = 0
					else Return8[3] = 1
					let p = -1
					for (let i = 4; i < FunReturn8.length + 4; i++)
						Return8[i] = FunReturn8[++p]
					if (this.#wsStatus) this.#ws.send(Return8.buffer)
				}
			}
		} else {
			alert('Sorry. WebSocket not supported by your Browser.')
			if (!this.#log) globalThis.close()
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
			let packet
			if (elem !== '') {
				const elem8 = new TextEncoder().encode(elem)
				packet = new Uint8Array(3 + elem8.length)
				packet[0] = this.#HEADER_SIGNATURE
				packet[1] = this.#HEADER_CLICK
				packet[2] = 0
				let p = -1
				for (let i = 3; i < elem8.length + 3; i++)
					packet[i] = elem8[++p]
			} else {
				packet = new Uint8Array(4)
				packet[0] = this.#HEADER_SIGNATURE
				packet[1] = this.#HEADER_CLICK
				packet[2] = 0
				packet[3] = 0
			}
			this.#ws.send(packet.buffer)
			if (this.#log) console.log('WebUI -> Click [' + elem + ']')
		}
	}
	#sendEventNavigation(url: string) {
		if (this.#hasEvents && this.#wsStatus && url !== '') {
			const url8 = new TextEncoder().encode(url)
			const packet = new Uint8Array(3 + url8.length)
			packet[0] = this.#HEADER_SIGNATURE
			packet[1] = this.#HEADER_SWITCH
			packet[2] = 0
			let p = -1
			for (let i = 3; i < url8.length + 3; i++) packet[i] = url8[++p]
			this.#ws.send(packet.buffer)
			if (this.#log) console.log('WebUI -> Navigation [' + url + ']')
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
		if (this.#log) console.log('WebUI -> Func [' + fn + '](' + value + ')')
		const fn8 = new TextEncoder().encode(fn)
		const value8 = new TextEncoder().encode(value)
		const packet = new Uint8Array(3 + fn8.length + 1 + value8.length)
		const callId = this.#fnId[0]++
		packet[0] = this.#HEADER_SIGNATURE
		packet[1] = this.#HEADER_CALL_FUNC
		packet[2] = callId
		let p = 3
		for (let i = 0; i < fn8.length; i++) {
			packet[p] = fn8[i]
			p++
		}
		packet[p] = 0
		p++
		if (value8.length > 0) {
			for (let i = 0; i < value8.length; i++) {
				packet[p] = value8[i]
				p++
			}
		} else {
			packet[p] = 0
		}
		return new Promise((resolve) => {
			this.#fnPromiseResolve[callId] = resolve
			this.#ws.send(packet.buffer)
		})
	}

	// -- APIs --------------------------
	fn(fn: string, value: string) {
		if (!fn || !this.#wsStatus) return Promise.resolve()
		if (typeof value === 'undefined') value = ''
		if (
			!this.#hasEvents &&
			!this.#bindList.includes(this.#winNum + '/' + fn)
		)
			return Promise.resolve()
		return this.#fnPromise(fn, value)
	}
	log(status: boolean) {
		if (status) {
			console.log('WebUI -> Log Enabled.')
			this.#log = true
		} else {
			console.log('WebUI -> Log Disabled.')
			this.#log = false
		}
	}
	encode(str: string) {
		return btoa(str)
	}
	decode(str: string) {
		return atob(str)
	}
}

const webui = new WebUiClient()
//@ts-ignore globally expose webui APIs
globalThis.webui = webui

const inputs = document.getElementsByTagName('input')

for (let i = 0; i < inputs.length; i++) {
	inputs[i].addEventListener('contextmenu', function (event) {
		event.stopPropagation()
	})
}
