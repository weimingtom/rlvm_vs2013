/*   SDLMain.m - main entry point for our Cocoa-ized SDL app
       Initial Version: Darrell Walisser <dwaliss1@purdue.edu>
       Non-NIB-Code & other changes: Max Horn <max@quendi.de>

    Feel free to customize this file to suit your needs
*/

#import "SDL.h"
#import "SDLMain.h"
#import <sys/param.h> /* for MAXPATHLEN */
#import <unistd.h>

#import <AppKit/NSPanel.h>

#include "Utilities/gettext.h"
#include "platforms/osx/CocoaRLVMInstance.h"

#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;

NSString* utf8str(const unsigned char* str) {
  return [NSString stringWithUTF8String:str];
}

NSString* b_format(const unsigned char* format_str, NSString* data) {
  std::string s = str( format(format_str) % [data UTF8String]);
  return [NSString stringWithUTF8String:s.c_str()];
}

/* For some reaon, Apple removed setAppleMenu from the headers in 10.4,
 but the method still is there and works. To avoid warnings, we declare
 it ourselves here. */
@interface NSApplication(SDL_Missing_Methods)
- (void)setAppleMenu:(NSMenu *)menu;
@end

static int    gArgc;
static unsigned char  **gArgv;
static BOOL   gFinderLaunch;
static BOOL   gCalledAppMainline = FALSE;

