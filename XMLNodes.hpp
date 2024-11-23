#ifndef _XML_NODES_HPP_
#define _XML_NODES_HPP_

#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>


#define ROOT_NODE_NAME "__ROOT__"

#define EMPTY_NODE_NAME "__UNNAMED__"


namespace XML
{

class XMLNode;

using childrensContainerType = std::vector<XMLNode>;

using attributesContainerType = std::unordered_map<std::string, std::string>;

// boost unordered_flat_map and boost fast_allocator could be used for even more speedup...

class XMLNode 
{
    std::string name;

    std::string value;

    childrensContainerType children;

    attributesContainerType attributes;


    static std::pair<std::string, attributesContainerType> parseTag(std::string tag)
    {
        const auto notWhiteSpace = [](const auto c) { return not std::isspace(c); };

        tag.erase(tag.begin(), std::find_if(tag.begin(), tag.end(), notWhiteSpace));

        const auto itFirstWhite = std::find_if(tag.begin(), tag.end(), [](const auto c) { return std::isspace(c); });

        const auto tagName = tag.substr(0, std::distance(tag.begin(), itFirstWhite));

        tag.erase(tag.begin(), itFirstWhite);

        tag.erase(tag.begin(), std::find_if(tag.begin(), tag.end(), notWhiteSpace));

        bool inQuotes = false;

        std::size_t startNameIndex = 0, startValueIndex = 0;

        std::string name;

        attributesContainerType attributes;

        for ( std::size_t index = 0; index < tag.size(); ++index )
        {
            const auto c = tag[index];

            if ( inQuotes )
            {
                if ( c == '"' and index and tag[index - 1] not_eq '\\' )
                {
                    attributes.insert_or_assign(name, tag.substr(startValueIndex, index - startValueIndex));

                    inQuotes = false;
                }
            }

            else
            {
                if ( std::isspace(c) )
                    continue;

                else if ( c == ',' )
                    startNameIndex = index + 1;

                else if ( c == '=' )
                {
                    name = tag.substr(startNameIndex, index - startNameIndex);

                    name.erase(name.begin(), std::find_if(name.begin(), name.end(), notWhiteSpace));

                    name.erase(std::find_if(name.rbegin(), name.rend(), notWhiteSpace).base(), name.end());
                }

                else if ( c == '"' and index and tag[index - 1] not_eq '\\' )
                {
                    startValueIndex = index + 1;

                    inQuotes = true;
                }
            }
        }

        return { tagName, attributes };
    }


public:

    explicit XMLNode(const std::string& nameArg = ""): 
    name{ nameArg.empty() ? EMPTY_NODE_NAME : nameArg }, value{}, children{}, attributes{} {}

    XMLNode(const std::string& nameArg, const std::string& valueArg, const attributesContainerType& attrs): 
    name{ nameArg }, value{ valueArg }, children{}, attributes{ attrs } {}

    void setName(const std::string& nameArg) { this->name = nameArg; }

    std::string getName(void) const { return this->name; }

    void setValue(const std::string& valueArg) { this->value = valueArg; }

    std::string getValue(void) const { return this->value; }

    void setAttributes(const attributesContainerType& attrs) { this->attributes = attrs; }

    attributesContainerType getAttributes(void) { return this->attributes; }

    const attributesContainerType& getConstRefAttributes(void) const noexcept { return this->attributes; }

    void clearAttributes(void) { this->attributes.clear(); }

    std::string getAttribute(const std::string& name) 
    { try { return this->attributes.at(name); } catch(...) { return ""; } }

    void setAttribute(const std::string& name, const std::string& value) 
    { this->attributes.insert_or_assign(name, value); }

    void clearChildren(void) { this->children.clear(); }

    void setChildren(const childrensContainerType& childrenArg) { this->children = childrenArg; }

    std::size_t getChildrenCount(void) const { return this->children.size(); }

    const childrensContainerType& getChildrenConstRef(void) const { return this->children; }

    childrensContainerType& getChildrenRef(void) noexcept { return this->children; }

    XMLNode& getChildRefByIndex(const std::size_t index) { return this->children.at(index); }

    const XMLNode& getChildConstRefByIndex(const std::size_t index) const { return this->children.at(index); }

    XMLNode* getChildPtrByName(const std::string& name, std::size_t index = 1)
    {
        index = std::max(index, (decltype(index)) 1);

        auto it = std::find_if(this->children.begin(), this->children.end(), 
            [&name, &index](const auto& x){ return x.getName() == name and --index == 0; });

        return it not_eq this->children.end() ? std::addressof(*it) : nullptr;
    }

