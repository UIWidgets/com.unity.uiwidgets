#ifndef PLATFORM_IOS_FRAMEWORK_SOURCE_UIWIDGETSMESSAGEMANAGER_H_
#define PLATFORM_IOS_FRAMEWORK_SOURCE_UIWIDGETSMESSAGEMANAGER_H_
#import <UIKit/UIKit.h>

void UIWidgetsMethodMessage(NSString* channel, NSString* method, NSArray *args);

#endif