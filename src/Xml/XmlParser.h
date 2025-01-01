#pragma once

#include "Util/util.h"
#include "XmlLexer.h"

struct XmlNode
{
    XmlNode* parent;
    std::vector<XmlNode> children;
    std::string tag;
    std::string content;
    std::map<std::string, std::variant<int, float, std::string>> attributes;
};

class XmlParser
{
public:
    XmlParser(const std::vector<XmlToken>& tokens) : m_tokens(tokens), m_i(0) {}

    XmlNode parse()
    {
        m_root = XmlNode{
            .parent = nullptr,
            .children = {},
            .tag = "",
            .content = "",
            .attributes = {}
        };
        m_i = 0;

        if (m_tokens.empty())
            return m_root;

        createNodes(&m_root);
        return m_root;
    }

    static void traverseNodes(const XmlNode& node, const std::function<void(const XmlNode&)>& func)
    {
        for (auto child : node.children)
            traverseNodes(child, func);

        func(node);
    }

    static void print(const XmlNode& node, const int level = 0)
    {
        if (level > 0) {
            std::string ident(level-1, '\t');
            std::cout << ident << "tag: " << node.tag;
            if (!node.attributes.empty()) {
                std::cout << ", attr:";
                for (auto&[name, val] : node.attributes) {
                    if (val.index() == 0)
                        std::cout << " " << name << "=" << std::to_string(std::get<int>(val));
                    else if (val.index() == 1)
                        std::cout << " " << name << "=" << std::to_string(std::get<float>(val));
                    else
                        std::cout << " " << name << "=" << std::get<std::string>(val);
                }
            }
            std::cout << ", content: " << node.content << '\n';
        }

        for (auto child : node.children)
            print(child, level+1);
    }

private:
    void createNodes(XmlNode* parent)
    {
        while(m_i < m_tokens.size()) {
            auto token = m_tokens[m_i++];
            
            // add content
            if (token.type == XmlTokenType::CONTENT) {
                parent->content = token.data;
            }
            // add full node
            else if (token.type == XmlTokenType::NODE_FULL) {
                parent->children.push_back(XmlNode{
                    .parent = parent,
                    .children = {},
                    .tag = token.tag,
                    .content = "",
                    .attributes = {}
                });
                parseAttributes(parent->children.back(), token);
            }
            // start new node
            else if (token.type == XmlTokenType::NODE_START) {
                parent->children.push_back(XmlNode{
                    .parent = parent,
                    .children = {},
                    .tag = token.tag,
                    .content = "",
                    .attributes = {}
                });
                parseAttributes(parent->children.back(), token);
                createNodes(&parent->children.back());
            }
            // node end
            else if (token.type == XmlTokenType::NODE_END) {
                return;
            }
        }
    }

    void parseAttributes(XmlNode& node, const XmlToken& token)
    {
        const std::string attr = token.data;

        bool isDigit = false;
        bool isInt = false;
        bool isVal = false;
        bool valFirst = false;
        bool quotExpectet = false;
        std::string name, val;
        for (const char c : attr) {
            if (!isVal && c == ' ')
                continue;

            if (isVal) {
                if (valFirst && c != ' ') {
                    valFirst = false;
                    quotExpectet = c == '"';
                } 
                else if (!valFirst) {
                    if ((quotExpectet && c == '"') || (!quotExpectet && c == ' ')) {
                        quotExpectet = false;
                        isVal = false;
                        if (!val.empty() && !name.empty()) {
                            if (isDigit)
                                try {
                                    if (isInt)
                                        node.attributes.emplace(name, std::stoi(val));
                                    else 
                                        node.attributes.emplace(name, std::stof(val));
                                }
                                catch(std::exception& e) {
                                    node.attributes.emplace(name, val);
                                }
                            else
                                node.attributes.emplace(name, val);
                        }
                        name = "";
                    }
                    else {
                        if (c == ' ' || c == ',')
                            isDigit = false;

                        if (c == '.')
                            isInt = false;

                        val += c;
                    }
                }
            } else {
                if (c == '=') {
                    isVal = true;
                    valFirst = true;
                    isInt = true;
                    isDigit = true;
                    val = "";
                } 
                else {
                    name += c;
                }
            }
        }
    }

    std::vector<XmlToken> m_tokens;
    size_t m_i;
    XmlNode m_root;

};