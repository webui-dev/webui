type B64string = string;

function webui_fn(fn: string, value: string): Promise<void>; 
function webui_log(status: boolean): void;
function webui_encode(str: string): B64string;
function webui_decode(str: B64string): string;