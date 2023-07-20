/**
 * Allows you to automatically bind an event listener to newly added
 * elements that match a specific selector within a given root element.
 * Track dom update to rebind event listeners automatically.
 *
 * @param {HTMLElement} root - The root element to observe for changes.
 * @param {string} targetSelector - Query selector matching elements that you want to bind the event listener to.
 * @param {K} type - Type of event listener to bind (same as for addEventListener).
 * @param listener - Event listener to bind (same as for addEventListener).
 * @param {boolean | AddEventListenerOptions} [options] - Event listener options (same as for addEventListener).
 * @returns the used observer to allow disconnect.
 */
export function addRefreshableEventListener<
	K extends keyof HTMLElementEventMap
>(
	root: HTMLElement,
	targetSelector: string,
	type: K,
	listener: (this: HTMLElement, ev: HTMLElementEventMap[K]) => unknown,
	options?: boolean | AddEventListenerOptions
) {
	function rebindListener(mutations: MutationRecord[]) {
		for (const mutation of mutations) {
			for (const node of mutation.addedNodes) {
				if (!(node instanceof HTMLElement)) return // Target only html elements
				if (node.matches(targetSelector)) {
					// Bind event on added nodes
					node.addEventListener<K>(type, listener, options)
				}
				for (const child of node.querySelectorAll(targetSelector)) {
					if (!(child instanceof HTMLElement)) continue //Target only html elements
					child.addEventListener<K>(type, listener, options)
				}
			}
		}
	}

	const observer = new MutationObserver(rebindListener) //Set mutation observer callback
	observer.observe(root, { subtree: true, childList: true }) // Observe root element and all his children
	return observer // Allow user to stop observer for performance issues
}

/**
 * Async function constructor
 */
export const AsyncFunction = async function () {}.constructor
