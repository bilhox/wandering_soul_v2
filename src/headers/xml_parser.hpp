
#include <stack>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>

class Node{

    public :

        Node * parent = nullptr;
        std::string content;
        std::string tag;
        std::map<std::string , std::string> attributes;

        bool isEmpty() const;
        Node(bool orphean = false);
        void addchild(Node& t);
        Node& operator[](std::size_t idx);
        const std::vector<Node> get_children() const;
        unsigned int getChildNumber() const;
        std::vector<const Node*> postfixeParcours() const;
        std::vector<const Node*> findall(std::string tag) const;
        void updatePointers();
        bool isOrphean() const;
    
    private:

        std::vector<Node> children;
        bool m_orphean {false};

};

class Tree{

    public:
        std::map<std::string , std::string> infos;
        Node root;
        Tree(Node root = Node());
};

Tree parse(std::string file);