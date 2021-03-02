#include "pch.h"
#include <fstream>

void menuButtonThingy(CCMenuItemSpriteExtra* item, const char* text, float y, float fontSize = 8.5f) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    static_cast<CCSprite*>(item->getNormalImage())->setOpacity(0);
    auto label = CCLabelTTF::create(text, "Arial", fontSize);
    auto size = label->getScaledContentSize();
    CCSize cSize = { size.width + 10.f, size.height + 5.f };
    item->setContentSize(cSize);
    label->setPosition({ cSize.width / 2.f, cSize.height / 2.f });
    item->addChild(label);
    item->setPosition({ size.width / 2.f, y });
}

void menuButtonThingy(CCObject* obj, const char* text, float y) {
    menuButtonThingy(dynamic_cast<CCMenuItemSpriteExtra*>(obj), text, y);
}

inline bool(__thiscall* MenuLayer_init)(cocos2d::CCLayer* self);
bool __fastcall MenuLayer_initHook(cocos2d::CCLayer* self) {
    bool ret = MenuLayer_init(self);

    auto children = self->getChildren();

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    
    auto sprTitle = dynamic_cast<CCSprite*>(children->objectAtIndex(1));
    sprTitle->setScale(0.6f);
    sprTitle->setPositionX(winSize.width / 4.f + 20.f);
    sprTitle->setPositionY(winSize.height / 2.f + 30.f);

    auto menu = dynamic_cast<CCMenu*>(children->objectAtIndex(2));
    menu->setPosition({ 50.f, winSize.height / 2.f + 5.f});

    float spacing = 13.f;
    int i = 0;
    
    {
        auto children = menu->getChildren();

        menuButtonThingy(children->objectAtIndex(0), "PLAY", spacing * -i++);
        menuButtonThingy(children->objectAtIndex(2), "ONLINE", spacing * -i++);
        menuButtonThingy(children->objectAtIndex(1), "ICONS", spacing * -i++);
        menuButtonThingy(children->objectAtIndex(3), "PROFILE", spacing * -i++);
    }

    // children->objectAtIndex(3) // menu with options and achievements
    // children->objectAtIndex(4) // robtop social medias (useless)
    // children->objectAtIndex(5) // exit button and more games (textureldr)
    // children->objectAtIndex(6) // user name

    auto menu2 = dynamic_cast<CCMenu*>(children->objectAtIndex(3));

    auto btnAchievements = dynamic_cast<CCMenuItemSpriteExtra*>(menu2->getChildren()->objectAtIndex(0));
    menuButtonThingy(btnAchievements, "ACHIEVEMENTS", spacing * -i++);
    btnAchievements->removeFromParent();
    menu->addChild(btnAchievements);

    auto btnSettings = dynamic_cast<CCMenuItemSpriteExtra*>(menu2->getChildren()->objectAtIndex(0));
    menuButtonThingy(btnSettings, "SETTINGS", spacing * -i++);
    menu2->removeChild(btnSettings, false);
    menu->addChild(btnSettings);

    auto btnRewards = dynamic_cast<CCMenuItemSpriteExtra*>(menu2->getChildren()->objectAtIndex(2));
    menuButtonThingy(btnRewards, "DAILY REWARDS", spacing * -i++);
    menu2->removeChild(btnRewards, false);
    menu->addChild(btnRewards);

    auto menu3 = dynamic_cast<CCMenu*>(children->objectAtIndex(5));

    auto btnTextures = dynamic_cast<CCMenuItemSpriteExtra*>(menu3->getChildren()->objectAtIndex(0));
    menuButtonThingy(btnTextures, "TEXTURES", spacing * -i++);
    menu3->removeChild(btnTextures, false);
    menu->addChild(btnTextures);

    auto btnQuit = dynamic_cast<CCMenuItemSpriteExtra*>(menu3->getChildren()->objectAtIndex(0));
    menuButtonThingy(btnQuit, "QUIT", spacing * -i++);
    menu3->removeChild(btnQuit, false);
    menu->addChild(btnQuit);

    dynamic_cast<CCMenu*>(children->objectAtIndex(4))->removeFromParentAndCleanup(true);
    dynamic_cast<CCNode*>(children->objectAtIndex(6))->removeFromParentAndCleanup(true);
    menu2->removeFromParentAndCleanup(true);
    menu3->removeFromParentAndCleanup(true);

    return ret;
}

DWORD WINAPI my_thread(void* hModule) {
#ifdef _DEBUG
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    static std::ofstream conout("CONOUT$", std::ios::out);
    std::cout.rdbuf(conout.rdbuf());

    std::cout << "hi" << std::endl;
#endif

    MH_STATUS err;
    if ((err = MH_Initialize()) != MH_OK) {
        std::cout << "ini wtf? " << err << std::endl;
        return 0;
    }

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    if ((err = MH_CreateHook(reinterpret_cast<void*>(base + 0x1907b0), MenuLayer_initHook, reinterpret_cast<void**>(&MenuLayer_init))) != MH_OK) {
        std::cout << "cre wtf? " << err << std::endl;
        return 0;
    }

    if ((err = MH_EnableHook(MH_ALL_HOOKS)) != MH_OK) {
        std::cout << "ena wtf? " << err << std::endl;
        return 0;
    }

#ifdef _DEBUG
    std::string line;
    std::getline(std::cin, line);
    MH_RemoveHook(reinterpret_cast<void*>(base + 0x1907b0));
    MH_Uninitialize();
    fclose(stdout);
    fclose(stdin);
    FreeConsole();
    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(hModule), 0);
#endif

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0x1000, my_thread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

