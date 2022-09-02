#include "Platform.h"
#include "../SharedConstants.h"

#import <Cocoa/Cocoa.h>

NSString* Str(LPCWSTR str) {
    return [[NSString alloc] initWithBytes:str length:wcslen(str)*sizeof(*str) encoding:NSUTF32LittleEndianStringEncoding];
}

void ShowMessageBox() {
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Yes"];
    [alert addButtonWithTitle:@"No"];
    [alert setMessageText:Str(ERROR_TITLE)];
    [alert setInformativeText:Str(ERROR_MESSAGE)];
    [alert setAlertStyle:NSWarningAlertStyle];

    // Force the alert to the top
    auto panel = static_cast<NSPanel*>([alert window]);
    panel.floatingPanel = YES;

    if ([alert runModal] == NSAlertFirstButtonReturn) {
        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:Str(ERROR_URL)]];
    }
}