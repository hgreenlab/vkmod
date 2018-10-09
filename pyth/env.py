import json
fp = open("structures.json", 'r');
str_json = fp.read();
fp.close();
jsonobj = json.loads(str_json)

indentcount = 0
def setindent(ident):
    global indentcount
    indentcount = ident


def indentblock(string, count=-1):
    global indentcount
    if count < 0:
        count = indentcount
    print("IDENT: " + str(count))
    lines = string.split('\n')
    for i in range(len(lines)):
        lines[i] = (' '*count) + lines[i]

    return '\n'.join(lines)

def NAME(string):
    vkobj = string
    ret = ""
    for char in vkobj:
        if char.isupper():
            ret += char

    if string.find("Package") != -1:
        vkobj = "package_" + vkobj[0:len(vkobj)-1]
    return vkobj.lower()

getAbreviation = NAME

INSERTIONS = {
    "decl" : {
        "bind" : "",
        "get"  : ""
    },
    "impl" : {
        "bind" : "",
        "get"  : ""
    }
}

class Package():
    def __init__(self, ctype="Instance", children=[], createinfo=None):

        self.ctype = ctype
        self.children = children

        self.createinfo = createinfo
        if createinfo == "":
            self.createinfo = None
        elif self.createinfo == None:
            self.createinfo = self.ctype+"CreateInfo"

            """
            template <>
            struct Package<{1}> {{
                {0} vkinfo;
                {1} vkobj;
                /* BEGIN CHILDREN */
{2}
                /* END CHILDREN */

                Package();
                Package({0} vkinfo);
            }};
            """.format(self.createinfo, self.ctype, "\n".join("\t\t\t\t\t\t\t\tstd::vector<{0}> v_{1};".format(child, getAbreviation(child)) for child in self.children))

            """
            void push(std::string key, {0} in);
            template <>
            {0} get(std::string key);
            template <>
            std::vector<{0}> get(std::string key);
            /*
            void pop(std::string key) {{
                auto* s = reinterpret_cast<std::stack<std::vector<{0}>>*>(self.bind_map[key]);
                s->pop();
            }}
            */
            """
            #.format(typ)
            """
            void push(std::string key, {0} in) {{
                auto* s = reinterpret_cast<std::stack<{0}>*>(self.bind_map[key]);
                if (s->empty())
                    s->push({0}());

                s->push(in);
            }}
            template <>
            {0} get(std::string key) {{
                auto* s = reinterpret_cast<std::stack<{0}>*>(self.bind_map[key]);
                massert(s->empty() != true, "get(key)", "stack is empty, try push(key, <{0}>())");

                return s->top();
            }}
            template <>
            {0}* get(std::string key) {{
                auto* s = reinterpret_cast<std::stack<{0}>*>(self.bind_map[key]);
                massert(s->empty() != true, "get(key)", "stack is empty, try push(key, <{0}>())");

                return &s->top();
            }}
            """
            #.format(typ)

            """
            void push({0} in);
            template <>
            void pop<{0}>();
            template <>
            {0} get();
            template <>
            std::vector<{0}> get();
            """
            #.format(typ)
            """\t\t\tstd::stack<{0}> s_{1};\n"""
            #.format(typ, getAbreviation(typ))
            """
            void push({0} in) {{
                self.s_{1}.push(in);
            }}
            template <>
            void pop<{0}>() {{

            }}
            template <>
            {0} get() {{
                massert(self.s_{1}.empty() != true, "get()", "top vector is empty! Try: push(<{0}>())");
                return self.s_{1}.top();
            }}
            template <>
            {0}* get() {{
                massert(self.s_{1}.empty() != true, "get()", "top vector is empty! Try: push(<{0}>())");
                return &self.s_{1}.top();
            }}
            """
            #.format(typ, getAbreviation(typ))


packages = [
    Package(ctype="Instance", children=["PhysicalDevice"]),
    #Package(ctype="PhysicalDevice", children=["Device"]),
    Package(ctype="Device", children=["SwapchainKHR", "Semaphore", "Queue", "Image", "Buffer", "Pipeline", "DescriptorPool", "CommandPool"]),
    Package(ctype="SwapchainKHR", children=["Image"], createinfo="vk::SwapchainCreateInfoKHR"),
    Package(ctype="Image", children=["ImageView"]),
    Package(ctype="CommandPool", children=["CommandBuffer"])
]

vkstructures = []
for pak in packages:
    for child in pak.children:
        if not child in vkstructures:
            vkstructures.append(child)
