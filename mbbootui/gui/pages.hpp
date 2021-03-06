// pages.hpp - Base classes for page manager of GUI

#ifndef _PAGES_HEADER_HPP
#define _PAGES_HEADER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "minzip/Zip.h"

#include "gui/gui.hpp"
#include "gui/rapidxml.hpp"

using namespace rapidxml;

enum TOUCH_STATE
{
    TOUCH_START = 0,
    TOUCH_DRAG = 1,
    TOUCH_RELEASE = 2,
    TOUCH_HOLD = 3,
    TOUCH_REPEAT = 4
};

struct COLOR {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;

    COLOR() : red(0), green(0), blue(0), alpha(0) {}
    COLOR(unsigned char r, unsigned char g, unsigned char b,
          unsigned char a = 255)
        : red(r), green(g), blue(b), alpha(a) {}
};

struct language_struct
{
    std::string filename;
    std::string displayvalue;
};

inline bool operator < (const language_struct& language1,
                        const language_struct& language2)
{
    return language1.displayvalue < language2.displayvalue;
}

extern std::vector<language_struct> Language_List;

// Utility Functions
int ConvertStrToColor(std::string str, COLOR* color);
int gui_forceRender();
int gui_changePage(std::string newPage);
int gui_changeOverlay(std::string newPage);

class Resource;
class ResourceManager;
class RenderObject;
class ActionObject;
class InputObject;
class MouseCursor;
class GUIObject;
class HardwareKeyboard;

class Page
{
public:
    Page(xml_node<>* page, std::vector<xml_node<>*> *templates);
    virtual ~Page();

    std::string GetName()
    {
        return mName;
    }

public:
    virtual int Render();
    virtual int Update();
    virtual int NotifyTouch(TOUCH_STATE state, int x, int y);
    virtual int NotifyKey(int key, bool down);
    virtual int NotifyCharInput(int ch);
    virtual int SetKeyBoardFocus(int inFocus);
    virtual int NotifyVarChange(const std::string& varName,
                                const std::string& value);
    virtual void SetPageFocus(int inFocus);

protected:
    std::string mName;
    std::vector<GUIObject*> mObjects;
    std::vector<RenderObject*> mRenders;
    std::vector<ActionObject*> mActions;
    std::vector<InputObject*> mInputs;

    ActionObject* mTouchStart;
    COLOR mBackground;

protected:
    bool ProcessNode(xml_node<>* page, std::vector<xml_node<>*> *templates, int depth);
};

struct LoadingContext;

class PageSet
{
public:
    PageSet();
    virtual ~PageSet();

public:
    int Load(LoadingContext& ctx, const std::string& filename);
    int LoadLanguage(char* languageFile, ZipArchive* package);
    void MakeEmergencyConsoleIfNeeded();

    Page* FindPage(const std::string& name);
    int SetPage(const std::string& page);
    int SetOverlay(Page* page);
    const ResourceManager* GetResources();

    // Helper routine for identifing if we're the current page
    int IsCurrentPage(Page* page);
    std::string GetCurrentPage() const;

    // These are routing routines
    int Render();
    int Update();
    int NotifyTouch(TOUCH_STATE state, int x, int y);
    int NotifyKey(int key, bool down);
    int NotifyCharInput(int ch);
    int SetKeyBoardFocus(int inFocus);
    int NotifyVarChange(const std::string& varName, const std::string& value);

    void AddStringResource(std::string resource_source, std::string resource_name, std::string value);

protected:
    int LoadDetails(LoadingContext& ctx, xml_node<>* root);
    int LoadPages(LoadingContext& ctx, xml_node<>* pages);
    int LoadVariables(xml_node<>* vars);

protected:
    ResourceManager* mResources;
    std::vector<Page*> mPages;
    Page* mCurrentPage;
    std::vector<Page*> mOverlays; // Special case for popup dialogs and the lock screen
};

class PageManager
{
public:
    // Used by GUI
    static char* LoadFileToBuffer(const std::string &filename, ZipArchive* package);
    static void LoadLanguageList(ZipArchive* package);
    static void LoadLanguage(const std::string& filename);
    static int LoadPackage(const std::string& name, const std::string& package,
                           const std::string& startpage);
    static PageSet* SelectPackage(const std::string& name);
    static int ReloadPackage(const std::string& name,
                             const std::string& package);
    static void ReleasePackage(const std::string& name);
    static int RunReload();
    static void RequestReload();
    static void SetStartPage(const std::string& page_name);

    // Used for actions and pages
    static int ChangePage(const std::string& name);
    static int ChangeOverlay(const std::string& name);
    static const ResourceManager* GetResources();
    static std::string GetCurrentPage();

    // Helper to identify if a particular page is the active page
    static int IsCurrentPage(Page* page);

    // These are routing routines
    static int Render();
    static int Update();
    static int NotifyTouch(TOUCH_STATE state, int x, int y);
    static int NotifyKey(int key, bool down);
    static int NotifyCharInput(int ch);
    static int SetKeyBoardFocus(int inFocus);
    static int NotifyVarChange(std::string varName, std::string value);

    static MouseCursor *GetMouseCursor();
    static void LoadCursorData(xml_node<>* node);

    static HardwareKeyboard *GetHardwareKeyboard();

    static xml_node<>* FindStyle(std::string name);
    static void AddStringResource(std::string resource_source, std::string resource_name, std::string value);

protected:
    static PageSet* FindPackage(const std::string& name);
    static void LoadLanguageListDir(const std::string& dir);

protected:
    static std::unordered_map<std::string, PageSet*> mPageSets;
    static PageSet* mCurrentSet;
    static MouseCursor *mMouseCursor;
    static HardwareKeyboard *mHardwareKeyboard;
    static bool mReloadTheme;
    static std::string mStartPage;
    static LoadingContext* currentLoadingContext;
};

#endif  // _PAGES_HEADER_HPP
