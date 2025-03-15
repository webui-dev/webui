/*
  WebUI Library
  https://webui.me
  https://github.com/webui-dev/webui
  Copyright (c) 2020-2025 Hassan Draga.
  Licensed under MIT License.
  All rights reserved.
  Canada.
*/

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

#define WEBUI_MAX_IDS (256) // Max windows, servers and threads
extern bool webui_interface_is_app_running(void);

void (*close_callback)(int index) = NULL;
void _webui_macos_wv_check_exit();

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate, WKNavigationDelegate>
{
    NSWindow *windows[WEBUI_MAX_IDS];
    WKWebView *webViews[WEBUI_MAX_IDS];
}
- (NSWindow *)windowAtIndex:(int)index;
- (void)setWindow:(NSWindow *)window atIndex:(int)index;
- (WKWebView *)webViewAtIndex:(int)index;
- (void)setWebView:(WKWebView *)webView atIndex:(int)index;
@end

@implementation AppDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        for (int i = 0; i < WEBUI_MAX_IDS; i++) {
            windows[i] = nil;
            webViews[i] = nil;
        }

        [NSTimer scheduledTimerWithTimeInterval:1.0
            target:self
            selector:@selector(_webui_macos_wv_timer)
            userInfo:nil
            repeats:YES];
    }
    return self;
}

- (void)_webui_macos_wv_timer {
    _webui_macos_wv_check_exit();
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\tapplicationDidFinishLaunching()\n");
    #endif
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
}

- (void)windowWillClose:(NSNotification *)notification {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\twindowWillClose()\n");
    #endif
    for (int i = 0; i < WEBUI_MAX_IDS; i++) {
        if (windows[i] == [notification object]) {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\twindowWillClose() -> Index [%d]\n", i);
            #endif
            if (close_callback) {
                close_callback(i);
            }
            windows[i] = nil;
            webViews[i] = nil;
            break;
        }
    }

    BOOL anyWindowOpen = NO;
    for (int i = 0; i < WEBUI_MAX_IDS; i++) {
        if (windows[i] != nil) {
            anyWindowOpen = YES;
            break;
        }
    }

    if (!anyWindowOpen) {
        [NSApp stop:nil];
    }
}

#pragma mark - Accessors

- (NSWindow *)windowAtIndex:(int)index {
    return windows[index];
}

- (void)setWindow:(NSWindow *)window atIndex:(int)index {
    windows[index] = window;
}

- (WKWebView *)webViewAtIndex:(int)index {
    return webViews[index];
}

- (void)setWebView:(WKWebView *)webView atIndex:(int)index {
    webViews[index] = webView;
}

#pragma mark - WKNavigationDelegate

- (void)webView:(WKWebView *)webView didFinishNavigation:(WKNavigation *)navigation {
    NSString *title = webView.title;
    for (int i = 0; i < WEBUI_MAX_IDS; i++) {
        if (webViews[i] == webView) {
            NSWindow *window = windows[i];
            [window setTitle:title];
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\tdidFinishNavigation([%d])\n", i);
            printf("[ObjC]\t\t\tdidFinishNavigation([%d]) -> Updated title [%s]\n", i, [title UTF8String]);
            #endif
            break;
        }
    }
}

@end

AppDelegate *delegate;

void _webui_macos_wv_start() {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_start()\n");
    #endif

    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        // Ensure the event loop keeps running
        [app finishLaunching];

        // Run the application event loop
        [app run];

        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_start() -> Exit.\n");
        #endif
    }
}

bool _webui_macos_wv_new(int index, bool frameless) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_new([%d], [%d])\n", index, frameless);
    #endif

    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_new([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    if (!delegate) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_new([%d]) -> Creating NSApplication and AppDelegate\n", index);
        #endif
        NSApplication *app = [NSApplication sharedApplication];
        delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
    }

    NSRect frame = NSMakeRect(0, 0, 800, 600);

    // Set window style
    NSWindowStyleMask windowStyle = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
    if (frameless) {
        windowStyle = NSWindowStyleMaskBorderless;
    }

    NSWindow *window = [[NSWindow alloc] initWithContentRect:frame
                                                   styleMask:windowStyle
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    if (!window) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_new([%d]) -> Failed to create NSWindow\n", index);
        #endif
        return false;
    }
    if (!frameless) {
        [window setTitle:@"Loading..."];
    }
    [window setDelegate:delegate];

    WKWebView *webView = [[WKWebView alloc] initWithFrame:[[window contentView] bounds]];
    if (!webView) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_new([%d]) -> Failed to create WKWebView\n", index);
        #endif
        return false;
    }
    [webView setNavigationDelegate:delegate];
    [webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [window.contentView addSubview:webView];

    [delegate setWindow:window atIndex:index];
    [delegate setWebView:webView atIndex:index];

    return true;
}

void _webui_macos_wv_new_thread_safe(int index, bool frameless) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_new_thread_safe([%d])\n", index);
    #endif
    dispatch_async(dispatch_get_main_queue(), ^{
        _webui_macos_wv_new(index, frameless);
    });
}