    void addChild(const XMLNode& nodeArg) { this->children.emplace_back(nodeArg); }

    XMLNode& addChild(const std::string& name, const std::string& value = "", const attributesContainerType& attrs = {}) 
    { return this->children.emplace_back(name, value, attrs); }

    void removeChildByIndex(std::size_t index) 
    {
        if ( index )
            --index;

        if ( index < this->children.size() ) 
            this->children.erase(this->children.begin() + index); 
    }

    void removeChildByName(const std::string& name, std::size_t index = 1)
    {
        index = std::max(index, (decltype(index)) 1);

        const auto it = std::find_if(this->children.begin(), this->children.end(), 
            [&name, &index](const auto& x){ return x.getName() == name and --index == 0; });

        if ( it not_eq this->children.end() )
            this->children.erase(it);
    }

    std::string toString(void) const
    { 
        std::stringstream ss;

        XMLNode::printXML(*this, 0, ss);

        return ss.str(); 
    }

    void fromString(const std::string& xml) 
    { 
        this->value.clear();

        this->children = XMLNode::parseXML(xml).getChildrenConstRef();
    }

    bool toFile(const std::string& filePath)
    {
        std::ofstream filestream(filePath);

        if ( not filestream.is_open() )
            return false;

        XMLNode::printXML(*this, 0, filestream);

        return true;
    }

    bool fromFile(const std::string& filePath) 
    {
        std::ifstream filestream(filePath);

        if ( not filestream.is_open() )
            return false;

        this->value.clear();

        this->children = XMLNode::parseXML(
                std::string(( std::istreambuf_iterator<char>(filestream) ), ( std::istreambuf_iterator<char>() ))
            ).getChildrenConstRef();

        return true;
    }

    void print(void) { XMLNode::printXML(*this); }

    static void printXML(const XMLNode& node, std::uint16_t tabsArg = 0, std::ostream& out = std::cout)
    {
        out << std::string(tabsArg, '\t') << "<" << node.getName();

        std::size_t index = 0;

        const auto& attrs = node.getConstRefAttributes();

        for ( const auto& [name, value] : attrs )
        {
            out << " " << name << "=\"" << value << "\"";

            if ( ++index not_eq attrs.size() )
                out << ",";
        }

        out << ">";

        if ( node.getChildrenCount() )
        {
            out << "\n";

            for ( const auto& childNode : node.getChildrenConstRef() )
                printXML(childNode, tabsArg + 1, out);
        }

        else
            out << node.getValue();

        out << std::string(node.getChildrenCount() ? tabsArg : 0, '\t') << "</" << node.getName() << ">";

        out << "\n";
    }

    static XMLNode parseXML(const std::string& xml) 
    {
        try
        {
            XMLNode root { ROOT_NODE_NAME };

            XMLNode* current = std::addressof(root);    

            std::stack<XMLNode*> nodes { { current } };

            bool openingTag = false;

            std::string value;

            for ( std::size_t index { 0 }; index < xml.size(); )
            {
                if ( const auto c = xml[index]; not std::isspace(c) )
                {
                    if ( c == '<' )
                    {
                        const std::size_t start = index + 1, end = xml.find_first_of(">", start);

                        if ( xml[start] == '/' )
                        {
                            if ( XMLNode* const node = nodes.top(); xml.substr(start + 1, end - start - 1) == node->getName() )
                            {
                                if ( node->getChildrenCount() == 0 )
                                {
                                    node->setValue(value);

                                    value.clear();
                                }

                                nodes.pop();
                            }

                            else
                                return XMLNode{};

                            current = nodes.top();

                            openingTag = false;
                        }

                        else
                        {
                            const auto [tagName, attrs] = XMLNode::parseTag(xml.substr(start, end - start));

                            nodes.emplace((current = std::addressof(current->addChild(tagName, "", attrs))));

                            openingTag = true;
                        }

                        index = end;

                        continue;
                    }

                    else if ( c == '>' and openingTag )
                    {
                        const std::size_t start = index + 1, end = xml.find_first_of("<", start);

                        value = xml.substr(start, end - start);

                        index = end;

                        continue;
                    }
                }

                ++index;
            }

            return root;
        }

        catch(...)
        {
            return XMLNode{};
        }
    }
};

}

#endif
