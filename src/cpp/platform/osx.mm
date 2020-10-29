// Most of code in this file are copied from
// https://github.com/stream-labs/node-window-rendering/blob/streamlabs/src/window-osx.mm
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

NSOperatingSystemVersion OSversion = [NSProcessInfo processInfo].operatingSystemVersion;

void *createDisplayWindow(void *parentHandle) {
    NSView *viewParent = *static_cast<NSView **>(parentHandle);
    NSWindow *winParent = [viewParent window];
    NSView *view = [[NSView alloc] initWithFrame:NSMakeRect(1, 1, 1, 1)];

    if (OSversion.majorVersion == 10 && OSversion.minorVersion < 14) {
        // Less performant but solves flickering issue on macOS High Sierra and lower
        NSRect content_rect = NSMakeRect(1, 1, 1, 1);
        NSWindow *window = [
                [NSWindow alloc]
                initWithContentRect:content_rect
                          styleMask:NSWindowStyleMaskBorderless
                            backing:NSBackingStoreBuffered
                              defer:NO
        ];
        [winParent addChildWindow:window ordered:NSWindowAbove];
        window.ignoresMouseEvents = true;
        [window.contentView addSubview:view];
    } else {
        [winParent.contentView addSubview:view];
    }
    return static_cast<void *>(view);
}

void destroyWindow(void *windowHandle) {
    NSView *view = static_cast<NSView *>(windowHandle);
    [view removeFromSuperview];
    CFRelease(view);
    if (OSversion.majorVersion == 10 && OSversion.minorVersion < 14) {
        NSWindow *window = [view window];
        if (window) {
            [window close];
            CFRelease(window);
        }
    }
}

void moveWindow(void *windowHandle, int x, int y, int width, int height) {
    NSView *view = static_cast<NSView *>(windowHandle);
    NSWindow *window = [view window];

    if (OSversion.majorVersion == 10 && OSversion.minorVersion < 14) {
        NSWindow *parent = [window parentWindow];
        NSRect parentFrame = [parent frame];

        NSRect frame = [window frame];
        frame.size = NSMakeSize(width, height);
        frame.origin.x = parentFrame.origin.x + x;
        frame.origin.y = parentFrame.origin.y + y;

        [view setFrameSize:NSMakeSize(width, height)];
        [window setFrame:frame display:true animate:false];
    } else {
        [view setFrameSize:NSMakeSize(width, height)];
        [view setFrameOrigin:NSMakePoint(x, y)];
        view.needsDisplay = YES;
    }
}