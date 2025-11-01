#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "String.hpp"

// ============================================================================
// HTML Tag Enum - For performance
// ============================================================================
enum class Tag {
    TEXT,       // Special tag for text nodes
    DIV,
    SPAN,
    H1,
    H2,
    H3,
    H4,
    H5,
    H6,
    P,
    A,
    BUTTON,
    INPUT,
    TEXTAREA,
    SELECT,
    OPTION,
    UL,
    OL,
    LI,
    TABLE,
    THEAD,
    TBODY,
    TR,
    TD,
    TH,
    FORM,
    LABEL,
    IMG,
    BR,
    HR,
    // Add more as needed
};

// Helper to convert Tag enum to string
inline const char* tagToString(Tag tag) {
    switch (tag) {
        case Tag::TEXT: return "#text";
        case Tag::DIV: return "div";
        case Tag::SPAN: return "span";
        case Tag::H1: return "h1";
        case Tag::H2: return "h2";
        case Tag::H3: return "h3";
        case Tag::H4: return "h4";
        case Tag::H5: return "h5";
        case Tag::H6: return "h6";
        case Tag::P: return "p";
        case Tag::A: return "a";
        case Tag::BUTTON: return "button";
        case Tag::INPUT: return "input";
        case Tag::TEXTAREA: return "textarea";
        case Tag::SELECT: return "select";
        case Tag::OPTION: return "option";
        case Tag::UL: return "ul";
        case Tag::OL: return "ol";
        case Tag::LI: return "li";
        case Tag::TABLE: return "table";
        case Tag::THEAD: return "thead";
        case Tag::TBODY: return "tbody";
        case Tag::TR: return "tr";
        case Tag::TD: return "td";
        case Tag::TH: return "th";
        case Tag::FORM: return "form";
        case Tag::LABEL: return "label";
        case Tag::IMG: return "img";
        case Tag::BR: return "br";
        case Tag::HR: return "hr";
        default: return "div";
    }
}

// ============================================================================
// VNode - Virtual DOM Node
// ============================================================================
class VNode {
public:
    Tag tag;
    std::map<std::string, std::string> props;
    std::vector<VNode> children;

    // Constructor for element nodes
    VNode(Tag t, std::map<std::string, std::string> p = {}, std::vector<VNode> c = {})
        : tag(t), props(std::move(p)), children(std::move(c)) {}

    // Check if this is a text node
    bool isText() const {
        return tag == Tag::TEXT;
    }

    // Get text content (only valid for TEXT nodes)
    std::string getText() const {
        if (isText() && props.count("text")) {
            return props.at("text");
        }
        return "";
    }
};

// ============================================================================
// Helper Functions - Create text nodes
// ============================================================================
inline VNode text(const std::string& content) {
    return VNode(Tag::TEXT, {{"text", content}});
}

inline VNode text(const String& content) {
    return VNode(Tag::TEXT, {{"text", content.std_str()}});
}

inline VNode text(const char* content) {
    return VNode(Tag::TEXT, {{"text", content}});
}

// ============================================================================
// HTML Element Helper Functions
// ============================================================================
inline VNode div(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::DIV, std::move(props), std::move(children));
}

inline VNode span(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::SPAN, std::move(props), std::move(children));
}

inline VNode h1(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H1, std::move(props), std::move(children));
}

inline VNode h2(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H2, std::move(props), std::move(children));
}

inline VNode h3(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H3, std::move(props), std::move(children));
}

inline VNode h4(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H4, std::move(props), std::move(children));
}

inline VNode h5(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H5, std::move(props), std::move(children));
}

inline VNode h6(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::H6, std::move(props), std::move(children));
}

inline VNode p(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::P, std::move(props), std::move(children));
}

inline VNode a(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::A, std::move(props), std::move(children));
}

inline VNode button(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::BUTTON, std::move(props), std::move(children));
}

inline VNode input(std::map<std::string, std::string> props = {}) {
    return VNode(Tag::INPUT, std::move(props), {});
}

inline VNode textarea(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TEXTAREA, std::move(props), std::move(children));
}

inline VNode select(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::SELECT, std::move(props), std::move(children));
}

inline VNode option(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::OPTION, std::move(props), std::move(children));
}

inline VNode ul(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::UL, std::move(props), std::move(children));
}

inline VNode ol(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::OL, std::move(props), std::move(children));
}

inline VNode li(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::LI, std::move(props), std::move(children));
}

inline VNode table(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TABLE, std::move(props), std::move(children));
}

inline VNode thead(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::THEAD, std::move(props), std::move(children));
}

inline VNode tbody(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TBODY, std::move(props), std::move(children));
}

inline VNode tr(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TR, std::move(props), std::move(children));
}

inline VNode td(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TD, std::move(props), std::move(children));
}

inline VNode th(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::TH, std::move(props), std::move(children));
}

inline VNode form(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::FORM, std::move(props), std::move(children));
}

inline VNode label(std::map<std::string, std::string> props = {}, std::vector<VNode> children = {}) {
    return VNode(Tag::LABEL, std::move(props), std::move(children));
}

inline VNode img(std::map<std::string, std::string> props = {}) {
    return VNode(Tag::IMG, std::move(props), {});
}

inline VNode br() {
    return VNode(Tag::BR, {}, {});
}

inline VNode hr(std::map<std::string, std::string> props = {}) {
    return VNode(Tag::HR, std::move(props), {});
}

// ============================================================================
// Event Callback Helpers - Create event handler strings
// ============================================================================

// Generic callback with no event data
inline std::string Func(int eventId) {
    return "invokeCallback(" + std::to_string(eventId) + ")";
}

// Future: Mouse event callback (placeholder for future implementation)
// inline std::string FuncMouseEvent(int eventId) {
//     return "invokeCallbackMouseEvent(" + std::to_string(eventId) + ")";
// }

// Future: Keyboard event callback (placeholder for future implementation)
// inline std::string FuncKeyboardEvent(int eventId) {
//     return "invokeCallbackKeyboardEvent(" + std::to_string(eventId) + ")";
// }

// Future: Input event callback (placeholder for future implementation)
// inline std::string FuncInputEvent(int eventId) {
//     return "invokeCallbackInputEvent(" + std::to_string(eventId) + ")";
// }