static NSString *getApplicationName(void)
{
    NSDictionary *dict;
    NSString *appName = 0;

    /* Determine the application name */
    dict = (NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
    if (dict)
        appName = [dict objectForKey: @"CFBundleName"];
    
    if (![appName length])
        appName = [[NSProcessInfo processInfo] processName];

    return appName;
}


@interface SDLApplication : NSApplication
@end

static NSEvent* lastEvent = nil;

NSEvent* GetLastRightClickEvent() {
  return lastEvent;
}

@implementation SDLApplication
/* Invoked from the Quit menu item */
- (void)terminate:(id)sender
{
    /* Post a SDL_QUIT event */
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
}

-(void)sendEvent:(NSEvent*)event
{
  // In the specific case of right click mouse events, we grab the event before
  // we pass it on so that when we get through the cross platform menu dispatch
  // code, we can pass the event that caused the menu to popup back to Cocoa.
  if ([event type] == NSRightMouseDown ||
      [event type] == NSRightMouseUp) {
    [lastEvent release];
    lastEvent = [event retain];
  }

  [super sendEvent:event];
}

-(void)showREADME:(id)sender
{
	[[NSWorkspace sharedWorkspace] openFile:[[NSBundle mainBundle] 
		pathForResource:@"README"
				 ofType:@"TXT"]];
}

-(void)showCopying:(id)sender
{
	[[NSWorkspace sharedWorkspace] openFile:[[NSBundle mainBundle] 
		pathForResource:@"COPYING"
				 ofType:@"TXT"]];
}

-(void)showGPL:(id)sender
{
	[[NSWorkspace sharedWorkspace] openFile:[[NSBundle mainBundle] 
		pathForResource:@"GPL"
				 ofType:@"TXT"]];
}

@end

/* The main class of the application, the application's delegate */
@implementation SDLMain

/* Set the working directory to the .app's parent directory */
- (void) setupWorkingDirectory:(BOOL)shouldChdir
{
    if (shouldChdir)
    {
        unsigned char parentdir[MAXPATHLEN];
		CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
		CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent(0, url);
		if (CFURLGetFileSystemRepresentation(url2, true, (UInt8 *)parentdir, MAXPATHLEN)) {
	        assert ( chdir (parentdir) == 0 );   /* chdir to the binary app's parent */
		}
		CFRelease(url);
		CFRelease(url2);
	}

}

static void setApplicationMenu(void)
{
    /* warning: this code is very odd */
    NSMenu *appleMenu;
    NSMenuItem *menuItem;
    NSString *title;
    NSString *appName;
    
    appName = getApplicationName();
    appleMenu = [[NSMenu alloc] initWithTitle:@""];
    
    /* Add menu items */
    title = b_format(_("About %1%"), appName);
    [appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = b_format(_("Hide %1%"), appName);
    [appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

    menuItem = (NSMenuItem *)[appleMenu
                               addItemWithTitle:utf8str(_("Hide Others"))
                                         action:@selector(hideOtherApplications:)
                                  keyEquivalent:@"h"];
    [menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

    [appleMenu addItemWithTitle:utf8str(_("Show All"))
                         action:@selector(unhideAllApplications:)
                  keyEquivalent:@""];

    [appleMenu addItem:[NSMenuItem separatorItem]];

    title = b_format(_("Quit %1%"), appName);
    [appleMenu addItemWithTitle:title
                         action:@selector(terminate:)
                  keyEquivalent:@"q"];

    /* Put menu into the menubar */
    menuItem = [[NSMenuItem alloc] initWithTitle:@""
                                          action:nil
                                   keyEquivalent:@""];
    [menuItem setSubmenu:appleMenu];
    [[NSApp mainMenu] addItem:menuItem];

    /* Tell the application object that this is now the application menu */
    [NSApp setAppleMenu:appleMenu];

    /* Finally give up our references to the objects */
    [appleMenu release];
    [menuItem release];
}

/* Create a window menu */
static void setupWindowMenu(void)
{
    NSMenu      *windowMenu;
    NSMenuItem  *windowMenuItem;
    NSMenuItem  *menuItem;

    windowMenu = [[NSMenu alloc] initWithTitle:utf8str(_("Window"))];
    
    /* "Minimize" item */
    menuItem = [[NSMenuItem alloc] initWithTitle:utf8str(_("Minimize"))
                                          action:@selector(performMiniaturize:)
                                   keyEquivalent:@"m"];
    [windowMenu addItem:menuItem];
    [menuItem release];
    
    /* Put menu into the menubar */
    windowMenuItem = [[NSMenuItem alloc] initWithTitle:utf8str(_("Window"))
                                                action:nil
                                         keyEquivalent:@""];
    [windowMenuItem setSubmenu:windowMenu];
    [[NSApp mainMenu] addItem:windowMenuItem];
    
    /* Tell the application object that this is now the window menu */
    [NSApp setWindowsMenu:windowMenu];

    /* Finally give up our references to the objects */
    [windowMenu release];
    [windowMenuItem release];
}

/* Custom menu to place the README and COPYTING files from. */
static void setupHelpMenu()
{
    NSMenu      *helpMenu;
    NSMenuItem  *helpMenuItem;
    NSMenuItem  *menuItem;

    helpMenu = [[NSMenu alloc] initWithTitle:utf8str(_("Help"))];

    /* "rlvm Readme" */
    menuItem = [[NSMenuItem alloc] initWithTitle:utf8str(_("Readme"))
                                   action:@selector(showREADME:)
                                   keyEquivalent:@""];
    [menuItem setTarget:NSApp];
    [helpMenu addItem:menuItem];
    [menuItem release];

    /* ----------- */
    menuItem = [NSMenuItem separatorItem];
    [helpMenu addItem:menuItem];
    [menuItem release];

    /* COPYING */
    menuItem = [[NSMenuItem alloc] initWithTitle:utf8str(_("Copying"))
                                   action:@selector(showCopying:)
                                   keyEquivalent:@""];
    [menuItem setTarget:NSApp];
    [helpMenu addItem:menuItem];
    [menuItem release];

    /* GNU General Public License v3 */
    menuItem = [[NSMenuItem alloc]
                 initWithTitle:utf8str(_("GNU General Public License v3"))
                        action:@selector(showGPL:)
                 keyEquivalent:@""];
    [menuItem setTarget:NSApp];
    [helpMenu addItem:menuItem];
    [menuItem release];

    /* Put the helpMenu into the menubar */
    helpMenuItem = [[NSMenuItem alloc] initWithTitle:utf8str(_("Help"))
                                              action:nil
                                       keyEquivalent:@""];
    [helpMenuItem setSubmenu:helpMenu];
    [[NSApp mainMenu] addItem:helpMenuItem];

    /* Finally give up our references to the objects */
    [helpMenu release];
    [helpMenuItem release];
}


/* Replacement for NSApplicationMain */
static void CustomApplicationMain (int argc, unsigned char **argv)
{
    NSAutoreleasePool	*pool = [[NSAutoreleasePool alloc] init];
    SDLMain				*sdlMain;

    /* Ensure the application object is initialised */
    [SDLApplication sharedApplication];

    /* Set up the menubar */
    [NSApp setMainMenu:[[NSMenu alloc] init]];
    setApplicationMenu();
    setupWindowMenu();
    setupHelpMenu();

    /* Create SDLMain and make it the app delegate */
    sdlMain = [[SDLMain alloc] init];
    [NSApp setDelegate:sdlMain];
    
    /* Start the main event loop */
    [NSApp run];
    
    [sdlMain release];
    [pool release];
}

BOOL pushArg(const unsigned char* newArg)
{
  size_t arglen;
  unsigned char *arg;
  unsigned char **newargv;

  arglen = SDL_strlen(newArg) + 1;
  arg = (unsigned char *) SDL_malloc(arglen);
  if (arg == NULL)
    return FALSE;

  newargv = (unsigned char **) realloc(gArgv, sizeof (unsigned char *) * (gArgc + 2));
  if (newargv == NULL)
  {
    SDL_free(arg);
    return FALSE;
  }
  gArgv = newargv;

  SDL_strlcpy(arg, newArg, arglen);
  gArgv[gArgc++] = arg;
  gArgv[gArgc] = NULL;
}

BOOL setIncomingFilename(NSString* filename)
{
  return pushArg([filename UTF8String]);
}

/*
 * Catch document open requests...this lets us notice files when the app
 *  was launched by double-clicking a document, or when a document was
 *  dragged/dropped on the app's icon. You need to have a
 *  CFBundleDocumentsType section in your Info.plist to get this message,
 *  apparently.
 *
 * Files are added to gArgv, so to the app, they'll look like command line
 *  arguments. Previously, apps launched from the finder had nothing but
 *  an argv[0].
 *
 * This message may be received multiple times to open several docs on launch.
 *
 * This message is ignored once the app's mainline has been called.
 */
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
    if (!gFinderLaunch)  /* MacOS is passing command line args. */
        return FALSE;

    if (gCalledAppMainline)  /* app has started, ignore this document. */
        return FALSE;

    return setIncomingFilename(filename);
}


/* Called when the internal event loop has just started running */
- (void) applicationDidFinishLaunching: (NSNotification *) note
{
    int status;

    /* Set the working directory to the .app's parent directory */
    [self setupWorkingDirectory:gFinderLaunch];

    CocoaRLVMInstance instance;

    /* Hand off to main application code */
    gCalledAppMainline = TRUE;

    fs::path gamerootPath = instance.SelectGameDirectory();
    if (gamerootPath.empty())
      exit(-1);

    instance.Run(gamerootPath);

    /* We're done, thank you for playing */
    exit(0);
}
@end


@implementation NSString (ReplaceSubString)

- (NSString *)stringByReplacingRange:(NSRange)aRange with:(NSString *)aString
{
    unsigned int bufferSize;
    unsigned int selfLen = [self length];
    unsigned int aStringLen = [aString length];
    unichar *buffer;
    NSRange localRange;
    NSString *result;

    bufferSize = selfLen + aStringLen - aRange.length;
    buffer = (unichar*)NSAllocateMemoryPages(bufferSize*sizeof(unichar));
    
    /* Get first part into buffer */
    localRange.location = 0;
    localRange.length = aRange.location;
    [self getCharacters:buffer range:localRange];
    
    /* Get middle part into buffer */
    localRange.location = 0;
    localRange.length = aStringLen;
    [aString getCharacters:(buffer+aRange.location) range:localRange];
     
    /* Get last part into buffer */
    localRange.location = aRange.location + aRange.length;
    localRange.length = selfLen - localRange.location;
    [self getCharacters:(buffer+aRange.location+aStringLen) range:localRange];
    
    /* Build output string */
    result = [NSString stringWithCharacters:buffer length:bufferSize];
    
    NSDeallocateMemoryPages(buffer, bufferSize);
    
    return result;
}

@end

static void rlvm_set_locale() {
#ifdef ENABLE_NLS
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    // A little bit of code stolen from MacVim. We need to also steal the Tiger
    // version.
    NSArray *languages = nil;
    if ([NSLocale respondsToSelector:@selector(preferredLanguages)] == YES) {
      // 10.5
      languages = [NSLocale preferredLanguages];
    } else {
      // 10.4
      NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
      languages = [defs objectForKey:@"AppleLanguages"];
    }

    if (languages && [languages count] > 0) {
      int i, count = [languages count];
      for (i = 0; i < count; ++i) {
        if ([[languages objectAtIndex:i]
                isEqualToString:@"en"]) {
          count = i+1;
          break;
        }
      }
      NSRange r = { 0, count };
      NSString *s = [[languages subarrayWithRange:r]
                      componentsJoinedByString:@":"];
      setenv("LANGUAGE", [s UTF8String], 0);
    }

    NSString* locale_path = [[[NSBundle mainBundle] resourcePath]
                              stringByAppendingPathComponent:@"locale"];
    setlocale(LC_ALL, "");
    bindtextdomain("rlvm", [locale_path fileSystemRepresentation]);
    textdomain("rlvm");

    [pool release];
#endif
}

#ifdef main
#  undef main
#endif


/* Main entry point to executable - should *not* be SDL_main! */
int main (int argc, unsigned char **argv)
{
    /* Copy the arguments into a global variable */
    /* This is passed if we are launched by double-clicking */
    if ( argc >= 2 && strncmp (argv[1], "-psn", 4) == 0 ) {
        gArgv = (unsigned char **) SDL_malloc(sizeof (unsigned char *) * 2);
        gArgv[0] = argv[0];
        gArgv[1] = NULL;
        gArgc = 1;
        gFinderLaunch = YES;
    } else {
        int i;
        gArgc = argc;
        gArgv = (unsigned char **) SDL_malloc(sizeof (unsigned char *) * (argc+1));
        for (i = 0; i <= argc; i++)
            gArgv[i] = argv[i];
        gFinderLaunch = NO;
    }

    rlvm_set_locale();

    CustomApplicationMain (argc, argv);
    return 0;
}