bool _webui_macos_wv_show(int index, const char* urlString, int x, int y, int width, int height) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d])\n", index);
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> url: [%s]\n", index, urlString);
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> x: [%d]\n", index, x);
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> y: [%d]\n", index, y);
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> width: [%d]\n", index, width);
    printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> height: [%d]\n", index, height);
    #endif
    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    __block bool success = false;
    dispatch_async(dispatch_get_main_queue(), ^{
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> dispatch_async\n", index);
        #endif
        NSWindow *window = [delegate windowAtIndex:index];
        WKWebView *webView = [delegate webViewAtIndex:index];

        if (window && webView) {
            // Set window position and size
            NSRect frame = [window frame];
            frame.origin.x = x;
            frame.origin.y = y;
            frame.size.width = width;
            frame.size.height = height;
            [window setFrame:frame display:YES];

            // Load URL
            NSString *nsUrlString = [NSString stringWithUTF8String:urlString];
            if (nsUrlString) {
                NSURL *url = [NSURL URLWithString:nsUrlString];
                if (url) {
                    NSURLRequest *request = [NSURLRequest requestWithURL:url];
                    [webView loadRequest:request];
                }
            }

            [window makeKeyAndOrderFront:nil];
            success = true;
        }
        else {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\t_webui_macos_wv_show([%d]) -> Window or WebView not found.\n", index);
            #endif
        }
    });
    return success;
}

void _webui_macos_wv_process() {
    #ifdef WEBUI_LOG
    // printf("[ObjC]\t\t\t_webui_macos_wv_process()\n");
    #endif
    NSApplication *app = [NSApplication sharedApplication];
    NSEvent *event;

    // Process all pending events
    while ((event = [app nextEventMatchingMask:NSEventMaskAny
                        untilDate:[NSDate distantPast]
                        inMode:NSDefaultRunLoopMode
                        dequeue:YES])) {
        [app sendEvent:event];
    }
}

void _webui_macos_wv_stop() {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_stop()\n");
    #endif
    dispatch_async(dispatch_get_main_queue(), ^{
        NSApplication *app = [NSApplication sharedApplication];
        [app stop:nil];

        // Dummy event to immediately break `[app run]` in `_webui_macos_wv_start`
        NSEvent *event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                            location:NSMakePoint(0, 0)
                            modifierFlags:0
                            timestamp:0
                            windowNumber:0
                            context:nil
                            subtype:0
                            data1:0
                            data2:0];
        [app postEvent:event atStart:YES];
    });
}

void _webui_macos_wv_check_exit() {
    if (!webui_interface_is_app_running()) {
        _webui_macos_wv_stop();
    }
}

bool _webui_macos_wv_close(int index) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_close([%d])\n", index);
    #endif
    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_close([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    __block bool success = false;
    dispatch_async(dispatch_get_main_queue(), ^{
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_close([%d]) -> dispatch_async\n", index);
        #endif
        NSWindow *window = [delegate windowAtIndex:index];
        if (window) {
            [window orderOut:nil];
            success = true;
        }
        else {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\t_webui_macos_wv_close([%d]) -> Window not found.\n", index);
            #endif
        }
    });
    return success;
}

bool _webui_macos_wv_set_position(int index, int x, int y) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_set_position([%d])\n", index);
    printf("[ObjC]\t\t\t_webui_macos_wv_set_position([%d]) -> x: [%d]\n", index, x);
    printf("[ObjC]\t\t\t_webui_macos_wv_set_position([%d]) -> y: [%d]\n", index, y);
    #endif
    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_set_position([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    __block bool success = false;
    dispatch_async(dispatch_get_main_queue(), ^{
        NSWindow *window = [delegate windowAtIndex:index];
        if (window) {
            NSRect frame = [window frame];
            frame.origin.x = x;
            frame.origin.y = y;
            [window setFrame:frame display:YES];
            success = true;
        }
        else {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\t_webui_macos_wv_set_position([%d]) -> Window not found.\n", index);
            #endif
        }
    });
    return success;
}

bool _webui_macos_wv_set_size(int index, int width, int height) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_set_size([%d])\n", index);
    printf("[ObjC]\t\t\t_webui_macos_wv_set_size([%d]) -> width: [%d]\n", index, width);
    printf("[ObjC]\t\t\t_webui_macos_wv_set_size([%d]) -> height: [%d]\n", index, height);
    #endif
    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_set_size([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    __block bool success = false;
    dispatch_async(dispatch_get_main_queue(), ^{
        NSWindow *window = [delegate windowAtIndex:index];
        if (window) {
            NSRect frame = [window frame];
            frame.size.width = width;
            frame.size.height = height;
            [window setFrame:frame display:YES];
            success = true;
        }
        else {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\t_webui_macos_wv_set_size([%d]) -> Window not found.\n", index);
            #endif
        }
    });
    return success;
}

bool _webui_macos_wv_navigate(int index, const char* urlString) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_navigate([%d])\n", index);
    printf("[ObjC]\t\t\t_webui_macos_wv_navigate([%d]) -> url: [%s]\n", index, urlString);
    #endif
    if (index < 0 || index >= WEBUI_MAX_IDS) {
        #ifdef WEBUI_LOG
        printf("[ObjC]\t\t\t_webui_macos_wv_navigate([%d]) -> Invalid index\n", index);
        #endif
        return false;
    }

    __block bool success = false;
    dispatch_async(dispatch_get_main_queue(), ^{
        WKWebView *webView = [delegate webViewAtIndex:index];
        if (webView) {
            NSString *nsUrlString = [NSString stringWithUTF8String:urlString];
            if (nsUrlString) {
                NSURL *url = [NSURL URLWithString:nsUrlString];
                if (url) {
                    NSURLRequest *request = [NSURLRequest requestWithURL:url];
                    [webView loadRequest:request];
                    success = true;
                }
            }
        }
        else {
            #ifdef WEBUI_LOG
            printf("[ObjC]\t\t\t_webui_macos_wv_navigate([%d]) -> WebView not found.\n", index);
            #endif
        }
    });
    return success;
}

void _webui_macos_wv_set_close_cb(void (*cb)(int index)) {
    #ifdef WEBUI_LOG
    printf("[ObjC]\t\t\t_webui_macos_wv_set_close_cb()\n");
    #endif
    close_callback = cb;
}
