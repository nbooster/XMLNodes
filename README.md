# XMLNodes
A simple and fast C++ header only XML handler library.

Usage:
```C++
#include <string>
#include <XMLNodes.hpp>

std::string XML = R"("<A>1</A> <B>2</B> <C attr1="val1", attr2 = "val2"> 3 </C>)";
XML::XMLNode Node("RootNode", "Optional Value", {});
Node.fromString(XML);
Node.print();
```

Output:
```
<RootNode>
        <A>1</A>
        <B>2</B>
        <C attr2="val2", attr1="val1"> 3 </C>
</RootNode>
```

*Note: Prefer this format:

```
<Root>   
  <A> val </A>
</Root>
```

rather than this:
```
<Root A = " val "></Root>
```

if possible, for faster parsing...

API

```C++
explicit XMLNode(const std::string& nameArg = "");

XMLNode(const std::string& nameArg, const std::string& valueArg, const attributesContainerType& attrs);

void setName(const std::string& nameArg);

std::string getName(void) const;

void setValue(const std::string& valueArg);

std::string getValue(void) const;

void setAttributes(const attributesContainerType& attrs);

attributesContainerType getAttributes(void);

const attributesContainerType& getConstRefAttributes(void) const noexcept;

void clearAttributes(void);

std::string getAttribute(const std::string& name);

void setAttribute(const std::string& name, const std::string& value);

void clearChildren(void);

void setChildren(const childrensContainerType& childrenArg);

std::size_t getChildrenCount(void) const;

const childrensContainerType& getChildrenConstRef(void) const;

childrensContainerType& getChildrenRef(void) noexcept;

XMLNode& getChildRefByIndex(const std::size_t index);

const XMLNode& getChildConstRefByIndex(const std::size_t index) const;

XMLNode* getChildPtrByName(const std::string& name, std::size_t index = 1);

void addChild(const XMLNode& nodeArg);

XMLNode& addChild(const std::string& name, const std::string& value = "", const attributesContainerType& attrs = {});

void removeChildByIndex(std::size_t index);

void removeChildByName(const std::string& name, std::size_t index = 1);

std::string toString(void) const;

void fromString(const std::string& xml);

bool toFile(const std::string& filePath);

bool fromFile(const std::string& filePath);

void print(void);

static void printXML(const XMLNode& node, std::uint16_t tabsArg = 0, std::ostream& out = std::cout);

static XMLNode parseXML(const std::string& xml);
```
