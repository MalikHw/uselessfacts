#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class $modify(UselessFactMod, MenuLayer) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_task;
    };

    bool init() {
        if (!MenuLayer::init()) return false;
        
        auto spr = CCSprite::create("button.png"_spr);
        if (!spr) {
            log::error("Failed to load button.png");
            return true;
        }
        spr->setScale(0.9f);

        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(UselessFactMod::onTestButton)
        );
        btn->setID("useless-fact-button");

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu) {
            log::error("Could not find bottom-menu node!");
            return true;
        }

        bottomMenu->addChild(btn);
        bottomMenu->updateLayout();

        return true;
    }

    void onTestButton(CCObject* sender) {
        web::WebRequest req;
        req.timeout(std::chrono::seconds(10));

        m_fields->m_task.spawn(
            "Fetch useless fact",
            req.get("https://uselessfacts.jsph.pl/api/v2/facts/random?language=en"),
            [](web::WebResponse res) {
                if (!res.ok()) {
                    log::error("Web request failed: {}", res.code());
                    FLAlertLayer::create("Geode Testing", "Failed to fetch a fact :(", "OK")->show();
                    return;
                }

                auto json = res.json().unwrapOr(matjson::Value{});
                std::string source = json.contains("text")
                    ? json["text"].asString().unwrapOr("No fact found, check internet")
                    : "No text found.";

                FLAlertLayer::create("Useless Fact", source.c_str(), "ye ok")->show();
            }
        );
    }
};
