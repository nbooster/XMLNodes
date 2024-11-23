# XMLNodes
A simple and fast C++ header only XML handler library.

Usage:

#include <string>
#include <XMLNodes.hpp>

std::string XML = R"("<A>1</A> <B>2</B> <C attr1="val1", attr2 = "val2"> 3 </C>)";
XML::XMLNode Node("RootNode", "Optional Value", {});
Node.fromString(XML);
Node.print();

Output:

<RootNode>
        <A>1</A>
        <B>2</B>
        <C attr2="val2", attr1="val1"> 3 </C>
</RootNode>

*Note: Prefer this format:

<Root>
  <A> val </A>
</Root>

rather than this:

<Root A = " val "></Root>

if possible, for faster parsing...
