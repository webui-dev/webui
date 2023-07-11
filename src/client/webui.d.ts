export type B64string = string;

/**
 * Call a backend function from the frontend.
 * @param fn - Backend bind name.
 * @param value - Payload to send.
 * @return - Response of the backend callback.
 * @example
 * ```c
 * //Backend (C)
 * webui_bind(window, "get_cwd", get_current_working_directory);
 * ```
 * ```js
 * //Frontend (JS)
 * const cwd = await webui_fn("get_cwd");
 * ```
 * @example
 * ```c
 * //Backend (C)
 * webui_bind(window, "write_file", write_file);
 * ```
 * ```js
 * //Frontend (JS)
 * webui_fn("write_file", "content to write")
 *  .then(() => console.log("file writed"))
 *  .catch(() => console.error("can't write the file"))
 * ```
 */
export function webui_fn(fn: string, value?: string): Promise<string | undefined>; 

/**
 * Active or deactivate webui debug logging.
 * @param status - log status to set.
 */
export function webui_log(status: boolean): void;

/**
 * Encode a string into base64.
 * @param str - string to encode.
 */
export function webui_encode(str: string): B64string;

/**
 * Decode a base64 string.
 * @param str - base64 string to decode.
 */
export function webui_decode(str: B64string): string;
